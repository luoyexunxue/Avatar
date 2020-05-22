//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CCameraViewer.h"
#include "CMatrix4.h"
#include "CVector3.h"
#include <cmath>

/**
* ���캯��
*/
CCameraViewer::CCameraViewer() {
	m_fDamping = 5.0f;
	m_fDistance = 1.0f;
	m_fDistanceInAdvance = m_fDistance;
	m_cPosition.SetValue(0.0f, -1.0f, 0.0f);
	m_cTargetPos.SetValue(0.0f, 0.0f, 0.0f);
}

/**
* ��ȡ�������
* @return viewer
*/
const char* CCameraViewer::GetName() const {
	return "viewer";
}

/**
* ������Ϣ����
*/
void CCameraViewer::Input(CInputManager::SInput* input) {
	if (!m_bControlAttached) return;
	if (input->bMove) {
		CMatrix4 viewMat = m_cViewMatrix;
		CVector3 movement = CVector3(input->fRightLeft, input->fUpDown, -input->fForthBack, 0.0f);
		CVector3 offset = viewMat.Transpose() * movement;
		// �������Ԥ��λ��
		CVector3 lookVec = m_cOrientInAdvance * CVector3(0, -m_fDistanceInAdvance, 0);
		CVector3 position = m_cTargetPos + lookVec;
		// �����С�������ʾ���߷���
		if (lookVec.DotProduct(lookVec + offset) > 0.0f) {
			SetPosition(position + offset);
		}
		// ����۲����Ϊ������
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
* ��������߶�
*/
void CCameraViewer::SetHeight(float height) {
	CVector3 position = m_cTargetPos - m_cOrientInAdvance * CVector3(0, m_fDistanceInAdvance, 0);
	position[2] = height;
	SetPosition(position);
}

/**
* �������λ��
*/
void CCameraViewer::SetPosition(const CVector3& pos) {
	m_cPosition = pos;
	m_fDistanceInAdvance = (pos - m_cTargetPos).Length();
}

/**
* ���������λ
*/
void CCameraViewer::SetAngle(float yaw, float pitch, float roll) {
	CMatrix4 rotMat;
	rotMat.RotateY(roll);
	rotMat.RotateX(pitch);
	rotMat.RotateZ(yaw);
	m_cOrientInAdvance.FromMatrix(rotMat);
}

/**
* �������Ŀ��
*/
void CCameraViewer::SetTarget(const CVector3& pos) {
	m_cTargetPos = pos;
}

/**
* �������
*/
void CCameraViewer::Update(float dt) {
	dt *= m_fDamping;
	if (dt > 1.0f) dt = 1.0f;
	// ��ֵ����
	m_fDistance += (m_fDistanceInAdvance - m_fDistance) * dt;
	m_cOrientation = m_cOrientation.Slerp(m_cOrientInAdvance, dt);
	// �������λ��
	m_cPosition = m_cTargetPos + m_cOrientation * CVector3(0, -m_fDistance, 0);
	m_cLookVector = m_cOrientation * CVector3::Y;
	m_cUpVector = m_cOrientation * CVector3::Z;
	CCamera::Update(dt);
}

/**
* ��������ϵ��
* @param k ����ϵ����Խ����Ա���ԽС
*/
void CCameraViewer::SetDamping(float k) {
	m_fDamping = k;
}