//================================================
// Copyright (c) 2021 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CASYNCTASK_H_
#define _CASYNCTASK_H_
#include "AvatarConfig.h"
#include "CEngine.h"
#include <vector>
#include <queue>
#include <mutex>
#ifdef AVATAR_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#include <pthread.h>
#endif
using std::vector;
using std::queue;
using std::mutex;

/**
* @brief 异步任务工具类
*/
class AVATAR_EXPORT CAsyncTask {
public:
	//! 默认构造函数
	CAsyncTask();
	//! 指定线程数构造函数
	CAsyncTask(int threadCount);
	//! 使用引擎对象构造
	CAsyncTask(CEngine* engine, int threadCount);
	//! 默认析构函数
	virtual ~CAsyncTask();

	//! 启动异步任务
	void Start();
	//! 取消异步任务
	void Cancel();
	//! 处理异步事件
	void Handle();

	//! 任务进度通知
	virtual void OnProgress(int percent, void* userData);
	//! 任务完成通知
	virtual void OnCompleted(bool cancel);

	//! 报告进度
	virtual void ReportProgress(int percent, void* userData);
	//! 是否已取消
	virtual bool IsCancel();
	//! 异步任务实现
	virtual void DoWork() = 0;

private:
	//! 事件通知接口
	class CTaskHandler : public CEngine::CNotifyHandler {
	public:
		//! 异步任务对象
		CAsyncTask* m_pAsyncTask;
		//! 默认构造函数
		CTaskHandler(CAsyncTask* task): m_pAsyncTask(task) {}
		//! 事件通知方法
		virtual void Handle();
	};
	//! 事件定义
	typedef struct _STaskEvent {
		int eventType;
		int percentage;
		bool cancellation;
		void* userData;
		_STaskEvent() : eventType(-1) {}
		_STaskEvent(bool cancel) : eventType(2), cancellation(cancel) {}
		_STaskEvent(int percent, void* param) : eventType(1), percentage(percent), userData(param) {}
	} STaskEvent;

private:
	//! 任务是否在运行
	bool m_bRunning;
	//! 是否已经被取消
	bool m_bCancel;
	//! 任务线程数
	int m_iThreadCount;
	//! 互斥锁
	mutex m_cMutex;
	//! 引擎对象指针
	CEngine* m_pEngine;
	//! 任务事件回调
	CTaskHandler* m_pTaskHandler;
	//! 事件队列
	queue<STaskEvent> m_queEvents;
#ifdef AVATAR_WINDOWS
	//! 线程句柄
	vector<HANDLE> m_vecThreads;
	//! 工作线程
	static DWORD WINAPI RunBackground(LPVOID param);
#else
	//! 线程句柄
	vector<pthread_t> m_vecThreads;
	//! 工作线程
	static void* RunBackground(void* param);
#endif
};

#endif