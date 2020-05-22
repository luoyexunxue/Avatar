//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CProfile.h"
#include "CTimer.h"
#include "CLog.h"
#include <cstring>

/**
* ���캯��
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
* ��ȡ�ڵ�����
*/
inline const char* CProfileNode::GetName() {
	return m_pName;
}

/**
* ����һ���ӽڵ�
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
* ��ȡ���ڵ�
*/
inline CProfileNode* CProfileNode::GetParent() {
	return m_pParent;
}

/**
* ������ʱ
*/
void CProfileNode::StartTimer() {
	if (!m_bActive) {
		m_fStartTime = CTimer::Reset("__profile__", false);
		m_iTotalCount += 1;
		m_bActive = true;
	}
}

/**
* ֹͣ��ʱ
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
* ��������
*/
void CProfileManager::Start(const char* name) {
	if (!m_pCurrentNode) m_pCurrentNode = new CProfileNode(name);
	if (name != m_pCurrentNode->GetName()) {
		m_pCurrentNode = m_pCurrentNode->GetChild(name);
	}
	m_pCurrentNode->StartTimer();
}

/**
* ֹͣ����
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
* ��ӡ�������
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
* ��������ڵ�
*/
void CProfileManager::Clear() {
	if (m_pCurrentNode) {
		CProfileNode* parent = m_pCurrentNode->GetParent();
		ClearNode(m_pCurrentNode);
		m_pCurrentNode = parent;
	}
}

/**
* �ݹ����������Ϣ
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
* �ݹ���������ڵ�
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
* �������ڵ�
*/
CProfileNode* CProfileManager::m_pCurrentNode = 0;