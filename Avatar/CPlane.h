//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPLANE_H_
#define _CPLANE_H_
#include "AvatarConfig.h"
#include "CVector3.h"
#include "CMatrix4.h"

/**
* @brief �ռ�ƽ����
*/
class AVATAR_EXPORT CPlane {
public:
	//! Ĭ�Ϲ��캯��
	CPlane();
	//! ���ƹ��캯��
	CPlane(const CPlane& plane);
	//! ��������ʽ��xyzd����ƽ��
	CPlane(const float plane[4]);
	//! ��xyzd����ƽ��
	CPlane(float x, float y, float z, float d);
	//! �ɷ�������d����ƽ��
	CPlane(const CVector3& normal, float d);
	//! ��ƽ���ϵ�һ��ͷ���������ƽ��
	CPlane(const CVector3& point, const CVector3& normal);

	//! ����ƽ��һ�㷽��
	void SetValue(const float plane[4], bool normalized);
	//! ����ƽ��һ�㷽��
	void SetValue(float x, float y, float z, float d, bool normalized);

	//! �õ�ƽ���ϵ�һ��
	CVector3 GetPlanePoint() const;
	//! ��ȡ�����
	CVector3 GetMirrorPoint(const CVector3& point) const;
	//! ��ȡƽ���Ӧ�ķ������
	CMatrix4 GetReflectMatrix() const;
	//! ʹ�þ���任ƽ��
	CPlane& Transform(const CMatrix4& matrix);
	//! ��ת������
	CPlane& FlipNormal();

	//! �Ƿ�����۲���
	bool IsFrontFacing(const CVector3& lookDir) const;
	//! ��ȡƽ����ƽ��Ľ���
	bool IntersectPlane(const CPlane& plane, CVector3& linePoint, CVector3& lineDir) const;
	//! ��ȡƽ����ֱ�ߵĽ���
	bool IntersectLine(const CVector3& linePoint, const CVector3& lineDir, CVector3& point) const;
	//! ��ȡƽ�����߶εĽ���
	bool IntersectLineSegment(const CVector3& p1, const CVector3& p2, CVector3& point) const;
	//! ��ȡ�㵽ƽ��ľ���
	float Distance(const CVector3& point) const;

public:
	//! ��������� ==
	bool operator == (const CPlane& plane) const;
	//! ��������� !=
	bool operator != (const CPlane& plane) const;

public:
	/**
	* ƽ�淽�� nx * X + ny * Y + nz * Z + d = 0;
	* m_fNormal = [nx, ny, nz] ��λ����
	* m_fDistance = d;
	*/
	float m_fNormal[3];
	float m_fDistance;
};

#endif