//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
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
* 线程方法
*/
typedef void(*Runnable)(class CThread*, void*);

/**
* @brief 线程类
*/
class AVATAR_EXPORT CThread {
public:
	//! 默认构造函数
	CThread();
	//! 使用线程方法初始化
	CThread(Runnable func);
	//! 默认析构函数
	~CThread();

	//! 启动线程
	void Start();
	//! 启动线程并制定参数
	void Start(void* param);
	//! 启动制定线程并传递参数
	void Start(Runnable func, void* param);
	//! 发送停止信号
	void Stop();
	//! 等待线程退出
	void Join();
	//! 线程是否运行
	bool IsRunning() { return m_bRunning; }

	//! 加互斥锁
	void Lock();
	//! 尝试锁定
	bool TryLock();
	//! 互斥解锁
	void Unlock();

private:
	//! 线程运行状态
	bool m_bRunning;
	//! 线程方法
	Runnable m_pRunnable;
	//! 线程参数
	void* m_pParameter;
#ifdef AVATAR_WINDOWS
	//! 线程句柄
	HANDLE m_pThread;
	//! 互斥锁
	CRITICAL_SECTION m_sMutex;
	//! 工作线程
	static DWORD WINAPI Run(LPVOID param);
#else
	//! 线程句柄
	pthread_t m_pThread;
	//! 互斥锁
	pthread_mutex_t m_sMutex;
	//! 工作线程
	static void* Run(void* param);
#endif
};

#endif