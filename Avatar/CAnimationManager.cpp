//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CAnimationManager.h"
#include <cmath>

/**
* 构造函数
*/
CAnimationManager::CAnimationManager() {
}

/**
* 析构函数
*/
CAnimationManager::~CAnimationManager() {
	m_pInstance = 0;
}

/**
* 单例实例
*/
CAnimationManager* CAnimationManager::m_pInstance = 0;

/**
* 销毁动画管理器
*/
void CAnimationManager::Destroy() {
	delete this;
}

/**
* 动画更新，此方法由引擎内部调用
* @param dt 时间步长
*/
void CAnimationManager::Update(float dt) {
	if (m_lstAnimation.empty()) return;

	// 遍历动画，对每个动画进行更新
	list<SAnimationData>::iterator iter = m_lstAnimation.begin();
	while (iter != m_lstAnimation.end()) {
		SAnimationData* anim = &*iter;
		++iter;
		// 动画未启动
		if (!anim->animationStart) continue;
		bool animationFinish = false;
		// 计算动画时间
		if (anim->animationDelay > 0.0f) {
			anim->animationDelay -= dt;
			if (anim->animationDelay > 0.0f) continue;
			else anim->animationTime += anim->animationDelay;
		}
		anim->animationTime += dt;
		while (anim->animationTime > anim->duration) {
			anim->animationTime -= anim->duration;
			anim->animationCount++;
			if (anim->reciprocating) {
				anim->reversing = !anim->reversing;
				anim->animationCount -= anim->reversing ? 1 : 0;
			}
			if (anim->numberRepeat >= 0 && anim->animationCount >= anim->numberRepeat) {
				anim->animationTime = anim->duration;
				animationFinish = true;
				break;
			}
		}
		// 通过 animationTime 和 duration 以及插值方法计算当前插值
		float t = anim->animationTime / anim->duration;
		switch (anim->intepolator) {
		case LINEAR: break;
		case ACCELERATE: t = t * t; break;
		case DECELERATE: t = 1.0f - (1.0f - t) * (1.0f - t); break;
		case ACCELERATEDECELERATE: t = cosf((t + 1) * 3.141593f) * 0.5f + 0.5f; break;
		}
		// 插值计算 缩放 旋转 平移
		CSceneNode* node = anim->sceneNode;
		if (anim->useScale) {
			if (anim->reversing) node->m_cScale = anim->toScale.Lerp(anim->fromScale, t);
			else node->m_cScale = anim->fromScale.Lerp(anim->toScale, t);
		}
		if (anim->useOrientation) {
			if (anim->reversing) node->m_cOrientation = anim->toOrientation.Slerp(anim->fromOrientation, t);
			else node->m_cOrientation = anim->fromOrientation.Slerp(anim->toOrientation, t);
		}
		if (anim->usePosition) {
			if (anim->reversing) node->m_cPosition = anim->toPosition.Lerp(anim->fromPosition, t);
			else node->m_cPosition = anim->fromPosition.Lerp(anim->toPosition, t);
		}
		node->Transform();
		if (animationFinish) {
			m_lstAnimation.remove(*anim);
		}
	}
}

/**
* 设置动画参数
* @param node 关联的场景节点
* @param intepolator 动画插值方式
* @param duration 一次动画的时长
* @param repeat 重复次数，为负数表示无限重复
* @param swing 是否往复运动
*/
void CAnimationManager::SetAnimation(CSceneNode* node, Interpolator intepolator, float duration, int repeat, bool swing) {
	// 检查场景节点的动画是否已存在
	list<SAnimationData>::iterator iter = m_lstAnimation.begin();
	while (iter != m_lstAnimation.end()) {
		SAnimationData* anim = &*iter;
		if (anim->sceneNode == node) {
			anim->intepolator = intepolator;
			anim->duration = duration;
			anim->numberRepeat = repeat;
			anim->reciprocating = swing;
			anim->reversing = false;
			anim->animationStart = false;
			anim->animationTime = 0.0f;
			anim->animationCount = 0;
			return;
		}
		++iter;
	}
	// 若不存在，则初始化动画数据，并加入动画列表
	SAnimationData anim;
	anim.sceneNode = node;
	anim.intepolator = intepolator;
	anim.duration = duration;
	anim.numberRepeat = repeat;
	anim.reciprocating = swing;
	anim.useScale = false;
	anim.useOrientation = false;
	anim.usePosition = false;
	anim.reversing = false;
	anim.animationStart = false;
	anim.animationTime = 0.0f;
	anim.animationCount = 0;
	m_lstAnimation.push_back(anim);
}

/**
* 设置缩放动画
* @param node 关联的场景节点
* @param from 动画开始时的缩放
* @param to 动画结束时的缩放
* @return 成功返回 true
*/
bool CAnimationManager::AnimateScale(CSceneNode* node, const CVector3& from, const CVector3& to) {
	list<SAnimationData>::iterator iter = m_lstAnimation.begin();
	while (iter != m_lstAnimation.end()) {
		SAnimationData* anim = &*iter;
		if (anim->sceneNode == node) {
			anim->fromScale = from;
			anim->toScale = to;
			anim->useScale = true;
			return true;
		}
		++iter;
	}
	return false;
}

/**
* 设置旋转动画
* @param node 关联的场景节点
* @param from 动画开始时的旋转
* @param to 动画结束时的旋转
* @return 成功返回 true
*/
bool CAnimationManager::AnimateRotation(CSceneNode* node, const CQuaternion& from, const CQuaternion& to) {
	list<SAnimationData>::iterator iter = m_lstAnimation.begin();
	while (iter != m_lstAnimation.end()) {
		SAnimationData* anim = &*iter;
		if (anim->sceneNode == node) {
			anim->fromOrientation = from;
			anim->toOrientation = to;
			anim->useOrientation = true;
			return true;
		}
		++iter;
	}
	return false;
}

/**
* 设置平移动画
* @param node 关联的场景节点
* @param from 动画开始时的位置
* @param to 动画结束时的位置
* @return 成功返回 true
*/
bool CAnimationManager::AnimateTranslation(CSceneNode* node, const CVector3& from, const CVector3& to) {
	list<SAnimationData>::iterator iter = m_lstAnimation.begin();
	while (iter != m_lstAnimation.end()) {
		SAnimationData* anim = &*iter;
		if (anim->sceneNode == node) {
			anim->fromPosition = from;
			anim->toPosition = to;
			anim->usePosition = true;
			return true;
		}
		++iter;
	}
	return false;
}

/**
* 开始动画
* @param node 关联的场景节点
* @param delay 延时 delay 秒后开始动画
* @return 成功返回 true
*/
bool CAnimationManager::Start(CSceneNode* node, float delay) {
	list<SAnimationData>::iterator iter = m_lstAnimation.begin();
	while (iter != m_lstAnimation.end()) {
		SAnimationData* anim = &*iter;
		if (anim->sceneNode == node) {
			anim->animationStart = true;
			anim->animationDelay = delay;
			return true;
		}
		++iter;
	}
	return false;
}

/**
* 暂停动画
* @param node 关联的场景节点
* @return 成功返回 true
*/
bool CAnimationManager::Pause(CSceneNode* node) {
	list<SAnimationData>::iterator iter = m_lstAnimation.begin();
	while (iter != m_lstAnimation.end()) {
		SAnimationData* anim = &*iter;
		if (anim->sceneNode == node) {
			anim->animationStart = false;
			return true;
		}
		++iter;
	}
	return false;
}

/**
* 停止动画
* @param node 关联的场景节点
* @return 成功返回 true
*/
bool CAnimationManager::Stop(CSceneNode* node) {
	list<SAnimationData>::iterator iter = m_lstAnimation.begin();
	while (iter != m_lstAnimation.end()) {
		SAnimationData* anim = &*iter;
		if (anim->sceneNode == node) {
			// 对场景节点变换到最终状态后停止
			if (anim->useScale) anim->sceneNode->m_cScale = anim->toScale;
			if (anim->useOrientation) anim->sceneNode->m_cOrientation = anim->toOrientation;
			if (anim->usePosition) anim->sceneNode->m_cPosition = anim->toPosition;
			anim->sceneNode->Transform();
			m_lstAnimation.erase(iter);
			return true;
		}
		++iter;
	}
	return false;
}

/**
* 清除所有动画
*/
void CAnimationManager::Clear() {
	m_lstAnimation.clear();
}

/**
* 获取执行的所有动画节点列表
* @param animationList 输出场景节点列表
*/
void CAnimationManager::GetAnimationList(vector<CSceneNode*>& animationList) {
	animationList.resize(m_lstAnimation.size());
	list<SAnimationData>::iterator iter = m_lstAnimation.begin();
	int index = 0;
	while (iter != m_lstAnimation.end()) {
		animationList[index++] = iter->sceneNode;
		++iter;
	}
}