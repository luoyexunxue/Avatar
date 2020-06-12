//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSoundManager.h"
#include "AvatarConfig.h"
#include "CEngine.h"
#include "CStringUtil.h"
#include "CLog.h"
#include "thirdparty/openal/al.h"
#include "thirdparty/openal/alc.h"
#include <cmath>
#include <cstring>

#ifdef AVATAR_WINDOWS
#pragma comment (lib, "thirdparty/openal/openal32.lib")
#endif

/**
* 构造函数
*/
CSoundManager::CSoundManager() {
	m_pCaptureDevice = 0;
	m_iCaptureSampleBits = 0;
	InitOpenAL(0);
}

/**
* 析构函数
*/
CSoundManager::~CSoundManager() {
	m_pInstance = 0;
}

/**
* 单例实例
*/
CSoundManager* CSoundManager::m_pInstance = 0;

/**
* 销毁声音管理器
*/
void CSoundManager::Destroy() {
	if (m_pCaptureDevice) StopRecord();
	map<int, SSoundSource>::iterator iter = m_mapSource.begin();
	while (iter != m_mapSource.end()) {
		alDeleteSources(1, &iter->second.source);
		++iter;
	}
	for (size_t i = 0; i < m_vecBuffer.size(); i++) {
		if (m_vecBuffer[i].valid) {
			alDeleteBuffers(1, &m_vecBuffer[i].buffer);
		}
	}
	ExitOpenAL();
	delete this;
}

/**
* 从文件创建音源
* @param file 音频文件路径
* @return SoundId
*/
int CSoundManager::Create(const string& file) {
	// 生成一个音源
	SSoundSource sourceItem;
	sourceItem.soundId = 0;
	sourceItem.bufferCount = 0;
	alGenSources(1, &sourceItem.source);
	// 检查是否已经载入相同的声音资源
	for (size_t i = 0; i < m_vecBuffer.size(); i++) {
		if (m_vecBuffer[i].file == file) {
			sourceItem.bufferIndex[0] = i;
			sourceItem.bufferCount = 1;
			break;
		}
	}
	// 载入声音资源
	if (sourceItem.bufferCount == 0) {
		SSoundBuffer bufferItem;
		bufferItem.file = file;
		bufferItem.valid = false;
		CFileManager::CAudioFile* pFile = ReadAudio(file);
		if (pFile) {
			bufferItem.valid = true;
			bufferItem.channels = pFile->channels;
			bufferItem.sampleBits = pFile->sampleBits;
			bufferItem.frequency = pFile->sampleRate;
			// 通道和采样位数
			ALenum format = AL_FORMAT_MONO8;
			if (pFile->channels == 1 && pFile->sampleBits == 8) format = AL_FORMAT_MONO8;
			else if (pFile->channels == 1 && pFile->sampleBits == 16) format = AL_FORMAT_MONO16;
			else if (pFile->channels == 2 && pFile->sampleBits == 8) format = AL_FORMAT_STEREO8;
			else if (pFile->channels == 2 && pFile->sampleBits == 16) format = AL_FORMAT_STEREO16;
			else bufferItem.valid = false;
			if (bufferItem.valid) {
				alGenBuffers(1, &bufferItem.buffer);
				alBufferData(bufferItem.buffer, format, pFile->contents, pFile->size, pFile->sampleRate);
			}
			delete pFile;
		}
		// 加入到列表
		sourceItem.bufferIndex[0] = m_vecBuffer.size();
		sourceItem.bufferCount = 1;
		m_vecBuffer.push_back(bufferItem);
		if (!bufferItem.valid) {
			CLog::Warn("Invalid sound file '%s'", file.c_str());
		}
	}
	// 判断是否为有效资源并设置衰减属性
	if (m_vecBuffer[sourceItem.bufferIndex[0]].valid) {
		alSourcei(sourceItem.source, AL_BUFFER, m_vecBuffer[sourceItem.bufferIndex[0]].buffer);
		alSourcef(sourceItem.source, AL_ROLLOFF_FACTOR, 0.5f);
	} else {
		sourceItem.bufferCount = 0;
	}
	// 生成一个 SoundId
	int maxSoundId = 0;
	map<int, SSoundSource>::iterator iter = m_mapSource.begin();
	while (iter != m_mapSource.end()) {
		if (maxSoundId < iter->second.soundId) maxSoundId = iter->second.soundId;
		++iter;
	}
	// 加入到 MAP 中
	sourceItem.soundId = maxSoundId + 1;
	m_mapSource.insert(std::pair<int, SSoundSource>(sourceItem.soundId, sourceItem));
	return sourceItem.soundId;
}

/**
* 从内存创建音源
* @param channel 通道数
* @param sampleRate 采样速率
* @param sampleBits 采样位数
* @param size 数据长度
* @param data 数据指针
* @return SoundID
*/
int CSoundManager::Create(int channel, int sampleRate, int sampleBits, int size, const void* data) {
	// 生成一个音源
	SSoundSource sourceItem;
	sourceItem.soundId = 0;
	sourceItem.bufferCount = 0;
	alGenSources(1, &sourceItem.source);
	// 缓冲区命名
	string file = "mem_";
	file.append(std::to_string(channel)).append("_");
	file.append(std::to_string(sampleRate)).append("_");
	file.append(std::to_string(sampleBits)).append("_");
	file.append(std::to_string(size)).append("_");
	file.append(CStringUtil::Guid());
	// 载入声音资源
	SSoundBuffer bufferItem;
	bufferItem.file = file;
	bufferItem.valid = true;
	bufferItem.channels = channel;
	bufferItem.sampleBits = sampleBits;
	bufferItem.frequency = sampleRate;
	// 通道和采样位数
	ALenum format = AL_FORMAT_MONO8;
	if (channel == 1 && sampleBits == 8) format = AL_FORMAT_MONO8;
	else if (channel == 1 && sampleBits == 16) format = AL_FORMAT_MONO16;
	else if (channel == 2 && sampleBits == 8) format = AL_FORMAT_STEREO8;
	else if (channel == 2 && sampleBits == 16) format = AL_FORMAT_STEREO16;
	else bufferItem.valid = false;
	if (bufferItem.valid) {
		alGenBuffers(1, &bufferItem.buffer);
		alBufferData(bufferItem.buffer, format, data, size, sampleRate);
	}
	// 加入到列表
	sourceItem.bufferIndex[0] = m_vecBuffer.size();
	sourceItem.bufferCount = 1;
	m_vecBuffer.push_back(bufferItem);
	// 判断是否为有效资源并设置衰减属性
	if (m_vecBuffer[sourceItem.bufferIndex[0]].valid) {
		alSourcei(sourceItem.source, AL_BUFFER, m_vecBuffer[sourceItem.bufferIndex[0]].buffer);
		alSourcef(sourceItem.source, AL_ROLLOFF_FACTOR, 0.5f);
	} else {
		sourceItem.bufferCount = 0;
	}
	// 生成一个 SoundID
	int maxSoundId = 0;
	map<int, SSoundSource>::iterator iter = m_mapSource.begin();
	while (iter != m_mapSource.end()) {
		if (maxSoundId < iter->second.soundId) maxSoundId = iter->second.soundId;
		++iter;
	}
	// 加入到 MAP 中
	sourceItem.soundId = maxSoundId + 1;
	m_mapSource.insert(std::pair<int, SSoundSource>(sourceItem.soundId, sourceItem));
	return sourceItem.soundId;
}

/**
* 创建音频流源
* @param channel 通道数
* @param sampleRate 采样速率
* @param sampleBits 采样位数
*/
int CSoundManager::Create(int channel, int sampleRate, int sampleBits) {
	// 生成一个音源
	SSoundSource sourceItem;
	sourceItem.soundId = 0;
	sourceItem.bufferCount = 2;
	alGenSources(1, &sourceItem.source);
	// 这里需要初始化队列数据
	ALenum format;
	const unsigned char silent[4] = { 0x00, 0x00, 0x00, 0x00 };
	switch (channel | sampleBits) {
	case 0x09: format = AL_FORMAT_MONO8; break;
	case 0x11: format = AL_FORMAT_MONO16; break;
	case 0x0A: format = AL_FORMAT_STEREO8; break;
	case 0x12: format = AL_FORMAT_STEREO16; break;
	default: format = AL_FORMAT_MONO8; break;
	}
	// 生成 Buffer 并放入队列
	for (int i = 0; i < sourceItem.bufferCount; i++) {
		SSoundBuffer bufferItem;
		bufferItem.file = "stream";
		bufferItem.valid = true;
		bufferItem.channels = channel;
		bufferItem.sampleBits = sampleBits;
		bufferItem.frequency = sampleRate;
		alGenBuffers(1, &bufferItem.buffer);
		sourceItem.bufferIndex[i] = m_vecBuffer.size();
		m_vecBuffer.push_back(bufferItem);
		alBufferData(bufferItem.buffer, format, silent, 4, sampleRate);
		alSourceQueueBuffers(sourceItem.source, 1, &bufferItem.buffer);
	}
	// 设置属性并启动播放
	alSourcei(sourceItem.source, AL_LOOPING, AL_FALSE);
	alSourcef(sourceItem.source, AL_ROLLOFF_FACTOR, 0.5f);
	alSourcePlay(sourceItem.source);
	// 生成一个 SoundID
	int maxSoundId = 0;
	map<int, SSoundSource>::iterator iter = m_mapSource.begin();
	while (iter != m_mapSource.end()) {
		if (maxSoundId < iter->second.soundId) maxSoundId = iter->second.soundId;
		++iter;
	}
	// 加入到 MAP 中
	sourceItem.soundId = maxSoundId + 1;
	m_mapSource.insert(std::pair<int, SSoundSource>(sourceItem.soundId, sourceItem));
	return sourceItem.soundId;
}

/**
* 获取音源数量
* @return 音源数量
*/
int CSoundManager::SoundCount() {
	return m_mapSource.size();
}

/**
* 检查音源是否有效
* @param soundId 音源ID
* @return 有效性
*/
bool CSoundManager::IsValid(int soundId) {
	map<int, SSoundSource>::iterator iter = m_mapSource.find(soundId);
	if (iter != m_mapSource.end() && iter->second.bufferCount > 0) {
		return m_vecBuffer[iter->second.bufferIndex[0]].valid;
	}
	return false;
}

/**
* 播放指定 ID 的音源
* @param soundId 音源ID
*/
void CSoundManager::Play(int soundId) {
	map<int, SSoundSource>::iterator iter = m_mapSource.find(soundId);
	if (iter != m_mapSource.end() && iter->second.bufferCount > 0) {
		alSourcePlay(iter->second.source);
	}
}

/**
* 暂停指定 ID 的音源
* @param soundId 音源ID
*/
void CSoundManager::Pause(int soundId) {
	map<int, SSoundSource>::iterator iter = m_mapSource.find(soundId);
	if (iter != m_mapSource.end() && iter->second.bufferCount > 0) {
		alSourcePause(iter->second.source);
	}
}

/**
* 对所有音源暂停或取消暂停
* @param pause 暂停或播放
*/
void CSoundManager::Pause(bool pause) {
	map<int, SSoundSource>::iterator iter = m_mapSource.begin();
	while (iter != m_mapSource.end()) {
		SSoundSource& sound = iter->second;
		if (pause) {
			if (IsPlaying(sound.soundId)) Pause(sound.soundId);
		} else {
			if (IsPaused(sound.soundId)) Play(sound.soundId);
		}
		++iter;
	}
}

/**
* 停止指定 ID 的音源
* @param soundId 音源ID
*/
void CSoundManager::Stop(int soundId) {
	map<int, SSoundSource>::iterator iter = m_mapSource.find(soundId);
	if (iter != m_mapSource.end() && iter->second.bufferCount > 0) {
		alSourceStop(iter->second.source);
	}
}

/**
* 销毁指定 ID 的音源
* @param soundId 音源ID
*/
void CSoundManager::Drop(int soundId) {
	map<int, SSoundSource>::iterator iter = m_mapSource.find(soundId);
	if (iter != m_mapSource.end()) {
		alDeleteSources(1, &iter->second.source);
		m_mapSource.erase(iter);
	}
}

/**
* 获取音量
* @return 当前音量(范围0-100)
*/
int CSoundManager::GetVolume() {
	float volume = 0.0f;
	alGetListenerf(AL_GAIN, &volume);
	return static_cast<int>(volume * 100);
}

/**
* 设置音量
* @param volume 音量(范围0-100)
*/
void CSoundManager::SetVolume(int volume) {
	if (volume >= 0 && volume <= 100) {
		alListenerf(AL_GAIN, volume * 0.01f);
	}
}

/**
* 更新音频数据
* @param original 原始文件
* @param file 新音频文件
*/
bool CSoundManager::Update(const string& original, const string& file) {
	size_t select = 0;
	CFileManager::CAudioFile* pFile = 0;
	for (size_t i = 0; i < m_vecBuffer.size(); i++) {
		if (m_vecBuffer[i].file == original) {
			select = i;
			pFile = ReadAudio(file);
			break;
		}
	}
	if (!pFile) return false;
	// 音频格式
	ALenum format = AL_FORMAT_MONO8;
	if (pFile->channels == 1 && pFile->sampleBits == 8) format = AL_FORMAT_MONO8;
	else if (pFile->channels == 1 && pFile->sampleBits == 16) format = AL_FORMAT_MONO16;
	else if (pFile->channels == 2 && pFile->sampleBits == 8) format = AL_FORMAT_STEREO8;
	else if (pFile->channels == 2 && pFile->sampleBits == 16) format = AL_FORMAT_STEREO16;
	else {
		delete pFile;
		return false;
	}
	if (!m_vecBuffer[select].valid) {
		m_vecBuffer[select].valid = true;
		alGenBuffers(1, &m_vecBuffer[select].buffer);
	}
	m_vecBuffer[select].file = file;
	m_vecBuffer[select].channels = pFile->channels;
	m_vecBuffer[select].sampleBits = pFile->sampleBits;
	m_vecBuffer[select].frequency = pFile->sampleRate;
	alBufferData(m_vecBuffer[select].buffer, format, pFile->contents, pFile->size, pFile->sampleRate);
	delete pFile;
	return true;
}

/**
* 更新音源，仅对音频流有效，对文件创建的音频无效
* @param soundId 音源ID
* @param data 音频数据
* @param size 数据长度
* @return 是否成功加入到缓冲队列
* @attention 当方法返回 false 时，需要等待缓冲处理完再次调用，
*	需要避免数据丢失导致的声音不连续
*/
bool CSoundManager::Update(int soundId, void* data, int size) {
	map<int, SSoundSource>::iterator iter = m_mapSource.find(soundId);
	if (iter != m_mapSource.end() && iter->second.bufferCount > 1) {
		int processed;
		unsigned int source = iter->second.source;
		alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
		if (processed > 0) {
			// 获取 Buffer 属性
			ALenum format;
			SSoundBuffer& sourceBuffer = m_vecBuffer[iter->second.bufferIndex[0]];
			switch (sourceBuffer.channels | sourceBuffer.sampleBits) {
			case 0x09: format = AL_FORMAT_MONO8; break;
			case 0x11: format = AL_FORMAT_MONO16; break;
			case 0x0A: format = AL_FORMAT_STEREO8; break;
			case 0x12: format = AL_FORMAT_STEREO16; break;
			default: format = AL_FORMAT_MONO8; break;
			}
			// 更新缓冲区
			ALuint buffer;
			alSourceUnqueueBuffers(source, 1, &buffer);
			alBufferData(buffer, format, data, size, sourceBuffer.frequency);
			alSourceQueueBuffers(source, 1, &buffer);
			// 若已停止播放则需要启动播放
			ALint playing;
			alGetSourcei(source, AL_SOURCE_STATE, &playing);
			if (playing != AL_PLAYING) alSourcePlay(source);
			return true;
		}
	}
	return false;
}

/**
* 检查指定的音源是否播放
* @param soundId 音源ID
* @return 播放则返回 true
*/
bool CSoundManager::IsPlaying(int soundId) {
	map<int, SSoundSource>::iterator iter = m_mapSource.find(soundId);
	if (iter != m_mapSource.end() && iter->second.bufferCount > 0) {
		ALint state;
		alGetSourcei(iter->second.source, AL_SOURCE_STATE, &state);
		return state == AL_PLAYING;
	}
	return false;
}

/**
* 检查指定的音源是否暂停
* @param soundId 音源ID
* @return 暂停则返回 true
*/
bool CSoundManager::IsPaused(int soundId) {
	map<int, SSoundSource>::iterator iter = m_mapSource.find(soundId);
	if (iter != m_mapSource.end() && iter->second.bufferCount > 0) {
		ALint state;
		alGetSourcei(iter->second.source, AL_SOURCE_STATE, &state);
		return state == AL_PAUSED;
	}
	return false;
}

/**
* 检查指定的音源是否停止
* @param soundId 音源ID
* @return 停止则返回 true
*/
bool CSoundManager::IsStopped(int soundId) {
	map<int, SSoundSource>::iterator iter = m_mapSource.find(soundId);
	if (iter != m_mapSource.end() && iter->second.bufferCount > 0) {
		ALint state;
		alGetSourcei(iter->second.source, AL_SOURCE_STATE, &state);
		return state == AL_STOPPED;
	}
	return true;
}

/**
* 设置音源增益，循环，音调属性
* @param soundId 音源ID
* @param loop 是否循环
* @param gain 增益大小
* @param pitch 音调
* @return 设置是否成功
*/
bool CSoundManager::SetAttrib(int soundId, bool loop, float gain, float pitch) {
	map<int, SSoundSource>::iterator iter = m_mapSource.find(soundId);
	if (iter != m_mapSource.end() && iter->second.bufferCount) {
		unsigned int source = iter->second.source;
		alSourcef (source, AL_PITCH, pitch);
		alSource3f(source, AL_VELOCITY, 0, 0, 0);
		alSourcei (source, AL_LOOPING, loop? AL_TRUE: AL_FALSE);
		alSourcef (source, AL_GAIN, gain);
		return alGetError() == AL_NO_ERROR;
	}
	return false;
}

/**
* 设置音源位置
* @param soundId 音源ID
* @param pos 位置
*/
void CSoundManager::SetPosition(int soundId, const CVector3& pos) {
	map<int, SSoundSource>::iterator iter = m_mapSource.find(soundId);
	if (iter != m_mapSource.end() && iter->second.bufferCount) {
		alSourcefv(iter->second.source, AL_POSITION, pos.m_fValue);
	}
}

/**
* 设置音源为背景音乐，跟随听众位置改变
* @param soundId 音源ID
*/
void CSoundManager::SetBackground(int soundId) {
	map<int, SSoundSource>::iterator iter = m_mapSource.find(soundId);
	if (iter != m_mapSource.end() && iter->second.bufferCount) {
		alSourcei(iter->second.source, AL_SOURCE_RELATIVE, AL_TRUE);
		float listenerPos[3];
		alGetListenerfv(AL_POSITION, listenerPos);
		alSourcefv(iter->second.source, AL_POSITION, listenerPos);
	}
}

/**
* 设置听众位置
* @param pos 位置
*/
void CSoundManager::ListenerPos(const CVector3& pos) {
	alListenerfv(AL_POSITION, pos.m_fValue);
}

/**
* 设置听众速度
* @param vel 速度
*/
void CSoundManager::ListenerVel(const CVector3& vel) {
	alListenerfv(AL_VELOCITY, vel.m_fValue);
}

/**
* 设置听众方位
* @param to 朝向
* @param up 向上向量
*/
void CSoundManager::ListenerOri(const CVector3& to, const CVector3& up) {
	const float* t = to.m_fValue;
	const float* u = up.m_fValue;
	float orient[6] = { t[0], t[1], t[2], u[0], u[1], u[2] };
	alListenerfv(AL_ORIENTATION, orient);
}

/**
* 设置听众方位
* @param yaw 方位角
* @param pitch 俯仰角
* @param roll 翻滚角
*/
void CSoundManager::ListenerOri(float yaw, float pitch, float roll) {
	float sinx = sinf(yaw);
	float cosx = cosf(yaw);
	float siny = sinf(pitch);
	float cosy = cosf(pitch);
	float sinz = sinf(roll);
	float cosz = cosf(roll);
	float tox = -cosy * sinx;
	float toy = cosy * cosx;
	float toz = siny;
	float upx = sinz * cosx + cosz * siny * sinx;
	float upy = sinz * sinx - cosz * siny * cosx;
	float upz = cosz * cosy;
	float orient[6] = { tox, toy, toz, upx, upy, upz };
	alListenerfv(AL_ORIENTATION, orient);
}

/**
* 根据指定的参数开始录音
* @param sampleBits 采样位数
* @param frequency 采样频率
* @param bufferSize 缓冲区大小
* @return 录音开启成功
*/
bool CSoundManager::StartRecord(int sampleBits, int frequency, int bufferSize) {
	// 获取默认的音频采集器
	const ALCchar* szDefaultCaptureDevice = alcGetString(0, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER);
	// 只支持 8位或者 16位单声道编码方式
	ALenum format;
	if (sampleBits == 8) format = AL_FORMAT_MONO8;
	else if (sampleBits == 16) format = AL_FORMAT_MONO16;
	else return false;
	// 最大缓冲区大小限制在 10240 B，且为偶数
	bufferSize &= 0xFFFFFFFE;
	if (bufferSize > 10240) bufferSize = 10240;
	m_pCaptureDevice = alcCaptureOpenDevice(szDefaultCaptureDevice, frequency, format, bufferSize);
	if (m_pCaptureDevice) {
		m_iCaptureSampleBits = sampleBits;
		alcCaptureStart(m_pCaptureDevice);
		return true;
	}
	CLog::Error("Start sound record failed");
	return false;
}

/**
* 获取录音数据
* @param buffer 数据缓冲区
* @param bufferSize 缓冲区大小
* @return 实际读取到的数据大小
*/
int CSoundManager::GetRecordData(unsigned char* buffer, int bufferSize) {
	// 检测有多少样本被采集
	ALint samplesAvailable;
	alcGetIntegerv(m_pCaptureDevice, ALC_CAPTURE_SAMPLES, 1, &samplesAvailable);
	int bytePerSample = m_iCaptureSampleBits >> 3;
	int sampleCount = bufferSize / bytePerSample;
	if (samplesAvailable > sampleCount) samplesAvailable = sampleCount;
	if (samplesAvailable > 0) {
		alcCaptureSamples(m_pCaptureDevice, (ALchar*)buffer, samplesAvailable);
		return samplesAvailable * bytePerSample;
	}
	return 0;
}

/**
* 停止录音
*/
void CSoundManager::StopRecord() {
	if (m_pCaptureDevice) {
		alcCaptureStop(m_pCaptureDevice);
		alcCaptureCloseDevice(m_pCaptureDevice);
		m_pCaptureDevice = 0;
	}
}

/**
* 获取管理的所有音频文件列表
* @param soundList 输出音频文件列表
*/
void CSoundManager::GetSoundList(vector<string>& soundList) {
	soundList.resize(m_vecBuffer.size());
	vector<SSoundBuffer>::iterator iter = m_vecBuffer.begin();
	int index = 0;
	while (iter != m_vecBuffer.end()) {
		soundList[index++] = iter->file;
		++iter;
	}
}

/**
* 读取音频文件
* @param file 文件路径
* @return 音频对象指针
*/
CFileManager::CAudioFile* CSoundManager::ReadAudio(const string& file) {
	// 目前支持 WAV、MP3 两种格式
	CFileManager::CAudioFile* pFile = 0;
	string ext = CStringUtil::UpperCase(CFileManager::GetExtension(file));
	if (ext == "WAV") pFile = new CFileManager::CAudioFile(CFileManager::WAV);
	else if (ext == "MP3") pFile = new CFileManager::CAudioFile(CFileManager::MP3);
	else {
		CLog::Warn("Audio file type is not supported '%s'", file.c_str());
		return 0;
	}
	// 读取文件
	if (!CEngine::GetFileManager()->ReadFile(file, pFile)) {
		CLog::Warn("Read audio file error '%s'", file.c_str());
		delete pFile;
		return 0;
	}
	return pFile;
}

/**
* 初始化 OpenAL 实例
* @param device 设备名称，默认NULL
* @return 成功标志
*/
bool CSoundManager::InitOpenAL(const char* device) {
	ALCdevice* pDevice = alcOpenDevice((ALCchar*)device);
	ALCcontext* pContext = alcCreateContext(pDevice, NULL);
    alcMakeContextCurrent(pContext);
    if (alcGetError(pDevice) != ALC_NO_ERROR) return false;
	alDistanceModel(AL_EXPONENT_DISTANCE_CLAMPED);
    return true;
}

/**
* 销毁 OpenAL 实例
*/
void CSoundManager::ExitOpenAL() {
	ALCcontext* pContext = alcGetCurrentContext();
	ALCdevice* pDevice = alcGetContextsDevice(pContext);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(pContext);
    alcCloseDevice(pDevice);
}