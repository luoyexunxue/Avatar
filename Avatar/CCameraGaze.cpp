//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
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
* 构造函数
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
* 获取相机名称
* @return gaze
*/
const char* CCameraGaze::GetName() const {
	return "gaze";
}

/**
* 控制信息输入
*/
void CCameraGaze::Input(CInputManager::SInput* input) {
	if (!m_bControlAttached) return;
	if (input->bMove) {
		CMatrix4 viewMat = m_cViewMatrix;
		CVector3 movement = CVector3(input->fRightLeft, input->fUpDown, -input->fForthBack, 0.0f);
		CVector3 offset = viewMat.Transpose() * movement;
		// 采用向量点积防止过冲
		CVector3 lookVecBefore = m_cTargetPos - m_cPosInAdvance;
		CVector3 lookVecAfter = lookVecBefore - offset;
		// 若点积小于零则表示 LookVector 反向，过冲
		if (lookVecBefore.DotProduct(lookVecAfter) > 0) {
			SetPosition(m_cPosInAdvance + offset);
		}
	}
	if (input->bTurn) {
		m_fYawInAdvance += input->fYaw;
		m_fPitchInAdvance += input->fPitch;
		m_fRollInAdvance += input->fRoll;
		// 限制 Yaw 范围
		RestrictYawRange(false);
		// 旋转角限制在 -89° 到 89°之间
		const float rollMax = 1.553343f;
		if (m_fPitchInAdvance < m_fPitchRange[0]) m_fPitchInAdvance = m_fPitchRange[0];
		else if (m_fPitchInAdvance > m_fPitchRange[1]) m_fPitchInAdvance = m_fPitchRange[1];
		if (m_fRollInAdvance < -rollMax) m_fRollInAdvance = -rollMax;
		else if (m_fRollInAdvance > rollMax) m_fRollInAdvance = rollMax;
		// 重新定位相机位置
		float distance = (m_cPosInAdvance - m_cTargetPos).Length();
		float sina = sinf(m_fYawInAdvance);
		float cosa = cosf(m_fYawInAdvance);
		float cosb = cosf(m_fPitchInAdvance);
		float sinb = sinf(m_fPitchInAdvance);
		m_cPosInAdvance = m_cTargetPos + CVector3(distance * cosb * sina, -distance * cosb * cosa, -distance * sinb);
	}
}

/**
* 设置相机高度
*/
void CCameraGaze::SetHeight(float height) {
	SetPosition(CVector3(m_cPosition[0], m_cPosition[1], height));
}

/**
* 设置相机位置
*/
void CCameraGaze::SetPosition(const CVector3& pos) {
	CVector3 distance = m_cTargetPos - pos;
	float length = distance.Length();
	if (length > 0.0f) distance.Scale(1.0f / length);
	else distance.SetValue(m_cLookVector);
	// 保持最小和最大距离
	if (length < m_fMinDistance) m_cPosInAdvance = m_cTargetPos - distance * m_fMinDistance;
	else if (length > m_fMaxDistance) m_cPosInAdvance = m_cTargetPos - distance * m_fMaxDistance;
	else m_cPosInAdvance = pos;
	// 更新相机方位角
	FromVectorToAngle(distance, m_cUpVector, &m_fYawInAdvance, &m_fPitchInAdvance, 0);
	RestrictYawRange(true);
	// 俯仰角旋转角限制
	const float rollMax = 1.553343f;
	if (m_fPitchInAdvance < m_fPitchRange[0]) m_fPitchInAdvance = m_fPitchRange[0];
	else if (m_fPitchInAdvance > m_fPitchRange[1]) m_fPitchInAdvance = m_fPitchRange[1];
	if (m_fRollInAdvance < -rollMax) m_fRollInAdvance = -rollMax;
	else if (m_fRollInAdvance > rollMax) m_fRollInAdvance = rollMax;
}

/**
* 设置相机方位
*/
void CCameraGaze::SetAngle(float yaw, float pitch, float roll) {
	m_fYawInAdvance = yaw;
	m_fPitchInAdvance = pitch;
	m_fRollInAdvance = roll;
	// 限制 Yaw 范围
	RestrictYawRange(true);
	// 旋转角限制在 -89° 到 89°之间
	const float rollMax = 1.553343f;
	if (m_fPitchInAdvance < m_fPitchRange[0]) m_fPitchInAdvance = m_fPitchRange[0];
	else if (m_fPitchInAdvance > m_fPitchRange[1]) m_fPitchInAdvance = m_fPitchRange[1];
	if (m_fRollInAdvance < -rollMax) m_fRollInAdvance = -rollMax;
	else if (m_fRollInAdvance > rollMax) m_fRollInAdvance = rollMax;
	// 重新定位相机位置
	float distance = (m_cPosInAdvance - m_cTargetPos).Length();
	float sina = sinf(m_fYawInAdvance);
	float cosa = cosf(m_fYawInAdvance);
	float cosb = cosf(m_fPitchInAdvance);
	float sinb = sinf(m_fPitchInAdvance);
	m_cPosInAdvance = m_cTargetPos + CVector3(distance * cosb * sina, -distance * cosb * cosa, -distance * sinb);
}

/**
* 设置相机目标
*/
void CCameraGaze::SetTarget(const CVector3& pos) {
	CVector3 distance = pos - m_cPosInAdvance;
	float length = distance.Length();
	if (length == 0.0f) distance.SetValue(m_cLookVector * m_fMinDistance);
	else if (length < m_fMinDistance) distance.Scale(m_fMinDistance / length);
	else if (length > m_fMaxDistance) distance.Scale(m_fMaxDistance / length);
	m_cTargetPos = pos;
	m_cPosInAdvance = pos - distance;
	// 更新相机方位角
	FromVectorToAngle(distance.Normalize(), m_cUpVector, &m_fYawInAdvance, &m_fPitchInAdvance, 0);
	RestrictYawRange(true);
	// 俯仰角旋转角限制
	const float rollMax = 1.553343f;
	if (m_fPitchInAdvance < m_fPitchRange[0]) m_fPitchInAdvance = m_fPitchRange[0];
	else if (m_fPitchInAdvance > m_fPitchRange[1]) m_fPitchInAdvance = m_fPitchRange[1];
	if (m_fRollInAdvance < -rollMax) m_fRollInAdvance = -rollMax;
	else if (m_fRollInAdvance > rollMax) m_fRollInAdvance = rollMax;
}

/**
* 更新相机
*/
void CCameraGaze::Update(float dt) {
	dt *= m_fDamping;
	if (dt > 1.0f) dt = 1.0f;
	// 差值计算
	m_fYaw += (m_fYawInAdvance - m_fYaw) * dt;
	m_fPitch += (m_fPitchInAdvance - m_fPitch) * dt;
	m_fRoll += (m_fRollInAdvance - m_fRoll) * dt;
	// 计算相机参数
	FromAngleToVector(m_fYaw, m_fPitch, m_fRoll, m_cLookVector, m_cUpVector);
	// 距离目标的平移
	float from = (m_cPosition - m_cTargetPos).Length();
	float to = (m_cPosInAdvance - m_cTargetPos).Length();
	m_cPosition = m_cTargetPos - m_cLookVector * (from + (to - from) * dt);
	CCamera::Update(dt);
}

/**
* 设置阻尼系数
* @param k 阻尼系数，越大惯性表现越小
*/
void CCameraGaze::SetDamping(float k) {
	m_fDamping = k;
}

/**
* 设置相机距离目标最近距离
*/
void CCameraGaze::SetMinDistance(float distance) {
	m_fMinDistance = distance;
}

/**
* 设置相机距离目标点最远距离
*/
void CCameraGaze::SetMaxDistance(float distance) {
	m_fMaxDistance = distance;
}

/**
* 设置相机俯仰角范围
* @param min 最小俯仰角（弧度）
* @param max 最大俯仰角（弧度）
*/
void CCameraGaze::SetPitchRange(float min, float max) {
	const float pitchMax = 1.553343f;
	if (min < -pitchMax) min = -pitchMax;
	else if (min > pitchMax) min = pitchMax;
	if (max < -pitchMax) max = -pitchMax;
	else if (max > pitchMax) max = pitchMax;
	m_fPitchRange[0] = min;
	m_fPitchRange[1] = max;
	if (m_fPitchInAdvance < m_fPitchRange[0]) m_fPitchInAdvance = m_fPitchRange[0];
	if (m_fPitchInAdvance > m_fPitchRange[1]) m_fPitchInAdvance = m_fPitchRange[1];
}

/**
* 限制方位角
*/
void CCameraGaze::RestrictYawRange(bool shortest) {
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