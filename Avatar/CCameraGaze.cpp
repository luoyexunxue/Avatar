//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CCameraGaze.h"
#include "CMatrix4.h"
#include "CVector3.h"
#include <cmath>
#include <cfloat>

#ifndef FLT_MAX
#define FLT_MAX __FLT_MAX__
#endif

/**
* ���캯��
*/
CCameraGaze::CCameraGaze() {
	m_fDamping = 5.0f;
	m_fMinDistance = 1.0f;
	m_fMaxDistance = FLT_MAX;
	m_fPitchRange[0] = -1.553343f;
	m_fPitchRange[1] = 1.553343f;
	m_fYawInAdvance = m_fYaw;
	m_fPitchInAdvance = m_fPitch;
	m_fRollInAdvance = m_fRoll;
	m_cPosition.SetValue(0.0f, -1.0f, 0.0f);
	m_cTargetPos.SetValue(0.0f, 0.0f, 0.0f);
	m_cPosInAdvance.SetValue(m_cPosition);
}

/**
* ��ȡ�������
* @return gaze
*/
const char* CCameraGaze::GetName() const {
	return "gaze";
}

/**
* ������Ϣ����
*/
void CCameraGaze::Input(CInputManager::SInput* input) {
	if (!m_bControlAttached) return;
	if (input->bMove) {
		CMatrix4 viewMat = m_cViewMatrix;
		CVector3 movement = CVector3(input->fRightLeft, input->fUpDown, -input->fForthBack, 0.0f);
		CVector3 offset = viewMat.Transpose() * movement;
		// �������������ֹ����
		CVector3 lookVecBefore = m_cTargetPos - m_cPosInAdvance;
		CVector3 lookVecAfter = lookVecBefore - offset;
		// �����С�������ʾ LookVector ���򣬹���
		if (lookVecBefore.DotProduct(lookVecAfter) > 0) {
			SetPosition(m_cPosInAdvance + offset);
		}
	}
	if (input->bTurn) {
		m_fYawInAdvance += input->fYaw;
		m_fPitchInAdvance += input->fPitch;
		m_fRollInAdvance += input->fRoll;
		// ���� Yaw ��Χ
		RestrictYawRange(false);
		// ��������ת�������� -89�� �� 89��֮��
		if (m_fPitchInAdvance < m_fPitchRange[0]) m_fPitchInAdvance = m_fPitchRange[0];
		else if (m_fPitchInAdvance > m_fPitchRange[1]) m_fPitchInAdvance = m_fPitchRange[1];
		if (m_fRollInAdvance < -1.553343f) m_fRollInAdvance = -1.553343f;
		else if (m_fRollInAdvance > 1.553343f) m_fRollInAdvance = 1.553343f;
		// ���¶�λ���λ��
		float distance = (m_cPosInAdvance - m_cTargetPos).Length();
		float sina = sinf(m_fYawInAdvance);
		float cosa = cosf(m_fYawInAdvance);
		float cosb = cosf(m_fPitchInAdvance);
		float sinb = sinf(m_fPitchInAdvance);
		m_cPosInAdvance = m_cTargetPos + CVector3(distance * cosb * sina, -distance * cosb * cosa, -distance * sinb);
	}
}

/**
* ��������߶�
*/
void CCameraGaze::SetHeight(float height) {
	SetPosition(CVector3(m_cPosition[0], m_cPosition[1], height));
}

/**
* �������λ��
*/
void CCameraGaze::SetPosition(const CVector3& pos) {
	CVector3 distance = m_cTargetPos - pos;
	float length = distance.Length();
	if (length > 0.0f) distance.Scale(1.0f / length);
	else distance.SetValue(m_cLookVector);
	// ������С��������
	if (length < m_fMinDistance) m_cPosInAdvance = m_cTargetPos - distance * m_fMinDistance;
	else if (length > m_fMaxDistance) m_cPosInAdvance = m_cTargetPos - distance * m_fMaxDistance;
	else m_cPosInAdvance = pos;
	// ���������λ��
	GetYawPitchRoll(distance, m_cUpVector, &m_fYawInAdvance, &m_fPitchInAdvance, 0);
	RestrictYawRange(true);
	// ��������ת������
	if (m_fPitchInAdvance < m_fPitchRange[0]) m_fPitchInAdvance = m_fPitchRange[0];
	else if (m_fPitchInAdvance > m_fPitchRange[1]) m_fPitchInAdvance = m_fPitchRange[1];
	if (m_fRollInAdvance < -1.553343f) m_fRollInAdvance = -1.553343f;
	else if (m_fRollInAdvance > 1.553343f) m_fRollInAdvance = 1.553343f;
}

/**
* ���������λ
*/
void CCameraGaze::SetAngle(float yaw, float pitch, float roll) {
	m_fYawInAdvance = yaw;
	m_fPitchInAdvance = pitch;
	m_fRollInAdvance = roll;
	// ���� Yaw ��Χ
	RestrictYawRange(true);
	// ��������ת�������� -89�� �� 89��֮��
	if (m_fPitchInAdvance < m_fPitchRange[0]) m_fPitchInAdvance = m_fPitchRange[0];
	else if (m_fPitchInAdvance > m_fPitchRange[1]) m_fPitchInAdvance = m_fPitchRange[1];
	if (m_fRollInAdvance < -1.553343f) m_fRollInAdvance = -1.553343f;
	else if (m_fRollInAdvance > 1.553343f) m_fRollInAdvance = 1.553343f;
	// ���¶�λ���λ��
	float distance = (m_cPosInAdvance - m_cTargetPos).Length();
	float sina = sinf(m_fYawInAdvance);
	float cosa = cosf(m_fYawInAdvance);
	float cosb = cosf(m_fPitchInAdvance);
	float sinb = sinf(m_fPitchInAdvance);
	m_cPosInAdvance = m_cTargetPos + CVector3(distance * cosb * sina, -distance * cosb * cosa, -distance * sinb);
}

/**
* �������Ŀ��
*/
void CCameraGaze::SetTarget(const CVector3& pos) {
	CVector3 distance = pos - m_cPosInAdvance;
	float length = distance.Length();
	if (length == 0.0f) distance.SetValue(m_cLookVector * m_fMinDistance);
	else if (length < m_fMinDistance) distance.Scale(m_fMinDistance / length);
	else if (length > m_fMaxDistance) distance.Scale(m_fMaxDistance / length);
	m_cTargetPos = pos;
	m_cPosInAdvance = pos - distance;
	// ���������λ��
	GetYawPitchRoll(distance.Normalize(), m_cUpVector, &m_fYawInAdvance, &m_fPitchInAdvance, 0);
	RestrictYawRange(true);
	// ��������ת������
	if (m_fPitchInAdvance < m_fPitchRange[0]) m_fPitchInAdvance = m_fPitchRange[0];
	else if (m_fPitchInAdvance > m_fPitchRange[1]) m_fPitchInAdvance = m_fPitchRange[1];
	if (m_fRollInAdvance < -1.553343f) m_fRollInAdvance = -1.553343f;
	else if (m_fRollInAdvance > 1.553343f) m_fRollInAdvance = 1.553343f;
}

/**
* �������
*/
void CCameraGaze::Update(float dt) {
	dt *= m_fDamping;
	if (dt > 1.0f) dt = 1.0f;
	// ��ֵ����
	m_fYaw += (m_fYawInAdvance - m_fYaw) * dt;
	m_fPitch += (m_fPitchInAdvance - m_fPitch) * dt;
	m_fRoll += (m_fRollInAdvance - m_fRoll) * dt;
	// �����������
	GetLookVecUpVec(m_fYaw, m_fPitch, m_fRoll, m_cLookVector, m_cUpVector);
	// ����Ŀ���ƽ��
	float from = (m_cPosition - m_cTargetPos).Length();
	float to = (m_cPosInAdvance - m_cTargetPos).Length();
	m_cPosition = m_cTargetPos - m_cLookVector * (from + (to - from) * dt);
	CCamera::Update(dt);
}

/**
* ��������ϵ��
* @param k ����ϵ����Խ����Ա���ԽС
*/
void CCameraGaze::SetDamping(float k) {
	m_fDamping = k;
}

/**
* �����������Ŀ���������
*/
void CCameraGaze::SetMinDistance(float distance) {
	m_fMinDistance = distance;
}

/**
* �����������Ŀ�����Զ����
*/
void CCameraGaze::SetMaxDistance(float distance) {
	m_fMaxDistance = distance;
}

/**
* ������������Ƿ�Χ
* @param min ��С�����ǣ����ȣ�
* @param max ������ǣ����ȣ�
*/
void CCameraGaze::SetPitchRange(float min, float max) {
	if (min < -1.553343f) min  = -1.553343f;
	else if (min > 1.553343f) min  = 1.553343f;
	if (max < -1.553343f) max  = -1.553343f;
	else if (max > 1.553343f) max  = 1.553343f;
	m_fPitchRange[0] = min;
	m_fPitchRange[1] = max;
	if (m_fPitchInAdvance < m_fPitchRange[0]) m_fPitchInAdvance = m_fPitchRange[0];
	if (m_fPitchInAdvance > m_fPitchRange[1]) m_fPitchInAdvance = m_fPitchRange[1];
}

/**
* ���Ʒ�λ��
*/
void CCameraGaze::RestrictYawRange(bool shortest) {
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