//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CFrustum.h"
#include <cmath>

/**
* 默认构造函数
*/
CFrustum::CFrustum() {
	for (int i = 0; i < 6; i++) {
		m_fPlane[i][0] = 0.0f;
		m_fPlane[i][1] = 0.0f;
		m_fPlane[i][2] = 0.0f;
		m_fPlane[i][3] = 0.0f;
	}
}

/**
* 复制构造函数
*/
CFrustum::CFrustum(const CFrustum& frustum) {
	for (int i = 0; i < 6; i++) {
		m_fPlane[i][0] = frustum.m_fPlane[i][0];
		m_fPlane[i][1] = frustum.m_fPlane[i][1];
		m_fPlane[i][2] = frustum.m_fPlane[i][2];
		m_fPlane[i][3] = frustum.m_fPlane[i][3];
	}
}

/**
* 由6平面方程参数数组构造视景体
*/
CFrustum::CFrustum(const float frustum[6][4]) {
	for (int i = 0; i < 6; i++) {
		m_fPlane[i][0] = frustum[i][0];
		m_fPlane[i][1] = frustum[i][1];
		m_fPlane[i][2] = frustum[i][2];
		m_fPlane[i][3] = frustum[i][3];
	}
}

/**
* 由视图投影矩阵构造
*/
CFrustum::CFrustum(const CMatrix4& clipMat) {
	FromViewProj(clipMat);
}

/**
* 扩展视景体
* @param horizontal 水平扩展距离
* @param vertical 垂直扩展距离
* @param depth 前后扩展距离
* @return 扩展后的视景体引用
*/
CFrustum& CFrustum::Enlarge(float horizontal, float vertical, float depth) {
	CVector3 normal1(m_fPlane[0]);
	CVector3 normal2(m_fPlane[1]);
	CVector3 normal3(m_fPlane[2]);
	CVector3 normal4(m_fPlane[3]);
	CVector3 normal5(m_fPlane[4]);
	CVector3 normal6(m_fPlane[5]);
	m_fPlane[0][3] += normal1.DotProduct(normal1 * horizontal);
	m_fPlane[1][3] += normal2.DotProduct(normal2 * horizontal);
	m_fPlane[2][3] += normal3.DotProduct(normal3 * depth);
	m_fPlane[3][3] += normal4.DotProduct(normal4 * depth);
	m_fPlane[4][3] += normal5.DotProduct(normal5 * vertical);
	m_fPlane[5][3] += normal6.DotProduct(normal6 * vertical);
	return *this;
}

/**
* 由视图投影矩阵计算
*/
CFrustum& CFrustum::FromViewProj(const CMatrix4& viewProj) {
	for (unsigned int i = 0; i < 6; i++) {
		int row = i >> 1;
		float sign = i % 2 ? 1.0f : -1.0f;
		float f1 = viewProj.m_fValue[3] + sign * viewProj.m_fValue[row + 0];
		float f2 = viewProj.m_fValue[7] + sign * viewProj.m_fValue[row + 4];
		float f3 = viewProj.m_fValue[11] + sign * viewProj.m_fValue[row + 8];
		float f4 = viewProj.m_fValue[15] + sign * viewProj.m_fValue[row + 12];
		float t = 1.0f / sqrtf(f1 * f1 + f2 * f2 + f3 * f3);
		m_fPlane[i][0] = f1 * t;
		m_fPlane[i][1] = f2 * t;
		m_fPlane[i][2] = f3 * t;
		m_fPlane[i][3] = f4 * t;
	}
	return *this;
}

/**
* 检测点是否在视景体中
* @param point 测试的点位置
* @return 若点在视景体中，则返回 true
*/
bool CFrustum::IsContain(const CVector3& point) const {
	for (int i = 0; i < 6; i++) {
		float result = m_fPlane[i][0] * point.m_fValue[0];
		result += m_fPlane[i][1] * point.m_fValue[1];
		result += m_fPlane[i][2] * point.m_fValue[2];
		result += m_fPlane[i][3];
		if (result <= 0) {
			return false;
		}
	}
	return true;
}

/**
* 判断球是否在视景体内
* @param center 球体中心位置
* @param radius 球体半径
* @return 若球在视景体中，则返回 true
*/
bool CFrustum::IsSphereInside(const CVector3& center, float radius) const {
	for (int i = 0; i < 6; i++) {
		float result = m_fPlane[i][0] * center.m_fValue[0];
		result += m_fPlane[i][1] * center.m_fValue[1];
		result += m_fPlane[i][2] * center.m_fValue[2];
		result += m_fPlane[i][3] + radius;
		if (result <= 0) {
			return false;
		}
	}
	return true;
}

/**
* 判断包围盒是否在视景体内
* @param aabb 测试的 AABB 包围盒
* @return 若包围盒在视景体中，则返回 true
*/
bool CFrustum::IsAABBInside(const CBoundingBox& aabb) const {
	// 无效包围盒直接返回
	if (!aabb.IsValid()) return false;
	for (int i = 0; i < 6; i++) {
		// 根据平面法线判断测试点，比如法线在第一象限，则使用点 aabb.m_cMax 测试
		float testPoint[3] = {
			m_fPlane[i][0] >= 0? aabb.m_cMax.m_fValue[0]: aabb.m_cMin.m_fValue[0],
			m_fPlane[i][1] >= 0? aabb.m_cMax.m_fValue[1]: aabb.m_cMin.m_fValue[1],
			m_fPlane[i][2] >= 0? aabb.m_cMax.m_fValue[2]: aabb.m_cMin.m_fValue[2]
		};
		float result = m_fPlane[i][0] * testPoint[0];
		result += m_fPlane[i][1] * testPoint[1];
		result += m_fPlane[i][2] * testPoint[2];
		result += m_fPlane[i][3];
		if (result <= 0) {
			return false;
		}
	}
	return true;
}

/**
* 重载运算符 ==
*/
bool CFrustum::operator == (const CFrustum& frustum) const {
	for (int i = 0; i < 6; i++) {
		if (m_fPlane[i][0] != frustum.m_fPlane[i][0] ||
			m_fPlane[i][1] != frustum.m_fPlane[i][1] ||
			m_fPlane[i][2] != frustum.m_fPlane[i][2] ||
			m_fPlane[i][3] != frustum.m_fPlane[i][3]) {
			return false;
		}
	}
	return true;
}

/**
* 重载运算符 !=
*/
bool CFrustum::operator != (const CFrustum& frustum) const {
	return !(*this == frustum);
}