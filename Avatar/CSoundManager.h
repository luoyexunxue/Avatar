//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSOUNDMANAGER_H_
#define _CSOUNDMANAGER_H_
#include "AvatarConfig.h"
#include "CFileManager.h"
#include "CVector3.h"
#include <string>
#include <vector>
#include <map>
using std::string;
using std::vector;
using std::map;
typedef struct ALCdevice_struct ALCdevice;

/**
* @brief 三维音效管理器
*
* 创建音源时，应该使用单声道音源，不然无 3D 效果
*/
class AVATAR_EXPORT CSoundManager {
public:
	//! 获取管理器实例
	static CSoundManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CSoundManager();
		return m_pInstance;
	}
	//! 销毁声音管理器
	void Destroy();

	//! 从文件创建音源
	int Create(const string& file);
	//! 从内存创建音源
	int Create(int channel, int sampleRate, int sampleBits, int size, const void* data);
	//! 创建指定属性的音源
	int Create(int channel, int sampleRate, int sampleBits);
	//! 获取音源数量
	int SoundCount();

	//! 检查音源是否有效
	bool IsValid(int soundId);

	//! 播放指定音源
	void Play(int soundId);
	//! 暂停音源播放
	void Pause(int soundId);
	//! 暂停所有正在播放的声音
	void Pause(bool pause);
	//! 停止音源播放
	void Stop(int soundId);
	//! 方法只删除音源，不清空缓冲区
	void Drop(int soundId);
	//! 获取音量
	int GetVolume();
	//! 设置音量
	void SetVolume(int volume);

	//! 更新音频数据
	bool Update(const string& original, const string& file);
	//! 更新缓冲数据，只对音频流有效
	bool Update(int soundId, void* data, int size);

	//! 判断指定音源是否正在播放
	bool IsPlaying(int soundId);
	//! 判断指定音源是否已经暂停
	bool IsPaused(int soundId);
	//! 判断指定音源是否已经停止
	bool IsStopped(int soundId);

	//! 音源属性设置
	bool SetAttrib(int soundId, bool loop = false, float gain = 1.0f, float pitch = 1.0f);
	//! 设置音源位置
	void SetPosition(int soundId, const CVector3& pos);
	//! 设置为背景音源
	void SetBackground(int soundId);

	//! 设置听众位置
	void ListenerPos(const CVector3& pos);
	//! 设置听众速度
	void ListenerVel(const CVector3& vel);
	//! 设置听众方向
	void ListenerOri(const CVector3& to, const CVector3& up);
	//! 设置听众方向角
	void ListenerOri(float yaw, float pitch, float roll);

	//! 启动录音
	bool StartRecord(int sampleBits, int frequency, int bufferSize);
	//! 获取录音数据
	int GetRecordData(unsigned char* buffer, int bufferSize);
	//! 停止录音
	void StopRecord();

	//! 获取管理的所有音频文件列表
	void GetSoundList(vector<string>& soundList);

public:
	//! 读取音频
	static CFileManager::CAudioFile* ReadAudio(const string& file);

private:
	//! 声音资源缓冲
	typedef struct _SSoundBuffer {
		string file;
		bool valid;
		int channels;
		int sampleBits;
		int frequency;
		unsigned int buffer;
	} SSoundBuffer;
	//! 音源结构体，每个音源最多可以附加两个缓冲区用于音频流
	typedef struct _SSoundSource {
		int soundId;
		int bufferCount;
		int bufferIndex[2];
		unsigned int source;
	} SSoundSource;

private:
	CSoundManager();
	~CSoundManager();

	//! 初始化 OpenAL
	bool InitOpenAL(const char* device);
	//! 销毁 OpenAL
	void ExitOpenAL();

private:
	//! 音频缓冲列表
	vector<SSoundBuffer> m_vecBuffer;
	//! 音源列表
	map<int, SSoundSource> m_mapSource;
	//! 录音设备
	ALCdevice* m_pCaptureDevice;
	//! 录音采样位数
	int m_iCaptureSampleBits;
	//! 实例
	static CSoundManager* m_pInstance;
};

#endif