//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CTHREAD_H_
#define _CTHREAD_H_
#include "AvatarConfig.h"
#ifdef AVATAR_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#include <pthread.h>
#endif

/**
* �̷߳���
*/
typedef void(*Runnable)(class CThread*, void*);

/**
* @brief �߳���
*/
class AVATAR_EXPORT CThread {
public:
	//! Ĭ�Ϲ��캯��
	CThread();
	//! ʹ���̷߳�����ʼ��
	CThread(Runnable func);
	//! Ĭ����������
	~CThread();

	//! �����߳�
	void Start();
	//! �����̲߳��ƶ�����
	void Start(void* param);
	//! �����ƶ��̲߳����ݲ���
	void Start(Runnable func, void* param);
	//! ����ֹͣ�ź�
	void Stop();
	//! �ȴ��߳��˳�
	void Join();
	//! �߳��Ƿ�����
	bool IsRunning() { return m_bRunning; }

	//! �ӻ�����
	void Lock();
	//! ��������
	bool TryLock();
	//! �������
	void Unlock();

private:
	//! �߳�����״̬
	bool m_bRunning;
	//! �̷߳���
	Runnable m_pRunnable;
	//! �̲߳���
	void* m_pParameter;
#ifdef AVATAR_WINDOWS
	//! �߳̾��
	HANDLE m_pThread;
	//! ������
	CRITICAL_SECTION m_sMutex;
	//! �����߳�
	static DWORD WINAPI Run(LPVOID param);
#else
	//! �߳̾��
	pthread_t m_pThread;
	//! ������
	pthread_mutex_t m_sMutex;
	//! �����߳�
	static void* Run(void* param);
#endif
};

#endif