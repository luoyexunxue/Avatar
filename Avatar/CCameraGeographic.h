//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CCAMERAGEOGRAPHIC_H_
#define _CCAMERAGEOGRAPHIC_H_
#include "CCamera.h"
#include "CQuaternion.h"

/**
* @brief GIS��Ӧ�����
*
* ���ڵ�ͼ��������潨�������
*/
class AVATAR_EXPORT CCameraGeographic: public CCamera {
public:
	//! Ĭ�Ϲ��캯��
	CCameraGeographic();

	//! ��ȡ�������
	virtual const char* GetName() const;

	//! ������Ϣ����
	virtual void Input(CInputManager::SInput* input);
	//! ��������߶�
	virtual void SetHeight(float height);
	//! �������λ��
	virtual void SetPosition(const CVector3& pos);
	//! ���������λ
	virtual void SetAngle(float yaw, float pitch, float roll);
	//! �������Ŀ��
	virtual void SetTarget(const CVector3& pos);

	//! �������
	virtual void Update(float dt);

public:
	//! ��������ϵ��
	void SetDamping(float k);
	//! �����������Ŀ���������
	void SetMinDistance(float distance);
	//! �����������Ŀ����Զ����
	void SetMaxDistance(float distance);

private:
	//! ����ϵ��
	float m_fDamping;
	//! ��С����
	float m_fMinDistance;
	//! ������
	float m_fMaxDistance;
	//! ���Ԥ����λ��
	float m_fYawInAdvance;
	//! ���Ԥ��������
	float m_fPitchInAdvance;
	//! ���Ŀ�����
	float m_fDistance;
	//! ���Ԥ��Ŀ�����
	float m_fDistanceInAdvance;
	//! ���Ŀ��λ��
	CVector3 m_cTargetPos;
	//! ���Ԥ��Ŀ��
	CVector3 m_cTargetInAdvance;
};

#endif