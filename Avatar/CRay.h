//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CRAY_H_
#define _CRAY_H_
#include "AvatarConfig.h"
#include "CVector3.h"
#include "CMatrix4.h"
#include "CPlane.h"
#include "CBoundingBox.h"

/**
* @brief �ռ�������
*/
class AVATAR_EXPORT CRay {
public:
	//! Ĭ�Ϲ��캯��
	CRay();
	//! ���ƹ��캯��
	CRay(const CRay& ray);
	//! ʹ�����ͷ���������������
	CRay(const CVector3& origin, const CVector3& direction);

	//! ��������ֵ
	void SetValue(const CRay& ray);
	//! ��������ֵ
	void SetValue(const float origin[3], const float direction[3]);

	//! ʹ�þ���任����
	CRay& Transform(const CMatrix4& matrix);
	//! �����߷���
	CRay& Reverse();

	//! ��ȡ������ƽ��Ľ���
	bool IntersectPlane(const CPlane& plane, CVector3& point) const;
	//! ��ȡ����������Ľ���
	bool IntersectSphere(const CVector3& center, float radius, CVector3& point) const;
	//! ��ȡ�������Χ�еĽ���
	bool IntersectAABB(const CBoundingBox& aabb, CVector3& point) const;

public:
	//! ��������� ==
	bool operator == (const CRay& ray) const;
	//! ��������� !=
	bool operator != (const CRay& ray) const;

public:
	//! �������
	CVector3 m_cOrigin;
	//! ���߷���(��λ����)
	CVector3 m_cDirection;
};

#endif