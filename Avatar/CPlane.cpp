//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPlane.h"
#include <cmath>

/**
* 构造函数
*/
CPlane::CPlane() {
	m_fNormal[0] = 0;
	m_fNormal[1] = 0;
	m_fNormal[2] = 1.0f;
	m_fDistance = 0;
}

/**
* 复制构造函数
*/
CPlane::CPlane(const CPlane& plane) {
	m_fNormal[0] = plane.m_fNormal[0];
	m_fNormal[1] = plane.m_fNormal[1];
	m_fNormal[2] = plane.m_fNormal[2];
	m_fDistance = plane.m_fDistance;
}

/**
* 构造函数，plane 为平面一般方程的四个参数
*/
CPlane::CPlane(const float plane[4]) {
	float scale = 1.0f / sqrtf(plane[0] * plane[0] + plane[1] * plane[1] + plane[2] * plane[2]);
	m_fNormal[0] = plane[0] * scale;
	m_fNormal[1] = plane[1] * scale;
	m_fNormal[2] = plane[2] * scale;
	m_fDistance = plane[3] * scale;
}

/**
* 构造函数，x y z d 为平面一般方程的四个参数
*/
CPlane::CPlane(float x, float y, float z, float d) {
	float scale = 1.0f / sqrtf(x * x + y * y + z * z);
	m_fNormal[0] = x * scale;
	m_fNormal[1] = y * scale;
	m_fNormal[2] = z * scale;
	m_fDistance = d * scale;
}

/**
* 构造函数，由平面法向量和离开原点距离指定
*/
CPlane::CPlane(const CVector3& normal, float d) {
	float scale = 1.0f / normal.Length();
	m_fNormal[0] = normal.m_fValue[0] * scale;
	m_fNormal[1] = normal.m_fValue[1] * scale;
	m_fNormal[2] = normal.m_fValue[2] * scale;
	m_fDistance = -1.0f * d;
}

/**
* 构造函数，由平面内一点和平面法向量指定
*/
CPlane::CPlane(const CVector3& point, const CVector3& normal) {
	float scale = 1.0f / normal.Length();
	m_fNormal[0] = normal.m_fValue[0] * scale;
	m_fNormal[1] = normal.m_fValue[1] * scale;
	m_fNormal[2] = normal.m_fValue[2] * scale;
	m_fDistance = -normal.DotProduct(point) * scale;
}

/**
* 设置平面一般方程
* @note normalized=true 表示平面法向量已单位化
*/
void CPlane::SetValue(const float plane[4], bool normalized) {
	if (normalized) {
		m_fNormal[0] = plane[0];
		m_fNormal[1] = plane[1];
		m_fNormal[2] = plane[2];
		m_fDistance = plane[3];
	} else {
		float scale = 1.0f / sqrtf(plane[0] * plane[0] + plane[1] * plane[1] + plane[2] * plane[2]);
		m_fNormal[0] = plane[0] * scale;
		m_fNormal[1] = plane[1] * scale;
		m_fNormal[2] = plane[2] * scale;
		m_fDistance = plane[3] * scale;
	}
}

/**
* 设置平面一般方程
* @note normalized=true 表示平面法向量已单位化
*/
void CPlane::SetValue(float x, float y, float z, float d, bool normalized) {
	if (normalized) {
		m_fNormal[0] = x;
		m_fNormal[1] = y;
		m_fNormal[2] = z;
		m_fDistance = d;
	} else {
		float scale = 1.0f / sqrtf(x * x + y * y + z * z);
		m_fNormal[0] = x * scale;
		m_fNormal[1] = y * scale;
		m_fNormal[2] = z * scale;
		m_fDistance = d * scale;
	}
}

/**
* 得到平面上的一点
*/
CVector3 CPlane::GetPlanePoint() const {
	return CVector3(m_fNormal) * (-m_fDistance);
}

/**
* 获取指定点的镜像点
*/
CVector3 CPlane::GetMirrorPoint(const CVector3& point) const {
	CVector3 normal(m_fNormal);
	float distance = point.DotProduct(normal) + m_fDistance;
	return point - normal.Scale(distance * 2.0f);
}

/**
* 获取平面对应的反射矩阵
*/
CMatrix4 CPlane::GetReflectMatrix() const {
	float mat[16] = {
		-2 * m_fNormal[0] * m_fNormal[0] + 1, -2 * m_fNormal[1] * m_fNormal[0], -2 * m_fNormal[2] * m_fNormal[0], 0.0f,
		-2 * m_fNormal[0] * m_fNormal[1], -2 * m_fNormal[1] * m_fNormal[1] + 1, -2 * m_fNormal[2] * m_fNormal[1], 0.0f,
		-2 * m_fNormal[0] * m_fNormal[2], -2 * m_fNormal[1] * m_fNormal[2], -2 * m_fNormal[2] * m_fNormal[2] + 1, 0.0f,
		-2 * m_fNormal[0] * m_fDistance, -2 * m_fNormal[1] * m_fDistance, -2 * m_fNormal[2] * m_fDistance, 1.0f
	};
	return CMatrix4(mat);
}

/**
* 使用矩阵变换平面
* @param matrix 变换矩阵
* @note 计算原理
*	1.设平面内某点为 v(x,y,z,1) 平面表示为 p(a,b,c,d) 变换矩阵为 M 变换后的平面表示为 p'
*	2.等式 pT * v = 0 恒成立, 且 p'T * M * v = 0 也恒成立
*	3.通过上式可得 p'T * M = pT
*	4.经变换可得 p' = (M-1)T * p
*/
CPlane& CPlane::Transform(const CMatrix4& matrix) {
	CMatrix4 invertMat(matrix);
	invertMat.Invert();
	float x = m_fNormal[0];
	float y = m_fNormal[1];
	float z = m_fNormal[2];
	float d = m_fDistance;
	m_fNormal[0] = x * invertMat[0] + y * invertMat[1] + z * invertMat[2] + d * invertMat[3];
	m_fNormal[1] = x * invertMat[4] + y * invertMat[5] + z * invertMat[6] + d * invertMat[7];
	m_fNormal[2] = x * invertMat[8] + y * invertMat[9] + z * invertMat[10] + d * invertMat[11];
	m_fDistance = x * invertMat[12] + y * invertMat[13] + z * invertMat[14] + d * invertMat[15];
	return *this;
}

/**
* 翻转法向量
*/
CPlane& CPlane::FlipNormal() {
	m_fNormal[0] = -m_fNormal[0];
	m_fNormal[1] = -m_fNormal[1];
	m_fNormal[2] = -m_fNormal[2];
	m_fDistance = -m_fDistance;
	return *this;
}

/**
* 是否面向观察者
* @param lookDir 视线方向
* @return 若平面法相在观察者一侧，则返回true
*/
bool CPlane::IsFrontFacing(const CVector3& lookDir) const {
	return lookDir.DotProduct(CVector3(m_fNormal)) < 0;
}

/**
* 获取平面与平面的交线
* @param plane 测试平面
* @param linePoint 输出交线上的一点
* @param lineDir 输出交线方向向量
* @return 若两个平面平行，则返回false
*/
bool CPlane::IntersectPlane(const CPlane& plane, CVector3& linePoint, CVector3& lineDir) const {
	CVector3 normal1(m_fNormal);
	CVector3 normal2(plane.m_fNormal);
	lineDir = normal1.CrossProduct(normal2);
	float det = lineDir.DotProduct(lineDir);
	if (det < 0.000001f) return false;
	linePoint = lineDir.CrossProduct(normal2 * m_fDistance - normal1 * plane.m_fDistance);
	linePoint /= det;
	return true;
}

/**
* 获取平面与直线的交点
* @param linePoint 测试直线上的点
* @param lineDir 测试直线方向向量
* @param point 输出交点坐标
* @return 若直线与平面平行，则返回false
*/
bool CPlane::IntersectLine(const CVector3& linePoint, const CVector3& lineDir, CVector3& point) const {
	CVector3 normal(m_fNormal);
	float k = lineDir.DotProduct(normal);
	if (fabs(k) < 0.000001f) return false;
	float t = (linePoint.DotProduct(normal) + m_fDistance) / k;
	point = linePoint - lineDir * t;
	return true;
}

/**
* 获取平面与线段的交点
* @param p1 测试线段点1
* @param p2 测试线段点2
* @param point 输出交点坐标
* @return 若线段与平面无交点，则返回false
*/
bool CPlane::IntersectLineSegment(const CVector3& p1, const CVector3& p2, CVector3& point) const {
	CVector3 normal(m_fNormal);
	float d1 = p1.DotProduct(normal) + m_fDistance;
	float d2 = p2.DotProduct(normal) + m_fDistance;
	if (d1 * d2 > 0.0f) return false;
	if (d1 == 0.0f) { point.SetValue(p1); return true; }
	if (d2 == 0.0f) { point.SetValue(p2); return true; }
	float t = d1 / (d1 - d2);
	point = p1 * (1.0f - t) + p2 * t;
	return true;
}

/**
* 获取点到平面的距离
* @param point 测试点
* @return 距离，负值表示点在平面背面
*/
float CPlane::Distance(const CVector3& point) const {
	return point.DotProduct(CVector3(m_fNormal)) + m_fDistance;
}

/**
* 重载运算符 ==
*/
bool CPlane::operator == (const CPlane& plane) const {
	if (m_fNormal[0] == plane.m_fNormal[0] &&
		m_fNormal[1] == plane.m_fNormal[1] &&
		m_fNormal[2] == plane.m_fNormal[2] &&
		m_fDistance == plane.m_fDistance) {
		return true;
	}
	return false;
}

/**
* 重载运算符 !=
*/
bool CPlane::operator != (const CPlane& plane) const {
	return !(*this == plane);
}