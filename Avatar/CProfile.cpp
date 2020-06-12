//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CProfile.h"
#include "CTimer.h"
#include "CLog.h"
#include <cstring>

/**
* 构造函数
*/
CProfileNode::CProfileNode(const char* name) {
	m_pName = name;
	m_fStartTime = 0.0f;
	m_fStopTime = 0.0f;
	m_fTotalTime = 0.0f;
	m_fBestTime = 1.0e6;
	m_fWorstTime = 0.0f;
	m_iTotalCount = 0;
	m_bActive = false;
	m_pParent = 0;
}

/**
* 获取节点名称
*/
inline const char* CProfileNode::GetName() {
	return m_pName;
}

/**
* 生成一个子节点
*/
inline CProfileNode* CProfileNode::GetChild(const char* name) {
	list<CProfileNode*>::iterator iter = m_lstChildren.begin();
	while (iter != m_lstChildren.end()) {
		if ((*iter)->GetName() == name) {
			return *iter;
		}
		++iter;
	}
	CProfileNode* child = new CProfileNode(name);
	child->m_pParent = this;
	m_lstChildren.push_back(child);
	return child;
}

/**
* 获取父节点
*/
inline CProfileNode* CProfileNode::GetParent() {
	return m_pParent;
}

/**
* 启动计时
*/
void CProfileNode::StartTimer() {
	if (!m_bActive) {
		m_fStartTime = CTimer::Reset("__profile__", false);
		m_iTotalCount += 1;
		m_bActive = true;
	}
}

/**
* 停止计时
*/
void CProfileNode::StopTimer() {
	if (m_bActive) {
		m_fStopTime = CTimer::Reset("__profile__", false);
		float timeElapse = m_fStopTime - m_fStartTime;
		m_fTotalTime += timeElapse;
		if (m_fBestTime > timeElapse) m_fBestTime = timeElapse;
		if (m_fWorstTime < timeElapse) m_fWorstTime = timeElapse;
		m_bActive = false;
	}
}

/**
* 启动采样
*/
void CProfileManager::Start(const char* name) {
	if (!m_pCurrentNode) m_pCurrentNode = new CProfileNode(name);
	if (name != m_pCurrentNode->GetName()) {
		m_pCurrentNode = m_pCurrentNode->GetChild(name);
	}
	m_pCurrentNode->StartTimer();
}

/**
* 停止采样
*/
float CProfileManager::Stop() {
	float elapse = 0.0f;
	if (m_pCurrentNode) {
		m_pCurrentNode->StopTimer();
		elapse = m_pCurrentNode->GetTimeElapse();
		if (m_pCurrentNode->GetParent()) {
			m_pCurrentNode = m_pCurrentNode->GetParent();
		}
	}
	return elapse;
}

/**
* 打印采样结果
*/
void CProfileManager::Print() {
	CLog::Debug("================ Profile ====================");
	if (m_pCurrentNode) {
		m_pCurrentNode->StopTimer();
		PrintNode(m_pCurrentNode, 0);
	}
	CLog::Debug("=============== End Profile =================");
}

/**
* 清除采样节点
*/
void CProfileManager::Clear() {
	if (m_pCurrentNode) {
		CProfileNode* parent = m_pCurrentNode->GetParent();
		ClearNode(m_pCurrentNode);
		m_pCurrentNode = parent;
	}
}

/**
* 递归输出采样信息
*/
void CProfileManager::PrintNode(CProfileNode* node, int level) {
	if (!node || level >= 256) return;
	const char* fmt = "+%s<%s> (A %.6f, B %.6f, W %.6f) %d calls";
	char space[512];
	memset(space, ' ', level * 2);
	space[level * 2] = '\0';
	float average = node->GetTimeElapseAverage();
	float best = node->GetTimeElapseBest();
	float worst = node->GetTimeElapseWorst();
	CLog::Debug(fmt, space, node->GetName(), average, best, worst, node->GetTotalCount());
	list<CProfileNode*>::iterator iter = node->m_lstChildren.begin();
	while (iter != node->m_lstChildren.end()) {
		PrintNode(*iter, level + 1);
		++iter;
	}
}

/**
* 递归清除采样节点
*/
void CProfileManager::ClearNode(CProfileNode* node) {
	if (!node) return;
	list<CProfileNode*>::iterator iter = node->m_lstChildren.begin();
	while (iter != node->m_lstChildren.end()) {
		ClearNode(*iter);
		++iter;
	}
	delete node;
}

/**
* 采样根节点
*/
CProfileNode* CProfileManager::m_pCurrentNode = 0;