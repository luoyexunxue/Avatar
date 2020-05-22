//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CCamera.h"
#include "CMatrix4.h"
#include "CVector3.h"
#include <cmath>

/**
* ���캯��
*/
CCamera::CCamera() {
	m_cPosition.SetValue(0.0f, 0.0f, 0.0f);
	m_cLookVector.SetValue(CVector3::Y);
	m_cUpVector.SetValue(CVector3::Z);
	m_fYaw = 0.0f;
	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_bControlAttached = true;
	m_iViewWidth = 240;
	m_iViewHeight = 320;
	m_fFieldOfView = 60.0f;
	m_fClipNear = 0.2f;
	m_fClipFar = 2000.0f;
	m_pBindNode = 0;
}

/**
* ��������
*/
CCamera::~CCamera() {
}

/**
* ��ȡ�������
* @return default
*/
const char* CCamera::GetName() const {
	return "default";
}

/**
* ������Ϣ����
*/
void CCamera::Input(CInputManager::SInput* input) {
	if (!m_bControlAttached) return;
	if (input->bPosition) {
		m_cPosition.SetValue(input->fPosition);
	} else if (input->bMove) {
		CMatrix4 viewMat = m_cViewMatrix;
		CVector3 movement = CVector3(input->fRightLeft, input->fUpDown, -input->fForthBack, 0.0f);
		m_cPosition += viewMat.Transpose() * movement;
	}
	if (input->bOrientation) {
		CQuaternion orient(input->fOrientation);
		m_cLookVector = orient * CVector3::Y;
		m_cUpVector = orient * CVector3::Z;
	} else if (input->bTurn) {
		SetAngle(m_fYaw + input->fYaw, m_fPitch + input->fPitch, m_fRoll + input->fRoll);
	}
}

/**
* ��������߶�
*/
void CCamera::SetHeight(float height) {
	m_cPosition[2] = height;
}

/**
* �������λ��
*/
void CCamera::SetPosition(const CVector3& pos) {
	m_cPosition = pos;
}

/**
* ���������λ
*/
void CCamera::SetAngle(float yaw, float pitch, float roll) {
	m_fYaw = yaw;
	m_fPitch = pitch;
	m_fRoll = roll;
	// ��������ת�������� -89�� �� 89��֮��
	const float maxAngle = 1.553343f;
	if (m_fPitch < -maxAngle) m_fPitch  = -maxAngle;
	else if (m_fPitch > maxAngle) m_fPitch  = maxAngle;
	if (m_fRoll < -maxAngle) m_fRoll  = -maxAngle;
	else if (m_fRoll > maxAngle) m_fRoll  = maxAngle;
	// ���� LookVector UpVector ����
	GetLookVecUpVec(m_fYaw, m_fPitch, m_fRoll, m_cLookVector, m_cUpVector);
}

/**
* �������Ŀ��
*/
void CCamera::SetTarget(const CVector3& pos) {
	CVector3 lookVec = pos - m_cPosition;
	if (lookVec.Length() == 0) return;
	lookVec.Normalize();
	m_cLookVector = lookVec;
	// ���� Yaw��Pitch��Roll ֵ
	GetYawPitchRoll(m_cLookVector, m_cUpVector, &m_fYaw, &m_fPitch, &m_fRoll);
}

/**
* �������
*/
void CCamera::Update(float dt) {
	if (m_pBindNode) {
		m_cPosition = m_pBindNode->m_cWorldMatrix * m_cBindPosition;
		m_cLookVector = m_pBindNode->m_cWorldMatrix * (m_cBindOrientation * CVector3::Y);
		m_cUpVector = m_pBindNode->m_cWorldMatrix * (m_cBindOrientation * CVector3::Z);
		m_cLookVector.Normalize();
		m_cUpVector.Normalize();
	}
	UpdateViewMatrix();
	UpdateFrustum();
}

/**
* ��ȡ�ӿڿ��
*/
int CCamera::GetViewWidth() const {
	return m_iViewWidth;
}

/**
* ��ȡ�ӿڸ߶�
*/
int CCamera::GetViewHeight() const {
	return m_iViewHeight;
}

/**
* ��ȡ����ӽ�
*/
float CCamera::GetFieldOfView() const {
	return m_fFieldOfView;
}

/**
* ��ȡ�ӿڿ�߱�
*/
float CCamera::GetAspectRatio() const {
	const float vw = static_cast<float>(m_iViewWidth);
	const float vh = static_cast<float>(m_iViewHeight);
	return vw / vh;
}

/**
* ��ȡ���ü������
*/
float CCamera::GetNearClipDistance() const {
	return m_fClipNear;
}

/**
* ��ȡԶ�ü������
*/
float CCamera::GetFarClipDistance() const {
	return m_fClipFar;
}

/**
* ��������ӿڴ�С
*/
void CCamera::SetViewSize(int width, int height) {
	m_iViewWidth = width;
	m_iViewHeight = height;
}

/**
* ��������ӽ�
* @param fov Ϊ����ˮƽ�����ӽǣ���λΪ�Ƕ�
*/
void CCamera::SetFieldOfView(float fov) {
	m_fFieldOfView = fov;
}

/**
* ���òü������
*/
void CCamera::SetClipDistance(float zNear, float zFar) {
	m_fClipNear = zNear;
	m_fClipFar = zFar;
}

/**
* ����ͶӰ����
* @param ortho �Ƿ�Ϊ����ͶӰ
*/
void CCamera::UpdateProjMatrix(bool ortho) {
	const float w = static_cast<float>(m_iViewWidth);
	const float h = static_cast<float>(m_iViewHeight);
	if (ortho) {
		m_cProjMatrix.Ortho(w, h, 0.0f, m_fClipFar);
	} else {
		m_cProjMatrix.Perspective(m_fFieldOfView, w / h, m_fClipNear, m_fClipFar);
	}
}

/**
* ������ͼ����
*/
void CCamera::UpdateViewMatrix() {
	m_cViewMatrix.LookAt(m_cPosition, m_cLookVector, m_cUpVector);
}

/**
* ������׶��
*/
void CCamera::UpdateFrustum() {
	m_cFrustum.FromViewProj(m_cProjMatrix * m_cViewMatrix);
}

/**
* ��ȡͶӰ��������
*/
CMatrix4& CCamera::GetProjMatrix() {
	return m_cProjMatrix;
}

/**
* ��ȡ��ͼ��������
*/
CMatrix4& CCamera::GetViewMatrix() {
	return m_cViewMatrix;
}

/**
* ��ȡ��׶������
*/
const CFrustum& CCamera::GetFrustum() {
	return m_cFrustum;
}

/**
* ���������
* @param enable �Ƿ����������������
*/
void CCamera::Control(bool enable) {
	m_bControlAttached = enable;
}

/**
* ����ָ�������ڵ�
* @param sceneNode �󶨵ĳ����ڵ㣬Ϊ�ս������
* @param pos λ��ƫ��
* @param orient �Ƕ�ƫ��
* @attention ���������ڵ���൱�ڽ�����̶��� sceneNode ���棬���ҽ��� sceneNode �Ŀ���
*/
void CCamera::Bind(CSceneNode* sceneNode, const CVector3& pos, const CQuaternion& orient) {
	m_pBindNode = sceneNode;
	m_cBindPosition = pos;
	m_cBindOrientation = orient;
}

/**
* ���������λ�ǣ������ǣ���ת��
* @param lookVec �����������
* @param upVec ���������
* @param yaw ����õ���ˮƽ��
* @param pitch ����õ��ĸ�����
* @param roll ����õ��ķ�����
*/
void CCamera::GetYawPitchRoll(const CVector3& lookVec, const CVector3& upVec, float* yaw, float* pitch, float* roll) {
	float y = -atan2f(lookVec.m_fValue[0], lookVec.m_fValue[1]);
	float p = asinf(lookVec.m_fValue[2]);
	// Roll �Ƕȼ���ϸ��ӣ�-90 ~ 90��
	float siny = sinf(y);
	float cosy = cosf(y);
	float sinp = sinf(p);
	float cosp = cosf(p);
	// δ Roll ��ת�� Up ����
	CVector3 upVecOrg(sinp * siny, -sinp * cosy, cosp);
	float cosr = upVecOrg.DotProduct(upVec);
	if (cosr < -1.0f) cosr = -1.0f;
	else if (cosr > 1.0f) cosr = 1.0f;
	float r = acosf(cosr);
	// Roll ����
	if (upVecOrg.CrossProduct(upVec).DotProduct(lookVec) < 0) r = -r;
	if (yaw) *yaw = y;
	if (pitch) *pitch = p;
	if (roll) *roll = r;
}

/**
* �����������������������
* @param yaw ���ˮƽ��
* @param pitch ���������
* @param roll ���������
* @param lookVec ����õ��������������
* @param upVec ����õ������������
*/
void CCamera::GetLookVecUpVec(float yaw, float pitch, float roll, CVector3& lookVec, CVector3& upVec) {
	// ת��˳�� 1-roll��2-pitch��3-yaw
	CMatrix4 matrix;
	float cr = cosf(roll);
	float sr = sinf(roll);
	float cy = cosf(yaw);
	float sy = sinf(yaw);
	float cp = cosf(pitch);
	float sp = sinf(pitch);
	matrix(0, 0) = cy * cr - sy * sp * sr;
	matrix(1, 0) = sy * cr + cy * sp * sr;
	matrix(2, 0) = -cp * sr;
	matrix(0, 1) = -sy * cp;
	matrix(1, 1) = cy * cp;
	matrix(2, 1) = sp;
	matrix(0, 2) = cy * sr + sy * sp * cr;
	matrix(1, 2) = sy * sr - cy * sp * cr;
	matrix(2, 2) = cp * cr;
	// ���� LookVector��UpVector ��ֵ
	lookVec = matrix * CVector3::Y;
	upVec = matrix * CVector3::Z;
}