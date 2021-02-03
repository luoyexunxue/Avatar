//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CCamera.h"
#include "CMatrix4.h"
#include "CVector3.h"
#include <cmath>

/**
* 构造函数
*/
CCamera::CCamera() {
	m_cPosition.SetValue(0.0f, 0.0f, 0.0f);
	m_cLookVector.SetValue(CVector3::Y);
	m_cUpVector.SetValue(CVector3::Z);
	m_fYaw = 0.0f;
	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_bOrthoProject = false;
	m_bControlAttached = true;
	m_fPerspectSize[0] = 900.0f;
	m_fPerspectSize[1] = 600.0f;
	m_fOrthoSize[0] = 256.0f;
	m_fOrthoSize[1] = 256.0f;
	m_fFieldOfView = 60.0f;
	m_fClipNear = 0.2f;
	m_fClipFar = 2000.0f;
	m_pBindNode = 0;
}

/**
* 析构函数
*/
CCamera::~CCamera() {
}

/**
* 获取相机名称
* @return default
*/
const char* CCamera::GetName() const {
	return "default";
}

/**
* 控制信息输入
*/
void CCamera::Input(CInputManager::SInput* input) {
	if (!m_bControlAttached) return;
	if (input->bPosition) {
		m_cPosition.SetValue(input->fPosition);
	} else if (input->bMove) {
		CMatrix4 viewMat = m_cViewMatrix;
		CVector3 movement = CVector3(input->fMove[0], input->fMove[2], -input->fMove[1], 0.0f);
		m_cPosition += viewMat.Transpose() * movement;
	}
	if (input->bOrientation) {
		float theta = m_fYaw * 0.5f;
		CQuaternion orient = CQuaternion(0, 0, sinf(theta), cosf(theta)) * CQuaternion(input->fOrientation);
		m_cLookVector = orient * CVector3::Y;
		m_cUpVector = orient * CVector3::Z;
	} else if (input->bTurn) {
		SetAngle(m_fYaw + input->fTurn[0], m_fPitch + input->fTurn[1], m_fRoll + input->fTurn[2]);
	}
}

/**
* 设置相机高度
*/
void CCamera::SetHeight(float height) {
	m_cPosition[2] = height;
}

/**
* 设置相机位置
*/
void CCamera::SetPosition(const CVector3& pos) {
	m_cPosition = pos;
}

/**
* 设置相机方位
*/
void CCamera::SetAngle(float yaw, float pitch, float roll) {
	m_fYaw = yaw;
	m_fPitch = pitch;
	m_fRoll = roll;
	// 俯仰角旋转角限制在 -89° 到 89°之间
	const float maxAngle = 1.553343f;
	if (m_fPitch < -maxAngle) m_fPitch  = -maxAngle;
	else if (m_fPitch > maxAngle) m_fPitch  = maxAngle;
	if (m_fRoll < -maxAngle) m_fRoll  = -maxAngle;
	else if (m_fRoll > maxAngle) m_fRoll  = maxAngle;
	FromAngleToVector(m_fYaw, m_fPitch, m_fRoll, m_cLookVector, m_cUpVector);
}

/**
* 设置相机目标
*/
void CCamera::SetTarget(const CVector3& pos) {
	CVector3 lookVec = pos - m_cPosition;
	if (lookVec.Length() == 0) return;
	m_cLookVector = lookVec.Normalize();
	CVector3 rightVec = m_cLookVector.CrossProduct(CVector3::Z);
	if (rightVec.Length() < 1E-6) m_cUpVector = lookVec.Tangent();
	else m_cUpVector = rightVec.CrossProduct(m_cLookVector).Normalize();
	// 更新 yaw、pitch、roll 值
	FromVectorToAngle(m_cLookVector, m_cUpVector, &m_fYaw, &m_fPitch, &m_fRoll);
}

/**
* 更新相机
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
	UpdateProjMatrix();
	UpdateFrustum();
}

/**
* 获取视口宽度
*/
float CCamera::GetViewWidth() const {
	return m_bOrthoProject ? m_fOrthoSize[0] : m_fPerspectSize[0];
}

/**
* 获取视口高度
*/
float CCamera::GetViewHeight() const {
	return m_bOrthoProject ? m_fOrthoSize[1] : m_fPerspectSize[1];
}

/**
* 获取相机视角
*/
float CCamera::GetFieldOfView() const {
	return m_fFieldOfView;
}

/**
* 获取视口宽高比
*/
float CCamera::GetAspectRatio() const {
	return m_fPerspectSize[0] / m_fPerspectSize[1];
}

/**
* 获取近裁剪面距离
*/
float CCamera::GetNearClipDistance() const {
	return m_fClipNear;
}

/**
* 获取远裁剪面距离
*/
float CCamera::GetFarClipDistance() const {
	return m_fClipFar;
}

/**
* 是否是正交投影模式
*/
bool CCamera::IsOrthoProjection() const {
	return m_bOrthoProject;
}

/**
* 设置视口大小
* @param width 正交宽度
* @param height 正交高度
* @param ortho 是否为正交投影
*/
void CCamera::SetViewSize(float width, float height, bool ortho) {
	if (ortho) {
		m_fOrthoSize[0] = width;
		m_fOrthoSize[1] = height;
	} else {
		m_fPerspectSize[0] = width;
		m_fPerspectSize[1] = height;
	}
}

/**
* 设置相机视角
* @param fov 为横向水平方向视角，单位为角度
*/
void CCamera::SetFieldOfView(float fov) {
	m_fFieldOfView = fov;
}

/**
* 设置裁剪面距离
*/
void CCamera::SetClipDistance(float zNear, float zFar) {
	m_fClipNear = zNear;
	m_fClipFar = zFar;
}

/**
* 设置投影方式
* @param ortho 是否为正交投影
*/
void CCamera::SetOrthoProjection(bool ortho) {
	m_bOrthoProject = ortho;
}

/**
* 更新投影矩阵
*/
void CCamera::UpdateProjMatrix() {
	if (m_bOrthoProject) m_cProjMatrix.Orthographic(m_fOrthoSize[0], m_fOrthoSize[1], 0.0f, m_fClipFar);
	else m_cProjMatrix.Perspective(m_fFieldOfView, GetAspectRatio(), m_fClipNear, m_fClipFar);
}

/**
* 更新视图矩阵
*/
void CCamera::UpdateViewMatrix() {
	m_cViewMatrix.LookAt(m_cPosition, m_cLookVector, m_cUpVector);
}

/**
* 更新视锥体
*/
void CCamera::UpdateFrustum() {
	m_cFrustum.FromViewProj(m_cProjMatrix * m_cViewMatrix);
}

/**
* 获取投影矩阵引用
*/
CMatrix4& CCamera::GetProjMatrix() {
	return m_cProjMatrix;
}

/**
* 获取视图矩阵引用
*/
CMatrix4& CCamera::GetViewMatrix() {
	return m_cViewMatrix;
}

/**
* 获取视锥体引用
*/
const CFrustum& CCamera::GetFrustum() {
	return m_cFrustum;
}

/**
* 绑定输入控制
* @param enable 是否绑定输入进行相机控制
*/
void CCamera::Control(bool enable) {
	m_bControlAttached = enable;
}

/**
* 绑定至指定场景节点
* @param sceneNode 绑定的场景节点，为空解除依附
* @param pos 位置偏移
* @param orient 角度偏移
* @attention 绑定至场景节点后相当于将相机固定在 sceneNode 上面，并且仅受 sceneNode 的控制
*/
void CCamera::Bind(CSceneNode* sceneNode, const CVector3& pos, const CQuaternion& orient) {
	m_pBindNode = sceneNode;
	m_cBindPosition = pos;
	m_cBindOrientation = orient;
}

/**
* 计算相机方位角，俯仰角，旋转角
* @param look 相机视线向量
* @param up 相机上向量
* @param yaw 计算得到的水平角
* @param pitch 计算得到的俯仰角
* @param roll 计算得到的翻滚角
*/
void CCamera::FromVectorToAngle(const CVector3& look, const CVector3& up, float* yaw, float* pitch, float* roll) {
	float y = -atan2f(look.m_fValue[0], look.m_fValue[1]);
	float p = asinf(look.m_fValue[2]);
	// Roll 角度计算较复杂（-90 ~ 90）
	float siny = sinf(y);
	float cosy = cosf(y);
	float sinp = sinf(p);
	float cosp = cosf(p);
	// 未 Roll 旋转的 Up 向量
	CVector3 upVec(sinp * siny, -sinp * cosy, cosp);
	float cosr = upVec.DotProduct(up);
	if (cosr < -1.0f) cosr = -1.0f;
	else if (cosr > 1.0f) cosr = 1.0f;
	float r = acosf(cosr);
	// Roll 方向
	if (upVec.CrossProduct(up).DotProduct(look) < 0) r = -r;
	if (yaw) *yaw = y;
	if (pitch) *pitch = p;
	if (roll) *roll = r;
}

/**
* 计算相机视线向量和上向量
* @param yaw 相机水平角
* @param pitch 相机俯仰角
* @param roll 相机翻滚角
* @param look 计算得到的相机视线向量
* @param up 计算得到的相机上向量
*/
void CCamera::FromAngleToVector(float yaw, float pitch, float roll, CVector3& look, CVector3& up) {
	// 转动顺序 1-roll，2-pitch，3-yaw
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
	// 计算 LookVector，UpVector 的值
	look = matrix * CVector3::Y;
	up = matrix * CVector3::Z;
}