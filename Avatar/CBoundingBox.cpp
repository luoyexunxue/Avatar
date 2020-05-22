//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CBoundingBox.h"
#include "CVector3.h"
#include <cfloat>

#ifndef FLT_MAX
#define FLT_MAX __FLT_MAX__
#endif

/**
* 构造函数
*/
CBoundingBox::CBoundingBox() {
	m_cMin.SetValue(FLT_MAX, FLT_MAX, FLT_MAX);
	m_cMax.SetValue(-FLT_MAX, -FLT_MAX, -FLT_MAX);
}

/**
* 复制构造函数
*/
CBoundingBox::CBoundingBox(const CBoundingBox& aabb) {
	m_cMin = aabb.m_cMin;
	m_cMax = aabb.m_cMax;
}

/**
* 使用极值坐标构造包围盒
*/
CBoundingBox::CBoundingBox(const CVector3& min, const CVector3& max) {
	m_cMin.SetValue(min.m_fValue);
	m_cMax.SetValue(max.m_fValue);
}

/**
* 使用极值坐标构造包围盒
*/
CBoundingBox::CBoundingBox(float minx, float miny, float minz, float maxx, float maxy, float maxz) {
	m_cMin.SetValue(minx, miny, minz);
	m_cMax.SetValue(maxx, maxy, maxz);
}

/**
* 设置包围盒值
*/
void CBoundingBox::SetValue(const CBoundingBox& aabb) {
	m_cMin.SetValue(aabb.m_cMin);
	m_cMax.SetValue(aabb.m_cMax);
}

/**
* 设置包围盒值
*/
void CBoundingBox::SetValue(const CVector3& min, const CVector3& max) {
	m_cMin.SetValue(min.m_fValue);
	m_cMax.SetValue(max.m_fValue);
}

/**
* 设置包围盒值
*/
void CBoundingBox::SetValue(float minx, float miny, float minz, float maxx, float maxy, float maxz) {
	m_cMin.SetValue(minx, miny, minz);
	m_cMax.SetValue(maxx, maxy, maxz);
}

/**
* 设置为无效包围盒
*/
void CBoundingBox::SetInvalid() {
	m_cMin.SetValue(FLT_MAX, FLT_MAX, FLT_MAX);
	m_cMax.SetValue(-FLT_MAX, -FLT_MAX, -FLT_MAX);
}

/**
* 更新包围盒
* @param point 需要包含的点
* @note 包围盒将扩大至包含 point
*/
void CBoundingBox::Update(const CVector3& point) {
	if (point.m_fValue[0] > m_cMax[0]) m_cMax[0] = point.m_fValue[0];
	if (point.m_fValue[0] < m_cMin[0]) m_cMin[0] = point.m_fValue[0];
	if (point.m_fValue[1] > m_cMax[1]) m_cMax[1] = point.m_fValue[1];
	if (point.m_fValue[1] < m_cMin[1]) m_cMin[1] = point.m_fValue[1];
	if (point.m_fValue[2] > m_cMax[2]) m_cMax[2] = point.m_fValue[2];
	if (point.m_fValue[2] < m_cMin[2]) m_cMin[2] = point.m_fValue[2];
}

/**
* 更新包围盒
* @param aabb 需要包含的AABB包围盒
* @note 包围盒将扩大至包含 aabb
*/
void CBoundingBox::Update(const CBoundingBox& aabb) {
	for (int i = 0; i < 3; i++) {
		if (m_cMin[i] > aabb.m_cMin.m_fValue[i]) m_cMin[i] = aabb.m_cMin.m_fValue[i];
		if (m_cMax[i] < aabb.m_cMax.m_fValue[i]) m_cMax[i] = aabb.m_cMax.m_fValue[i];
	}
}

/**
* 判断是否为有效包围盒
*/
bool CBoundingBox::IsValid() const {
	if (m_cMin.m_fValue[0] > m_cMax.m_fValue[0]) return false;
	if (m_cMin.m_fValue[1] > m_cMax.m_fValue[1]) return false;
	if (m_cMin.m_fValue[2] > m_cMax.m_fValue[2]) return false;
	return true;
}

/**
* 是否和另一个包围盒重叠
*/
bool CBoundingBox::IsOverlap(const CBoundingBox& aabb) const {
	if (m_cMax.m_fValue[0] < aabb.m_cMin.m_fValue[0] || m_cMin.m_fValue[0] > aabb.m_cMax.m_fValue[0]) return false;
	if (m_cMax.m_fValue[1] < aabb.m_cMin.m_fValue[1] || m_cMin.m_fValue[1] > aabb.m_cMax.m_fValue[1]) return false;
	if (m_cMax.m_fValue[2] < aabb.m_cMin.m_fValue[2] || m_cMin.m_fValue[2] > aabb.m_cMax.m_fValue[2]) return false;
	return true;
}

/**
* 是否包含指定包围盒
*/
bool CBoundingBox::IsContain(const CBoundingBox& aabb) const {
	if (m_cMin.m_fValue[0] <= aabb.m_cMin.m_fValue[0] && m_cMax.m_fValue[0] >= aabb.m_cMax.m_fValue[0] &&
		m_cMin.m_fValue[1] <= aabb.m_cMin.m_fValue[1] && m_cMax.m_fValue[1] >= aabb.m_cMax.m_fValue[1] &&
		m_cMin.m_fValue[2] <= aabb.m_cMin.m_fValue[2] && m_cMax.m_fValue[2] >= aabb.m_cMax.m_fValue[2]) {
		return true;
	}
	return false;
}

/**
* 是否包含指定点
*/
bool CBoundingBox::IsContain(const CVector3& point) const {
	if (point.m_fValue[0] >= m_cMin.m_fValue[0] && point.m_fValue[0] <= m_cMax.m_fValue[0] &&
		point.m_fValue[1] >= m_cMin.m_fValue[1] && point.m_fValue[1] <= m_cMax.m_fValue[1] &&
		point.m_fValue[2] >= m_cMin.m_fValue[2] && point.m_fValue[2] <= m_cMax.m_fValue[2]) {
		return true;
	}
	return false;
}

/**
* 计算表面积
*/
float CBoundingBox::SurfaceArea() const {
	if (!IsValid()) return 0.0f;
	float dx = m_cMax.m_fValue[0] - m_cMin.m_fValue[0];
	float dy = m_cMax.m_fValue[1] - m_cMin.m_fValue[1];
	float dz = m_cMax.m_fValue[2] - m_cMin.m_fValue[2];
	return 2 * (dx * dy + dy * dz + dx * dz);
}

/**
* 计算各轴的大小
* @param axis 0-X 1-Y 2-Z
*/
float CBoundingBox::Size(int axis) const {
	if (!IsValid()) return 0.0f;
	if (axis == 0) return m_cMax.m_fValue[0] - m_cMin.m_fValue[0];
	if (axis == 1) return m_cMax.m_fValue[1] - m_cMin.m_fValue[1];
	if (axis == 2) return m_cMax.m_fValue[2] - m_cMin.m_fValue[2];
	return 0.0f;
}

/**
* 计算中心点位置
*/
CVector3 CBoundingBox::Center() const {
	return (m_cMax + m_cMin) * 0.5f;
}

/**
* 对包围盒相对于中心点进行缩放
*/
CBoundingBox& CBoundingBox::Scale(const CVector3& scale) {
	float dx = m_cMax.m_fValue[0] - m_cMin.m_fValue[0];
	float dy = m_cMax.m_fValue[1] - m_cMin.m_fValue[1];
	float dz = m_cMax.m_fValue[2] - m_cMin.m_fValue[2];
	dx = (scale.m_fValue[0] * dx - dx) * 0.5f;
	dy = (scale.m_fValue[1] * dy - dy) * 0.5f;
	dz = (scale.m_fValue[2] * dz - dz) * 0.5f;
	m_cMax.Add(dx, dy, dz);
	m_cMin.Sub(dx, dy, dz);
	return *this;
}

/**
* 对包围盒进行偏移
*/
CBoundingBox& CBoundingBox::Offset(const CVector3& offset) {
	m_cMax.Add(offset);
	m_cMin.Add(offset);
	return *this;
}

/**
* 重载 == 运算符
*/
bool CBoundingBox::operator == (const CBoundingBox& aabb) const {
	return (m_cMin == aabb.m_cMin && m_cMax == aabb.m_cMax);
}

/**
* 重载 != 运算符
*/
bool CBoundingBox::operator != (const CBoundingBox& aabb) const {
	return !(*this == aabb);
}

/**
* 重载 + 运算符，计算合集
*/
CBoundingBox CBoundingBox::operator + (const CBoundingBox& aabb) const {
	CBoundingBox boundingBox;
	for (int i = 0; i < 3; i++) {
		if (m_cMin.m_fValue[i] < aabb.m_cMin.m_fValue[i]) boundingBox.m_cMin[i] = m_cMin.m_fValue[i];
		else boundingBox.m_cMin[i] = aabb.m_cMin.m_fValue[i];
		if (m_cMax.m_fValue[i] > aabb.m_cMax.m_fValue[i]) boundingBox.m_cMax[i] = m_cMax.m_fValue[i];
		else boundingBox.m_cMax[i] = aabb.m_cMax.m_fValue[i];
	}
	return boundingBox;
}

/**
* 重载 += 运算符，计算合集
*/
CBoundingBox& CBoundingBox::operator += (const CBoundingBox& aabb) {
	for (int i = 0; i < 3; i++) {
		if (m_cMin[i] > aabb.m_cMin.m_fValue[i]) m_cMin[i] = aabb.m_cMin.m_fValue[i];
		if (m_cMax[i] < aabb.m_cMax.m_fValue[i]) m_cMax[i] = aabb.m_cMax.m_fValue[i];
	}
	return *this;
}