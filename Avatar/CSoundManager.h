//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* @brief ��ά��Ч������
*
* ������Դʱ��Ӧ��ʹ�õ�������Դ����Ȼ�� 3D Ч��
*/
class AVATAR_EXPORT CSoundManager {
public:
	//! ��ȡ������ʵ��
	static CSoundManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CSoundManager();
		return m_pInstance;
	}
	//! ��������������
	void Destroy();

	//! ���ļ�������Դ
	int Create(const string& file);
	//! ���ڴ洴����Դ
	int Create(int channel, int sampleRate, int sampleBits, int size, const void* data);
	//! ����ָ�����Ե���Դ
	int Create(int channel, int sampleRate, int sampleBits);
	//! ��ȡ��Դ����
	int SoundCount();

	//! �����Դ�Ƿ���Ч
	bool IsValid(int soundId);

	//! ����ָ����Դ
	void Play(int soundId);
	//! ��ͣ��Դ����
	void Pause(int soundId);
	//! ��ͣ�������ڲ��ŵ�����
	void Pause(bool pause);
	//! ֹͣ��Դ����
	void Stop(int soundId);
	//! ����ֻɾ����Դ������ջ�����
	void Drop(int soundId);
	//! ��ȡ����
	int GetVolume();
	//! ��������
	void SetVolume(int volume);

	//! ������Ƶ����
	bool Update(const string& original, const string& file);
	//! ���»������ݣ�ֻ����Ƶ����Ч
	bool Update(int soundId, void* data, int size);

	//! �ж�ָ����Դ�Ƿ����ڲ���
	bool IsPlaying(int soundId);
	//! �ж�ָ����Դ�Ƿ��Ѿ���ͣ
	bool IsPaused(int soundId);
	//! �ж�ָ����Դ�Ƿ��Ѿ�ֹͣ
	bool IsStopped(int soundId);

	//! ��Դ��������
	bool SetAttrib(int soundId, bool loop = false, float gain = 1.0f, float pitch = 1.0f);
	//! ������Դλ��
	void SetPosition(int soundId, const CVector3& pos);
	//! ����Ϊ������Դ
	void SetBackground(int soundId);

	//! ��������λ��
	void ListenerPos(const CVector3& pos);
	//! ���������ٶ�
	void ListenerVel(const CVector3& vel);
	//! �������ڷ���
	void ListenerOri(const CVector3& to, const CVector3& up);
	//! �������ڷ����
	void ListenerOri(float yaw, float pitch, float roll);

	//! ����¼��
	bool StartRecord(int sampleBits, int frequency, int bufferSize);
	//! ��ȡ¼������
	int GetRecordData(unsigned char* data, int size);
	//! ֹͣ¼��
	void StopRecord();

	//! ��ȡ�����������Ƶ�ļ��б�
	void GetSoundList(vector<string>& soundList);

public:
	//! ��ȡ��Ƶ
	static CFileManager::CAudioFile* ReadAudio(const string& file);

private:
	//! ������Դ����
	typedef struct _SSoundBuffer {
		string file;
		bool valid;
		int channels;
		int sampleBits;
		int frequency;
		unsigned int buffer;
	} SSoundBuffer;
	//! ��Դ�ṹ�壬ÿ����Դ�����Ը�������������������Ƶ��
	typedef struct _SSoundSource {
		int soundId;
		int bufferCount;
		int bufferIndex[2];
		unsigned int source;
	} SSoundSource;

private:
	CSoundManager();
	~CSoundManager();

	//! ��ʼ�� OpenAL
	bool InitOpenAL(const char* device);
	//! ���� OpenAL
	void ExitOpenAL();

private:
	//! ��Ƶ�����б�
	vector<SSoundBuffer> m_vecBuffer;
	//! ��Դ�б�
	map<int, SSoundSource> m_mapSource;
	//! ¼���豸
	ALCdevice* m_pCaptureDevice;
	//! ¼������λ��
	int m_iCaptureSampleBits;
	//! ʵ��
	static CSoundManager* m_pInstance;
};

#endif