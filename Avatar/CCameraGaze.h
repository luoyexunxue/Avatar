//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CCAMERAGAZE_H_
#define _CCAMERAGAZE_H_
#include "CCamera.h"

/**
* @brief �̶�Ŀ��������
*/
class AVATAR_EXPORT CCameraGaze: public CCamera {
public:
	//! Ĭ�Ϲ��캯��
	CCameraGaze();

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
	//! �����������Ŀ��������
	void SetMaxDistance(float distance);
	//! ���ø����Ƿ�Χ
	void SetPitchRange(float min, float max);

private:
	//! ���Ʒ�λ��
	inline void RestrictYawRange(bool shortest);

private:
	//! ����ϵ��
	float m_fDamping;
	//! ��С����
	float m_fMinDistance;
	//! ������
	float m_fMaxDistance;
	//! �����Ƿ�Χ
	float m_fPitchRange[2];
	//! ���Ԥ����λ��
	float m_fYawInAdvance;
	//! ���Ԥ��������
	float m_fPitchInAdvance;
	//! ���Ԥ��������
	float m_fRollInAdvance;
	//! Ŀ��λ��
	CVector3 m_cTargetPos;
	//! ���Ԥ��λ��
	CVector3 m_cPosInAdvance;
};

#endif