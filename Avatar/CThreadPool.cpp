//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CThreadPool.h"
#include "CTimer.h"

/**
* Ĭ�Ϲ��캯��
*/
CThreadPool::CThreadPool(int size) {
	m_bRunning = true;
	m_vecThread.resize(size);
#ifdef AVATAR_WINDOWS
	InitializeCriticalSection(&m_sMutex);
	for (size_t i = 0; i < m_vecThread.size(); i++) {
		m_vecThread[i] = CreateThread(0, 0, TaskThread, this, 0, 0);
	}
#else
	pthread_mutex_init(&m_sMutex, 0);
	for (size_t i = 0; i < m_vecThread.size(); i++) {
		pthread_create(&m_vecThread[i], 0, TaskThread, this);
	}
#endif
}

/**
* Ĭ����������
*/
CThreadPool::~CThreadPool() {
	m_bRunning = false;
#ifdef AVATAR_WINDOWS
	for (size_t i = 0; i < m_vecThread.size(); i++) {
		WaitForSingleObject(m_vecThread[i], INFINITE);
	}
	DeleteCriticalSection(&m_sMutex);
#else
	for (size_t i = 0; i < m_vecThread.size(); i++) {
		pthread_join(m_vecThread[i], 0);
	}
	pthread_mutex_destroy(&m_sMutex);
#endif
	m_vecThread.clear();
	// ɾ��δִ�е�����
	while (!m_queTask.empty()) {
		STask task = m_queTask.front();
		delete task.task;
		m_queTask.pop();
	}
}

/**
* ��������
*/
void CThreadPool::AddTask(CThreadTask* task, void* param) {
	STask item;
	item.task = task;
	item.parameter = param;
#ifdef AVATAR_WINDOWS
	EnterCriticalSection(&m_sMutex);
	m_queTask.push(item);
	LeaveCriticalSection(&m_sMutex);
#else
	pthread_mutex_lock(&m_sMutex);
	m_queTask.push(item);
	pthread_mutex_unlock(&m_sMutex);
#endif
}

/**
* ��ȡ�ܵ��߳�����
*/
int CThreadPool::GetThreadCount() {
	return m_vecThread.size();
}

/**
* ��ȡδ�������������
*/
int CThreadPool::GetTaskCount() {
	return m_queTask.size();
}

/**
* �����߳�
*/
#ifdef AVATAR_WINDOWS
DWORD WINAPI CThreadPool::TaskThread(LPVOID param) {
#else
void* CThreadPool::TaskThread(void* param) {
#endif
	CThreadPool* threadPool = (CThreadPool*)param;
	while (threadPool->IsRunning()) {
		STask item;
#ifdef AVATAR_WINDOWS
		EnterCriticalSection(&threadPool->m_sMutex);
#else
		pthread_mutex_lock(&threadPool->m_sMutex);
#endif
		if (!threadPool->m_queTask.empty()) {
			item = threadPool->m_queTask.front();
			threadPool->m_queTask.pop();
		}
#ifdef AVATAR_WINDOWS
		LeaveCriticalSection(&threadPool->m_sMutex);
#else
		pthread_mutex_unlock(&threadPool->m_sMutex);
#endif
		if (item.task) {
			item.task->Run(threadPool, item.parameter);
			delete item.task;
		} else CTimer::Sleep(10);
	}
	return 0;
}