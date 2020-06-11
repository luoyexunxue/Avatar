//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CThreadPool.h"
#include "CTimer.h"

/**
* 默认构造函数
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
* 默认析构函数
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
	// 删除未执行的任务
	while (!m_queTask.empty()) {
		STask task = m_queTask.front();
		delete task.task;
		m_queTask.pop();
	}
}

/**
* 新增任务
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
* 获取总的线程数量
*/
int CThreadPool::GetThreadCount() {
	return m_vecThread.size();
}

/**
* 获取未处理的任务数量
*/
int CThreadPool::GetTaskCount() {
	return m_queTask.size();
}

/**
* 工作线程
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