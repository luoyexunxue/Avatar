//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPROFILE_H_
#define _CPROFILE_H_
#include "AvatarConfig.h"
#include <list>
using std::list;

/**
* @brief �����궨��
*
* ʹ��ǰ����Ҫ��֤�ѳ�ʼ����ʱ���ʹ�����־�����Ȼ��ͨ�� PROFILE_RESULT() ��ȡ�������.
* @see CTimer::InitTimer()
* @see CLog::Create()
*/
#define PROFILE(name) CProfileSample _profile(name)

/**
* @brief ��ӡ�������
*/
#define PROFILE_RESULT() CProfileManager::Print()

/**
* @brief ���ò������
*/
#define PROFILE_RESET() CProfileManager::Clear()

/**
* @brief �����ڵ�
*/
class CProfileNode {
public:
	//! ʹ�����ƹ���ڵ�
	CProfileNode(const char* name);

	//! ��ȡ�ڵ�����
	inline const char* GetName();
	//! ����һ���ӽڵ�
	inline CProfileNode* GetChild(const char* name);
	//! ��ȡ���ڵ�
	inline CProfileNode* GetParent();

	//! ������ʱ
	void StartTimer();
	//! ֹͣ��ʱ
	void StopTimer();
	//! ��ȡ�������ʱ��
	inline float GetTimeElapse() { return m_fStopTime - m_fStartTime; }
	//! ��ȡƽ������ʱ��
	inline float GetTimeElapseAverage() { return m_fTotalTime / m_iTotalCount; }
	//! ��ȡ��̲���ʱ��
	inline float GetTimeElapseBest() { return m_fBestTime; }
	//! ��ȡ�����ʱ��
	inline float GetTimeElapseWorst() { return m_fWorstTime; }
	//! ��ȡ�ܵ��ô���
	inline int GetTotalCount() { return m_iTotalCount; }

private:
	//! ��¼�����ڵ������ַ���
	const char* m_pName;
	//! ��ʼʱ��
	float m_fStartTime;
	//! ����ʱ��
	float m_fStopTime;
	//! �ܵ���ʱ��
	float m_fTotalTime;
	//! ��̲���ʱ��
	float m_fBestTime;
	//! �����ʱ��
	float m_fWorstTime;
	//! ���ô���
	int m_iTotalCount;
	//! ����״̬
	bool m_bActive;
	//! ���ڵ�
	CProfileNode* m_pParent;
	//! �ӽڵ�
	list<CProfileNode*> m_lstChildren;
	//! ������Ԫ��
	friend class CProfileManager;
};

/**
* @brief �������������
*/
class AVATAR_EXPORT CProfileManager {
public:
	//! ��������
	static void Start(const char* name);
	//! ֹͣ����
	static float Stop();
	//! ��ʾ�������
	static void Print();
	//! ��������ڵ�
	static void Clear();

private:
	//! �ݹ����������Ϣ
	static void PrintNode(CProfileNode* node, int level);
	//! �ݹ���������ڵ�
	static void ClearNode(CProfileNode* node);

private:
	//! �����ڵ�
	static CProfileNode* m_pCurrentNode;
};

/**
* @brief ���ܲ�����
*/
class CProfileSample {
public:
	//! ���캯����������
	CProfileSample(const char* name) {
		CProfileManager::Start(name);
	}
	//! ����������������
	~CProfileSample() {
		CProfileManager::Stop();
	}
};

#endif