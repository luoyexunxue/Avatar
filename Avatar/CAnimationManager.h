//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
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
	//! 开始动画
	bool Start(CSceneNode* node, int repeat, bool swing, bool relative, float delay = 0.0f);
	//! 暂停动画
	bool Pause(CSceneNode* node);
	//! 停止动画
	bool Stop(CSceneNode* node, bool reset);
	//! 清除所有动画
	void Clear();

	//! 添加缩放动画
	void AddScale(CSceneNode* node, const CVector3& value, float duration, Interpolator interpolator);
	//! 添加旋转动画
	void AddRotation(CSceneNode* node, const CQuaternion& value, float duration, Interpolator interpolator);
	//! 添加平移动画
	void AddTranslation(CSceneNode* node, const CVector3& value, float duration, Interpolator interpolator);

	//! 获取执行的所有动画节点列表
	void GetAnimationList(vector<CSceneNode*>& animationList);

private:
	//! 缩放动画关键点
	typedef struct _SScaleKey {
		float time;
		Interpolator interpolator;
		CVector3 value;
	} SScaleKey;
	//! 旋转动画关键点
	typedef struct _SRotationKey {
		float time;
		Interpolator interpolator;
		CQuaternion value;
	} SRotationKey;
	//! 平移动画关键点
	typedef struct _STranslationKey {
		float time;
		Interpolator interpolator;
		CVector3 value;
	} STranslationKey;
	//! 动画数据定义
	typedef struct _SAnimationData {
		CSceneNode* sceneNode;
		vector<SScaleKey> scale;
		vector<SRotationKey> rotation;
		vector<STranslationKey> translation;
		int numberRepeat;
		bool reciprocating;
		bool incremental;
		float duration;
		//! 内部计算变量
		int repeatCount;
		bool reversing;
		bool animationStart;
		float animationDelay;
		float animationTime;
		size_t scaleIndex;
		size_t rotationIndex;
		size_t translationIndex;
		//! 比较运算符重载
		bool operator == (const _SAnimationData& other) const {
			return this->sceneNode == other.sceneNode;
		}
	} SAnimationData;

private:
	CAnimationManager();
	~CAnimationManager();

	//! 获取关联场景节点的动画数据
	SAnimationData* GetAnimationData(CSceneNode* node, bool create);
	//! 根据插值方式计算正确插值
	float Interpolate(float t, float t1, float t2, Interpolator intepolator);

private:
	//! 动画列表
	list<SAnimationData*> m_lstAnimation;
	//! 实例
	static CAnimationManager* m_pInstance;
};

#endif