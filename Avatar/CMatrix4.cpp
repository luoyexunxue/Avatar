//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CMatrix4.h"
#include "CQuaternion.h"
#include "CPlane.h"
#include <cmath>

/**
* 静态常量
*/
const CMatrix4 CMatrix4::Identity;

/**
* 构造函数
*/
CMatrix4::CMatrix4() {
	SetIdentity();
}

/**
* 复制构造函数
*/
CMatrix4::CMatrix4(const CMatrix4& matrix) {
	m_fValue[0] = matrix.m_fValue[0];
	m_fValue[1] = matrix.m_fValue[1];
	m_fValue[2] = matrix.m_fValue[2];
	m_fValue[3] = matrix.m_fValue[3];
	m_fValue[4] = matrix.m_fValue[4];
	m_fValue[5] = matrix.m_fValue[5];
	m_fValue[6] = matrix.m_fValue[6];
	m_fValue[7] = matrix.m_fValue[7];
	m_fValue[8] = matrix.m_fValue[8];
	m_fValue[9] = matrix.m_fValue[9];
	m_fValue[10] = matrix.m_fValue[10];
	m_fValue[11] = matrix.m_fValue[11];
	m_fValue[12] = matrix.m_fValue[12];
	m_fValue[13] = matrix.m_fValue[13];
	m_fValue[14] = matrix.m_fValue[14];
	m_fValue[15] = matrix.m_fValue[15];
}

/**
* 构造函数，参数 matrix 为列优先排列
*/
CMatrix4::CMatrix4(const float matrix[16]) {
	m_fValue[0] = matrix[0];
	m_fValue[1] = matrix[1];
	m_fValue[2] = matrix[2];
	m_fValue[3] = matrix[3];
	m_fValue[4] = matrix[4];
	m_fValue[5] = matrix[5];
	m_fValue[6] = matrix[6];
	m_fValue[7] = matrix[7];
	m_fValue[8] = matrix[8];
	m_fValue[9] = matrix[9];
	m_fValue[10] = matrix[10];
	m_fValue[11] = matrix[11];
	m_fValue[12] = matrix[12];
	m_fValue[13] = matrix[13];
	m_fValue[14] = matrix[14];
	m_fValue[15] = matrix[15];
}

/**
* 设置矩阵值
*/
void CMatrix4::SetValue(const CMatrix4& matrix) {
	m_fValue[0] = matrix.m_fValue[0];
	m_fValue[1] = matrix.m_fValue[1];
	m_fValue[2] = matrix.m_fValue[2];
	m_fValue[3] = matrix.m_fValue[3];
	m_fValue[4] = matrix.m_fValue[4];
	m_fValue[5] = matrix.m_fValue[5];
	m_fValue[6] = matrix.m_fValue[6];
	m_fValue[7] = matrix.m_fValue[7];
	m_fValue[8] = matrix.m_fValue[8];
	m_fValue[9] = matrix.m_fValue[9];
	m_fValue[10] = matrix.m_fValue[10];
	m_fValue[11] = matrix.m_fValue[11];
	m_fValue[12] = matrix.m_fValue[12];
	m_fValue[13] = matrix.m_fValue[13];
	m_fValue[14] = matrix.m_fValue[14];
	m_fValue[15] = matrix.m_fValue[15];
}

/**
* 设置矩阵值
*/
void CMatrix4::SetValue(const float matrix[16]) {
	m_fValue[0] = matrix[0];
	m_fValue[1] = matrix[1];
	m_fValue[2] = matrix[2];
	m_fValue[3] = matrix[3];
	m_fValue[4] = matrix[4];
	m_fValue[5] = matrix[5];
	m_fValue[6] = matrix[6];
	m_fValue[7] = matrix[7];
	m_fValue[8] = matrix[8];
	m_fValue[9] = matrix[9];
	m_fValue[10] = matrix[10];
	m_fValue[11] = matrix[11];
	m_fValue[12] = matrix[12];
	m_fValue[13] = matrix[13];
	m_fValue[14] = matrix[14];
	m_fValue[15] = matrix[15];
}

/**
* 设置为单位矩阵
*/
CMatrix4& CMatrix4::SetIdentity() {
	m_fValue[0] = 1.0f;
	m_fValue[1] = 0.0f;
	m_fValue[2] = 0.0f;
	m_fValue[3] = 0.0f;
	m_fValue[4] = 0.0f;
	m_fValue[5] = 1.0f;
	m_fValue[6] = 0.0f;
	m_fValue[7] = 0.0f;
	m_fValue[8] = 0.0f;
	m_fValue[9] = 0.0f;
	m_fValue[10] = 1.0f;
	m_fValue[11] = 0.0f;
	m_fValue[12] = 0.0f;
	m_fValue[13] = 0.0f;
	m_fValue[14] = 0.0f;
	m_fValue[15] = 1.0f;
	return *this;
}

/**
* 计算逆矩阵
*/
CMatrix4& CMatrix4::Invert() {
	float r[16];
	const float* m = m_fValue;
	r[0] = m[5] * m[10] * m[15] + m[6] * m[13] * m[11] + m[7] * m[9] * m[14];
	r[0] -= m[5] * m[14] * m[11] + m[6] * m[9] * m[15] + m[7] * m[13] * m[10];
	r[1] = m[1] * m[14] * m[11] + m[2] * m[9] * m[15] + m[3] * m[13] * m[10];
	r[1] -= m[1] * m[10] * m[15] + m[2] * m[13] * m[11] + m[3] * m[9] * m[14];
	r[2] = m[1] * m[6] * m[15] + m[2] * m[13] * m[7] + m[3] * m[5] * m[14];
	r[2] -= m[1] * m[14] * m[7] + m[2] * m[5] * m[15] + m[3] * m[13] * m[6];
	r[3] = m[1] * m[10] * m[7] + m[2] * m[5] * m[11] + m[3] * m[9] * m[6];
	r[3] -= m[1] * m[6] * m[11] + m[2] * m[9] * m[7] + m[3] * m[5] * m[10];
	r[4] = m[4] * m[14] * m[11] + m[6] * m[8] * m[15] + m[7] * m[12] * m[10];
	r[4] -= m[4] * m[10] * m[15] + m[6] * m[12] * m[11] + m[7] * m[8] * m[14];
	r[5] = m[0] * m[10] * m[15] + m[2] * m[12] * m[11] + m[3] * m[8] * m[14];
	r[5] -= m[0] * m[14] * m[11] + m[2] * m[8] * m[15] + m[3] * m[12] * m[10];
	r[6] = m[0] * m[14] * m[7] + m[2] * m[4] * m[15] + m[3] * m[12] * m[6];
	r[6] -= m[0] * m[6] * m[15] + m[2] * m[12] * m[7] + m[3] * m[4] * m[14];
	r[7] = m[0] * m[6] * m[11] + m[2] * m[8] * m[7] + m[3] * m[4] * m[10];
	r[7] -= m[0] * m[10] * m[7] + m[2] * m[4] * m[11] + m[3] * m[8] * m[6];
	r[8] = m[4] * m[9] * m[15] + m[5] * m[12] * m[11] + m[7] * m[8] * m[13];
	r[8] -= m[4] * m[13] * m[11] + m[5] * m[8] * m[15] + m[7] * m[12] * m[9];
	r[9] = m[0] * m[13] * m[11] + m[1] * m[8] * m[15] + m[3] * m[12] * m[9];
	r[9] -= m[0] * m[9] * m[15] + m[1] * m[12] * m[11] + m[3] * m[8] * m[13];
	r[10] = m[0] * m[5] * m[15] + m[1] * m[12] * m[7] + m[3] * m[4] * m[13];
	r[10] -= m[0] * m[13] * m[7] + m[1] * m[4] * m[15] + m[3] * m[12] * m[5];
	r[11] = m[0] * m[9] * m[7] + m[1] * m[4] * m[11] + m[3] * m[8] * m[5];
	r[11] -= m[0] * m[5] * m[11] + m[1] * m[8] * m[7] + m[3] * m[4] * m[9];
	r[12] = m[4] * m[13] * m[10] + m[5] * m[8] * m[14] + m[6] * m[12] * m[9];
	r[12] -= m[4] * m[9] * m[14] + m[5] * m[12] * m[10] + m[6] * m[8] * m[13];
	r[13] = m[0] * m[9] * m[14] + m[1] * m[12] * m[10] + m[2] * m[8] * m[13];
	r[13] -= m[0] * m[13] * m[10] + m[1] * m[8] * m[14] + m[2] * m[12] * m[9];
	r[14] = m[0] * m[13] * m[6] + m[1] * m[4] * m[14] + m[2] * m[12] * m[5];
	r[14] -= m[0] * m[5] * m[14] + m[1] * m[12] * m[6] + m[2] * m[4] * m[13];
	r[15] = m[0] * m[5] * m[10] + m[1] * m[8] * m[6] + m[2] * m[4] * m[9];
	r[15] -= m[0] * m[9] * m[6] + m[1] * m[4] * m[10] + m[2] * m[8] * m[5];

	float det = 1.0f / (m[0] * r[0] + m[1] * r[4] + m[2] * r[8] + m[3] * r[12]);
	for (int i = 0; i < 16; i++) m_fValue[i] = r[i] * det;
	return *this;
}

/**
* 计算转置矩阵
*/
CMatrix4& CMatrix4::Transpose() {
	float temp;
	temp = m_fValue[1]; m_fValue[1] = m_fValue[4]; m_fValue[4] = temp;
	temp = m_fValue[2]; m_fValue[2] = m_fValue[8]; m_fValue[8] = temp;
	temp = m_fValue[3]; m_fValue[3] = m_fValue[12]; m_fValue[12] = temp;
	temp = m_fValue[6]; m_fValue[6] = m_fValue[9]; m_fValue[9] = temp;
	temp = m_fValue[7]; m_fValue[7] = m_fValue[13]; m_fValue[13] = temp;
	temp = m_fValue[11]; m_fValue[11] = m_fValue[14]; m_fValue[14] = temp;
	return *this;
}

/**
* 计算斜对称矩阵
* @note SkewSymmetric(a) * b 等价于 a x b
*/
CMatrix4& CMatrix4::SkewSymmetric(const CVector3& vector) {
	m_fValue[0] = 0.0f;
	m_fValue[1] = vector.m_fValue[2];
	m_fValue[2] = -vector.m_fValue[1];
	m_fValue[3] = 0.0f;
	m_fValue[4] = -vector.m_fValue[2];
	m_fValue[5] = 0.0f;
	m_fValue[6] = vector.m_fValue[0];
	m_fValue[7] = 0.0f;
	m_fValue[8] = vector.m_fValue[1];
	m_fValue[9] = -vector.m_fValue[0];
	m_fValue[10] = 0.0f;
	m_fValue[11] = 0.0f;
	m_fValue[12] = 0.0f;
	m_fValue[13] = 0.0f;
	m_fValue[14] = 0.0f;
	m_fValue[15] = 0.0f;
	return *this;
}

/**
* 计算被缩放的矩阵
* @param scale 缩放向量，向量每个元素缩放矩阵每一列
*/
CMatrix4& CMatrix4::SetScaled(const CVector3& scale) {
	m_fValue[0] *= scale.m_fValue[0];
	m_fValue[1] *= scale.m_fValue[0];
	m_fValue[2] *= scale.m_fValue[0];
	m_fValue[3] *= scale.m_fValue[0];
	m_fValue[4] *= scale.m_fValue[1];
	m_fValue[5] *= scale.m_fValue[1];
	m_fValue[6] *= scale.m_fValue[1];
	m_fValue[7] *= scale.m_fValue[1];
	m_fValue[8] *= scale.m_fValue[2];
	m_fValue[9] *= scale.m_fValue[2];
	m_fValue[10] *= scale.m_fValue[2];
	m_fValue[11] *= scale.m_fValue[2];
	return *this;
}

/**
* 缩放操作
*/
CMatrix4& CMatrix4::Scale(const CVector3& scale) {
	m_fValue[0] *= scale.m_fValue[0];
	m_fValue[1] *= scale.m_fValue[1];
	m_fValue[2] *= scale.m_fValue[2];
	m_fValue[4] *= scale.m_fValue[0];
	m_fValue[5] *= scale.m_fValue[1];
	m_fValue[6] *= scale.m_fValue[2];
	m_fValue[8] *= scale.m_fValue[0];
	m_fValue[9] *= scale.m_fValue[1];
	m_fValue[10] *= scale.m_fValue[2];
	m_fValue[12] *= scale.m_fValue[0];
	m_fValue[13] *= scale.m_fValue[1];
	m_fValue[14] *= scale.m_fValue[2];
	return *this;
}

/**
* 缩放操作
*/
CMatrix4& CMatrix4::Scale(float sx, float sy, float sz) {
	m_fValue[0] *= sx;
	m_fValue[1] *= sy;
	m_fValue[2] *= sz;
	m_fValue[4] *= sx;
	m_fValue[5] *= sy;
	m_fValue[6] *= sz;
	m_fValue[8] *= sx;
	m_fValue[9] *= sy;
	m_fValue[10] *= sz;
	m_fValue[12] *= sx;
	m_fValue[13] *= sy;
	m_fValue[14] *= sz;
	return *this;
}

/**
* 绕 X 轴旋转矩阵
*/
CMatrix4& CMatrix4::RotateX(float angle) {
	float sina = sinf(angle);
	float cosa = cosf(angle);
	float mTemp[8];
	mTemp[0] = cosa * m_fValue[1] - sina * m_fValue[2];
	mTemp[1] = cosa * m_fValue[2] + sina * m_fValue[1];
	mTemp[2] = cosa * m_fValue[5] - sina * m_fValue[6];
	mTemp[3] = cosa * m_fValue[6] + sina * m_fValue[5];
	mTemp[4] = cosa * m_fValue[9] - sina * m_fValue[10];
	mTemp[5] = cosa * m_fValue[10] + sina * m_fValue[9];
	mTemp[6] = cosa * m_fValue[13] - sina * m_fValue[14];
	mTemp[7] = cosa * m_fValue[14] + sina * m_fValue[13];
	m_fValue[1] = mTemp[0]; m_fValue[2] = mTemp[1];
	m_fValue[5] = mTemp[2]; m_fValue[6] = mTemp[3];
	m_fValue[9] = mTemp[4]; m_fValue[10] = mTemp[5];
	m_fValue[13] = mTemp[6]; m_fValue[14] = mTemp[7];
	return *this;
}

/**
* 绕 Y 轴旋转矩阵
*/
CMatrix4& CMatrix4::RotateY(float angle) {
	float sina = sinf(angle);
	float cosa = cosf(angle);
	float mTemp[8];
	mTemp[0] = cosa * m_fValue[0] + sina * m_fValue[2];
	mTemp[1] = cosa * m_fValue[2] - sina * m_fValue[0];
	mTemp[2] = cosa * m_fValue[4] + sina * m_fValue[6];
	mTemp[3] = cosa * m_fValue[6] - sina * m_fValue[4];
	mTemp[4] = cosa * m_fValue[8] + sina * m_fValue[10];
	mTemp[5] = cosa * m_fValue[10] - sina * m_fValue[8];
	mTemp[6] = cosa * m_fValue[12] + sina * m_fValue[14];
	mTemp[7] = cosa * m_fValue[14] - sina * m_fValue[12];
	m_fValue[0] = mTemp[0]; m_fValue[2] = mTemp[1];
	m_fValue[4] = mTemp[2]; m_fValue[6] = mTemp[3];
	m_fValue[8] = mTemp[4]; m_fValue[10] = mTemp[5];
	m_fValue[12] = mTemp[6]; m_fValue[14] = mTemp[7];
	return *this;
}

/**
* 绕 Z 轴旋转矩阵
*/
CMatrix4& CMatrix4::RotateZ(float angle) {
	float sina = sinf(angle);
	float cosa = cosf(angle);
	float mTemp[8];
	mTemp[0] = cosa * m_fValue[0] - sina * m_fValue[1];
	mTemp[1] = cosa * m_fValue[1] + sina * m_fValue[0];
	mTemp[2] = cosa * m_fValue[4] - sina * m_fValue[5];
	mTemp[3] = cosa * m_fValue[5] + sina * m_fValue[4];
	mTemp[4] = cosa * m_fValue[8] - sina * m_fValue[9];
	mTemp[5] = cosa * m_fValue[9] + sina * m_fValue[8];
	mTemp[6] = cosa * m_fValue[12] - sina * m_fValue[13];
	mTemp[7] = cosa * m_fValue[13] + sina * m_fValue[12];
	m_fValue[0] = mTemp[0]; m_fValue[1] = mTemp[1];
	m_fValue[4] = mTemp[2]; m_fValue[5] = mTemp[3];
	m_fValue[8] = mTemp[4]; m_fValue[9] = mTemp[5];
	m_fValue[12] = mTemp[6]; m_fValue[13] = mTemp[7];
	return *this;
}

/**
* 平移操作
*/
CMatrix4& CMatrix4::Translate(const CVector3& pos) {
	m_fValue[12] += pos.m_fValue[0];
	m_fValue[13] += pos.m_fValue[1];
	m_fValue[14] += pos.m_fValue[2];
	return *this;
}

/**
* 平移操作
*/
CMatrix4& CMatrix4::Translate(float dx, float dy, float dz) {
	m_fValue[12] += dx;
	m_fValue[13] += dy;
	m_fValue[14] += dz;
	return *this;
}

/**
* 生成变换矩阵.
* @note 变换顺序 1. scale 2. rot 3. pos
*/
void CMatrix4::MakeTransform(const CVector3& scale, const CMatrix4& rot, const CVector3& pos) {
	m_fValue[0] = scale.m_fValue[0] * rot.m_fValue[0];
	m_fValue[1] = scale.m_fValue[0] * rot.m_fValue[1];
	m_fValue[2] = scale.m_fValue[0] * rot.m_fValue[2];
	m_fValue[3] = 0.0f;
	m_fValue[4] = scale.m_fValue[1] * rot.m_fValue[4];
	m_fValue[5] = scale.m_fValue[1] * rot.m_fValue[5];
	m_fValue[6] = scale.m_fValue[1] * rot.m_fValue[6];
	m_fValue[7] = 0.0f;
	m_fValue[8] = scale.m_fValue[2] * rot.m_fValue[8];
	m_fValue[9] = scale.m_fValue[2] * rot.m_fValue[9];
	m_fValue[10] = scale.m_fValue[2] * rot.m_fValue[10];
	m_fValue[11] = 0.0f;
	m_fValue[12] = pos.m_fValue[0];
	m_fValue[13] = pos.m_fValue[1];
	m_fValue[14] = pos.m_fValue[2];
	m_fValue[15] = 1.0f;
}

/**
* 生成变换矩阵，旋转由四元数表示.
* @note 变换顺序 1. scale 2. rot 3. pos
*/
void CMatrix4::MakeTransform(const CVector3& scale, const CQuaternion& rot, const CVector3& pos) {
	MakeTransform(scale, rot.ToMatrix(), pos);
}

/**
* 计算近斜平面裁剪投影矩阵
*/
CMatrix4& CMatrix4::ObliqueNearPlaneClipping(const CMatrix4& proj, const CPlane& clip) {
	float x = ((clip.m_fNormal[0] > 0? 1: (clip.m_fNormal[0] < 0? -1: 0)) + proj.m_fValue[8]) / proj.m_fValue[0];
	float y = ((clip.m_fNormal[1] > 0? 1: (clip.m_fNormal[1] < 0? -1: 0)) + proj.m_fValue[9]) / proj.m_fValue[5];
	float z = -1.0f;
	float w = (1.0f + proj.m_fValue[10]) / proj.m_fValue[14];
	float scale = 2.0f / (clip.m_fNormal[0] * x + clip.m_fNormal[1] * y + clip.m_fNormal[2] * z + clip.m_fDistance * w);

	// 修改投影矩阵的第三行
	m_fValue[0] = proj.m_fValue[0];
	m_fValue[1] = proj.m_fValue[1];
	m_fValue[2] = clip.m_fNormal[0] * scale;
	m_fValue[3] = proj.m_fValue[3];
	m_fValue[4] = proj.m_fValue[4];
	m_fValue[5] = proj.m_fValue[5];
	m_fValue[6] = clip.m_fNormal[1] * scale;
	m_fValue[7] = proj.m_fValue[7];
	m_fValue[8] = proj.m_fValue[8];
	m_fValue[9] = proj.m_fValue[9];
	m_fValue[10] = clip.m_fNormal[2] * scale + 1.0f;
	m_fValue[11] = proj.m_fValue[11];
	m_fValue[12] = proj.m_fValue[12];
	m_fValue[13] = proj.m_fValue[13];
	m_fValue[14] = clip.m_fDistance * scale;
	m_fValue[15] = proj.m_fValue[15];
	return *this;
}

/**
* 计算视图矩阵
*/
CMatrix4& CMatrix4::LookAt(const CVector3& pos, const CVector3& look, const CVector3& up) {
	CVector3 right = look.CrossProduct(up);
	m_fValue[0] = right.m_fValue[0];
	m_fValue[1] = up.m_fValue[0];
	m_fValue[2] = -look.m_fValue[0];
	m_fValue[3] = 0.0f;
	m_fValue[4] = right.m_fValue[1];
	m_fValue[5] = up.m_fValue[1];
	m_fValue[6] = -look.m_fValue[1];
	m_fValue[7] = 0.0f;
	m_fValue[8] = right.m_fValue[2];
	m_fValue[9] = up.m_fValue[2];
	m_fValue[10] = -look.m_fValue[2];
	m_fValue[11] = 0.0f;
	m_fValue[12] = -right.DotProduct(pos);
	m_fValue[13] = -up.DotProduct(pos);
	m_fValue[14] = look.DotProduct(pos);
	m_fValue[15] = 1.0f;
	return *this;
}

/**
* 计算正交投影矩阵
*/
CMatrix4& CMatrix4::Orthographic(float width, float height, float zNear, float zFar) {
	m_fValue[0] = 2.0f / width;
	m_fValue[1] = 0.0f;
	m_fValue[2] = 0.0f;
	m_fValue[3] = 0.0f;
	m_fValue[4] = 0.0f;
	m_fValue[5] = 2.0f / height;
	m_fValue[6] = 0.0f;
	m_fValue[7] = 0.0f;
	m_fValue[8] = 0.0f;
	m_fValue[9] = 0.0f;
	m_fValue[10] = 2.0f / (zNear - zFar);
	m_fValue[11] = 0.0f;
	m_fValue[12] = 0.0f;
	m_fValue[13] = 0.0f;
	m_fValue[14] = (zNear + zFar) / (zNear - zFar);
	m_fValue[15] = 1.0f;
	return *this;
}

/**
* 计算透视投影矩阵
*/
CMatrix4& CMatrix4::Perspective(float fov, float aspect, float zNear, float zFar) {
	const float zRange = zNear - zFar;
	const float cotFov = 1.0f / tanf(fov * 0.008727f);
	m_fValue[0] = cotFov;
	m_fValue[1] = 0.0f;
	m_fValue[2] = 0.0f;
	m_fValue[3] = 0.0f;
	m_fValue[4] = 0.0f;
	m_fValue[5] = aspect * cotFov;
	m_fValue[6] = 0.0f;
	m_fValue[7] = 0.0f;
	m_fValue[8] = 0.0f;
	m_fValue[9] = 0.0f;
	m_fValue[10] = (zNear + zFar) / zRange;
	m_fValue[11] = -1.0f;
	m_fValue[12] = 0.0f;
	m_fValue[13] = 0.0f;
	m_fValue[14] = 2.0f * zNear * zFar / zRange;
	m_fValue[15] = 0.0f;
	return *this;
}

/**
* 重载运算符 []
*/
float& CMatrix4::operator [] (int index) {
	return m_fValue[index];
}

/**
* 重载运算符 ()
*/
float& CMatrix4::operator () (int row, int col) {
	return m_fValue[(col << 2) + row];
}

/**
* 重载运算符 ==
*/
bool CMatrix4::operator == (const CMatrix4& matrix) const {
	for (int i = 0; i < 16; i++) {
		if (m_fValue[i] != matrix.m_fValue[i]) return false;
	}
	return true;
}

/**
* 重载运算符 !=
*/
bool CMatrix4::operator != (const CMatrix4& matrix) const {
	return !(*this == matrix);
}

/**
* 重载运算符 +
*/
CMatrix4 CMatrix4::operator + (const CMatrix4& matrix) const {
	float mTemp[16];
	for (int i = 0; i < 16; i++) {
		mTemp[i] = m_fValue[i] + matrix.m_fValue[i];
	}
	return CMatrix4(mTemp);
}

/**
* 重载运算符 -
*/
CMatrix4 CMatrix4::operator - (const CMatrix4& matrix) const {
	float mTemp[16];
	for (int i = 0; i < 16; i++) {
		mTemp[i] = m_fValue[i] - matrix.m_fValue[i];
	}
	return CMatrix4(mTemp);
}

/**
* 矩阵乘
*/
CMatrix4 CMatrix4::operator * (const CMatrix4& matrix) const {
	float mTemp[16];
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			int index = i * 4 + j;
			mTemp[index] = m_fValue[j] * matrix.m_fValue[i * 4];
			mTemp[index] += m_fValue[j + 4] * matrix.m_fValue[i * 4 + 1];
			mTemp[index] += m_fValue[j + 8] * matrix.m_fValue[i * 4 + 2];
			mTemp[index] += m_fValue[j + 12] * matrix.m_fValue[i * 4 + 3];
		}
	}
	return CMatrix4(mTemp);
}

/**
* 矩阵乘向量
*/
CVector3 CMatrix4::operator * (const CVector3& vector) const {
	CVector3 vecOut;
	for (int i = 0; i < 4; i++) {
		float rowValue = m_fValue[i] * vector.m_fValue[0];
		rowValue += m_fValue[4 + i] * vector.m_fValue[1];
		rowValue += m_fValue[8 + i] * vector.m_fValue[2];
		rowValue += m_fValue[12 + i] * vector.m_fValue[3];
		vecOut[i] = rowValue;
	}
	return vecOut;
}

/**
* 乘四元数运算
*/
CQuaternion CMatrix4::operator * (const CQuaternion& rotation) const {
	 return CQuaternion().FromMatrix(*this) * rotation;
}

/**
* 矩阵乘数值
*/
CMatrix4 CMatrix4::operator * (float factor) const {
	float mTemp[16];
	for (int i = 0; i < 16; i++) {
		mTemp[i] = m_fValue[i] * factor;
	}
	return CMatrix4(mTemp);
}

/**
* 重载运算符 +=
*/
CMatrix4& CMatrix4::operator += (const CMatrix4& matrix) {
	for (int i = 0; i < 16; i++) {
		m_fValue[i] += matrix.m_fValue[i];
	}
	return *this;
}

/**
* 重载运算符 -=
*/
CMatrix4& CMatrix4::operator -= (const CMatrix4& matrix) {
	for (int i = 0; i < 16; i++) {
		m_fValue[i] -= matrix.m_fValue[i];
	}
	return *this;
}

/**
* 矩阵乘
*/
CMatrix4& CMatrix4::operator *= (const CMatrix4& matrix) {
	float mTemp[16];
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			int index = i * 4 + j;
			mTemp[index] = m_fValue[j] * matrix.m_fValue[i * 4];
			mTemp[index] += m_fValue[j + 4] * matrix.m_fValue[i * 4 + 1];
			mTemp[index] += m_fValue[j + 8] * matrix.m_fValue[i * 4 + 2];
			mTemp[index] += m_fValue[j + 12] * matrix.m_fValue[i * 4 + 3];
		}
	}
	SetValue(mTemp);
	return *this;
}

/**
* 矩阵乘数值
*/
CMatrix4& CMatrix4::operator *= (float factor) {
	for (int i = 0; i < 16; i++) {
		m_fValue[i] *= factor;
	}
	return *this;
}