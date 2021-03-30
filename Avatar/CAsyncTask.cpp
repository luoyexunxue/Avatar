//================================================
// Copyright (c) 2021 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CAsyncTask.h"
#include "CEngine.h"

/**
* 默认构造函数
* @note 需要主线程调用 Handle 来处理事件
*/
CAsyncTask::CAsyncTask() {
	m_bRunning = false;
	m_bCancel = false;
	m_iThreadCount = 1;
	m_pEngine = 0;
	m_pTaskHandler = new CTaskHandler(this);
}

/**
* 指定线程数的构造函数
* @param threadCount 任务线程数
* @note 需要主线程调用 Handle 来处理事件
*/
CAsyncTask::CAsyncTask(int threadCount) {
	m_bRunning = false;
	m_bCancel = false;
	m_iThreadCount = threadCount;
	m_pEngine = 0;
	m_pTaskHandler = new CTaskHandler(this);
}

/**
* 使用引擎对象构建
* @param engine 引擎对象指针
* @param threadCount 任务线程数
* @note 在引擎逻辑更新时自动处理事件, 不需要调用 Handle
*/
CAsyncTask::CAsyncTask(CEngine* engine, int threadCount) {
	m_bRunning = false;
	m_bCancel = false;
	m_iThreadCount = threadCount;
	m_pEngine = engine;
	m_pTaskHandler = new CTaskHandler(this);
	m_pEngine->RegisterEvent("update", m_pTaskHandler);
}

/**
* 默认析构函数
*/
CAsyncTask::~CAsyncTask() {
	m_bRunning = false;
	m_bCancel = true;
	for (size_t i = 0; i < m_vecThreads.size(); i++) {
#ifdef AVATAR_WINDOWS
		WaitForSingleObject(m_vecThreads[i], INFINITE);
#else
		pthread_join(m_vecThreads[i], 0);
#endif
	}
	m_pTaskHandler->Handle();
	if (m_pEngine) m_pEngine->UnRegisterEvent("update", m_pTaskHandler, true);
	else delete m_pTaskHandler;
}

/**
* 启动异步任务
* @note 请在主线程中调用
*/
void CAsyncTask::Start() {
	if (m_bRunning) return;
	m_bRunning = true;
	m_bCancel = false;
	m_vecThreads.resize(m_iThreadCount);
	for (size_t i = 0; i < m_vecThreads.size(); i++) {
#ifdef AVATAR_WINDOWS
		m_vecThreads[i] = CreateThread(0, 0, CAsyncTask::RunBackground, this, 0, 0);
#else
		pthread_create(&m_vecThreads[i], 0, CAsyncTask::RunBackground, this);
#endif
	}
}

/**
* 取消异步任务
*/
void CAsyncTask::Cancel() {
	m_cMutex.lock();
	m_bCancel = true;
	m_cMutex.unlock();
}

/**
* 处理异步事件
*/
void CAsyncTask::Handle() {
	if (!m_pEngine) m_pTaskHandler->Handle();
}

/**
* 任务进度通知
* @param percent 进度百分比
* @param userData 用户数据
* @note 被主线程调用
*/
void CAsyncTask::OnProgress(int percent, void* userData) {
}

/**
* 任务完成通知
* @param cancel 是否被用户取消
* @note 被主线程调用
*/
void CAsyncTask::OnCompleted(bool cancel) {
}

/**
* 报告进度
* @note 请在异步线程中调用
*/
void CAsyncTask::ReportProgress(int percent, void* userData) {
	m_cMutex.lock();
	m_queEvents.push(STaskEvent(percent, userData));
	m_cMutex.unlock();
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
#ifdef AVATAR_WINDOWS
DWORD WINAPI CAsyncTask::RunBackground(LPVOID param) {
#else
void* CAsyncTask::RunBackground(void* param) {
#endif
	CAsyncTask* task = reinterpret_cast<CAsyncTask*>(param);
	task->DoWork();
	task->m_cMutex.lock();
	task->m_queEvents.push(STaskEvent(task->m_bCancel));
	task->m_cMutex.unlock();
	return 0;
}

/**
* 任务事件处理
*/
void CAsyncTask::CTaskHandler::Handle() {
	while (true) {
		STaskEvent evt;
		m_pAsyncTask->m_cMutex.lock();
		if (!m_pAsyncTask->m_queEvents.empty()) {
			evt = m_pAsyncTask->m_queEvents.front();
			m_pAsyncTask->m_queEvents.pop();
		}
		m_pAsyncTask->m_cMutex.unlock();
		if (evt.eventType == 1) m_pAsyncTask->OnProgress(evt.percentage, evt.userData);
		else if (evt.eventType == 2) m_pAsyncTask->OnCompleted(evt.cancellation);
		else break;
	}
}