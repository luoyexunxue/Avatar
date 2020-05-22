//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CANIMATIONMANAGER_H_
#define _CANIMATIONMANAGER_H_
#include "AvatarConfig.h"
#include "CSceneNode.h"
#include "CVector3.h"
#include "CQuaternion.h"
#include <vector>
#include <list>
using std::vector;
using std::list;

/**
* @brief 动画管理器
*/
class AVATAR_EXPORT CAnimationManager {
public:
	//! 获取管理器实例
	static CAnimationManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CAnimationManager();
		return m_pInstance;
	}

	//! 实例销毁
	void Destroy();
	//! 动画更新
	void Update(float dt);

public:
	//! 支持的动画插值方式
	enum Interpolator { LINEAR, ACCELERATE, DECELERATE, ACCELERATEDECELERATE };

public:
	//! 设置动画参数
	void SetAnimation(CSceneNode* node, Interpolator intepolator, float duration, int repeat, bool swing);
	//! 设置缩放动画
	bool AnimateScale(CSceneNode* node, const CVector3& from, const CVector3& to);
	//! 设置旋转动画
	bool AnimateRotation(CSceneNode* node, const CQuaternion& from, const CQuaternion& to);
	//! 设置平移动画
	bool AnimateTranslation(CSceneNode* node, const CVector3& from, const CVector3& to);

	//! 开始动画
	bool Start(CSceneNode* node, float delay = 0.0f);
	//! 暂停动画
	bool Pause(CSceneNode* node);
	//! 停止动画
	bool Stop(CSceneNode* node);
	//! 清除所有动画
	void Clear();

	//! 获取执行的所有动画节点列表
	void GetAnimationList(vector<CSceneNode*>& animationList);

private:
	CAnimationManager();
	~CAnimationManager();

private:
	//! 动画数据定义
	typedef struct _SAnimationData {
		CSceneNode* sceneNode;
		Interpolator intepolator;
		float duration;
		int numberRepeat;
		bool reciprocating;
		CVector3 fromScale;
		CVector3 toScale;
		CQuaternion fromOrientation;
		CQuaternion toOrientation;
		CVector3 fromPosition;
		CVector3 toPosition;
		//! 内部变量
		bool useScale;
		bool useOrientation;
		bool usePosition;
		bool reversing;
		bool animationStart;
		float animationTime;
		float animationDelay;
		int animationCount;
		//! 比较运算符重载
		bool operator == (const _SAnimationData& other) const {
			return this->sceneNode == other.sceneNode;
		}
	} SAnimationData;

private:
	//! 动画列表
	list<SAnimationData> m_lstAnimation;
	//! 实例
	static CAnimationManager* m_pInstance;
};

#endif