//================================================
// Copyright (c) 2021 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CAsyncTask.h"
#include "CEngine.h"

/**
* 默认构造函数
*/
CAsyncTask::CAsyncTask(CEngine* engine) {
	m_bCancel = false;
	m_pEngine = engine;
	m_pTaskHandler = new CTaskHandler(this);
	m_pThread = new CThread();
	m_pEngine->RegisterEvent("update", m_pTaskHandler);
}

/**
* 默认析构函数
*/
CAsyncTask::~CAsyncTask() {
	m_pEngine->UnRegisterEvent("update", m_pTaskHandler, true);
	delete m_pThread;
}

/**
* 任务进度通知
* @param percent 进度百分比
* @note 被主线程调用
*/
void CAsyncTask::OnProgress(int percent) {
}

/**
* 任务完成通知
* @param cancel 是否被用户取消
* @note 被主线程调用
*/
void CAsyncTask::OnCompleted(bool cancel) {
}

/**
* 启动异步任务
* @note 请在主线程中调用
*/
void CAsyncTask::Run() {
	m_pThread->Start(CAsyncTask::RunBackground, this);
}

/**
* 取消异步任务
*/
void CAsyncTask::Cancel() {
	m_pThread->Lock();
	m_bCancel = true;
	m_pThread->Unlock();
}

/**
* 报告进度
* @note 请在异步线程中调用
*/
void CAsyncTask::ReportProgress(int percent) {
	m_pThread->Lock();
	m_queEvents.push(STaskEvent(percent));
	m_pThread->Unlock();
}

/**
* 是否已取消
*/
bool CAsyncTask::IsCancel() {
	return m_bCancel;
}

/**
* 后台线程方法
*/
void CAsyncTask::RunBackground(class CThread* thread, void* param) {
	CAsyncTask* task = reinterpret_cast<CAsyncTask*>(param);
	task->DoWork();
	task->m_pThread->Lock();
	task->m_queEvents.push(STaskEvent(task->m_bCancel));
	task->m_pThread->Unlock();
}

/**
* 任务事件处理
*/
void CAsyncTask::CTaskHandler::Handle() {
	while (true) {
		STaskEvent event;
		m_pAsyncTask->m_pThread->Lock();
		if (!m_pAsyncTask->m_queEvents.empty()) {
			event = m_pAsyncTask->m_queEvents.front();
			m_pAsyncTask->m_queEvents.pop();
		}
		m_pAsyncTask->m_pThread->Unlock();
		if (event.eventType == 1) m_pAsyncTask->OnProgress(event.percentage);
		else if (event.eventType == 2) m_pAsyncTask->OnCompleted(event.cancellation);
		else break;
	}
}