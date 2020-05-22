//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CCameraChase.h"
#include "CMatrix4.h"
#include "CVector3.h"
#include <cmath>

/**
* ���캯��
*/
CCameraChase::CCameraChase() {
	m_fDamping = 5.0f;
	m_fYawInAdvance = m_fYaw;
	m_fPitchInAdvance = m_fPitch;
	m_fRollInAdvance = m_fRoll;
	m_cPosInAdvance = m_cPosition;
}

/**
* ��ȡ�������
* @return chase
*/
const char* CCameraChase::GetName() const {
	return "chase";
}

/**
* ������Ϣ����
*/
void CCameraChase::Input(CInputManager::SInput* input) {
	if (!m_bControlAttached) return;
	if (input->bMove) {
		CMatrix4 viewMat = m_cViewMatrix;
		CVector3 movement = CVector3(input->fRightLeft, input->fUpDown, -input->fForthBack, 0.0f);
		m_cPosInAdvance += viewMat.Transpose() * movement;
	}
	if (input->bTurn) {
		m_fYawInAdvance += input->fYaw;
		m_fPitchInAdvance += input->fPitch;
		m_fRollInAdvance += input->fRoll;
		RestrictYawRange(false);
		// ��������ת�������� -89�� �� 89��֮��
		if (m_fPitchInAdvance < -1.553343f) m_fPitchInAdvance = -1.553343f;
		else if (m_fPitchInAdvance > 1.553343f) m_fPitchInAdvance = 1.553343f;
		if (m_fRollInAdvance < -1.553343f) m_fRollInAdvance = -1.553343f;
		else if (m_fRollInAdvance > 1.553343f) m_fRollInAdvance = 1.553343f;
	}
}

/**
* ��������߶�
*/
void CCameraChase::SetHeight(float height) {
	m_cPosInAdvance[2] = height;
	m_cPosition[2] = height;
}

/**
* �������λ��
*/
void CCameraChase::SetPosition(const CVector3& pos) {
	m_cPosInAdvance = pos;
}

/**
* ���������λ
*/
void CCameraChase::SetAngle(float yaw, float pitch, float roll) {
	m_fYawInAdvance = yaw;
	m_fPitchInAdvance = pitch;
	m_fRollInAdvance = roll;
	RestrictYawRange(true);
	// ��������ת�������� -89�� �� 89��֮��
	if (m_fPitchInAdvance < -1.553343f) m_fPitchInAdvance  = -1.553343f;
	else if (m_fPitchInAdvance > 1.553343f) m_fPitchInAdvance  = 1.553343f;
	if (m_fRollInAdvance < -1.553343f) m_fRollInAdvance  = -1.553343f;
	else if (m_fRollInAdvance > 1.553343f) m_fRollInAdvance  = 1.553343f;
}

/**
* �������Ŀ��
*/
void CCameraChase::SetTarget(const CVector3& pos) {
	CVector3 lookVec = pos - m_cPosInAdvance;
	if (lookVec.Length() == 0) return;
	// ���������λ��
	lookVec.Normalize();
	GetYawPitchRoll(lookVec, m_cUpVector, &m_fYawInAdvance, &m_fPitchInAdvance, 0);
	// ���� Yaw ��Χ
	RestrictYawRange(true);
}

/**
* �������
*/
void CCameraChase::Update(float dt) {
	dt *= m_fDamping;
	if (dt > 1.0f) dt = 1.0f;
	// ��ֵ����
	m_fYaw += (m_fYawInAdvance - m_fYaw) * dt;
	m_fPitch += (m_fPitchInAdvance - m_fPitch) * dt;
	m_fRoll += (m_fRollInAdvance - m_fRoll) * dt;
	m_cPosition += (m_cPosInAdvance - m_cPosition) * dt;
	// ���� Look �� Up ����
	GetLookVecUpVec(m_fYaw, m_fPitch, m_fRoll, m_cLookVector, m_cUpVector);
	CCamera::Update(dt);
}

/**
* ��������ϵ��
* @param k ����ϵ����Խ����Ա���ԽС
*/
void CCameraChase::SetDamping(float k) {
	m_fDamping = k;
}

/**
* ���Ʒ�λ��
*/
void CCameraChase::RestrictYawRange(bool shortest) {
	const float PI = 3.141592654f;
	const float PI2 = 6.283185307f;
	// ����ǶȲ�ֵ
	float theta = fmodf(m_fYawInAdvance - m_fYaw, PI2);
	// ���Ʒ�λ���� [0, 2PI) ��Χ��
	m_fYawInAdvance = fmodf(m_fYawInAdvance, PI2);
	if (m_fYawInAdvance < 0) m_fYawInAdvance += PI2;

	// ���ݽǶȲ�ֵ�ж�ת������
	if (shortest) {
		if (theta > PI) m_fYaw = m_fYawInAdvance - theta + PI2;
		else if (theta < -PI) m_fYaw = m_fYawInAdvance - theta - PI2;
		else m_fYaw = m_fYawInAdvance - theta;
	} else {
		m_fYaw = m_fYawInAdvance - theta;
	}
}