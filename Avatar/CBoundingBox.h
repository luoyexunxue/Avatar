//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CBOUNDINGBOX_H_
#define _CBOUNDINGBOX_H_
#include "AvatarConfig.h"
#include "CVector3.h"

/**
* @brief ������Χ����
*/
class AVATAR_EXPORT CBoundingBox {
public:
	//! Ĭ�Ϲ��캯��
	CBoundingBox();
	//! ���ƹ��캯��
	CBoundingBox(const CBoundingBox& aabb);
	//! ʹ�ü�ֵ���깹���Χ��
	CBoundingBox(const CVector3& min, const CVector3& max);
	//! ʹ�ü�ֵ���깹���Χ��
	CBoundingBox(float minx, float miny, float minz, float maxx, float maxy, float maxz);

	//! ���ð�Χ��ֵ
	void SetValue(const CBoundingBox& aabb);
	//! ���ð�Χ��ֵ
	void SetValue(const CVector3& min, const CVector3& max);
	//! ���ð�Χ��ֵ
	void SetValue(float minx, float miny, float minz, float maxx, float maxy, float maxz);
	//! ����Ϊ��Ч��Χ��
	void SetInvalid();
	//! ���°�Χ��
	void Update(const CVector3& point);
	//! ���°�Χ��
	void Update(const CBoundingBox& aabb);

	//! �ж��Ƿ�Ϊ��Ч��Χ��
	bool IsValid() const;
	//! �Ƿ����һ����Χ���ص�
	bool IsOverlap(const CBoundingBox& aabb) const;
	//! �Ƿ����ָ����Χ��
	bool IsContain(const CBoundingBox& aabb) const;
	//! �Ƿ����ָ����
	bool IsContain(const CVector3& point) const;
	//! ��������
	float SurfaceArea() const;
	//! �������Ĵ�С
	float Size(int axis) const;
	//! �������ĵ�λ��
	CVector3 Center() const;
	//! �԰�Χ�н�������
	CBoundingBox& Scale(const CVector3& scale);
	//! �԰�Χ�н���ƫ��
	CBoundingBox& Offset(const CVector3& offset);

public:
	//! ��������� ==
	bool operator == (const CBoundingBox& aabb) const;
	//! ��������� !=
	bool operator != (const CBoundingBox& aabb) const;
	//! ��������� +
	CBoundingBox operator + (const CBoundingBox& aabb) const;
	//! ��������� +=
	CBoundingBox& operator += (const CBoundingBox& aabb);

public:
	//! ��С(x,y,z)����
	CVector3 m_cMin;
	//! ���(x,y,z)����
	CVector3 m_cMax;
};

#endif