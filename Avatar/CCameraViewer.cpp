//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CCameraViewer.h"
#include "CMatrix4.h"
#include "CVector3.h"
#include <cmath>

/**
* 构造函数
*/
CCameraViewer::CCameraViewer() {
	m_fDamping = 5.0f;
	m_fDistance = 1.0f;
	m_fDistanceInAdvance = m_fDistance;
	m_cPosition.SetValue(0.0f, -1.0f, 0.0f);
	m_cTargetPos.SetValue(0.0f, 0.0f, 0.0f);
}

/**
* 获取相机名称
* @return viewer
*/
const char* CCameraViewer::GetName() const {
	return "viewer";
}

/**
* 控制信息输入
*/
void CCameraViewer::Input(CInputManager::SInput* input) {
	if (!m_bControlAttached) return;
	if (input->bMove) {
		CMatrix4 viewMat = m_cViewMatrix;
		CVector3 movement = CVector3(input->fRightLeft, input->fUpDown, -input->fForthBack, 0.0f);
		CVector3 offset = viewMat.Transpose() * movement;
		// 计算相机预定位置
		CVector3 lookVec = m_cOrientInAdvance * CVector3(0, -m_fDistanceInAdvance, 0);
		CVector3 position = m_cTargetPos + lookVec;
		// 若点积小于零则表示视线反向
		if (lookVec.DotProduct(lookVec + offset) > 0.0f) {
			SetPosition(position + offset);
		}
		// 处理观察距离为零的情况
		else if (m_fDistanceInAdvance < 0.001f && m_cLookVector.DotProduct(offset) > 0.0f) {
			SetPosition(position + offset);
		}
	}
	if (input->bTurn) {
		CQuaternion rotX = CQuaternion().FromAxisAngle(CVector3::X, -input->fPitch);
		CQuaternion rotY = CQuaternion().FromAxisAngle(CVector3::Y, -input->fRoll);
		CQuaternion rotZ = CQuaternion().FromAxisAngle(CVector3::Z, -input->fYaw);
		m_cOrientInAdvance = m_cOrientInAdvance * rotY * rotZ * rotX;
	}
}

/**
* 设置相机高度
*/
void CCameraViewer::SetHeight(float height) {
	CVector3 position = m_cTargetPos - m_cOrientInAdvance * CVector3(0, m_fDistanceInAdvance, 0);
	position[2] = height;
	SetPosition(position);
}

/**
* 设置相机位置
*/
void CCameraViewer::SetPosition(const CVector3& pos) {
	m_cPosition = pos;
	m_fDistanceInAdvance = (pos - m_cTargetPos).Length();
}

/**
* 设置相机方位
*/
void CCameraViewer::SetAngle(float yaw, float pitch, float roll) {
	CMatrix4 rotMat;
	rotMat.RotateY(roll);
	rotMat.RotateX(pitch);
	rotMat.RotateZ(yaw);
	m_cOrientInAdvance.FromMatrix(rotMat);
}

/**
* 设置相机目标
*/
void CCameraViewer::SetTarget(const CVector3& pos) {
	m_cTargetPos = pos;
}

/**
* 更新相机
*/
void CCameraViewer::Update(float dt) {
	dt *= m_fDamping;
	if (dt > 1.0f) dt = 1.0f;
	// 插值计算
	m_fDistance += (m_fDistanceInAdvance - m_fDistance) * dt;
	m_cOrientation = m_cOrientation.Slerp(m_cOrientInAdvance, dt);
	// 计算相机位置
	m_cPosition = m_cTargetPos + m_cOrientation * CVector3(0, -m_fDistance, 0);
	m_cLookVector = m_cOrientation * CVector3::Y;
	m_cUpVector = m_cOrientation * CVector3::Z;
	CCamera::Update(dt);
}

/**
* 设置阻尼系数
* @param k 阻尼系数，越大惯性表现越小
*/
void CCameraViewer::SetDamping(float k) {
	m_fDamping = k;
}