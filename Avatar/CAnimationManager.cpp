//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CAnimationManager.h"
#include <cmath>

/**
* ���캯��
*/
CAnimationManager::CAnimationManager() {
}

/**
* ��������
*/
CAnimationManager::~CAnimationManager() {
	m_pInstance = 0;
}

/**
* ����ʵ��
*/
CAnimationManager* CAnimationManager::m_pInstance = 0;

/**
* ���ٶ���������
*/
void CAnimationManager::Destroy() {
	delete this;
}

/**
* �������£��˷����������ڲ�����
* @param dt ʱ�䲽��
*/
void CAnimationManager::Update(float dt) {
	if (m_lstAnimation.empty()) return;

	// ������������ÿ���������и���
	list<SAnimationData>::iterator iter = m_lstAnimation.begin();
	while (iter != m_lstAnimation.end()) {
		SAnimationData* anim = &*iter;
		++iter;
		// ����δ����
		if (!anim->animationStart) continue;
		bool animationFinish = false;
		// ���㶯��ʱ��
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
		// ͨ�� animationTime �� duration �Լ���ֵ�������㵱ǰ��ֵ
		float t = anim->animationTime / anim->duration;
		switch (anim->intepolator) {
		case LINEAR: break;
		case ACCELERATE: t = t * t; break;
		case DECELERATE: t = 1.0f - (1.0f - t) * (1.0f - t); break;
		case ACCELERATEDECELERATE: t = cosf((t + 1) * 3.141593f) * 0.5f + 0.5f; break;
		}
		// ��ֵ���� ���� ��ת ƽ��
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
* ���ö�������
* @param node �����ĳ����ڵ�
* @param intepolator ������ֵ��ʽ
* @param duration һ�ζ�����ʱ��
* @param repeat �ظ�������Ϊ������ʾ�����ظ�
* @param swing �Ƿ������˶�
*/
void CAnimationManager::SetAnimation(CSceneNode* node, Interpolator intepolator, float duration, int repeat, bool swing) {
	// ��鳡���ڵ�Ķ����Ƿ��Ѵ���
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
	// �������ڣ����ʼ���������ݣ������붯���б�
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
* �������Ŷ���
* @param node �����ĳ����ڵ�
* @param from ������ʼʱ������
* @param to ��������ʱ������
* @return �ɹ����� true
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
* ������ת����
* @param node �����ĳ����ڵ�
* @param from ������ʼʱ����ת
* @param to ��������ʱ����ת
* @return �ɹ����� true
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
* ����ƽ�ƶ���
* @param node �����ĳ����ڵ�
* @param from ������ʼʱ��λ��
* @param to ��������ʱ��λ��
* @return �ɹ����� true
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
* ��ʼ����
* @param node �����ĳ����ڵ�
* @param delay ��ʱ delay ���ʼ����
* @return �ɹ����� true
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
* ��ͣ����
* @param node �����ĳ����ڵ�
* @return �ɹ����� true
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
* ֹͣ����
* @param node �����ĳ����ڵ�
* @return �ɹ����� true
*/
bool CAnimationManager::Stop(CSceneNode* node) {
	list<SAnimationData>::iterator iter = m_lstAnimation.begin();
	while (iter != m_lstAnimation.end()) {
		SAnimationData* anim = &*iter;
		if (anim->sceneNode == node) {
			// �Գ����ڵ�任������״̬��ֹͣ
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
* ������ж���
*/
void CAnimationManager::Clear() {
	m_lstAnimation.clear();
}

/**
* ��ȡִ�е����ж����ڵ��б�
* @param animationList ��������ڵ��б�
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