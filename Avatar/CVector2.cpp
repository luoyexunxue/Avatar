//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CVector2.h"
#include <cmath>

/**
* 静态常量
*/
const CVector2 CVector2::Zero(0.0f, 0.0f);
const CVector2 CVector2::X(1.0f, 0.0f);
const CVector2 CVector2::Y(0.0f, 1.0f);

/**
* 构造函数
*/
CVector2::CVector2() {
	m_fValue[0] = 0;
	m_fValue[1] = 0;
}

/**
* 复制构造函数
*/
CVector2::CVector2(const CVector2& vector) {
	m_fValue[0] = vector.m_fValue[0];
	m_fValue[1] = vector.m_fValue[1];
}

/**
* 构造函数
*/
CVector2::CVector2(float val) {
	m_fValue[0] = val;
	m_fValue[1] = val;
}

/**
* 构造函数
*/
CVector2::CVector2(const float vector[2]) {
	m_fValue[0] = vector[0];
	m_fValue[1] = vector[1];
}

/**
* 构造函数
*/
CVector2::CVector2(float x, float y) {
	m_fValue[0] = x;
	m_fValue[1] = y;
}

/**
* 构造函数
*/
CVector2::CVector2(int x, int y) {
	m_fValue[0] = static_cast<float>(x);
	m_fValue[1] = static_cast<float>(y);
}

/**
* 设置向量值
*/
void CVector2::SetValue(const CVector2& vector) {
	m_fValue[0] = vector.m_fValue[0];
	m_fValue[1] = vector.m_fValue[1];
}

/**
* 设置向量值
*/
void CVector2::SetValue(const float vector[2]) {
	m_fValue[0] = vector[0];
	m_fValue[1] = vector[1];
}

/**
* 设置向量值
*/
void CVector2::SetValue(float x, float y) {
	m_fValue[0] = x;
	m_fValue[1] = y;
}

/**
* 转换为单位向量
*/
CVector2& CVector2::Normalize() {
	float len = Length();
	if (len == 0) return *this;
	len = 1.0f / len;
	m_fValue[0] *= len;
	m_fValue[1] *= len;
	return *this;
}

/**
* 加指定值
*/
CVector2& CVector2::Add(const CVector2& vector) {
	m_fValue[0] += vector.m_fValue[0];
	m_fValue[1] += vector.m_fValue[1];
	return *this;
}

/**
* 加指定值
*/
CVector2& CVector2::Add(const float vector[2]) {
	m_fValue[0] += vector[0];
	m_fValue[1] += vector[1];
	return *this;
}

/**
* 加指定值
*/
CVector2& CVector2::Add(float x, float y) {
	m_fValue[0] += x;
	m_fValue[1] += y;
	return *this;
}

/**
* 减指定值
*/
CVector2& CVector2::Sub(const CVector2& vector) {
	m_fValue[0] -= vector.m_fValue[0];
	m_fValue[1] -= vector.m_fValue[1];
	return *this;
}

/**
* 减指定值
*/
CVector2& CVector2::Sub(const float vector[2]) {
	m_fValue[0] -= vector[0];
	m_fValue[1] -= vector[1];
	return *this;
}

/**
* 减指定值
*/
CVector2& CVector2::Sub(float x, float y) {
	m_fValue[0] -= x;
	m_fValue[1] -= y;
	return *this;
}

/**
* 缩放指定值
*/
CVector2& CVector2::Scale(float scale) {
	m_fValue[0] *= scale;
	m_fValue[1] *= scale;
	return *this;
}

/**
* 缩放指定值
*/
CVector2& CVector2::Scale(float sx, float sy) {
	m_fValue[0] *= sx;
	m_fValue[1] *= sy;
	return *this;
}

/**
* 取得向量的长度
*/
float CVector2::Length() const {
	float length = m_fValue[0] * m_fValue[0] + m_fValue[1] * m_fValue[1];
	if (length < 1E-9) return 0;
	return sqrtf(length);
}

/**
* 向量点乘
*/
float CVector2::DotProduct(const CVector2& vector) const {
	return m_fValue[0] * vector.m_fValue[0] + m_fValue[1] * vector.m_fValue[1];
}

/**
* 向量叉乘
*/
float CVector2::CrossProduct(const CVector2& vector) const {
	return m_fValue[0] * vector.m_fValue[1] - m_fValue[1] * vector.m_fValue[0];
}

/**
* 线性插值
*/
CVector2 CVector2::Lerp(const CVector2& vector, float t) const {
	float vTemp[2];
	vTemp[0] = m_fValue[0] + (vector.m_fValue[0] - m_fValue[0]) * t;
	vTemp[1] = m_fValue[1] + (vector.m_fValue[1] - m_fValue[1]) * t;
	return CVector2(vTemp);
}

/**
* 弧形插值
*/
CVector2 CVector2::Slerp(const CVector2& vector, float t) const {
	CVector2 temp;
	temp[0] = m_fValue[0] + (vector.m_fValue[0] - m_fValue[0]) * t;
	temp[1] = m_fValue[1] + (vector.m_fValue[1] - m_fValue[1]) * t;
	// 进行长度的插值
	float length1 = this->Length();
	float length2 = vector.Length();
	return temp.Scale((length1 + (length2 - length1) * t) / temp.Length());
}

/**
* 求反射向量，注意源向量和反射向量方向均在法向一侧
*/
CVector2 CVector2::Reflect(const CVector2& normal) const {
	CVector2 n(normal);
	n.Normalize();
	n.Scale(this->DotProduct(n) * 2.0f);
	return n - *this;
}

/**
* 重载运算符 []
*/
float& CVector2::operator [] (int index) {
	return m_fValue[index];
}

/**
* 重载运算符 ==
*/
bool CVector2::operator == (const CVector2& vector) const {
	if (m_fValue[0] == vector.m_fValue[0] &&
		m_fValue[1] == vector.m_fValue[1]) {
		return true;
	}
	return false;
}

/**
* 重载运算符 !=
*/
bool CVector2::operator != (const CVector2& vector) const {
	return !(*this == vector);
}

/**
* 重载运算符 +
*/
CVector2 CVector2::operator + (const CVector2& vector) const {
	float vTemp[2];
	vTemp[0] = m_fValue[0] + vector.m_fValue[0];
	vTemp[1] = m_fValue[1] + vector.m_fValue[1];
	return CVector2(vTemp);
}

/**
* 重载运算符 -
*/
CVector2 CVector2::operator - (const CVector2& vector) const {
	float vTemp[2];
	vTemp[0] = m_fValue[0] - vector.m_fValue[0];
	vTemp[1] = m_fValue[1] - vector.m_fValue[1];
	return CVector2(vTemp);
}

/**
* 重载运算符 *
*/
CVector2 CVector2::operator * (float factor) const {
	float vTemp[2];
	vTemp[0] = m_fValue[0] * factor;
	vTemp[1] = m_fValue[1] * factor;
	return CVector2(vTemp);
}

/**
* 重载运算符 /
*/
CVector2 CVector2::operator / (float factor) const {
	float vTemp[2];
	factor = 1.0f / factor;
	vTemp[0] = m_fValue[0] * factor;
	vTemp[1] = m_fValue[1] * factor;
	return CVector2(vTemp);
}

/**
* 重载运算符 +=
*/
CVector2& CVector2::operator += (const CVector2& vector) {
	m_fValue[0] += vector.m_fValue[0];
	m_fValue[1] += vector.m_fValue[1];
	return *this;
}

/**
* 重载运算符 -=
*/
CVector2& CVector2::operator -= (const CVector2& vector) {
	m_fValue[0] -= vector.m_fValue[0];
	m_fValue[1] -= vector.m_fValue[1];
	return *this;
}

/**
* 重载运算符 *=
*/
CVector2& CVector2::operator *= (float factor) {
	m_fValue[0] *= factor;
	m_fValue[1] *= factor;
	return *this;
}

/**
* 重载运算符 /=
*/
CVector2& CVector2::operator /= (float factor) {
	factor = 1.0f / factor;
	m_fValue[0] *= factor;
	m_fValue[1] *= factor;
	return *this;
}