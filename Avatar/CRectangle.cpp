//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CRectangle.h"

/**
* 静态常量
*/
const CRectangle CRectangle::Empty;

/**
* 构造函数
*/
CRectangle::CRectangle() {
	m_iValue[0] = 0;
	m_iValue[1] = 0;
	m_iValue[2] = 0;
	m_iValue[3] = 0;
}

/**
* 复制构造函数
*/
CRectangle::CRectangle(const CRectangle& rect) {
	m_iValue[0] = rect.m_iValue[0];
	m_iValue[1] = rect.m_iValue[1];
	m_iValue[2] = rect.m_iValue[2];
	m_iValue[3] = rect.m_iValue[3];
}

/**
* 构造函数，指定左上宽高值
*/
CRectangle::CRectangle(const int rect[4]) {
	m_iValue[0] = rect[0];
	m_iValue[1] = rect[1];
	m_iValue[2] = rect[2];
	m_iValue[3] = rect[3];
}

/**
* 构造函数，指定左上宽高值
*/
CRectangle::CRectangle(int x, int y, int w, int h) {
	m_iValue[0] = x;
	m_iValue[1] = y;
	m_iValue[2] = w;
	m_iValue[3] = h;
}

/**
* 设置矩形值
*/
void CRectangle::SetValue(const CRectangle& rect) {
	m_iValue[0] = rect.m_iValue[0];
	m_iValue[1] = rect.m_iValue[1];
	m_iValue[2] = rect.m_iValue[2];
	m_iValue[3] = rect.m_iValue[3];
}

/**
* 设置矩形值
*/
void CRectangle::SetValue(const int rect[4]) {
	m_iValue[0] = rect[0];
	m_iValue[1] = rect[1];
	m_iValue[2] = rect[2];
	m_iValue[3] = rect[3];
}

/**
* 设置矩形值
*/
void CRectangle::SetValue(int x, int y, int w, int h) {
	m_iValue[0] = x;
	m_iValue[1] = y;
	m_iValue[2] = w;
	m_iValue[3] = h;
}

/**
* 设置偏移
*/
CRectangle& CRectangle::Offset(int x, int y) {
	m_iValue[0] += x;
	m_iValue[1] += y;
	return *this;
}

/**
* 设置偏移
*/
CRectangle& CRectangle::Offset(int offset[2]) {
	m_iValue[0] += offset[0];
	m_iValue[1] += offset[1];
	return *this;
}

/**
* 设置偏移
*/
CRectangle& CRectangle::Offset(const CRectangle& rect) {
	m_iValue[0] += rect.m_iValue[0];
	m_iValue[1] += rect.m_iValue[1];
	return *this;
}

/**
* 设置缩放
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
* 设置宽高
*/
CRectangle& CRectangle::Resize(int width, int height) {
	m_iValue[2] = width;
	m_iValue[3] = height;
	return *this;
}

/**
* 设置区域为空
*/
void CRectangle::SetEmpty() {
	m_iValue[2] = 0;
	m_iValue[3] = 0;
}

/**
* 判断区域是否为空
*/
bool CRectangle::IsEmpty() const {
	if (m_iValue[2] <= 0 || m_iValue[3] <= 0) {
		return true;
	}
	return false;
}

/**
* 判断是否有交集
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
* 判断是否包含点
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
* 判断是否包含区域
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
* 获取左边位置
*/
int CRectangle::GetLeft() const {
	return m_iValue[0];
}

/**
* 获取上边位置
*/
int CRectangle::GetTop() const {
	return m_iValue[1];
}

/**
* 获取右边位置
*/
int CRectangle::GetRight() const {
	return m_iValue[0] + m_iValue[2];
}

/**
* 获取下边位置
*/
int CRectangle::GetBottom() const {
	return m_iValue[1] + m_iValue[3];
}

/**
* 获取宽度
*/
int CRectangle::GetWidth() const {
	return m_iValue[2];
}

/**
* 获取高度
*/
int CRectangle::GetHeight() const {
	return m_iValue[3];
}

/**
* 获取区域面积
*/
int CRectangle::GetArea() const {
	if (IsEmpty()) return 0;
	return m_iValue[2] * m_iValue[3];
}

/**
* 与 rect 求并集
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
* 与 rect 求交集
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
* 重载运算符 []
*/
int& CRectangle::operator [] (int index) {
	return m_iValue[index];
}

/**
* 重载运算符 ==
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
* 重载运算符 !=
*/
bool CRectangle::operator != (const CRectangle& rect) const {
	return !(*this == rect);
}

/**
* 并集计算
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
* 交集计算
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