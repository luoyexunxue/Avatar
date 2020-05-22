//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CRectangle.h"

/**
* ��̬����
*/
const CRectangle CRectangle::Empty;

/**
* ���캯��
*/
CRectangle::CRectangle() {
	m_iValue[0] = 0;
	m_iValue[1] = 0;
	m_iValue[2] = 0;
	m_iValue[3] = 0;
}

/**
* ���ƹ��캯��
*/
CRectangle::CRectangle(const CRectangle& rect) {
	m_iValue[0] = rect.m_iValue[0];
	m_iValue[1] = rect.m_iValue[1];
	m_iValue[2] = rect.m_iValue[2];
	m_iValue[3] = rect.m_iValue[3];
}

/**
* ���캯����ָ�����Ͽ��ֵ
*/
CRectangle::CRectangle(const int rect[4]) {
	m_iValue[0] = rect[0];
	m_iValue[1] = rect[1];
	m_iValue[2] = rect[2];
	m_iValue[3] = rect[3];
}

/**
* ���캯����ָ�����Ͽ��ֵ
*/
CRectangle::CRectangle(int x, int y, int w, int h) {
	m_iValue[0] = x;
	m_iValue[1] = y;
	m_iValue[2] = w;
	m_iValue[3] = h;
}

/**
* ���þ���ֵ
*/
void CRectangle::SetValue(const CRectangle& rect) {
	m_iValue[0] = rect.m_iValue[0];
	m_iValue[1] = rect.m_iValue[1];
	m_iValue[2] = rect.m_iValue[2];
	m_iValue[3] = rect.m_iValue[3];
}

/**
* ���þ���ֵ
*/
void CRectangle::SetValue(const int rect[4]) {
	m_iValue[0] = rect[0];
	m_iValue[1] = rect[1];
	m_iValue[2] = rect[2];
	m_iValue[3] = rect[3];
}

/**
* ���þ���ֵ
*/
void CRectangle::SetValue(int x, int y, int w, int h) {
	m_iValue[0] = x;
	m_iValue[1] = y;
	m_iValue[2] = w;
	m_iValue[3] = h;
}

/**
* ����ƫ��
*/
CRectangle& CRectangle::Offset(int x, int y) {
	m_iValue[0] += x;
	m_iValue[1] += y;
	return *this;
}

/**
* ����ƫ��
*/
CRectangle& CRectangle::Offset(int offset[2]) {
	m_iValue[0] += offset[0];
	m_iValue[1] += offset[1];
	return *this;
}

/**
* ����ƫ��
*/
CRectangle& CRectangle::Offset(const CRectangle& rect) {
	m_iValue[0] += rect.m_iValue[0];
	m_iValue[1] += rect.m_iValue[1];
	return *this;
}

/**
* ��������
*/
CRectangle& CRectangle::Scale(float scale, bool center) {
	if (center) {
		m_iValue[0] += static_cast<int>(m_iValue[2] * (0.5f - scale * 0.5f));
		m_iValue[1] += static_cast<int>(m_iValue[3] * (0.5f - scale * 0.5f));
	}
	m_iValue[2] = static_cast<int>(m_iValue[2] * scale);
	m_iValue[3] = static_cast<int>(m_iValue[3] * scale);
	return *this;
}

/**
* ��������Ϊ��
*/
void CRectangle::SetEmpty() {
	m_iValue[2] = 0;
	m_iValue[3] = 0;
}

/**
* �ж������Ƿ�Ϊ��
*/
bool CRectangle::IsEmpty() const {
	if (m_iValue[2] <= 0 || m_iValue[3] <= 0) {
		return true;
	}
	return false;
}

/**
* �ж��Ƿ��н���
*/
bool CRectangle::IsIntersect(const CRectangle& rect) const {
	if (IsEmpty() || rect.IsEmpty()) return false;
	if (m_iValue[0] < rect.m_iValue[0] - m_iValue[2] ||
		m_iValue[1] < rect.m_iValue[1] - m_iValue[3] ||
		m_iValue[0] > rect.m_iValue[0] + rect.m_iValue[2] ||
		m_iValue[1] > rect.m_iValue[1] + rect.m_iValue[3]) {
		return false;
	}
	return true;
}

/**
* �ж��Ƿ������
*/
bool CRectangle::IsContain(int x, int y) const {
	if (IsEmpty()) return false;
	if (x >= m_iValue[0] && x <= m_iValue[0] + m_iValue[2] &&
		y >= m_iValue[1] && y <= m_iValue[1] + m_iValue[3]) {
		return true;
	}
	return false;
}

/**
* �ж��Ƿ��������
*/
bool CRectangle::IsContain(const CRectangle& rect) const {
	if (IsEmpty() || rect.IsEmpty()) return false;
	if (rect.m_iValue[0] >= m_iValue[0] &&
		rect.m_iValue[1] >= m_iValue[1] &&
		rect.m_iValue[0] + rect.m_iValue[2] <= m_iValue[0] + m_iValue[2] &&
		rect.m_iValue[1] + rect.m_iValue[3] <= m_iValue[1] + m_iValue[3]) {
		return true;
	}
	return false;
}

/**
* ��ȡ���λ��
*/
int CRectangle::GetLeft() const {
	return m_iValue[0];
}

/**
* ��ȡ�ϱ�λ��
*/
int CRectangle::GetTop() const {
	return m_iValue[1];
}

/**
* ��ȡ�ұ�λ��
*/
int CRectangle::GetRight() const {
	return m_iValue[0] + m_iValue[2];
}

/**
* ��ȡ�±�λ��
*/
int CRectangle::GetBottom() const {
	return m_iValue[1] + m_iValue[3];
}

/**
* ��ȡ���
*/
int CRectangle::GetWidth() const {
	return m_iValue[2];
}

/**
* ��ȡ�߶�
*/
int CRectangle::GetHeight() const {
	return m_iValue[3];
}

/**
* ��ȡ�������
*/
int CRectangle::GetArea() const {
	if (IsEmpty()) return 0;
	return m_iValue[2] * m_iValue[3];
}

/**
* �� rect �󲢼�
*/
CRectangle CRectangle::Union(const CRectangle& rect) const {
	if (IsEmpty()) return rect;
	if (rect.IsEmpty()) return *this;
	int x1 = m_iValue[0] + m_iValue[2];
	int y1 = m_iValue[1] + m_iValue[3];
	int x2 = rect.m_iValue[0] + rect.m_iValue[2];
	int y2 = rect.m_iValue[1] + rect.m_iValue[3];
	int x = m_iValue[0] < rect.m_iValue[0]? m_iValue[0]: rect.m_iValue[0];
	int y = m_iValue[1] < rect.m_iValue[1]? m_iValue[1]: rect.m_iValue[1];
	int w = (x1 > x2? x1: x2) - x;
	int h = (y1 > y2? y1: y2) - y;
	return CRectangle(x, y, w, h);
}

/**
* �� rect �󽻼�
*/
CRectangle CRectangle::Intersect(const CRectangle& rect) const {
	int x1 = m_iValue[0] + m_iValue[2];
	int y1 = m_iValue[1] + m_iValue[3];
	int x2 = rect.m_iValue[0] + rect.m_iValue[2];
	int y2 = rect.m_iValue[1] + rect.m_iValue[3];
	int x = m_iValue[0] < rect.m_iValue[0]? rect.m_iValue[0]: m_iValue[0];
	int y = m_iValue[1] < rect.m_iValue[1]? rect.m_iValue[1]: m_iValue[1];
	int w = (x1 > x2? x2: x1) - x;
	int h = (y1 > y2? y2: y1) - y;
	if (w <= 0 || h <= 0) return CRectangle();
	return CRectangle(x, y, w, h);
}

/**
* ��������� []
*/
int& CRectangle::operator [] (int index) {
	return m_iValue[index];
}

/**
* ��������� ==
*/
bool CRectangle::operator == (const CRectangle& rect) const {
	if (m_iValue[0] == rect.m_iValue[0] &&
		m_iValue[1] == rect.m_iValue[1] &&
		m_iValue[2] == rect.m_iValue[2] &&
		m_iValue[3] == rect.m_iValue[3]) {
		return true;
	}
	return false;
}

/**
* ��������� !=
*/
bool CRectangle::operator != (const CRectangle& rect) const {
	return !(*this == rect);
}

/**
* ��������
*/
CRectangle& CRectangle::operator += (const CRectangle& rect) {
	if (IsEmpty()) {
		m_iValue[0] = rect.m_iValue[0];
		m_iValue[1] = rect.m_iValue[1];
		m_iValue[2] = rect.m_iValue[2];
		m_iValue[3] = rect.m_iValue[3];
		return *this;
	}
	if (rect.IsEmpty()) return *this;
	int x1 = m_iValue[0] + m_iValue[2];
	int y1 = m_iValue[1] + m_iValue[3];
	int x2 = rect.m_iValue[0] + rect.m_iValue[2];
	int y2 = rect.m_iValue[1] + rect.m_iValue[3];
	m_iValue[0] = m_iValue[0] < rect.m_iValue[0] ? m_iValue[0] : rect.m_iValue[0];
	m_iValue[1] = m_iValue[1] < rect.m_iValue[1] ? m_iValue[1] : rect.m_iValue[1];
	m_iValue[2] = (x1 > x2 ? x1 : x2) - m_iValue[0];
	m_iValue[3] = (y1 > y2 ? y1 : y2) - m_iValue[1];
	return *this;
}

/**
* ��������
*/
CRectangle& CRectangle::operator *= (const CRectangle& rect) {
	int x1 = m_iValue[0] + m_iValue[2];
	int y1 = m_iValue[1] + m_iValue[3];
	int x2 = rect.m_iValue[0] + rect.m_iValue[2];
	int y2 = rect.m_iValue[1] + rect.m_iValue[3];
	m_iValue[0] = m_iValue[0] < rect.m_iValue[0] ? rect.m_iValue[0] : m_iValue[0];
	m_iValue[1] = m_iValue[1] < rect.m_iValue[1] ? rect.m_iValue[1] : m_iValue[1];
	m_iValue[2] = (x1 > x2 ? x2 : x1) - m_iValue[0];
	m_iValue[3] = (y1 > y2 ? y2 : y1) - m_iValue[1];
	return *this;
}