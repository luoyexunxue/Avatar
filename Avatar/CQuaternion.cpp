//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CQuaternion.h"
#include <cmath>

/**
* 构造函数
*/
CQuaternion::CQuaternion() {
	m_fValue[0] = 0;
	m_fValue[1] = 0;
	m_fValue[2] = 0;
	m_fValue[3] = 1.0f;
}

/**
* 复制构造函数
*/
CQuaternion::CQuaternion(const CQuaternion& quaternion) {
	m_fValue[0] = quaternion.m_fValue[0];
	m_fValue[1] = quaternion.m_fValue[1];
	m_fValue[2] = quaternion.m_fValue[2];
	m_fValue[3] = quaternion.m_fValue[3];
}

/**
* 构造函数，指定四元数4个值
*/
CQuaternion::CQuaternion(const float quaternion[4]) {
	m_fValue[0] = quaternion[0];
	m_fValue[1] = quaternion[1];
	m_fValue[2] = quaternion[2];
	m_fValue[3] = quaternion[3];
}

/**
* 构造函数，指定四元数4个值
*/
CQuaternion::CQuaternion(float x, float y, float z, float w) {
	m_fValue[0] = x;
	m_fValue[1] = y;
	m_fValue[2] = z;
	m_fValue[3] = w;
}

/**
* 设置四元数值
*/
void CQuaternion::SetValue(const CQuaternion& quaternion) {
	m_fValue[0] = quaternion.m_fValue[0];
	m_fValue[1] = quaternion.m_fValue[1];
	m_fValue[2] = quaternion.m_fValue[2];
	m_fValue[3] = quaternion.m_fValue[3];
}

/**
* 设置四元数值
*/
void CQuaternion::SetValue(float x, float y, float z, float w) {
	m_fValue[0] = x;
	m_fValue[1] = y;
	m_fValue[2] = z;
	m_fValue[3] = w;
}

/**
* 单位化
*/
CQuaternion& CQuaternion::Normalize() {
	float l1 = m_fValue[0] * m_fValue[0];
	float l2 = m_fValue[1] * m_fValue[1];
	float l3 = m_fValue[2] * m_fValue[2];
	float l4 = m_fValue[3] * m_fValue[3];
	float len = l1 + l2 + l3 + l4;
	if (len == 0.0f) {
		m_fValue[3] = 1.0f;
		return *this;
	}
	len = sqrtf(len);
	len = 1.0f / len;
	m_fValue[0] *= len;
	m_fValue[1] *= len;
	m_fValue[2] *= len;
	m_fValue[3] *= len;
	return *this;
}

/**
* 共轭
*/
CQuaternion& CQuaternion::Conjugate() {
	m_fValue[0] = -m_fValue[0];
	m_fValue[1] = -m_fValue[1];
	m_fValue[2] = -m_fValue[2];
	return *this;
}

/**
* 取逆
*/
CQuaternion& CQuaternion::Inverse() {
	float x2 = m_fValue[0] * m_fValue[0];
	float y2 = m_fValue[1] * m_fValue[1];
	float z2 = m_fValue[2] * m_fValue[2];
	float w2 = m_fValue[3] * m_fValue[3];
	float norm = 1.0f / (x2 + y2 + z2 + w2);
	m_fValue[0] *= norm;
	m_fValue[1] *= norm;
	m_fValue[2] *= norm;
	m_fValue[3] *= norm;
	return *this;
}

/**
* 由欧拉角转为四元数
* @note 生成的四元数表示先绕 X 轴旋转 x 弧度，再绕 Y 轴旋转 y 弧度，再绕 Z 轴旋转 z 弧度
*/
CQuaternion& CQuaternion::FromEulerAngles(float x, float y, float z) {
	x *= 0.5f;
	y *= 0.5f;
	z *= 0.5f;

	float sinx = sinf(x);
	float siny = sinf(y);
	float sinz = sinf(z);
	float cosx = cosf(x);
	float cosy = cosf(y);
	float cosz = cosf(z);

	float cycz = cosy * cosz;
	float sysz = siny * sinz;
	float sycz = siny * cosz;
	float cysz = cosy * sinz;

	m_fValue[0] = sinx * cycz - cosx * sysz;
	m_fValue[1] = cosx * sycz + sinx * cysz;
	m_fValue[2] = cosx * cysz - sinx * sycz;
	m_fValue[3] = cosx * cycz + sinx * sysz;

	Normalize();
	return *this;
}

/**
* 由轴角转为四元数
*/
CQuaternion& CQuaternion::FromAxisAngle(const CVector3& axis, float angle) {
	CVector3 vn(axis);
	vn.Normalize();
	angle *= 0.5f;
	float sina = sinf(angle);
	m_fValue[0] = sina * vn[0];
	m_fValue[1] = sina * vn[1];
	m_fValue[2] = sina * vn[2];
	m_fValue[3] = cosf(angle);
	return *this;
}

/**
* 由向量转为四元数
*/
CQuaternion& CQuaternion::FromVector(const CVector3& from, const CVector3& to) {
	CVector3 a(from);
	CVector3 b(to);
	a.Normalize();
	b.Normalize();
	float cosa = a.DotProduct(b);
	if (cosa < -1.0f) cosa = -1.0f;
	else if (cosa > 1.0f) cosa = 1.0f;
	return FromAxisAngle(a.CrossProduct(b), acosf(cosa));
}

/**
* 由矩阵转为四元数
*/
CQuaternion& CQuaternion::FromMatrix(const CMatrix4& matrix) {
	const float* m = matrix.m_fValue;
	float tr = m[0] + m[5] + m[10];
	if (tr >= 0.0f) {
		float s = sqrtf(tr + m[15]);
		m_fValue[3] = 0.5f * s;
		s = 0.5f / s;
		m_fValue[0] = (m[6] - m[9]) * s;
		m_fValue[1] = (m[8] - m[2]) * s;
		m_fValue[2] = (m[1] - m[4]) * s;
	} else {
		int i = 0;
		int j = 1;
		int k = 2;
		if (m[5] > m[0]) {
			i = 1;
			j = 2;
			k = 0;
		}
		if (m[10] > m[i * 4 + i]) {
			i = 2;
			j = 0;
			k = 1;
		}
		float s = sqrtf(m[i * 4 + i] - m[j * 4 + j] - m[k * 4 + k] + m[15]);
		m_fValue[i] = s * 0.5f;
		if (s != 0.0f) s = 0.5f / s;
		m_fValue[3] = (m[j * 4 + k] - m[k * 4 + j]) * s;
		m_fValue[j] = (m[i * 4 + j] + m[j * 4 + i]) * s;
		m_fValue[k] = (m[i * 4 + k] + m[k * 4 + i]) * s;
	}
	return *this;
}

/**
* 转换为欧拉角表示
*/
void CQuaternion::ToEulerAngles(float* x, float* y, float* z) const {
	float tempax = 1.0f - 2.0f * (m_fValue[0] * m_fValue[0] + m_fValue[1] * m_fValue[1]);
	float tempay = 2.0f * (m_fValue[3] * m_fValue[0] + m_fValue[1] * m_fValue[2]);
	float tempbx = 1.0f - 2.0f * (m_fValue[1] * m_fValue[1] + m_fValue[2] * m_fValue[2]);
	float tempby = 2.0f * (m_fValue[3] * m_fValue[2] + m_fValue[0] * m_fValue[1]);

	*x = asinf(2.0f * (m_fValue[3] * m_fValue[1] - m_fValue[2] * m_fValue[0]));
	*y = atan2f(tempay, tempax);
	*z = atan2f(tempby, tempbx);
}

/**
* 转换为轴角表示
*/
void CQuaternion::ToAxisAngle(CVector3& axis, float* angle) const {
	float len = sqrtf(m_fValue[0] * m_fValue[0] + m_fValue[1] * m_fValue[1] + m_fValue[2] * m_fValue[2]);
	float scale = 1.0f / len;
	axis[0] = m_fValue[0] * scale;
	axis[1] = m_fValue[1] * scale;
	axis[2] = m_fValue[2] * scale;
	*angle = acosf(m_fValue[3]) * 2.0f;
}

/**
* 转换为矩阵表示
*/
CMatrix4 CQuaternion::ToMatrix() const {
	float x2 = 2.0f * m_fValue[0] * m_fValue[0];
	float y2 = 2.0f * m_fValue[1] * m_fValue[1];
	float z2 = 2.0f * m_fValue[2] * m_fValue[2];
	float xy = 2.0f * m_fValue[0] * m_fValue[1];
	float xz = 2.0f * m_fValue[0] * m_fValue[2];
	float yz = 2.0f * m_fValue[1] * m_fValue[2];
	float wx = 2.0f * m_fValue[3] * m_fValue[0];
	float wy = 2.0f * m_fValue[3] * m_fValue[1];
	float wz = 2.0f * m_fValue[3] * m_fValue[2];

	CMatrix4 matrix;
	matrix(0, 0) = 1.0f - (y2 + z2);
	matrix(1, 0) = xy + wz;
	matrix(2, 0) = xz - wy;
	matrix(3, 0) = 0.0f;
	matrix(0, 1) = xy - wz;
	matrix(1, 1) = 1.0f - (x2 + z2);
	matrix(2, 1) = yz + wx;
	matrix(3, 1) = 0.0f;
	matrix(0, 2) = xz + wy;
	matrix(1, 2) = yz - wx;
	matrix(2, 2) = 1.0f - (x2 + y2);
	matrix(3, 2) = 0.0f;
	matrix(0, 3) = 0.0f;
	matrix(1, 3) = 0.0f;
	matrix(2, 3) = 0.0f;
	matrix(3, 3) = 1.0f;
	return matrix;
}

/**
* 球形插值
*/
CQuaternion CQuaternion::Slerp(const CQuaternion& quaternion, float t) const {
	float cosa = m_fValue[0] * quaternion.m_fValue[0];
	cosa += m_fValue[1] * quaternion.m_fValue[1];
	cosa += m_fValue[2] * quaternion.m_fValue[2];
	cosa += m_fValue[3] * quaternion.m_fValue[3];
	if (cosa < -1.0f) cosa = -1.0f;
	else if (cosa > 1.0f) cosa = 1.0f;
	float angle = acosf(cosa);
	float sina = sinf(angle);
	float t1, t2;
	if (sina > 0.001f) {
		float inv = 1.0f / sina;
		t1 = sinf((1.0f - t) * angle) * inv;
		t2 = sinf(t * angle) * inv;
	} else {
		t1 = 1.0f - t;
		t2 = t;
	}
	CQuaternion result = *this * t1 + quaternion * t2;
	return result.Normalize();
}

/**
* 最短路径球形插值
*/
CQuaternion CQuaternion::SlerpShortest(const CQuaternion& quaternion, float t) const {
	float cosa = m_fValue[0] * quaternion.m_fValue[0];
	cosa += m_fValue[1] * quaternion.m_fValue[1];
	cosa += m_fValue[2] * quaternion.m_fValue[2];
	cosa += m_fValue[3] * quaternion.m_fValue[3];
	CQuaternion q(quaternion);
	if (cosa < 0.0f) {
		q.m_fValue[0] = -q.m_fValue[0];
		q.m_fValue[1] = -q.m_fValue[1];
		q.m_fValue[2] = -q.m_fValue[2];
		q.m_fValue[3] = -q.m_fValue[3];
		cosa = -cosa;
	}
	if (cosa < -1.0f) cosa = -1.0f;
	else if (cosa > 1.0f) cosa = 1.0f;
	float angle = acosf(cosa);
	float sina = sinf(angle);
	float t1, t2;
	if (sina > 0.001f) {
		float inv = 1.0f / sina;
		t1 = sinf((1.0f - t) * angle) * inv;
		t2 = sinf(t * angle) * inv;
	} else {
		t1 = 1.0f - t;
		t2 = t;
	}
	CQuaternion result = *this * t1 + q * t2;
	return result.Normalize();
}

/**
* 重载运算符 []
*/
float& CQuaternion::operator [] (int index) {
	return m_fValue[index];
}

/**
* 重载运算符 ==
*/
bool CQuaternion::operator == (const CQuaternion& quaternion) const {
	if (m_fValue[0] == quaternion.m_fValue[0] &&
		m_fValue[1] == quaternion.m_fValue[1] &&
		m_fValue[2] == quaternion.m_fValue[2] &&
		m_fValue[3] == quaternion.m_fValue[3]) {
		return true;
	}
	return false;
}

/**
* 重载运算符 !=
*/
bool CQuaternion::operator != (const CQuaternion& quaternion) const {
	return !(*this == quaternion);
}

/**
* 重载运算符 +
*/
CQuaternion CQuaternion::operator + (const CQuaternion& quaternion) const {
	float qTemp[4];
	qTemp[0] = m_fValue[0] + quaternion.m_fValue[0];
	qTemp[1] = m_fValue[1] + quaternion.m_fValue[1];
	qTemp[2] = m_fValue[2] + quaternion.m_fValue[2];
	qTemp[3] = m_fValue[3] + quaternion.m_fValue[3];
	return CQuaternion(qTemp);
}

/**
* 重载运算符 -
*/
CQuaternion CQuaternion::operator - (const CQuaternion& quaternion) const {
	float qTemp[4];
	qTemp[0] = m_fValue[0] - quaternion.m_fValue[0];
	qTemp[1] = m_fValue[1] - quaternion.m_fValue[1];
	qTemp[2] = m_fValue[2] - quaternion.m_fValue[2];
	qTemp[3] = m_fValue[3] - quaternion.m_fValue[3];
	return CQuaternion(qTemp);
}

/**
* 重载运算符 *
*/
CQuaternion CQuaternion::operator * (const CQuaternion& quaternion) const {
	float qTemp[4];
	qTemp[0] = m_fValue[3] * quaternion.m_fValue[0] + m_fValue[0] * quaternion.m_fValue[3];
	qTemp[0] += m_fValue[1] * quaternion.m_fValue[2] - m_fValue[2] * quaternion.m_fValue[1];
	qTemp[1] = m_fValue[3] * quaternion.m_fValue[1] + m_fValue[1] * quaternion.m_fValue[3];
	qTemp[1] += m_fValue[2] * quaternion.m_fValue[0] - m_fValue[0] * quaternion.m_fValue[2];
	qTemp[2] = m_fValue[3] * quaternion.m_fValue[2] + m_fValue[2] * quaternion.m_fValue[3];
	qTemp[2] += m_fValue[0] * quaternion.m_fValue[1] - m_fValue[1] * quaternion.m_fValue[0];
	qTemp[3] = m_fValue[3] * quaternion.m_fValue[3] - m_fValue[0] * quaternion.m_fValue[0];
	qTemp[3] -= m_fValue[1] * quaternion.m_fValue[1] + m_fValue[2] * quaternion.m_fValue[2];
	return CQuaternion(qTemp);
}

/**
* 重载运算符 *
*/
CQuaternion CQuaternion::operator * (float scale) const {
	float qTemp[4];
	qTemp[0] = m_fValue[0] * scale;
	qTemp[1] = m_fValue[1] * scale;
	qTemp[2] = m_fValue[2] * scale;
	qTemp[3] = m_fValue[3] * scale;
	return CQuaternion(qTemp);
}

/**
* 重载运算符 *
* @note 四元数乘以向量等同于施加旋转到向量上
*/
CVector3 CQuaternion::operator * (const CVector3& vector) const {
	CQuaternion qvec(vector.m_fValue[0], vector.m_fValue[1], vector.m_fValue[2], 0);
	CQuaternion qres = *this * qvec;
	CQuaternion quat = *this;
	qres *= quat.Conjugate();
	return CVector3(qres.m_fValue[0], qres.m_fValue[1], qres.m_fValue[2], qvec.m_fValue[3]);
}

/**
* 重载运算符 +=
*/
CQuaternion& CQuaternion::operator += (const CQuaternion& quaternion) {
	m_fValue[0] += quaternion.m_fValue[0];
	m_fValue[1] += quaternion.m_fValue[1];
	m_fValue[2] += quaternion.m_fValue[2];
	m_fValue[3] += quaternion.m_fValue[3];
	return *this;
}

/**
* 重载运算符 -=
*/
CQuaternion& CQuaternion::operator -= (const CQuaternion& quaternion) {
	m_fValue[0] -= quaternion.m_fValue[0];
	m_fValue[1] -= quaternion.m_fValue[1];
	m_fValue[2] -= quaternion.m_fValue[2];
	m_fValue[3] -= quaternion.m_fValue[3];
	return *this;
}

/**
* 重载运算符 *=
*/
CQuaternion& CQuaternion::operator *= (const CQuaternion& quaternion) {
	float qTemp[4];
	qTemp[0] = m_fValue[3] * quaternion.m_fValue[0] + m_fValue[0] * quaternion.m_fValue[3];
	qTemp[0] += m_fValue[1] * quaternion.m_fValue[2] - m_fValue[2] * quaternion.m_fValue[1];
	qTemp[1] = m_fValue[3] * quaternion.m_fValue[1] + m_fValue[1] * quaternion.m_fValue[3];
	qTemp[1] += m_fValue[2] * quaternion.m_fValue[0] - m_fValue[0] * quaternion.m_fValue[2];
	qTemp[2] = m_fValue[3] * quaternion.m_fValue[2] + m_fValue[2] * quaternion.m_fValue[3];
	qTemp[2] += m_fValue[0] * quaternion.m_fValue[1] - m_fValue[1] * quaternion.m_fValue[0];
	qTemp[3] = m_fValue[3] * quaternion.m_fValue[3] - m_fValue[0] * quaternion.m_fValue[0];
	qTemp[3] -= m_fValue[1] * quaternion.m_fValue[1] + m_fValue[2] * quaternion.m_fValue[2];
	m_fValue[0] = qTemp[0];
	m_fValue[1] = qTemp[1];
	m_fValue[2] = qTemp[2];
	m_fValue[3] = qTemp[3];
	return *this;
}

/**
* 重载运算符 *=
*/
CQuaternion& CQuaternion::operator *= (float scale) {
	m_fValue[0] *= scale;
	m_fValue[1] *= scale;
	m_fValue[2] *= scale;
	m_fValue[3] *= scale;
	return *this;
}