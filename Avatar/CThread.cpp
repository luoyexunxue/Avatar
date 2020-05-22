//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CThread.h"
#include "CTimer.h"

/**
* Ĭ�Ϲ��캯��
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
* ʹ���̷߳�����ʼ��
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
* Ĭ����������
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
* �����߳�
*/
void CThread::Start() {
	Start(0);
}

/**
* �����̲߳��ƶ�����
*/
void CThread::Start(void* param) {
	Start(m_pRunnable, param);
}

/**
* �����ƶ��̲߳����ݲ���
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
* ����ֹͣ�ź�
*/
void CThread::Stop() {
	m_bRunning = false;
}

/**
* �ȴ��߳��˳�
*/
void CThread::Join() {
#ifdef AVATAR_WINDOWS
	WaitForSingleObject(m_pThread, INFINITE);
#else
	pthread_join(m_pThread, 0);
#endif
}

/**
* �ӻ�����
*/
void CThread::Lock() {
#ifdef AVATAR_WINDOWS
	EnterCriticalSection(&m_sMutex);
#else
	pthread_mutex_lock(&m_sMutex);
#endif
}

/**
* ���Լ���
*/
bool CThread::TryLock() {
#ifdef AVATAR_WINDOWS
	return TryEnterCriticalSection(&m_sMutex) != FALSE;
#else
	return pthread_mutex_trylock(&m_sMutex) == 0;
#endif
}

/**
* �������
*/
void CThread::Unlock() {
#ifdef AVATAR_WINDOWS
	LeaveCriticalSection(&m_sMutex);
#else
	pthread_mutex_unlock(&m_sMutex);
#endif
}

/**
* �����߳�
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