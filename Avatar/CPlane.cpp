//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPlane.h"
#include <cmath>

/**
* ���캯��
*/
CPlane::CPlane() {
	m_fNormal[0] = 0;
	m_fNormal[1] = 0;
	m_fNormal[2] = 1.0f;
	m_fDistance = 0;
}

/**
* ���ƹ��캯��
*/
CPlane::CPlane(const CPlane& plane) {
	m_fNormal[0] = plane.m_fNormal[0];
	m_fNormal[1] = plane.m_fNormal[1];
	m_fNormal[2] = plane.m_fNormal[2];
	m_fDistance = plane.m_fDistance;
}

/**
* ���캯����plane Ϊƽ��һ�㷽�̵��ĸ�����
*/
CPlane::CPlane(const float plane[4]) {
	float scale = 1.0f / sqrtf(plane[0] * plane[0] + plane[1] * plane[1] + plane[2] * plane[2]);
	m_fNormal[0] = plane[0] * scale;
	m_fNormal[1] = plane[1] * scale;
	m_fNormal[2] = plane[2] * scale;
	m_fDistance = plane[3] * scale;
}

/**
* ���캯����x y z d Ϊƽ��һ�㷽�̵��ĸ�����
*/
CPlane::CPlane(float x, float y, float z, float d) {
	float scale = 1.0f / sqrtf(x * x + y * y + z * z);
	m_fNormal[0] = x * scale;
	m_fNormal[1] = y * scale;
	m_fNormal[2] = z * scale;
	m_fDistance = d * scale;
}

/**
* ���캯������ƽ�淨�������뿪ԭ�����ָ��
*/
CPlane::CPlane(const CVector3& normal, float d) {
	float scale = 1.0f / normal.Length();
	m_fNormal[0] = normal.m_fValue[0] * scale;
	m_fNormal[1] = normal.m_fValue[1] * scale;
	m_fNormal[2] = normal.m_fValue[2] * scale;
	m_fDistance = -1.0f * d;
}

/**
* ���캯������ƽ����һ���ƽ�淨����ָ��
*/
CPlane::CPlane(const CVector3& point, const CVector3& normal) {
	float scale = 1.0f / normal.Length();
	m_fNormal[0] = normal.m_fValue[0] * scale;
	m_fNormal[1] = normal.m_fValue[1] * scale;
	m_fNormal[2] = normal.m_fValue[2] * scale;
	m_fDistance = -normal.DotProduct(point) * scale;
}

/**
* ����ƽ��һ�㷽��
* @note normalized=true ��ʾƽ�淨�����ѵ�λ��
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
* ����ƽ��һ�㷽��
* @note normalized=true ��ʾƽ�淨�����ѵ�λ��
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
* �õ�ƽ���ϵ�һ��
*/
CVector3 CPlane::GetPlanePoint() const {
	return CVector3(m_fNormal) * (-m_fDistance);
}

/**
* ��ȡָ����ľ����
*/
CVector3 CPlane::GetMirrorPoint(const CVector3& point) const {
	CVector3 normal(m_fNormal);
	float distance = point.DotProduct(normal) + m_fDistance;
	return point - normal.Scale(distance * 2.0f);
}

/**
* ��ȡƽ���Ӧ�ķ������
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
* ʹ�þ���任ƽ��
* @param matrix �任����
* @note ����ԭ��
*	1.��ƽ����ĳ��Ϊ v(x,y,z,1) ƽ���ʾΪ p(a,b,c,d) �任����Ϊ M �任���ƽ���ʾΪ p'
*	2.��ʽ pT * v = 0 �����, �� p'T * M * v = 0 Ҳ�����
*	3.ͨ����ʽ�ɵ� p'T * M = pT
*	4.���任�ɵ� p' = (M-1)T * p
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
* ��ת������
*/
CPlane& CPlane::FlipNormal() {
	m_fNormal[0] = -m_fNormal[0];
	m_fNormal[1] = -m_fNormal[1];
	m_fNormal[2] = -m_fNormal[2];
	m_fDistance = -m_fDistance;
	return *this;
}

/**
* �Ƿ�����۲���
* @param lookDir ���߷���
* @return ��ƽ�淨���ڹ۲���һ�࣬�򷵻�true
*/
bool CPlane::IsFrontFacing(const CVector3& lookDir) const {
	return lookDir.DotProduct(CVector3(m_fNormal)) < 0;
}

/**
* ��ȡƽ����ƽ��Ľ���
* @param plane ����ƽ��
* @param linePoint ��������ϵ�һ��
* @param lineDir ������߷�������
* @return ������ƽ��ƽ�У��򷵻�false
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
* ��ȡƽ����ֱ�ߵĽ���
* @param linePoint ����ֱ���ϵĵ�
* @param lineDir ����ֱ�߷�������
* @param point �����������
* @return ��ֱ����ƽ��ƽ�У��򷵻�false
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
* ��ȡƽ�����߶εĽ���
* @param p1 �����߶ε�1
* @param p2 �����߶ε�2
* @param point �����������
* @return ���߶���ƽ���޽��㣬�򷵻�false
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
* ��ȡ�㵽ƽ��ľ���
* @param point ���Ե�
* @return ���룬��ֵ��ʾ����ƽ�汳��
*/
float CPlane::Distance(const CVector3& point) const {
	return point.DotProduct(CVector3(m_fNormal)) + m_fDistance;
}

/**
* ��������� ==
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
* ��������� !=
*/
bool CPlane::operator != (const CPlane& plane) const {
	return !(*this == plane);
}