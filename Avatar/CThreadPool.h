//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CTHREADPOOL_H_
#define _CTHREADPOOL_H_
#include "AvatarConfig.h"
#ifdef AVATAR_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#include <pthread.h>
#endif
#include <vector>
#include <queue>
using std::vector;
using std::queue;

/**
* @brief �߳�����
*/
class CThreadTask {
public:
	//! ����������
	virtual ~CThreadTask() {}
	//! ִ���������
	virtual void Run(class CThreadPool* owner, void* param) = 0;
};

/**
* @brief �̳߳�
*/
class AVATAR_EXPORT CThreadPool {
public:
	//! Ĭ�Ϲ��캯��
	CThreadPool(int size);
	//! Ĭ����������
	~CThreadPool();

	//! �Ƿ���������
	bool IsRunning() { return m_bRunning; }
	//! ��������
	void AddTask(CThreadTask* task, void* param);

	//! ��ȡ�ܵ��߳�����
	int GetThreadCount();
	//! ��ȡδ�������������
	int GetTaskCount();

private:
	//! �����߳�
#ifdef AVATAR_WINDOWS
	static DWORD WINAPI TaskThread(LPVOID param);
#else
	static void* TaskThread(void* param);
#endif
	//! �����������
	typedef struct _STask {
		CThreadTask* task;
		void* parameter;
		_STask(): task(0), parameter(0) {}
	} STask;

private:
	//! �̳߳�����״̬
	bool m_bRunning;
	//! �����̶߳���
	queue<STask> m_queTask;
#ifdef AVATAR_WINDOWS
	//! �߳��б�
	vector<HANDLE> m_vecThread;
	//! ������л�����
	CRITICAL_SECTION m_sMutex;
#else
	//! �߳��б�
	vector<pthread_t> m_vecThread;
	//! ������л�����
	pthread_mutex_t m_sMutex;
#endif
};

#endif