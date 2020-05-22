//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CVector2.h"
#include <cmath>

/**
* ��̬����
*/
const CVector2 CVector2::Zero(0.0f, 0.0f);
const CVector2 CVector2::X(1.0f, 0.0f);
const CVector2 CVector2::Y(0.0f, 1.0f);

/**
* ���캯��
*/
CVector2::CVector2() {
	m_fValue[0] = 0;
	m_fValue[1] = 0;
}

/**
* ���ƹ��캯��
*/
CVector2::CVector2(const CVector2& vector) {
	m_fValue[0] = vector.m_fValue[0];
	m_fValue[1] = vector.m_fValue[1];
}

/**
* ���캯��
*/
CVector2::CVector2(float val) {
	m_fValue[0] = val;
	m_fValue[1] = val;
}

/**
* ���캯��
*/
CVector2::CVector2(const float vector[2]) {
	m_fValue[0] = vector[0];
	m_fValue[1] = vector[1];
}

/**
* ���캯��
*/
CVector2::CVector2(float x, float y) {
	m_fValue[0] = x;
	m_fValue[1] = y;
}

/**
* ���캯��
*/
CVector2::CVector2(int x, int y) {
	m_fValue[0] = static_cast<float>(x);
	m_fValue[1] = static_cast<float>(y);
}

/**
* ��������ֵ
*/
void CVector2::SetValue(const CVector2& vector) {
	m_fValue[0] = vector.m_fValue[0];
	m_fValue[1] = vector.m_fValue[1];
}

/**
* ��������ֵ
*/
void CVector2::SetValue(const float vector[2]) {
	m_fValue[0] = vector[0];
	m_fValue[1] = vector[1];
}

/**
* ��������ֵ
*/
void CVector2::SetValue(float x, float y) {
	m_fValue[0] = x;
	m_fValue[1] = y;
}

/**
* ת��Ϊ��λ����
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
* ��ָ��ֵ
*/
CVector2& CVector2::Add(const CVector2& vector) {
	m_fValue[0] += vector.m_fValue[0];
	m_fValue[1] += vector.m_fValue[1];
	return *this;
}

/**
* ��ָ��ֵ
*/
CVector2& CVector2::Add(const float vector[2]) {
	m_fValue[0] += vector[0];
	m_fValue[1] += vector[1];
	return *this;
}

/**
* ��ָ��ֵ
*/
CVector2& CVector2::Add(float x, float y) {
	m_fValue[0] += x;
	m_fValue[1] += y;
	return *this;
}

/**
* ��ָ��ֵ
*/
CVector2& CVector2::Sub(const CVector2& vector) {
	m_fValue[0] -= vector.m_fValue[0];
	m_fValue[1] -= vector.m_fValue[1];
	return *this;
}

/**
* ��ָ��ֵ
*/
CVector2& CVector2::Sub(const float vector[2]) {
	m_fValue[0] -= vector[0];
	m_fValue[1] -= vector[1];
	return *this;
}

/**
* ��ָ��ֵ
*/
CVector2& CVector2::Sub(float x, float y) {
	m_fValue[0] -= x;
	m_fValue[1] -= y;
	return *this;
}

/**
* ����ָ��ֵ
*/
CVector2& CVector2::Scale(float scale) {
	m_fValue[0] *= scale;
	m_fValue[1] *= scale;
	return *this;
}

/**
* ����ָ��ֵ
*/
CVector2& CVector2::Scale(float sx, float sy) {
	m_fValue[0] *= sx;
	m_fValue[1] *= sy;
	return *this;
}

/**
* ȡ�������ĳ���
*/
float CVector2::Length() const {
	float length = m_fValue[0] * m_fValue[0] + m_fValue[1] * m_fValue[1];
	if (length < 1E-9) return 0;
	return sqrtf(length);
}

/**
* �������
*/
float CVector2::DotProduct(const CVector2& vector) const {
	return m_fValue[0] * vector.m_fValue[0] + m_fValue[1] * vector.m_fValue[1];
}

/**
* �������
*/
float CVector2::CrossProduct(const CVector2& vector) const {
	return m_fValue[0] * vector.m_fValue[1] - m_fValue[1] * vector.m_fValue[0];
}

/**
* ���Բ�ֵ
*/
CVector2 CVector2::Lerp(const CVector2& vector, float t) const {
	float vTemp[2];
	vTemp[0] = m_fValue[0] + (vector.m_fValue[0] - m_fValue[0]) * t;
	vTemp[1] = m_fValue[1] + (vector.m_fValue[1] - m_fValue[1]) * t;
	return CVector2(vTemp);
}

/**
* ���β�ֵ
*/
CVector2 CVector2::Slerp(const CVector2& vector, float t) const {
	CVector2 temp;
	temp[0] = m_fValue[0] + (vector.m_fValue[0] - m_fValue[0]) * t;
	temp[1] = m_fValue[1] + (vector.m_fValue[1] - m_fValue[1]) * t;
	// ���г��ȵĲ�ֵ
	float length1 = this->Length();
	float length2 = vector.Length();
	return temp.Scale((length1 + (length2 - length1) * t) / temp.Length());
}

/**
* ����������ע��Դ�����ͷ�������������ڷ���һ��
*/
CVector2 CVector2::Reflect(const CVector2& normal) const {
	CVector2 n(normal);
	n.Normalize();
	n.Scale(this->DotProduct(n) * 2.0f);
	return n - *this;
}

/**
* ��������� []
*/
float& CVector2::operator [] (int index) {
	return m_fValue[index];
}

/**
* ��������� ==
*/
bool CVector2::operator == (const CVector2& vector) const {
	if (m_fValue[0] == vector.m_fValue[0] &&
		m_fValue[1] == vector.m_fValue[1]) {
		return true;
	}
	return false;
}

/**
* ��������� !=
*/
bool CVector2::operator != (const CVector2& vector) const {
	return !(*this == vector);
}

/**
* ��������� +
*/
CVector2 CVector2::operator + (const CVector2& vector) const {
	float vTemp[2];
	vTemp[0] = m_fValue[0] + vector.m_fValue[0];
	vTemp[1] = m_fValue[1] + vector.m_fValue[1];
	return CVector2(vTemp);
}

/**
* ��������� -
*/
CVector2 CVector2::operator - (const CVector2& vector) const {
	float vTemp[2];
	vTemp[0] = m_fValue[0] - vector.m_fValue[0];
	vTemp[1] = m_fValue[1] - vector.m_fValue[1];
	return CVector2(vTemp);
}

/**
* ��������� *
*/
CVector2 CVector2::operator * (float factor) const {
	float vTemp[2];
	vTemp[0] = m_fValue[0] * factor;
	vTemp[1] = m_fValue[1] * factor;
	return CVector2(vTemp);
}

/**
* ��������� /
*/
CVector2 CVector2::operator / (float factor) const {
	float vTemp[2];
	factor = 1.0f / factor;
	vTemp[0] = m_fValue[0] * factor;
	vTemp[1] = m_fValue[1] * factor;
	return CVector2(vTemp);
}

/**
* ��������� +=
*/
CVector2& CVector2::operator += (const CVector2& vector) {
	m_fValue[0] += vector.m_fValue[0];
	m_fValue[1] += vector.m_fValue[1];
	return *this;
}

/**
* ��������� -=
*/
CVector2& CVector2::operator -= (const CVector2& vector) {
	m_fValue[0] -= vector.m_fValue[0];
	m_fValue[1] -= vector.m_fValue[1];
	return *this;
}

/**
* ��������� *=
*/
CVector2& CVector2::operator *= (float factor) {
	m_fValue[0] *= factor;
	m_fValue[1] *= factor;
	return *this;
}

/**
* ��������� /=
*/
CVector2& CVector2::operator /= (float factor) {
	factor = 1.0f / factor;
	m_fValue[0] *= factor;
	m_fValue[1] *= factor;
	return *this;
}