//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPROFILE_H_
#define _CPROFILE_H_
#include "AvatarConfig.h"
#include <list>
using std::list;

/**
* @brief 采样宏定义
*
* 使用前，需要保证已初始化定时器和创建日志输出，然后通过 PROFILE_RESULT() 获取采样结果.
* @see CTimer::InitTimer()
* @see CLog::Create()
*/
#define PROFILE(name) CProfileSample _profile(name)

/**
* @brief 打印采样结果
*/
#define PROFILE_RESULT() CProfileManager::Print()

/**
* @brief 重置采样结果
*/
#define PROFILE_RESET() CProfileManager::Clear()

/**
* @brief 采样节点
*/
class CProfileNode {
public:
	//! 使用名称构造节点
	CProfileNode(const char* name);

	//! 获取节点名称
	inline const char* GetName();
	//! 生成一个子节点
	inline CProfileNode* GetChild(const char* name);
	//! 获取父节点
	inline CProfileNode* GetParent();

	//! 启动计时
	void StartTimer();
	//! 停止计时
	void StopTimer();
	//! 获取最近采样时间
	inline float GetTimeElapse() { return m_fStopTime - m_fStartTime; }
	//! 获取平均采样时间
	inline float GetTimeElapseAverage() { return m_fTotalTime / m_iTotalCount; }
	//! 获取最短采样时间
	inline float GetTimeElapseBest() { return m_fBestTime; }
	//! 获取最长采样时间
	inline float GetTimeElapseWorst() { return m_fWorstTime; }
	//! 获取总调用次数
	inline int GetTotalCount() { return m_iTotalCount; }

private:
	//! 记录采样节点名称字符串
	const char* m_pName;
	//! 起始时间
	float m_fStartTime;
	//! 结束时间
	float m_fStopTime;
	//! 总调用时间
	float m_fTotalTime;
	//! 最短采样时间
	float m_fBestTime;
	//! 最长采样时间
	float m_fWorstTime;
	//! 调用次数
	int m_iTotalCount;
	//! 启动状态
	bool m_bActive;
	//! 父节点
	CProfileNode* m_pParent;
	//! 子节点
	list<CProfileNode*> m_lstChildren;
	//! 设置友元类
	friend class CProfileManager;
};

/**
* @brief 性能评测管理器
*/
class AVATAR_EXPORT CProfileManager {
public:
	//! 启动采样
	static void Start(const char* name);
	//! 停止采样
	static float Stop();
	//! 显示采样结果
	static void Print();
	//! 清除采样节点
	static void Clear();

private:
	//! 递归输出采样信息
	static void PrintNode(CProfileNode* node, int level);
	//! 递归清除采样节点
	static void ClearNode(CProfileNode* node);

private:
	//! 采样节点
	static CProfileNode* m_pCurrentNode;
};

/**
* @brief 性能采样器
*/
class CProfileSample {
public:
	//! 构造函数启动采样
	CProfileSample(const char* name) {
		CProfileManager::Start(name);
	}
	//! 析构函数结束采样
	~CProfileSample() {
		CProfileManager::Stop();
	}
};

#endif