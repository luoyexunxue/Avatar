//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* ���캯��
*/
CSoundManager::CSoundManager() {
	m_pCaptureDevice = 0;
	m_iCaptureSampleBits = 16;
	InitOpenAL(0);
}

/**
* ��������
*/
CSoundManager::~CSoundManager() {
	m_pInstance = 0;
}

/**
* ����ʵ��
*/
CSoundManager* CSoundManager::m_pInstance = 0;

/**
* ��������������
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
* ���ļ�������Դ
* @param file ��Ƶ�ļ�·��
* @return SoundId
*/
int CSoundManager::Create(const string& file) {
	// ����һ����Դ
	SSoundSource sourceItem;
	sourceItem.soundId = 0;
	sourceItem.bufferCount = 0;
	alGenSources(1, &sourceItem.source);
	// ����Ƿ��Ѿ�������ͬ��������Դ
	for (size_t i = 0; i < m_vecBuffer.size(); i++) {
		if (m_vecBuffer[i].file == file) {
			sourceItem.bufferIndex[0] = i;
			sourceItem.bufferCount = 1;
			break;
		}
	}
	// ����������Դ
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
			// ͨ���Ͳ���λ��
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
		// ���뵽�б�
		sourceItem.bufferIndex[0] = m_vecBuffer.size();
		sourceItem.bufferCount = 1;
		m_vecBuffer.push_back(bufferItem);
		if (!bufferItem.valid) {
			CLog::Warn("Invalid sound file '%s'", file.c_str());
		}
	}
	// �ж��Ƿ�Ϊ��Ч��Դ������˥������
	if (m_vecBuffer[sourceItem.bufferIndex[0]].valid) {
		alSourcei(sourceItem.source, AL_BUFFER, m_vecBuffer[sourceItem.bufferIndex[0]].buffer);
		alSourcef(sourceItem.source, AL_ROLLOFF_FACTOR, 0.5f);
	} else {
		sourceItem.bufferCount = 0;
	}
	// ����һ�� SoundId
	int maxSoundId = 0;
	map<int, SSoundSource>::iterator iter = m_mapSource.begin();
	while (iter != m_mapSource.end()) {
		if (maxSoundId < iter->second.soundId) maxSoundId = iter->second.soundId;
		++iter;
	}
	// ���뵽 MAP ��
	sourceItem.soundId = maxSoundId + 1;
	m_mapSource.insert(std::pair<int, SSoundSource>(sourceItem.soundId, sourceItem));
	return sourceItem.soundId;
}

/**
* ���ڴ洴����Դ
* @param channel ͨ����
* @param sampleRate ��������
* @param sampleBits ����λ��
* @param size ���ݳ���
* @param data ����ָ��
* @return SoundID
*/
int CSoundManager::Create(int channel, int sampleRate, int sampleBits, int size, const void* data) {
	// ����һ����Դ
	SSoundSource sourceItem;
	sourceItem.soundId = 0;
	sourceItem.bufferCount = 0;
	alGenSources(1, &sourceItem.source);
	// ����������
	string file = "mem_";
	file.append(std::to_string(channel)).append("_");
	file.append(std::to_string(sampleRate)).append("_");
	file.append(std::to_string(sampleBits)).append("_");
	file.append(std::to_string(size)).append("_");
	file.append(CStringUtil::Guid());
	// ����������Դ
	SSoundBuffer bufferItem;
	bufferItem.file = file;
	bufferItem.valid = true;
	bufferItem.channels = channel;
	bufferItem.sampleBits = sampleBits;
	bufferItem.frequency = sampleRate;
	// ͨ���Ͳ���λ��
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
	// ���뵽�б�
	sourceItem.bufferIndex[0] = m_vecBuffer.size();
	sourceItem.bufferCount = 1;
	m_vecBuffer.push_back(bufferItem);
	// �ж��Ƿ�Ϊ��Ч��Դ������˥������
	if (m_vecBuffer[sourceItem.bufferIndex[0]].valid) {
		alSourcei(sourceItem.source, AL_BUFFER, m_vecBuffer[sourceItem.bufferIndex[0]].buffer);
		alSourcef(sourceItem.source, AL_ROLLOFF_FACTOR, 0.5f);
	} else {
		sourceItem.bufferCount = 0;
	}
	// ����һ�� SoundID
	int maxSoundId = 0;
	map<int, SSoundSource>::iterator iter = m_mapSource.begin();
	while (iter != m_mapSource.end()) {
		if (maxSoundId < iter->second.soundId) maxSoundId = iter->second.soundId;
		++iter;
	}
	// ���뵽 MAP ��
	sourceItem.soundId = maxSoundId + 1;
	m_mapSource.insert(std::pair<int, SSoundSource>(sourceItem.soundId, sourceItem));
	return sourceItem.soundId;
}

/**
* ������Ƶ��Դ
* @param channel ͨ����
* @param sampleRate ��������
* @param sampleBits ����λ��
*/
int CSoundManager::Create(int channel, int sampleRate, int sampleBits) {
	// ����һ����Դ
	SSoundSource sourceItem;
	sourceItem.soundId = 0;
	sourceItem.bufferCount = 2;
	alGenSources(1, &sourceItem.source);
	// ������Ҫ��ʼ����������
	ALenum format;
	const unsigned char silent[4] = { 0x00, 0x00, 0x00, 0x00 };
	switch (channel | sampleBits) {
	case 0x09: format = AL_FORMAT_MONO8; break;
	case 0x11: format = AL_FORMAT_MONO16; break;
	case 0x0A: format = AL_FORMAT_STEREO8; break;
	case 0x12: format = AL_FORMAT_STEREO16; break;
	default: format = AL_FORMAT_MONO8; break;
	}
	// ���� Buffer ���������
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
	// �������Բ���������
	alSourcei(sourceItem.source, AL_LOOPING, AL_FALSE);
	alSourcef(sourceItem.source, AL_ROLLOFF_FACTOR, 0.5f);
	alSourcePlay(sourceItem.source);
	// ����һ�� SoundID
	int maxSoundId = 0;
	map<int, SSoundSource>::iterator iter = m_mapSource.begin();
	while (iter != m_mapSource.end()) {
		if (maxSoundId < iter->second.soundId) maxSoundId = iter->second.soundId;
		++iter;
	}
	// ���뵽 MAP ��
	sourceItem.soundId = maxSoundId + 1;
	m_mapSource.insert(std::pair<int, SSoundSource>(sourceItem.soundId, sourceItem));
	return sourceItem.soundId;
}

/**
* ��ȡ��Դ����
* @return ��Դ����
*/
int CSoundManager::SoundCount() {
	return m_mapSource.size();
}

/**
* �����Դ�Ƿ���Ч
* @param soundId ��ԴID
* @return ��Ч��
*/
bool CSoundManager::IsValid(int soundId) {
	map<int, SSoundSource>::iterator iter = m_mapSource.find(soundId);
	if (iter != m_mapSource.end() && iter->second.bufferCount > 0) {
		return m_vecBuffer[iter->second.bufferIndex[0]].valid;
	}
	return false;
}

/**
* ����ָ�� ID ����Դ
* @param soundId ��ԴID
*/
void CSoundManager::Play(int soundId) {
	map<int, SSoundSource>::iterator iter = m_mapSource.find(soundId);
	if (iter != m_mapSource.end() && iter->second.bufferCount > 0) {
		alSourcePlay(iter->second.source);
	}
}

/**
* ��ָͣ�� ID ����Դ
* @param soundId ��ԴID
*/
void CSoundManager::Pause(int soundId) {
	map<int, SSoundSource>::iterator iter = m_mapSource.find(soundId);
	if (iter != m_mapSource.end() && iter->second.bufferCount > 0) {
		alSourcePause(iter->second.source);
	}
}

/**
* ��������Դ��ͣ��ȡ����ͣ
* @param pause ��ͣ�򲥷�
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
* ָֹͣ�� ID ����Դ
* @param soundId ��ԴID
*/
void CSoundManager::Stop(int soundId) {
	map<int, SSoundSource>::iterator iter = m_mapSource.find(soundId);
	if (iter != m_mapSource.end() && iter->second.bufferCount > 0) {
		alSourceStop(iter->second.source);
	}
}

/**
* ����ָ�� ID ����Դ
* @param soundId ��ԴID
*/
void CSoundManager::Drop(int soundId) {
	map<int, SSoundSource>::iterator iter = m_mapSource.find(soundId);
	if (iter != m_mapSource.end()) {
		alDeleteSources(1, &iter->second.source);
		m_mapSource.erase(iter);
	}
}

/**
* ��ȡ����
* @return ��ǰ����(��Χ0-100)
*/
int CSoundManager::GetVolume() {
	float volume = 0.0f;
	alGetListenerf(AL_GAIN, &volume);
	return static_cast<int>(volume * 100);
}

/**
* ��������
* @param volume ����(��Χ0-100)
*/
void CSoundManager::SetVolume(int volume) {
	if (volume >= 0 && volume <= 100) {
		alListenerf(AL_GAIN, volume * 0.01f);
	}
}

/**
* ������Ƶ����
* @param original ԭʼ�ļ�
* @param file ����Ƶ�ļ�
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
	// ��Ƶ��ʽ
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
* ������Դ��������Ƶ����Ч�����ļ���������Ƶ��Ч
* @param soundId ��ԴID
* @param data ��Ƶ����
* @param size ���ݳ���
* @return �Ƿ�ɹ����뵽�������
* @attention ���������� false ʱ����Ҫ�ȴ����崦�����ٴε��ã�
*	��Ҫ�������ݶ�ʧ���µ�����������
*/
bool CSoundManager::Update(int soundId, void* data, int size) {
	map<int, SSoundSource>::iterator iter = m_mapSource.find(soundId);
	if (iter != m_mapSource.end() && iter->second.bufferCount > 1) {
		int processed;
		unsigned int source = iter->second.source;
		alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
		if (processed > 0) {
			// ��ȡ Buffer ����
			ALenum format;
			SSoundBuffer& sourceBuffer = m_vecBuffer[iter->second.bufferIndex[0]];
			switch (sourceBuffer.channels | sourceBuffer.sampleBits) {
			case 0x09: format = AL_FORMAT_MONO8; break;
			case 0x11: format = AL_FORMAT_MONO16; break;
			case 0x0A: format = AL_FORMAT_STEREO8; break;
			case 0x12: format = AL_FORMAT_STEREO16; break;
			default: format = AL_FORMAT_MONO8; break;
			}
			// ���»�����
			ALuint buffer;
			alSourceUnqueueBuffers(source, 1, &buffer);
			alBufferData(buffer, format, data, size, sourceBuffer.frequency);
			alSourceQueueBuffers(source, 1, &buffer);
			// ����ֹͣ��������Ҫ��������
			ALint playing;
			alGetSourcei(source, AL_SOURCE_STATE, &playing);
			if (playing != AL_PLAYING) alSourcePlay(source);
			return true;
		}
	}
	return false;
}

/**
* ���ָ������Դ�Ƿ񲥷�
* @param soundId ��ԴID
* @return �����򷵻� true
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
* ���ָ������Դ�Ƿ���ͣ
* @param soundId ��ԴID
* @return ��ͣ�򷵻� true
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
* ���ָ������Դ�Ƿ�ֹͣ
* @param soundId ��ԴID
* @return ֹͣ�򷵻� true
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
* ������Դ���棬ѭ������������
* @param soundId ��ԴID
* @param loop �Ƿ�ѭ��
* @param gain �����С
* @param pitch ����
* @return �����Ƿ�ɹ�
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
* ������Դλ��
* @param soundId ��ԴID
* @param pos λ��
*/
void CSoundManager::SetPosition(int soundId, const CVector3& pos) {
	map<int, SSoundSource>::iterator iter = m_mapSource.find(soundId);
	if (iter != m_mapSource.end() && iter->second.bufferCount) {
		alSourcefv(iter->second.source, AL_POSITION, pos.m_fValue);
	}
}

/**
* ������ԴΪ�������֣���������λ�øı�
* @param soundId ��ԴID
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
* ��������λ��
* @param pos λ��
*/
void CSoundManager::ListenerPos(const CVector3& pos) {
	alListenerfv(AL_POSITION, pos.m_fValue);
}

/**
* ���������ٶ�
* @param vel �ٶ�
*/
void CSoundManager::ListenerVel(const CVector3& vel) {
	alListenerfv(AL_VELOCITY, vel.m_fValue);
}

/**
* �������ڷ�λ
* @param to ����
* @param up ��������
*/
void CSoundManager::ListenerOri(const CVector3& to, const CVector3& up) {
	const float* t = to.m_fValue;
	const float* u = up.m_fValue;
	float orient[6] = { t[0], t[1], t[2], u[0], u[1], u[2] };
	alListenerfv(AL_ORIENTATION, orient);
}

/**
* �������ڷ�λ
* @param yaw ��λ��
* @param pitch ������
* @param roll ������
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
* ����ָ���Ĳ�����ʼ¼��
* @param sampleBits ����λ��
* @param frequency ����Ƶ��
* @param bufferSize ��������С
* @return ¼�������ɹ�
* @note һ�� sampleBits=16, frequency=22050, bufferSize=4410
*/
bool CSoundManager::StartRecord(int sampleBits, int frequency, int bufferSize) {
	// ��ȡĬ�ϵ���Ƶ�ɼ���
	const ALCchar* szDefaultCaptureDevice = alcGetString(0, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER);
	// ֻ֧�� 8λ���� 16λ���������뷽ʽ
	ALenum format;
	if (sampleBits == 8) format = AL_FORMAT_MONO8;
	else if (sampleBits == 16) format = AL_FORMAT_MONO16;
	else return false;
	// ��󻺳�����С������ 10240 B����Ϊż��
	if (bufferSize > 10240) bufferSize = 10240;
	if (bufferSize % 2) bufferSize -= 1;

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
* ��ȡ¼�����ݣ�����ʵ�����ݴ�С
* @param data ���ݻ�����
* @param size ���ݻ�������С
* @return ʵ�ʷ������ݴ�С
*/
int CSoundManager::GetRecordData(unsigned char* data, int size) {
	// ����ж����������ɼ�
	ALint samplesAvailable;
	alcGetIntegerv(m_pCaptureDevice, ALC_CAPTURE_SAMPLES, 1, &samplesAvailable);
	int samplePerByte = m_iCaptureSampleBits >> 3;
	int maxSampleSize = size / samplePerByte;
	if (samplesAvailable > maxSampleSize) samplesAvailable = maxSampleSize;
	if (samplesAvailable > 0) {
		alcCaptureSamples(m_pCaptureDevice, (ALchar*)data, samplesAvailable);
		return samplesAvailable * samplePerByte;
	}
	return 0;
}

/**
* ֹͣ¼��
*/
void CSoundManager::StopRecord() {
	if (m_pCaptureDevice) {
		alcCaptureStop(m_pCaptureDevice);
		alcCaptureCloseDevice(m_pCaptureDevice);
		m_pCaptureDevice = 0;
	}
}

/**
* ��ȡ�����������Ƶ�ļ��б�
* @param soundList �����Ƶ�ļ��б�
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
* ��ȡ��Ƶ�ļ�
* @param file �ļ�·��
* @return ��Ƶ����ָ��
*/
CFileManager::CAudioFile* CSoundManager::ReadAudio(const string& file) {
	// Ŀǰ֧�� WAV��MP3 ���ָ�ʽ
	CFileManager::CAudioFile* pFile = 0;
	string ext = CStringUtil::UpperCase(CFileManager::GetExtension(file));
	if (ext == "WAV") pFile = new CFileManager::CAudioFile(CFileManager::WAV);
	else if (ext == "MP3") pFile = new CFileManager::CAudioFile(CFileManager::MP3);
	else {
		CLog::Warn("Audio file type is not supported '%s'", file.c_str());
		return 0;
	}
	// ��ȡ�ļ�
	if (!CEngine::GetFileManager()->ReadFile(file, pFile)) {
		CLog::Warn("Read audio file error '%s'", file.c_str());
		delete pFile;
		return 0;
	}
	return pFile;
}

/**
* ��ʼ�� OpenAL ʵ��
* @param device �豸���ƣ�Ĭ��NULL
* @return �ɹ���־
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
* ���� OpenAL ʵ��
*/
void CSoundManager::ExitOpenAL() {
	ALCcontext* pContext = alcGetCurrentContext();
	ALCdevice* pDevice = alcGetContextsDevice(pContext);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(pContext);
    alcCloseDevice(pDevice);
}