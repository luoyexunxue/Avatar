//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CVector3.h"
#include <cmath>

/**
* 静态常量
*/
const CVector3 CVector3::Zero(0.0f, 0.0f, 0.0f, 0.0f);
const CVector3 CVector3::One(1.0f, 1.0f, 1.0f, 0.0f);
const CVector3 CVector3::X(1.0f, 0.0f, 0.0f, 0.0f);
const CVector3 CVector3::Y(0.0f, 1.0f, 0.0f, 0.0f);
const CVector3 CVector3::Z(0.0f, 0.0f, 1.0f, 0.0f);

/**
* 构造函数
*/
CVector3::CVector3() {
	m_fValue[0] = 0;
	m_fValue[1] = 0;
	m_fValue[2] = 0;
	m_fValue[3] = 1.0f;
}

/**
* 复制构造函数
*/
CVector3::CVector3(const CVector3& vector) {
	m_fValue[0] = vector.m_fValue[0];
	m_fValue[1] = vector.m_fValue[1];
	m_fValue[2] = vector.m_fValue[2];
	m_fValue[3] = vector.m_fValue[3];
}

/**
* 构造函数
*/
CVector3::CVector3(float val) {
	m_fValue[0] = val;
	m_fValue[1] = val;
	m_fValue[2] = val;
	m_fValue[3] = 1.0f;
}

/**
* 构造函数
*/
CVector3::CVector3(const float vector[3]) {
	m_fValue[0] = vector[0];
	m_fValue[1] = vector[1];
	m_fValue[2] = vector[2];
	m_fValue[3] = 1.0f;
}

/**
* 构造函数
*/
CVector3::CVector3(float x, float y, float z) {
	m_fValue[0] = x;
	m_fValue[1] = y;
	m_fValue[2] = z;
	m_fValue[3] = 1.0f;
}

/**
* 构造函数，采用 x y z w 构造其次向量
*/
CVector3::CVector3(const float vector[3], float w) {
	m_fValue[0] = vector[0];
	m_fValue[1] = vector[1];
	m_fValue[2] = vector[2];
	m_fValue[3] = w;
}

/**
* 构造函数，采用 x y z w 构造其次向量
*/
CVector3::CVector3(float x, float y, float z, float w) {
	m_fValue[0] = x;
	m_fValue[1] = y;
	m_fValue[2] = z;
	m_fValue[3] = w;
}

/**
* 设置向量值
*/
void CVector3::SetValue(const CVector3& vector) {
	m_fValue[0] = vector.m_fValue[0];
	m_fValue[1] = vector.m_fValue[1];
	m_fValue[2] = vector.m_fValue[2];
	m_fValue[3] = vector.m_fValue[3];
}

/**
* 设置向量值
*/
void CVector3::SetValue(const float vector[3]) {
	m_fValue[0] = vector[0];
	m_fValue[1] = vector[1];
	m_fValue[2] = vector[2];
}

/**
* 设置向量值
*/
void CVector3::SetValue(float x, float y, float z) {
	m_fValue[0] = x;
	m_fValue[1] = y;
	m_fValue[2] = z;
}

/**
* 设置向量值
*/
void CVector3::SetValue(const float vector[3], float w) {
	m_fValue[0] = vector[0];
	m_fValue[1] = vector[1];
	m_fValue[2] = vector[2];
	m_fValue[3] = w;
}

/**
* 设置向量值
*/
void CVector3::SetValue(float x, float y, float z, float w) {
	m_fValue[0] = x;
	m_fValue[1] = y;
	m_fValue[2] = z;
	m_fValue[3] = w;
}

/**
* 转换为单位向量
*/
CVector3& CVector3::Normalize() {
	float len = Length();
	if (len == 0) {
		m_fValue[0] = 0.0f;
		m_fValue[1] = 0.0f;
		m_fValue[2] = 0.0f;
		return *this;
	}
	len = 1.0f / len;
	m_fValue[0] *= len;
	m_fValue[1] *= len;
	m_fValue[2] *= len;
	return *this;
}

/**
* 将每个分量转换为倒数
*/
CVector3& CVector3::Inverse() {
	m_fValue[0] = 1.0f / m_fValue[0];
	m_fValue[1] = 1.0f / m_fValue[1];
	m_fValue[2] = 1.0f / m_fValue[2];
	return *this;
}

/**
* 加指定值
*/
CVector3& CVector3::Add(const CVector3& vector) {
	m_fValue[0] += vector.m_fValue[0];
	m_fValue[1] += vector.m_fValue[1];
	m_fValue[2] += vector.m_fValue[2];
	return *this;
}

/**
* 加指定值
*/
CVector3& CVector3::Add(const float vector[3]) {
	m_fValue[0] += vector[0];
	m_fValue[1] += vector[1];
	m_fValue[2] += vector[2];
	return *this;
}

/**
* 加指定值
*/
CVector3& CVector3::Add(float x, float y, float z) {
	m_fValue[0] += x;
	m_fValue[1] += y;
	m_fValue[2] += z;
	return *this;
}

/**
* 减指定值
*/
CVector3& CVector3::Sub(const CVector3& vector) {
	m_fValue[0] -= vector.m_fValue[0];
	m_fValue[1] -= vector.m_fValue[1];
	m_fValue[2] -= vector.m_fValue[2];
	return *this;
}

/**
* 减指定值
*/
CVector3& CVector3::Sub(const float vector[3]) {
	m_fValue[0] -= vector[0];
	m_fValue[1] -= vector[1];
	m_fValue[2] -= vector[2];
	return *this;
}

/**
* 减指定值
*/
CVector3& CVector3::Sub(float x, float y, float z) {
	m_fValue[0] -= x;
	m_fValue[1] -= y;
	m_fValue[2] -= z;
	return *this;
}

/**
* 缩放指定值
*/
CVector3& CVector3::Scale(float scale) {
	m_fValue[0] *= scale;
	m_fValue[1] *= scale;
	m_fValue[2] *= scale;
	return *this;
}

/**
* 缩放指定值
*/
CVector3& CVector3::Scale(float sx, float sy, float sz) {
	m_fValue[0] *= sx;
	m_fValue[1] *= sy;
	m_fValue[2] *= sz;
	return *this;
}

/**
* 取得向量的长度
*/
float CVector3::Length() const {
	float length = m_fValue[0] * m_fValue[0];
	length += m_fValue[1] * m_fValue[1];
	length += m_fValue[2] * m_fValue[2];
	if (length < 1E-9) return 0.0f;
	return sqrtf(length);
}

/**
* 向量点乘
*/
float CVector3::DotProduct(const CVector3& vector) const {
	float result = m_fValue[0] * vector.m_fValue[0];
	result += m_fValue[1] * vector.m_fValue[1];
	result += m_fValue[2] * vector.m_fValue[2];
	return result;
}

/**
* 向量叉乘
*/
CVector3 CVector3::CrossProduct(const CVector3& vector) const {
	float vTemp[3];
	vTemp[0] = m_fValue[1] * vector.m_fValue[2] - m_fValue[2] * vector.m_fValue[1];
	vTemp[1] = m_fValue[2] * vector.m_fValue[0] - m_fValue[0] * vector.m_fValue[2];
	vTemp[2] = m_fValue[0] * vector.m_fValue[1] - m_fValue[1] * vector.m_fValue[0];
	return CVector3(vTemp);
}

/**
* 线性插值
*/
CVector3 CVector3::Lerp(const CVector3& vector, float t) const {
	float vTemp[3];
	vTemp[0] = m_fValue[0] + (vector.m_fValue[0] - m_fValue[0]) * t;
	vTemp[1] = m_fValue[1] + (vector.m_fValue[1] - m_fValue[1]) * t;
	vTemp[2] = m_fValue[2] + (vector.m_fValue[2] - m_fValue[2]) * t;
	return CVector3(vTemp);
}

/**
* 球形插值
*/
CVector3 CVector3::Slerp(const CVector3& vector, float t) const {
	CVector3 v1 = *this;
	CVector3 v2 = vector;
	float omega = v1.Normalize().DotProduct(v2.Normalize());
	if (omega < 0.001f) {
		return Lerp(vector, t);
	} else {
		float denom = 1.0f / sinf(omega);
		float t1 = sinf(omega * (1.0f - t)) * denom;
		float t2 = sinf(omega * t) * denom;
		return (*this * t1) + (vector * t2);
	}
}

/**
* 求反射向量，注意源向量和反射向量方向均在法向一侧
*/
CVector3 CVector3::Reflect(const CVector3& normal) const {
	CVector3 n(normal);
	n.Normalize();
	n.Scale(this->DotProduct(n) * 2.0f);
	return n - *this;
}

/**
* 生成切向量
* @attention 生成的切向量不是单位向量
*/
CVector3 CVector3::Tangent() const {
	const float x = fabs(m_fValue[0]);
	const float y = fabs(m_fValue[1]);
	const float z = fabs(m_fValue[2]);
	if (x <= y && x <= z) return this->CrossProduct(CVector3::X);
	else if (y <= x && y <= z) return this->CrossProduct(CVector3::Y);
	else return this->CrossProduct(CVector3::Z);
}

/**
* 重载运算符 []
*/
float& CVector3::operator [] (int index) {
	return m_fValue[index];
}

/**
* 重载运算符 ==
*/
bool CVector3::operator == (const CVector3& vector) const {
	if (m_fValue[0] == vector.m_fValue[0] &&
		m_fValue[1] == vector.m_fValue[1] &&
		m_fValue[2] == vector.m_fValue[2]) {
		return true;
	}
	return false;
}

/**
* 重载运算符 !=
*/
bool CVector3::operator != (const CVector3& vector) const {
	return !(*this == vector);
}

/**
* 重载运算符 +
*/
CVector3 CVector3::operator + (const CVector3& vector) const {
	float vTemp[3];
	vTemp[0] = m_fValue[0] + vector.m_fValue[0];
	vTemp[1] = m_fValue[1] + vector.m_fValue[1];
	vTemp[2] = m_fValue[2] + vector.m_fValue[2];
	return CVector3(vTemp);
}

/**
* 重载运算符 -
*/
CVector3 CVector3::operator - (const CVector3& vector) const {
	float vTemp[3];
	vTemp[0] = m_fValue[0] - vector.m_fValue[0];
	vTemp[1] = m_fValue[1] - vector.m_fValue[1];
	vTemp[2] = m_fValue[2] - vector.m_fValue[2];
	return CVector3(vTemp);
}

/**
* 重载运算符 *
*/
CVector3 CVector3::operator * (const CVector3& vector) const {
	float vTemp[3];
	vTemp[0] = m_fValue[0] * vector.m_fValue[0];
	vTemp[1] = m_fValue[1] * vector.m_fValue[1];
	vTemp[2] = m_fValue[2] * vector.m_fValue[2];
	return CVector3(vTemp);
}

/**
* 重载运算符 *
*/
CVector3 CVector3::operator * (float factor) const {
	float vTemp[3];
	vTemp[0] = m_fValue[0] * factor;
	vTemp[1] = m_fValue[1] * factor;
	vTemp[2] = m_fValue[2] * factor;
	return CVector3(vTemp);
}

/**
* 重载运算符 /
*/
CVector3 CVector3::operator / (float factor) const {
	float vTemp[3];
	factor = 1.0f / factor;
	vTemp[0] = m_fValue[0] * factor;
	vTemp[1] = m_fValue[1] * factor;
	vTemp[2] = m_fValue[2] * factor;
	return CVector3(vTemp);
}

/**
* 重载运算符 +=
*/
CVector3& CVector3::operator += (const CVector3& vector) {
	m_fValue[0] += vector.m_fValue[0];
	m_fValue[1] += vector.m_fValue[1];
	m_fValue[2] += vector.m_fValue[2];
	return *this;
}

/**
* 重载运算符 -=
*/
CVector3& CVector3::operator -= (const CVector3& vector) {
	m_fValue[0] -= vector.m_fValue[0];
	m_fValue[1] -= vector.m_fValue[1];
	m_fValue[2] -= vector.m_fValue[2];
	return *this;
}

/**
* 重载运算符 *=
*/
CVector3& CVector3::operator *= (const CVector3& vector) {
	m_fValue[0] *= vector.m_fValue[0];
	m_fValue[1] *= vector.m_fValue[1];
	m_fValue[2] *= vector.m_fValue[2];
	return *this;
}

/**
* 重载运算符 *=
*/
CVector3& CVector3::operator *= (float factor) {
	m_fValue[0] *= factor;
	m_fValue[1] *= factor;
	m_fValue[2] *= factor;
	return *this;
}

/**
* 重载运算符 /=
*/
CVector3& CVector3::operator /= (float factor) {
	factor = 1.0f / factor;
	m_fValue[0] *= factor;
	m_fValue[1] *= factor;
	m_fValue[2] *= factor;
	return *this;
}