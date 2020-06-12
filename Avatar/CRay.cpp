//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CRay.h"
#include <cmath>

/**
* 构造函数
*/
CRay::CRay() {
	m_cOrigin.SetValue(0, 0, 0);
	m_cDirection.SetValue(0, 1, 0, 0);
}

/**
* 复制构造函数
*/
CRay::CRay(const CRay& ray) {
	m_cOrigin = ray.m_cOrigin;
	m_cDirection = ray.m_cDirection;
}

/**
* 构造函数，指定起点和方向
*/
CRay::CRay(const CVector3& origin, const CVector3& direction) {
	m_cOrigin = origin;
	m_cDirection = direction;
	m_cDirection[3] = 0;
	m_cDirection.Normalize();
}

/**
* 设置射线值
*/
void CRay::SetValue(const CRay& ray) {
	m_cOrigin = ray.m_cOrigin;
	m_cDirection = ray.m_cDirection;
}

/**
* 设置射线值
*/
void CRay::SetValue(const float origin[3], const float direction[3]) {
	m_cOrigin.SetValue(origin);
	m_cDirection.SetValue(direction);
	m_cDirection.Normalize();
}

/**
* 使用矩阵变换射线
*/
CRay& CRay::Transform(const CMatrix4& matrix) {
	m_cOrigin = matrix * m_cOrigin;
	m_cDirection = matrix * m_cDirection;
	return *this;
}

/**
* 将射线反向
*/
CRay& CRay::Reverse() {
	m_cDirection.Scale(-1.0f);
	return *this;
}

/**
* 获取射线与平面的交点
*/
bool CRay::IntersectPlane(const CPlane& plane, CVector3& point) const {
	CVector3 normal(plane.m_fNormal);
	float t = m_cDirection.DotProduct(normal);
	// 射线与平面平行
	if (t == 0.0f) return false;
	float s = (plane.m_fDistance + m_cOrigin.DotProduct(normal)) / t;
	// 射线与平面不相交
	if (s > 0.0f) return false;
	point = m_cOrigin - m_cDirection * s;
	return true;
}

/**
* 获取射线与球体的交点
*/
bool CRay::IntersectSphere(const CVector3& center, float radius, CVector3& point) const {
	// 将球体移至坐标原点进行计算
	CVector3 originRel = m_cOrigin - center;
	float oo = originRel.DotProduct(originRel);
	float od = originRel.DotProduct(m_cDirection);
	float delta = radius * radius + od * od - oo;
	// 无交点
	if (delta < 0) return false;
	delta = sqrtf(delta);
	float t1 = -(od + delta);
	float t2 = -(od - delta);
	CVector3 pt1 = m_cDirection * t1 + originRel;
	CVector3 pt2 = m_cDirection * t2 + originRel;
	// 获取到射线起点距离最近的交点
	CVector3 len1 = pt1 - originRel;
	CVector3 len2 = pt2 - originRel;
	if (len1.DotProduct(len1) > len2.DotProduct(len2)) {
		point = pt2 + center;
	} else {
		point = pt1 + center;
	}
	return true;
}

/**
* 获取射线与包围盒的交点
*/
bool CRay::IntersectAABB(const CBoundingBox& aabb, CVector3& point) const {
	// 包围盒无效
	if (!aabb.IsValid()) return false;
	// 射线起点在包围盒内
	if (aabb.IsContain(m_cOrigin)) return true;

	// 依次检查各个面的相交情况
	float distance = -1.0f;
	for (int i = 0; i < 3; i++) {
		int indexA = (i + 1) % 3;
		int indexB = (i + 2) % 3;
		if (m_cOrigin.m_fValue[i] < aabb.m_cMin.m_fValue[i] && m_cDirection.m_fValue[i] > 0) {
			float t = (aabb.m_cMin.m_fValue[i] - m_cOrigin.m_fValue[i]) / m_cDirection.m_fValue[i];
			if (t > 0 && (t < distance || distance < 0.0f)) {
				CVector3 hit = m_cOrigin + m_cDirection * t;
				if (hit[indexA] >= aabb.m_cMin.m_fValue[indexA] && hit[indexA] <= aabb.m_cMax.m_fValue[indexA] &&
					hit[indexB] >= aabb.m_cMin.m_fValue[indexB] && hit[indexB] <= aabb.m_cMax.m_fValue[indexB]) {
					distance = t;
					point = hit;
				}
			}
		}
		if (m_cOrigin.m_fValue[i] > aabb.m_cMax.m_fValue[i] && m_cDirection.m_fValue[i] < 0) {
			float t = (aabb.m_cMax.m_fValue[i] - m_cOrigin.m_fValue[i]) / m_cDirection.m_fValue[i];
			if (t > 0 && (t < distance || distance < 0.0f)) {
				CVector3 hit = m_cOrigin + m_cDirection * t;
				if (hit[indexA] >= aabb.m_cMin.m_fValue[indexA] && hit[indexA] <= aabb.m_cMax.m_fValue[indexA] &&
					hit[indexB] >= aabb.m_cMin.m_fValue[indexB] && hit[indexB] <= aabb.m_cMax.m_fValue[indexB]) {
					distance = t;
					point = hit;
				}
			}
		}
	}
	return distance >= 0.0f;
}

/**
* 重载运算符 ==
*/
bool CRay::operator == (const CRay& ray) const {
	return (m_cOrigin == ray.m_cOrigin && m_cDirection == ray.m_cDirection);
}

/**
* 重载运算符 !=
*/
bool CRay::operator != (const CRay& ray) const {
	return !(*this == ray);
}