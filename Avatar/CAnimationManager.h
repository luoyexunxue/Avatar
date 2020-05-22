//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* @brief ����������
*/
class AVATAR_EXPORT CAnimationManager {
public:
	//! ��ȡ������ʵ��
	static CAnimationManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CAnimationManager();
		return m_pInstance;
	}

	//! ʵ������
	void Destroy();
	//! ��������
	void Update(float dt);

public:
	//! ֧�ֵĶ�����ֵ��ʽ
	enum Interpolator { LINEAR, ACCELERATE, DECELERATE, ACCELERATEDECELERATE };

public:
	//! ���ö�������
	void SetAnimation(CSceneNode* node, Interpolator intepolator, float duration, int repeat, bool swing);
	//! �������Ŷ���
	bool AnimateScale(CSceneNode* node, const CVector3& from, const CVector3& to);
	//! ������ת����
	bool AnimateRotation(CSceneNode* node, const CQuaternion& from, const CQuaternion& to);
	//! ����ƽ�ƶ���
	bool AnimateTranslation(CSceneNode* node, const CVector3& from, const CVector3& to);

	//! ��ʼ����
	bool Start(CSceneNode* node, float delay = 0.0f);
	//! ��ͣ����
	bool Pause(CSceneNode* node);
	//! ֹͣ����
	bool Stop(CSceneNode* node);
	//! ������ж���
	void Clear();

	//! ��ȡִ�е����ж����ڵ��б�
	void GetAnimationList(vector<CSceneNode*>& animationList);

private:
	CAnimationManager();
	~CAnimationManager();

private:
	//! �������ݶ���
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
		//! �ڲ�����
		bool useScale;
		bool useOrientation;
		bool usePosition;
		bool reversing;
		bool animationStart;
		float animationTime;
		float animationDelay;
		int animationCount;
		//! �Ƚ����������
		bool operator == (const _SAnimationData& other) const {
			return this->sceneNode == other.sceneNode;
		}
	} SAnimationData;

private:
	//! �����б�
	list<SAnimationData> m_lstAnimation;
	//! ʵ��
	static CAnimationManager* m_pInstance;
};

#endif