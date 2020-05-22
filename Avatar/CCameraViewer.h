//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CCAMERAVIEWER_H_
#define _CCAMERAVIEWER_H_
#include "CCamera.h"
#include "CQuaternion.h"

/**
* @brief ģ����������
*
* ȫ��λ�۲죬�����������۲����壬���ģ��
*/
class AVATAR_EXPORT CCameraViewer: public CCamera {
public:
	//! Ĭ�Ϲ��캯��
	CCameraViewer();

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

private:
	//! ����ϵ��
	float m_fDamping;
	//! ���Ŀ�����
	float m_fDistance;
	//! ���Ԥ��Ŀ�����
	float m_fDistanceInAdvance;
	//! Ŀ��λ��
	CVector3 m_cTargetPos;
	//! �����λ
	CQuaternion m_cOrientation;
	//! ���Ԥ����λ
	CQuaternion m_cOrientInAdvance;
};

#endif