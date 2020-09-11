//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CAnimationManager.h"
#include <cmath>
#include <algorithm>

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
	Clear();
	delete this;
}

/**
* 动画更新，此方法由引擎内部调用
* @param dt 时间步长
*/
void CAnimationManager::Update(float dt) {
	if (m_lstAnimation.empty()) return;

	// 遍历动画，对每个动画进行更新
	list<SAnimationData*>::iterator iter = m_lstAnimation.begin();
	while (iter != m_lstAnimation.end()) {
		SAnimationData* anim = *iter++;
		// 动画未启动
		if (!anim->animationStart) continue;
		// 计算动画时间
		if (anim->animationDelay > 0.0f) {
			anim->animationDelay -= dt;
			if (anim->animationDelay > 0.0f) continue;
			else anim->animationTime += anim->animationDelay;
		}
		bool animationFinish = false;
		anim->animationTime += dt;
		while (anim->animationTime > anim->duration) {
			anim->animationTime -= anim->duration;
			anim->repeatCount++;
			if (anim->numberRepeat >= 0 && anim->repeatCount >= anim->numberRepeat) {
				anim->animationTime = anim->duration;
				animationFinish = true;
				break;
			} else {
				anim->scaleIndex = 0;
				anim->rotationIndex = 0;
				anim->translationIndex = 0;
			}
		}
		// 针对往复运动计算正确时间
		float time = anim->animationTime;
		if (anim->reciprocating) {
			anim->reversing = time > anim->duration * 0.5f;
			if (anim->reversing) time = anim->duration - time;
		}
		// 计算寻找当前动画关键帧
		if (anim->reversing) {
			while (anim->scaleIndex > 0 && time < anim->scale[anim->scaleIndex - 1].time) anim->scaleIndex--;
			while (anim->rotationIndex > 0 && time < anim->rotation[anim->rotationIndex - 1].time) anim->rotationIndex--;
			while (anim->translationIndex > 0 && time < anim->translation[anim->translationIndex - 1].time) anim->translationIndex--;
		} else {
			while (anim->scaleIndex < anim->scale.size() - 1 && time > anim->scale[anim->scaleIndex].time) anim->scaleIndex++;
			while (anim->rotationIndex < anim->rotation.size() - 1 && time > anim->rotation[anim->rotationIndex].time) anim->rotationIndex++;
			while (anim->translationIndex < anim->translation.size() - 1 && time > anim->translation[anim->translationIndex].time) anim->translationIndex++;
		}
		SScaleKey* s1 = &anim->scale[anim->scaleIndex];
		SScaleKey* s2 = &anim->scale[anim->scaleIndex > 0 ? anim->scaleIndex - 1 : 0];
		SRotationKey* r1 = &anim->rotation[anim->rotationIndex];
		SRotationKey* r2 = &anim->rotation[anim->rotationIndex > 0 ? anim->rotationIndex - 1 : 0];
		STranslationKey* t1 = &anim->translation[anim->translationIndex];
		STranslationKey* t2 = &anim->translation[anim->translationIndex > 0 ? anim->translationIndex - 1 : 0];
		// 通过 animationTime 以及插值方法计算当前插值
		CSceneNode* node = anim->sceneNode;
		node->m_cScale = s2->value.Lerp(s1->value, Interpolate(time, s2->time, s1->time, s1->interpolator));
		node->m_cOrientation = r2->value.Slerp(r1->value, Interpolate(time, r2->time, r1->time, r1->interpolator));
		node->m_cPosition = t2->value.Lerp(t1->value, Interpolate(time, t2->time, t1->time, t1->interpolator));
		node->Transform();
		if (animationFinish) {
			m_lstAnimation.remove(anim);
			delete anim;
		}
	}
}

/**
* 开始动画
* @param node 关联的场景节点
* @param repeat 重复次数，为负数表示无限重复
* @param swing 是否往复运动
* @param delay 延时 delay 秒后开始动画
* @return 成功返回 true
*/
bool CAnimationManager::Start(CSceneNode* node, int repeat, bool swing, float delay) {
	SAnimationData* anim = GetAnimationData(node, false);
	if (!anim) return false;
	const float ts = anim->scale.back().time;
	const float tr = anim->rotation.back().time;
	const float tt = anim->translation.back().time;
	anim->duration = std::max(ts, std::max(tr, tt));
	if (swing) anim->duration *= 2.0f;
	anim->numberRepeat = repeat;
	anim->reciprocating = swing;
	anim->animationDelay = delay;
	anim->scaleIndex = 0;
	anim->rotationIndex = 0;
	anim->translationIndex = 0;
	anim->animationTime = 0.0f;
	anim->repeatCount = 0;
	anim->animationStart = true;
	return true;
}

/**
* 暂停动画
* @param node 关联的场景节点
* @return 成功返回 true
*/
bool CAnimationManager::Pause(CSceneNode* node) {
	SAnimationData* anim = GetAnimationData(node, false);
	if (!anim) return false;
	anim->animationStart = false;
	return true;
}

/**
* 停止动画
* @param node 关联的场景节点
* @return 成功返回 true
*/
bool CAnimationManager::Stop(CSceneNode* node) {
	SAnimationData* anim = GetAnimationData(node, false);
	if (!anim) return false;
	// 对场景节点变换到最终状态后停止
	anim->sceneNode->m_cScale = anim->scale.back().value;
	anim->sceneNode->m_cOrientation = anim->rotation.back().value;
	anim->sceneNode->m_cPosition = anim->translation.back().value;
	anim->sceneNode->Transform();
	m_lstAnimation.remove(anim);
	delete anim;
	return true;
}

/**
* 清除所有动画
*/
void CAnimationManager::Clear() {
	list<SAnimationData*>::iterator iter = m_lstAnimation.begin();
	while (iter != m_lstAnimation.end()) {
		delete *iter++;
	}
	m_lstAnimation.clear();
}

/**
* 设置缩放动画
* @param node 关联的场景节点
* @param value 动画目标缩放值
* @param interpolator 动画插值方式
* @param duration 动画持续时间
*/
void CAnimationManager::AddScale(CSceneNode* node, const CVector3& value, Interpolator interpolator, float duration) {
	SAnimationData* anim = GetAnimationData(node, true);
	SScaleKey key;
	key.time = anim->scale.back().time + duration;
	key.interpolator = interpolator;
	key.value = value;
	anim->scale.push_back(key);
}

/**
* 设置旋转动画
* @param node 关联的场景节点
* @param value 动画目标旋转值
* @param interpolator 动画插值方式
* @param duration 动画持续时间
*/
void CAnimationManager::AddRotation(CSceneNode* node, const CQuaternion& value, Interpolator interpolator, float duration) {
	SAnimationData* anim = GetAnimationData(node, true);
	SRotationKey key;
	key.time = anim->rotation.back().time + duration;
	key.interpolator = interpolator;
	key.value = value;
	anim->rotation.push_back(key);
}

/**
* 设置平移动画
* @param node 关联的场景节点
* @param value 动画目标位置
* @param interpolator 动画插值方式
* @param duration 动画持续时间
*/
void CAnimationManager::AddTranslation(CSceneNode* node, const CVector3& value, Interpolator interpolator, float duration) {
	SAnimationData* anim = GetAnimationData(node, true);
	STranslationKey key;
	key.time = anim->translation.back().time + duration;
	key.interpolator = interpolator;
	key.value = value;
	anim->translation.push_back(key);
}

/**
* 获取执行的所有动画节点列表
* @param animationList 输出场景节点列表
*/
void CAnimationManager::GetAnimationList(vector<CSceneNode*>& animationList) {
	animationList.resize(m_lstAnimation.size());
	list<SAnimationData*>::iterator iter = m_lstAnimation.begin();
	int index = 0;
	while (iter != m_lstAnimation.end()) {
		animationList[index++] = (*iter)->sceneNode;
		++iter;
	}
}

/**
* 获取关联场景节点的动画数据
* @param node 关联的场景节点
* @param create 如果不存在是否创建
* @return 动画数据
*/
CAnimationManager::SAnimationData* CAnimationManager::GetAnimationData(CSceneNode* node, bool create) {
	list<SAnimationData*>::iterator iter = m_lstAnimation.begin();
	while (iter != m_lstAnimation.end()) {
		SAnimationData* anim = *iter++;
		if (anim->sceneNode == node) {
			return anim;
		}
	}
	if (!create) return 0;
	SScaleKey initScale;
	initScale.time = 0.0f;
	initScale.interpolator = LINEAR;
	initScale.value = node->m_cScale;
	SRotationKey initRotation;
	initRotation.time = 0.0f;
	initRotation.interpolator = LINEAR;
	initRotation.value = node->m_cOrientation;
	STranslationKey initTranslation;
	initTranslation.time = 0.0f;
	initTranslation.interpolator = LINEAR;
	initTranslation.value = node->m_cPosition;
	SAnimationData* anim = new SAnimationData();
	anim->sceneNode = node;
	anim->scale.push_back(initScale);
	anim->rotation.push_back(initRotation);
	anim->translation.push_back(initTranslation);
	anim->numberRepeat = 1;
	anim->reciprocating = false;
	anim->duration = 0.0f;
	anim->reversing = false;
	anim->repeatCount = 0;
	anim->animationStart = false;
	anim->animationDelay = 0.0f;
	anim->animationTime = 0.0f;
	anim->scaleIndex = 0;
	anim->rotationIndex = 0;
	anim->translationIndex = 0;
	m_lstAnimation.push_back(anim);
	return anim;
}

/**
* 根据插值方式计算正确插值
* @param t 当前值
* @param t1 起始值
* @param t2 结束值
* @param intepolator 插值方式
* @return 插值结果(0~1)
*/
float CAnimationManager::Interpolate(float t, float t1, float t2, Interpolator intepolator) {
	const float den = t2 - t1;
	if (den < 1E-6) return 1.0f;
	float k = std::max(std::min((t - t1) / den, 1.0f), 0.0f);
	switch (intepolator) {
	case LINEAR: break;
	case ACCELERATE: k = k * k; break;
	case DECELERATE: k = 1.0f - (1.0f - k) * (1.0f - k); break;
	case ACCELERATEDECELERATE: k = cosf((k + 1) * 3.141593f) * 0.5f + 0.5f; break;
	}
	return k;
}