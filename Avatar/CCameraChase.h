//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CCAMERACHASE_H_
#define _CCAMERACHASE_H_
#include "CCamera.h"

/**
* @brief ׷��ʽ���
*
* ��Ի���������ͻ�����������ƽ���ƶ�Ч��
*/
class AVATAR_EXPORT CCameraChase: public CCamera {
public:
	//! Ĭ�Ϲ��캯��
	CCameraChase();

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
	//! ���Ʒ�λ��
	inline void RestrictYawRange(bool shortest);

private:
	//! ����ϵ��
	float m_fDamping;
	//! ���Ԥ����λ��
	float m_fYawInAdvance;
	//! ���Ԥ��������
	float m_fPitchInAdvance;
	//! ���Ԥ��������
	float m_fRollInAdvance;
	//! ���Ԥ��λ��
	CVector3 m_cPosInAdvance;
};

#endif