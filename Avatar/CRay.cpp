//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CRay.h"
#include <cmath>

/**
* ���캯��
*/
CRay::CRay() {
	m_cOrigin.SetValue(0, 0, 0);
	m_cDirection.SetValue(0, 1, 0, 0);
}

/**
* ���ƹ��캯��
*/
CRay::CRay(const CRay& ray) {
	m_cOrigin = ray.m_cOrigin;
	m_cDirection = ray.m_cDirection;
}

/**
* ���캯����ָ�����ͷ���
*/
CRay::CRay(const CVector3& origin, const CVector3& direction) {
	m_cOrigin = origin;
	m_cDirection = direction;
	m_cDirection[3] = 0;
	m_cDirection.Normalize();
}

/**
* ��������ֵ
*/
void CRay::SetValue(const CRay& ray) {
	m_cOrigin = ray.m_cOrigin;
	m_cDirection = ray.m_cDirection;
}

/**
* ��������ֵ
*/
void CRay::SetValue(const float origin[3], const float direction[3]) {
	m_cOrigin.SetValue(origin);
	m_cDirection.SetValue(direction);
	m_cDirection.Normalize();
}

/**
* ʹ�þ���任����
*/
CRay& CRay::Transform(const CMatrix4& matrix) {
	m_cOrigin = matrix * m_cOrigin;
	m_cDirection = matrix * m_cDirection;
	return *this;
}

/**
* �����߷���
*/
CRay& CRay::Reverse() {
	m_cDirection.Scale(-1.0f);
	return *this;
}

/**
* ��ȡ������ƽ��Ľ���
*/
bool CRay::IntersectPlane(const CPlane& plane, CVector3& point) const {
	CVector3 normal(plane.m_fNormal);
	float t = m_cDirection.DotProduct(normal);
	// ������ƽ��ƽ��
	if (t == 0.0f) return false;
	float s = (plane.m_fDistance + m_cOrigin.DotProduct(normal)) / t;
	// ������ƽ�治�ཻ
	if (s > 0.0f) return false;
	point = m_cOrigin - m_cDirection * s;
	return true;
}

/**
* ��ȡ����������Ľ���
*/
bool CRay::IntersectSphere(const CVector3& center, float radius, CVector3& point) const {
	// ��������������ԭ����м���
	CVector3 originRel = m_cOrigin - center;
	float oo = originRel.DotProduct(originRel);
	float od = originRel.DotProduct(m_cDirection);
	float delta = radius * radius + od * od - oo;
	// �޽���
	if (delta < 0) return false;
	delta = sqrtf(delta);
	float t1 = -(od + delta);
	float t2 = -(od - delta);
	CVector3 pt1 = m_cDirection * t1 + originRel;
	CVector3 pt2 = m_cDirection * t2 + originRel;
	// ��ȡ����������������Ľ���
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
* ��ȡ�������Χ�еĽ���
*/
bool CRay::IntersectAABB(const CBoundingBox& aabb, CVector3& point) const {
	// ��Χ����Ч
	if (!aabb.IsValid()) return false;
	// ��������ڰ�Χ����
	if (aabb.IsContain(m_cOrigin)) return true;

	// ���μ���������ཻ���
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
* ��������� ==
*/
bool CRay::operator == (const CRay& ray) const {
	return (m_cOrigin == ray.m_cOrigin && m_cDirection == ray.m_cDirection);
}

/**
* ��������� !=
*/
bool CRay::operator != (const CRay& ray) const {
	return !(*this == ray);
}