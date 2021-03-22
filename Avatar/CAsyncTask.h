//================================================
// Copyright (c) 2021 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CASYNCTASK_H_
#define _CASYNCTASK_H_
#include "AvatarConfig.h"
#include "CEngine.h"
#include "CThread.h"

/**
* @brief 异步任务工具类
*/
class AVATAR_EXPORT CAsyncTask {
public:
	//! 默认构造函数
	CAsyncTask(CEngine* engine);
	//! 默认析构函数
	virtual ~CAsyncTask();

	//! 任务进度通知
	virtual void OnProgress(int percent);
	//! 任务完成通知
	virtual void OnCompleted(bool cancel);
	//! 启动异步任务
	virtual void Run();
	//! 取消异步任务
	virtual void Cancel();

	//! 报告进度
	virtual void ReportProgress(int percent);
	//! 是否已取消
	virtual bool IsCancel();
	//! 异步处理实现
	virtual void DoWork() = 0;

private:
	//! 后台线程方法
	static void RunBackground(class CThread* thread, void* param);

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
		_STaskEvent(): eventType(-1) {}
		_STaskEvent(int percent): eventType(1), percentage(percent) {}
		_STaskEvent(bool cancel): eventType(2), cancellation(cancel) {}
	} STaskEvent;

private:
	//! 引擎对象指针
	CEngine* m_pEngine;
	//! 任务事件回调
	CTaskHandler* m_pTaskHandler;
	//! 线程对象
	CThread* m_pThread;
	//! 是否已经被取消
	bool m_bCancel;
	//! 事件队列
	queue<STaskEvent> m_queEvents;
};

#endif