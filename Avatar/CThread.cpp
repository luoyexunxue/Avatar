//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CThread.h"
#include "CTimer.h"

/**
* 默认构造函数
*/
CThread::CThread() {
	m_bRunning = false;
	m_pRunnable = 0;
	m_pParameter = 0;
	m_pThread = 0;
#ifdef AVATAR_WINDOWS
	InitializeCriticalSection(&m_sMutex);
#else
	pthread_mutex_init(&m_sMutex, 0);
#endif
}

/**
* 使用线程方法初始化
*/
CThread::CThread(Runnable func) {
	m_bRunning = false;
	m_pRunnable = func;
	m_pParameter = 0;
	m_pThread = 0;
#ifdef AVATAR_WINDOWS
	InitializeCriticalSection(&m_sMutex);
#else
	pthread_mutex_init(&m_sMutex, 0);
#endif
}

/**
* 默认析构函数
*/
CThread::~CThread() {
	m_bRunning = false;
#ifdef AVATAR_WINDOWS
	WaitForSingleObject(m_pThread, INFINITE);
	DeleteCriticalSection(&m_sMutex);
#else
	pthread_join(m_pThread, 0);
	pthread_mutex_destroy(&m_sMutex);
#endif
}

/**
* 启动线程
*/
void CThread::Start() {
	Start(0);
}

/**
* 启动线程并制定参数
*/
void CThread::Start(void* param) {
	Start(m_pRunnable, param);
}

/**
* 启动制定线程并传递参数
*/
void CThread::Start(Runnable func, void* param) {
	if (m_bRunning) return;
	m_pRunnable = func;
	m_pParameter = param;
	m_bRunning = true;
#ifdef AVATAR_WINDOWS
	m_pThread = CreateThread(0, 0, Run, this, 0, 0);
#else
	pthread_create(&m_pThread, 0, Run, this);
#endif
}

/**
* 发送停止信号
*/
void CThread::Stop() {
	m_bRunning = false;
}

/**
* 等待线程退出
*/
void CThread::Join() {
#ifdef AVATAR_WINDOWS
	WaitForSingleObject(m_pThread, INFINITE);
#else
	pthread_join(m_pThread, 0);
#endif
}

/**
* 加互斥锁
*/
void CThread::Lock() {
#ifdef AVATAR_WINDOWS
	EnterCriticalSection(&m_sMutex);
#else
	pthread_mutex_lock(&m_sMutex);
#endif
}

/**
* 尝试加锁
*/
bool CThread::TryLock() {
#ifdef AVATAR_WINDOWS
	return TryEnterCriticalSection(&m_sMutex) != FALSE;
#else
	return pthread_mutex_trylock(&m_sMutex) == 0;
#endif
}

/**
* 互斥解锁
*/
void CThread::Unlock() {
#ifdef AVATAR_WINDOWS
	LeaveCriticalSection(&m_sMutex);
#else
	pthread_mutex_unlock(&m_sMutex);
#endif
}

/**
* 工作线程
*/
#ifdef AVATAR_WINDOWS
DWORD WINAPI CThread::Run(LPVOID param) {
#else
void* CThread::Run(void* param) {
#endif
	CThread* thread = (CThread*)param;
	thread->m_pRunnable(thread, thread->m_pParameter);
	return 0;
}