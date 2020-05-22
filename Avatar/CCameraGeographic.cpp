//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CCameraGeographic.h"
#include "CEngine.h"
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
CCameraGeographic::CCameraGeographic() {
	m_fDamping = 5.0f;
	m_fMinDistance = 5.0f;
	m_fMaxDistance = FLT_MAX;
	m_fYawInAdvance = m_fYaw;
	m_fPitchInAdvance = m_fPitch;
	m_fDistance = 10.0f;
	m_fDistanceInAdvance = m_fDistance;
	m_cPosition.SetValue(0.0f, 0.0f, 0.0f);
	m_cTargetPos.SetValue(0.0f, 1.0f, 0.0f);
	m_cTargetInAdvance.SetValue(m_cTargetPos);
}

/**
* ��ȡ�������
* @return geographic
*/
const char* CCameraGeographic::GetName() const {
	return "geographic";
}

/**
* ������Ϣ����
*/
void CCameraGeographic::Input(CInputManager::SInput* input) {
	if (!m_bControlAttached) return;
	if (input->bMove) {
		CVector3 movement = CVector3(input->fRightLeft, input->fUpDown, -input->fForthBack, 0.0f);
		float targetDistance = m_fDistanceInAdvance * tanf(m_fFieldOfView * 0.5f * 0.017453f);
		float scale_move = 0.0f;
		float scale_turn = 0.0f;
		CEngine::GetInputManager()->MouseSensitivity(false, scale_move, scale_turn);
		float scale = 2.0f * targetDistance / (m_iViewHeight * scale_move);
		CVector3 right = m_cLookVector.CrossProduct(m_cUpVector);
		CVector3 forward = CVector3::Z.CrossProduct(right);
		movement.Scale(scale, scale, -0.01f / scale_move);
		m_cTargetInAdvance += right.Scale(movement[0]) + forward.Scale(movement[1]);
		m_fDistanceInAdvance *= powf(0.98f, movement[2]);
		if (m_fDistanceInAdvance < m_fMinDistance) m_fDistanceInAdvance = m_fMinDistance;
		if (m_fDistanceInAdvance > m_fMaxDistance) m_fDistanceInAdvance = m_fMaxDistance;
	}
	if (input->bTurn) {
		SetAngle(m_fYawInAdvance + input->fYaw, m_fPitchInAdvance + input->fPitch, 0.0f);
	}
}

/**
* ��������߶�
*/
void CCameraGeographic::SetHeight(float height) {
	float distance = m_fDistanceInAdvance * cosf(m_fPitchInAdvance);
	m_fDistanceInAdvance = sqrtf(distance * distance + height * height);
	if (m_fDistanceInAdvance < m_fMinDistance) m_fDistanceInAdvance = m_fMinDistance;
	if (m_fDistanceInAdvance > m_fMaxDistance) m_fDistanceInAdvance = m_fMaxDistance;
	m_fPitchInAdvance = -acosf(distance / m_fDistanceInAdvance);
}

/**
* �������λ��
*/
void CCameraGeographic::SetPosition(const CVector3& pos) {
	CVector3 position = pos;
	if (position[2] < 0.0f) position[2] = 0.0f;
	CVector3 direction = m_cTargetInAdvance - position;
	m_fDistanceInAdvance = direction.Length();
	if (m_fDistanceInAdvance < m_fMinDistance) m_fDistanceInAdvance = m_fMinDistance;
	if (m_fDistanceInAdvance > m_fMaxDistance) m_fDistanceInAdvance = m_fMaxDistance;
	else if (m_fDistanceInAdvance > 0.001f) {
		CVector3 right = direction.Scale(1.0f / m_fDistanceInAdvance).CrossProduct(CVector3::Z);
		if (right.DotProduct(right) < 0.001f) right = direction.Tangent();
		CVector3 up = right.Normalize().CrossProduct(direction);
		GetYawPitchRoll(direction, up, &m_fYawInAdvance, &m_fPitchInAdvance, 0);
		// ��������ת�������� -89�� �� 0��֮��
		const float pitchMin = -1.553343f;
		if (m_fPitchInAdvance < pitchMin) m_fPitchInAdvance = pitchMin;
		else if (m_fPitchInAdvance > 0.0f) m_fPitchInAdvance = 0.0f;
	}
}

/**
* ���������λ
*/
void CCameraGeographic::SetAngle(float yaw, float pitch, float roll) {
	m_fYawInAdvance = yaw;
	m_fPitchInAdvance = pitch;
	// ���Ʒ�λ���� [0, 2PI) ��Χ��
	const float PI2 = 6.283185307f;
	float theta = fmodf(m_fYawInAdvance - m_fYaw, PI2);
	m_fYawInAdvance = fmodf(m_fYawInAdvance, PI2);
	if (m_fYawInAdvance < 0) m_fYawInAdvance += PI2;
	m_fYaw = m_fYawInAdvance - theta;
	// ��������ת�������� -89�� �� 0��֮��
	const float pitchMin = -1.553343f;
	if (m_fPitchInAdvance < pitchMin) m_fPitchInAdvance = pitchMin;
	else if (m_fPitchInAdvance > 0.0f) m_fPitchInAdvance = 0.0f;
}

/**
* �������Ŀ��
*/
void CCameraGeographic::SetTarget(const CVector3& pos) {
	m_cTargetInAdvance = pos;
}

/**
* �������
*/
void CCameraGeographic::Update(float dt) {
	dt *= m_fDamping;
	if (dt > 1.0f) dt = 1.0f;
	// ��ֵ����
	m_fYaw += (m_fYawInAdvance - m_fYaw) * dt;
	m_fPitch += (m_fPitchInAdvance - m_fPitch) * dt;
	m_fDistance += (m_fDistanceInAdvance - m_fDistance) * dt;
	m_cTargetPos += (m_cTargetInAdvance - m_cTargetPos) * dt;
	// �����������
	GetLookVecUpVec(m_fYaw, m_fPitch, m_fRoll, m_cLookVector, m_cUpVector);
	m_cPosition = m_cTargetPos - m_cLookVector * m_fDistance;
	CCamera::Update(dt);
}

/**
* ��������ϵ��
* @param k ����ϵ����Խ����Ա���ԽС
*/
void CCameraGeographic::SetDamping(float k) {
	m_fDamping = k;
}

/**
* �����������Ŀ���������
*/
void CCameraGeographic::SetMinDistance(float distance) {
	m_fMinDistance = distance;
}

/**
* �����������Ŀ����Զ����
*/
void CCameraGeographic::SetMaxDistance(float distance) {
	m_fMaxDistance = distance;
}