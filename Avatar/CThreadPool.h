//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
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
* @brief 线程任务
*/
class CThreadTask {
public:
	//! 虚析构函数
	virtual ~CThreadTask() {}
	//! 执行任务入口
	virtual void Run(class CThreadPool* owner, void* param) = 0;
};

/**
* @brief 线程池
*/
class AVATAR_EXPORT CThreadPool {
public:
	//! 默认构造函数
	CThreadPool(int size);
	//! 默认析构函数
	~CThreadPool();

	//! 是否正在运行
	bool IsRunning() { return m_bRunning; }
	//! 新增任务
	void AddTask(CThreadTask* task, void* param);

	//! 获取总的线程数量
	int GetThreadCount();
	//! 获取未处理的任务数量
	int GetTaskCount();

private:
	//! 工作线程
#ifdef AVATAR_WINDOWS
	static DWORD WINAPI TaskThread(LPVOID param);
#else
	static void* TaskThread(void* param);
#endif
	//! 工作任务参数
	typedef struct _STask {
		CThreadTask* task;
		void* parameter;
		_STask(): task(0), parameter(0) {}
	} STask;

private:
	//! 线程池运行状态
	bool m_bRunning;
	//! 任务线程队列
	queue<STask> m_queTask;
#ifdef AVATAR_WINDOWS
	//! 线程列表
	vector<HANDLE> m_vecThread;
	//! 任务队列互斥锁
	CRITICAL_SECTION m_sMutex;
#else
	//! 线程列表
	vector<pthread_t> m_vecThread;
	//! 任务队列互斥锁
	pthread_mutex_t m_sMutex;
#endif
};

#endif