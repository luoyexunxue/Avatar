//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
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
* 构造函数
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
* 获取相机名称
* @return geographic
*/
const char* CCameraGeographic::GetName() const {
	return "geographic";
}

/**
* 控制信息输入
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
* 设置相机高度
*/
void CCameraGeographic::SetHeight(float height) {
	float distance = m_fDistanceInAdvance * cosf(m_fPitchInAdvance);
	m_fDistanceInAdvance = sqrtf(distance * distance + height * height);
	if (m_fDistanceInAdvance < m_fMinDistance) m_fDistanceInAdvance = m_fMinDistance;
	if (m_fDistanceInAdvance > m_fMaxDistance) m_fDistanceInAdvance = m_fMaxDistance;
	m_fPitchInAdvance = -acosf(distance / m_fDistanceInAdvance);
}

/**
* 设置相机位置
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
		// 俯仰角旋转角限制在 -89° 到 0°之间
		const float pitchMin = -1.553343f;
		if (m_fPitchInAdvance < pitchMin) m_fPitchInAdvance = pitchMin;
		else if (m_fPitchInAdvance > 0.0f) m_fPitchInAdvance = 0.0f;
	}
}

/**
* 设置相机方位
*/
void CCameraGeographic::SetAngle(float yaw, float pitch, float roll) {
	m_fYawInAdvance = yaw;
	m_fPitchInAdvance = pitch;
	// 限制方位角在 [0, 2PI) 范围内
	const float PI2 = 6.283185307f;
	float theta = fmodf(m_fYawInAdvance - m_fYaw, PI2);
	m_fYawInAdvance = fmodf(m_fYawInAdvance, PI2);
	if (m_fYawInAdvance < 0) m_fYawInAdvance += PI2;
	m_fYaw = m_fYawInAdvance - theta;
	// 俯仰角旋转角限制在 -89° 到 0°之间
	const float pitchMin = -1.553343f;
	if (m_fPitchInAdvance < pitchMin) m_fPitchInAdvance = pitchMin;
	else if (m_fPitchInAdvance > 0.0f) m_fPitchInAdvance = 0.0f;
}

/**
* 设置相机目标
*/
void CCameraGeographic::SetTarget(const CVector3& pos) {
	m_cTargetInAdvance = pos;
}

/**
* 更新相机
*/
void CCameraGeographic::Update(float dt) {
	dt *= m_fDamping;
	if (dt > 1.0f) dt = 1.0f;
	// 插值计算
	m_fYaw += (m_fYawInAdvance - m_fYaw) * dt;
	m_fPitch += (m_fPitchInAdvance - m_fPitch) * dt;
	m_fDistance += (m_fDistanceInAdvance - m_fDistance) * dt;
	m_cTargetPos += (m_cTargetInAdvance - m_cTargetPos) * dt;
	// 计算相机参数
	GetLookVecUpVec(m_fYaw, m_fPitch, m_fRoll, m_cLookVector, m_cUpVector);
	m_cPosition = m_cTargetPos - m_cLookVector * m_fDistance;
	CCamera::Update(dt);
}

/**
* 设置阻尼系数
* @param k 阻尼系数，越大惯性表现越小
*/
void CCameraGeographic::SetDamping(float k) {
	m_fDamping = k;
}

/**
* 设置相机距离目标最近距离
*/
void CCameraGeographic::SetMinDistance(float distance) {
	m_fMinDistance = distance;
}

/**
* 设置相机距离目标最远距离
*/
void CCameraGeographic::SetMaxDistance(float distance) {
	m_fMaxDistance = distance;
}