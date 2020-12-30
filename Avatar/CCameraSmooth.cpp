//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CCameraSmooth.h"
#include "CMatrix4.h"
#include "CVector3.h"
#include <cmath>

/**
* 构造函数
*/
CCameraSmooth::CCameraSmooth() {
	m_fDamping = 5.0f;
	m_fYawInAdvance = m_fYaw;
	m_fPitchInAdvance = m_fPitch;
	m_fRollInAdvance = m_fRoll;
	m_cPosInAdvance = m_cPosition;
}

/**
* 获取相机名称
* @return smooth
*/
const char* CCameraSmooth::GetName() const {
	return "smooth";
}

/**
* 控制信息输入
*/
void CCameraSmooth::Input(CInputManager::SInput* input) {
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
		// 俯仰角旋转角限制在 -89° 到 89°之间
		const float maxAngle = 1.553343f;
		if (m_fPitchInAdvance < -maxAngle) m_fPitchInAdvance = -maxAngle;
		else if (m_fPitchInAdvance > maxAngle) m_fPitchInAdvance = maxAngle;
		if (m_fRollInAdvance < -maxAngle) m_fRollInAdvance = -maxAngle;
		else if (m_fRollInAdvance > maxAngle) m_fRollInAdvance = maxAngle;
	}
}

/**
* 设置相机高度
*/
void CCameraSmooth::SetHeight(float height) {
	m_cPosInAdvance[2] = height;
	m_cPosition[2] = height;
}

/**
* 设置相机位置
*/
void CCameraSmooth::SetPosition(const CVector3& pos) {
	m_cPosInAdvance = pos;
}

/**
* 设置相机方位
*/
void CCameraSmooth::SetAngle(float yaw, float pitch, float roll) {
	m_fYawInAdvance = yaw;
	m_fPitchInAdvance = pitch;
	m_fRollInAdvance = roll;
	RestrictYawRange(true);
	// 俯仰角旋转角限制在 -89° 到 89°之间
	const float maxAngle = 1.553343f;
	if (m_fPitchInAdvance < -maxAngle) m_fPitchInAdvance = -maxAngle;
	else if (m_fPitchInAdvance > maxAngle) m_fPitchInAdvance = maxAngle;
	if (m_fRollInAdvance < -maxAngle) m_fRollInAdvance = -maxAngle;
	else if (m_fRollInAdvance > maxAngle) m_fRollInAdvance = maxAngle;
}

/**
* 设置相机目标
*/
void CCameraSmooth::SetTarget(const CVector3& pos) {
	CVector3 lookVec = pos - m_cPosInAdvance;
	if (lookVec.Length() == 0) return;
	// 更新相机方位角
	lookVec.Normalize();
	FromVectorToAngle(lookVec, m_cUpVector, &m_fYawInAdvance, &m_fPitchInAdvance, 0);
	// 限制 Yaw 范围
	RestrictYawRange(true);
}

/**
* 更新相机
*/
void CCameraSmooth::Update(float dt) {
	dt *= m_fDamping;
	if (dt > 1.0f) dt = 1.0f;
	// 差值计算
	m_fYaw += (m_fYawInAdvance - m_fYaw) * dt;
	m_fPitch += (m_fPitchInAdvance - m_fPitch) * dt;
	m_fRoll += (m_fRollInAdvance - m_fRoll) * dt;
	m_cPosition += (m_cPosInAdvance - m_cPosition) * dt;
	// 更新 Look 和 Up 向量
	FromAngleToVector(m_fYaw, m_fPitch, m_fRoll, m_cLookVector, m_cUpVector);
	CCamera::Update(dt);
}

/**
* 设置阻尼系数
* @param k 阻尼系数，越大惯性表现越小
*/
void CCameraSmooth::SetDamping(float k) {
	m_fDamping = k;
}

/**
* 限制方位角
*/
void CCameraSmooth::RestrictYawRange(bool shortest) {
	const float PI = 3.141592654f;
	const float PI2 = 6.283185307f;
	// 保存角度差值
	float theta = fmodf(m_fYawInAdvance - m_fYaw, PI2);
	// 限制方位角在 [0, 2PI) 范围内
	m_fYawInAdvance = fmodf(m_fYawInAdvance, PI2);
	if (m_fYawInAdvance < 0) m_fYawInAdvance += PI2;

	// 根据角度差值判断转动方向
	if (shortest) {
		if (theta > PI) m_fYaw = m_fYawInAdvance - theta + PI2;
		else if (theta < -PI) m_fYaw = m_fYawInAdvance - theta - PI2;
		else m_fYaw = m_fYawInAdvance - theta;
	} else {
		m_fYaw = m_fYawInAdvance - theta;
	}
}