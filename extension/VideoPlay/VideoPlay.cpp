#include "VideoPlay.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <pthread.h>
#endif
#include <ctime>
#include <map>
using namespace std;
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#ifndef __ANDROID__
#include <libavdevice/avdevice.h>
#endif
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/imgutils.h>
}

#ifdef _WIN32
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "swresample.lib")
#endif

/**
* 播放上下文
*/
struct SPlayContext {
	AVFormatContext* inputContext;
	AVStream* videoStream;
	AVStream* audioStream;
	AVCodecContext* videoDecoder;
	AVCodecContext* audioDecoder;
	AVFrame* videoFrame;
	AVFrame* audioFrame;
	SwsContext* videoConverter;
	SwrContext* audioConverter;
	AVFrame* outputVideo;
	AVFrame* outputAudio;
	int audioBufferSize;
	int audioBufferLength;
	unsigned char* audioBuffer;
	float playClock;
	float setPlayTime;
	float streamLength;
	clock_t lastPlayTime;
	bool seekPosition;
	bool isRealtime;
	bool videoClosed;
	unsigned char* inputMemoryBuffer;
	int inputMemoryBufferSize;
	int inputMemoryBufferOffset;
#ifdef _WIN32
	HANDLE threadHandle;
	CRITICAL_SECTION mutex;
#else
	pthread_t threadHandle;
	pthread_mutex_t mutex;
#endif
};

// 播放库是否初始化
static bool Initialized = false;
// 内存读取回调
int BufferRead(void *context, uint8_t *buffer, int size);
// 视频解码线程
#ifdef _WIN32
DWORD WINAPI ThreadWorking(LPVOID pParam);
#define LOCK_BEGIN(x) EnterCriticalSection(&x)
#define LOCK_END(x) LeaveCriticalSection(&x)
#else
void* ThreadWorking(void* pParam);
#define LOCK_BEGIN(x) pthread_mutex_lock(&x)
#define LOCK_END(x) pthread_mutex_unlock(&x)
#endif

/**
* 打开视频文件
*/
DLL_EXPORT int VideoPlayOpen(const char* file, int size, void** context) {
	if (!Initialized) {
		av_register_all();
		avcodec_register_all();
#ifndef __ANDROID__
		avdevice_register_all();
#endif
		avformat_network_init();
		Initialized = true;
	}
	// 打开视频源
	SPlayContext* ctx = new SPlayContext();
	ctx->videoStream = 0;
	ctx->audioStream = 0;
	ctx->inputMemoryBufferSize = size;
	ctx->inputMemoryBuffer = size > 0 ? (unsigned char*)file : 0;
	ctx->inputContext = avformat_alloc_context();
	ctx->inputContext->max_analyze_duration = 5 * AV_TIME_BASE;
	ctx->isRealtime = strncmp(file, "rtsp://", 7) == 0 || strncmp(file, "rtmp://", 7) == 0;
	// 从内存中读取
	if (ctx->inputMemoryBuffer) {
		unsigned char* ioBuffer = (unsigned char*)av_malloc(4096);
		ctx->inputContext->pb = avio_alloc_context(ioBuffer, 4096, 0, ctx, BufferRead, 0, 0);
		ctx->inputMemoryBufferOffset = 0;
		file = "memory";
	}
	AVDictionary* options = 0;
	if (!strncmp("#listen", file + strlen(file) - 7, 7)) {
		av_dict_set(&options, "rtsp_flags", "listen", 0);
		av_dict_set_int(&options, "timeout", 60, 0);
	}
	AVInputFormat* fmt = 0;
	if (!strncmp("screen:", file, 7)) {
		ctx->isRealtime = true;
#ifdef _WIN32
		file = file + 7;
		fmt = av_find_input_format("gdigrab");
#else
		file = file + 6;
		fmt = av_find_input_format("x11grab");
#endif
	}
	if (avformat_open_input(&ctx->inputContext, file, fmt, options ? &options : 0) != 0 ||
		avformat_find_stream_info(ctx->inputContext, 0) < 0) {
		av_dict_free(&options);
		avformat_close_input(&ctx->inputContext);
		delete ctx;
		return -1;
	}
	av_dict_free(&options);
	// 查找音视频流
	for (unsigned int i = 0; i < ctx->inputContext->nb_streams; i++) {
		if (!ctx->videoStream && AVMEDIA_TYPE_VIDEO == ctx->inputContext->streams[i]->codecpar->codec_type) {
			ctx->videoStream = ctx->inputContext->streams[i];
		}
		if (!ctx->audioStream && AVMEDIA_TYPE_AUDIO == ctx->inputContext->streams[i]->codecpar->codec_type) {
			ctx->audioStream = ctx->inputContext->streams[i];
		}
	}
	// 查找视频解码器
	AVCodec* codec = ctx->videoStream? avcodec_find_decoder(ctx->videoStream->codecpar->codec_id): 0;
	if (!codec) {
		avformat_close_input(&ctx->inputContext);
		delete ctx;
		return -2;
	}
	// 打开解码器
	ctx->videoDecoder = avcodec_alloc_context3(codec);
	avcodec_parameters_to_context(ctx->videoDecoder, ctx->videoStream->codecpar);
	if (avcodec_open2(ctx->videoDecoder, codec, 0) < 0) {
		avcodec_free_context(&ctx->videoDecoder);
		avformat_close_input(&ctx->inputContext);
		delete ctx;
		return -3;
	}
	// 视频转换器
	int frameWidth = ctx->videoStream->codecpar->width;
	int frameHeight = ctx->videoStream->codecpar->height;
	AVPixelFormat format = (AVPixelFormat)ctx->videoStream->codecpar->format;
	ctx->videoConverter = sws_getContext(frameWidth, frameHeight, format, frameWidth, frameHeight, AV_PIX_FMT_RGB24, SWS_BILINEAR, 0, 0, 0);
	ctx->videoFrame = av_frame_alloc();
	ctx->outputVideo = av_frame_alloc();
	av_image_alloc(ctx->outputVideo->data, ctx->outputVideo->linesize, frameWidth, frameHeight, AV_PIX_FMT_RGB24, 1);
	ptrdiff_t linesize[4];
	linesize[0] = ctx->outputVideo->linesize[0];
	linesize[1] = ctx->outputVideo->linesize[1];
	linesize[2] = ctx->outputVideo->linesize[2];
	linesize[3] = ctx->outputVideo->linesize[3];
	av_image_fill_black(ctx->outputVideo->data, linesize, AV_PIX_FMT_RGB24, AVCOL_RANGE_MPEG, frameWidth, frameHeight);
	// 记录视频信息
	ctx->audioDecoder = 0;
	ctx->audioFrame = 0;
	ctx->outputAudio = 0;
	ctx->playClock = 0.0f;
	ctx->setPlayTime = 0.0f;
	ctx->lastPlayTime = 0;
	ctx->seekPosition = false;
	ctx->videoClosed = false;
	ctx->streamLength = (float)(ctx->videoStream->duration * av_q2d(ctx->videoStream->time_base));
	*context = ctx;
	// 开启解码线程
#ifdef _WIN32
	ctx->threadHandle = CreateThread(NULL, 0, ThreadWorking, ctx, 0, NULL);
	InitializeCriticalSection(&ctx->mutex);
#else
	pthread_create(&ctx->threadHandle, 0, ThreadWorking, ctx);
	pthread_mutex_init(&ctx->mutex, 0);
#endif
	return 0;
}

/**
* 获取音视频信息
*/
DLL_EXPORT int VideoPlayInfo(void* context, int* width, int* height, float* length, int* channel, int* sampleRate, int* sampleBits) {
	SPlayContext* ctx = (SPlayContext*)context;
	// 视频信息
	if (ctx->videoStream) {
		if (width) *width = ctx->videoStream->codecpar->width;
		if (height) *height = ctx->videoStream->codecpar->height;
		if (length) *length = ctx->streamLength;
	}
	// 音频信息
	if (ctx->audioStream) {
		if (channel) *channel = ctx->audioStream->codecpar->channels;
		if (sampleRate) *sampleRate = ctx->audioStream->codecpar->sample_rate;
		if (sampleBits) *sampleBits = 16;
	}
	return 0;
}

/**
* 设置播放时间
*/
DLL_EXPORT int VideoPlayTime(void* context, float time) {
	SPlayContext* ctx = (SPlayContext*)context;
	if (time < 0.0f) {
		ctx->setPlayTime = 0.0f;
		ctx->seekPosition = true;
	} else if (!ctx->isRealtime) {
		ctx->setPlayTime = time;
		ctx->seekPosition = true;
	} else return -1;
	return 0;
}

/**
* 获取视频帧
*/
DLL_EXPORT int VideoPlayVideo(void* context, unsigned char* data) {
	SPlayContext* ctx = (SPlayContext*)context;
	int frameWidth = ctx->videoStream->codecpar->width;
	int frameHeight = ctx->videoStream->codecpar->height;
	int rowCount = frameWidth * 3;
	for (int y = 0; y < frameHeight; y++) {
		memcpy(data + rowCount * y, ctx->outputVideo->data[0] + y * ctx->outputVideo->linesize[0], rowCount);
	}
	// 播放至结尾
	if (ctx->streamLength > 0.0f && ctx->setPlayTime > ctx->streamLength) return 1;
	return 0;
}

/**
* 获取音频帧
*/
DLL_EXPORT int VideoPlayAudio(void* context, unsigned char* data, int size) {
	SPlayContext* ctx = (SPlayContext*)context;
	if (!ctx->audioStream) return -1;
	if (!ctx->audioDecoder) {
		AVCodec* codec = avcodec_find_decoder(ctx->audioStream->codecpar->codec_id);
		if (!codec) {
			ctx->audioStream = 0;
			return -2;
		}
		AVCodecContext* decoder = avcodec_alloc_context3(codec);
		avcodec_parameters_to_context(decoder, ctx->audioStream->codecpar);
		if (avcodec_open2(decoder, codec, 0) < 0) {
			avcodec_free_context(&decoder);
			ctx->audioStream = 0;
			return -3;
		}
		uint64_t layout = decoder->channel_layout;
		if (layout == 0) {
			if (decoder->channels == 1) layout = AV_CH_LAYOUT_MONO;
			else if (decoder->channels == 2) layout = AV_CH_LAYOUT_STEREO;
			else {
				avcodec_free_context(&decoder);
				ctx->audioStream = 0;
				return -4;
			}
		}
		int sampleRate = decoder->sample_rate;
		AVSampleFormat input = decoder->sample_fmt;
		AVSampleFormat output = AV_SAMPLE_FMT_S16;
		ctx->audioConverter = swr_alloc_set_opts(0, layout, output, sampleRate, layout, input, sampleRate, 0, 0);
		swr_init(ctx->audioConverter);
		ctx->audioFrame = av_frame_alloc();
		ctx->outputAudio = av_frame_alloc();
		ctx->outputAudio->channels = decoder->channels;
		ctx->outputAudio->format = output;
		ctx->outputAudio->sample_rate = sampleRate;
		ctx->outputAudio->nb_samples = 8192;
		av_samples_alloc(ctx->outputAudio->data, ctx->outputAudio->linesize, decoder->channels, ctx->outputAudio->nb_samples, output, 0);
		ctx->audioBufferSize = decoder->channels * av_get_bytes_per_sample(output) * sampleRate;
		ctx->audioBufferLength = 0;
		ctx->audioBuffer = new unsigned char[ctx->audioBufferSize];
		ctx->audioDecoder = decoder;
	}
	LOCK_BEGIN(ctx->mutex);
	int bufferSize = size > ctx->audioBufferLength ? ctx->audioBufferLength : size;
	memcpy(data, ctx->audioBuffer, bufferSize);
	ctx->audioBufferLength -= bufferSize;
	memmove(ctx->audioBuffer, ctx->audioBuffer + bufferSize, ctx->audioBufferLength);
	LOCK_END(ctx->mutex);
	return bufferSize;
}

/**
* 关闭视频文件
*/
DLL_EXPORT int VideoPlayClose(void* context) {
	SPlayContext* ctx = (SPlayContext*)context;
	ctx->videoClosed = true;
#ifdef _WIN32
	WaitForSingleObject(ctx->threadHandle, INFINITE);
	DeleteCriticalSection(&ctx->mutex);
#else
	pthread_join(ctx->threadHandle, NULL);
	pthread_mutex_destroy(&ctx->mutex);
#endif
	if (ctx->videoDecoder) {
		sws_freeContext(ctx->videoConverter);
		av_freep(&ctx->outputVideo->data[0]);
		av_frame_free(&ctx->outputVideo);
		av_frame_free(&ctx->videoFrame);
		avcodec_free_context(&ctx->videoDecoder);
	}
	if (ctx->audioDecoder) {
		delete[] ctx->audioBuffer;
		swr_free(&ctx->audioConverter);
		av_freep(&ctx->outputAudio->data[0]);
		av_frame_free(&ctx->outputAudio);
		av_frame_free(&ctx->audioFrame);
		avcodec_free_context(&ctx->audioDecoder);
	}
	avformat_close_input(&ctx->inputContext);
	delete ctx;
	return 0;
}

/**
* 从内存缓冲区读取
*/
int BufferRead(void *context, uint8_t *buffer, int size) {
	SPlayContext* ctx = (SPlayContext*)context;
	if (ctx->inputMemoryBufferOffset + size > ctx->inputMemoryBufferSize) {
		size = ctx->inputMemoryBufferSize - ctx->inputMemoryBufferOffset;
	}
	memcpy(buffer, ctx->inputMemoryBuffer + ctx->inputMemoryBufferOffset, size);
	ctx->inputMemoryBufferOffset += size;
	return size;
}

/**
* 图像采集线程
*/
#ifdef _WIN32
DWORD WINAPI ThreadWorking(LPVOID pParam) {
#else
void* ThreadWorking(void* pParam) {
#endif
	SPlayContext* ctx = static_cast<SPlayContext*>(pParam);
	ctx->lastPlayTime = clock();
	while (!ctx->videoClosed) {
		// 从头开始
		if (ctx->seekPosition) {
			if (ctx->videoStream) {
				int64_t timestamp = (int64_t)(ctx->setPlayTime * ctx->videoStream->time_base.den) / ctx->videoStream->time_base.num;
				av_seek_frame(ctx->inputContext, ctx->videoStream->index, ctx->videoStream->start_time + timestamp, AVSEEK_FLAG_BACKWARD);
			} else if (ctx->audioStream) {
				int64_t timestamp = (int64_t)(ctx->setPlayTime * ctx->audioStream->time_base.den) / ctx->audioStream->time_base.num;
				av_seek_frame(ctx->inputContext, ctx->audioStream->index, ctx->audioStream->start_time + timestamp, AVSEEK_FLAG_BACKWARD);
			}
			if (ctx->videoDecoder) avcodec_flush_buffers(ctx->videoDecoder);
			if (ctx->audioDecoder) avcodec_flush_buffers(ctx->audioDecoder);
			ctx->seekPosition = false;
			ctx->playClock = ctx->setPlayTime;
			ctx->inputMemoryBufferOffset = 0;
		}
		// 防止播放过快
		AVPacket packet;
		clock_t currentTime = clock();
		ctx->setPlayTime += 0.001f * (currentTime - ctx->lastPlayTime);
		ctx->lastPlayTime = currentTime;
		if ((!ctx->isRealtime && ctx->playClock > ctx->setPlayTime) || av_read_frame(ctx->inputContext, &packet) < 0) {
#ifdef _WIN32
			::Sleep(5);
#else
			usleep(5000);
#endif
			continue;
		}
		// 视频解码
		if (ctx->videoStream && packet.stream_index == ctx->videoStream->index && ctx->videoDecoder) {
			avcodec_send_packet(ctx->videoDecoder, &packet);
			av_packet_unref(&packet);
			while (avcodec_receive_frame(ctx->videoDecoder, ctx->videoFrame) == 0) {
				ctx->playClock = (float)av_q2d(ctx->videoStream->time_base);
				ctx->playClock *= av_frame_get_best_effort_timestamp(ctx->videoFrame);
				sws_scale(ctx->videoConverter, ctx->videoFrame->data, ctx->videoFrame->linesize, 0,
					ctx->videoStream->codecpar->height, ctx->outputVideo->data, ctx->outputVideo->linesize);
			}
			continue;
		}
		// 音频解码
		if (ctx->audioStream && packet.stream_index == ctx->audioStream->index && ctx->audioDecoder) {
			avcodec_send_packet(ctx->audioDecoder, &packet);
			av_packet_unref(&packet);
			while (avcodec_receive_frame(ctx->audioDecoder, ctx->audioFrame) == 0) {
				int samples = swr_convert(ctx->audioConverter, ctx->outputAudio->data, ctx->outputAudio->nb_samples,
					(const uint8_t**)ctx->audioFrame->data, ctx->audioFrame->nb_samples);
				int count = samples * ctx->outputAudio->channels * 2;
				LOCK_BEGIN(ctx->mutex);
				if (ctx->audioBufferLength + count <= ctx->audioBufferSize) {
					memcpy(ctx->audioBuffer + ctx->audioBufferLength, ctx->outputAudio->data[0], count);
					ctx->audioBufferLength += count;
				}
				LOCK_END(ctx->mutex);
			}
			continue;
		}
		av_packet_unref(&packet);
	}
	return 0;
}