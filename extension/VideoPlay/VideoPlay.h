#ifndef _VIDEOPLAY_H_
#define _VIDEOPLAY_H_

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

//! 打开视频文件
DLL_EXPORT int VideoPlayOpen(const char* file, int size, void** context);

//! 获取音视频信息
DLL_EXPORT int VideoPlayInfo(void* context, int* width, int* height, float* length, int* channel, int* sampleRate, int* sampleBits);

//! 设置播放时间
DLL_EXPORT int VideoPlayTime(void* context, float time);

//! 跳至指定播放时间
DLL_EXPORT int VideoPlaySeek(void* context, float time);

//! 获取视频帧
DLL_EXPORT int VideoPlayVideo(void* context, unsigned char* data);

//! 获取音频帧
DLL_EXPORT int VideoPlayAudio(void* context, unsigned char* data, int size);

//! 关闭视频文件
DLL_EXPORT int VideoPlayClose(void* context);

#endif