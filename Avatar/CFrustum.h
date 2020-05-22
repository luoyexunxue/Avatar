//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CFRUSTUM_H_
#define _CFRUSTUM_H_
#include "AvatarConfig.h"
#include "CVector3.h"
#include "CMatrix4.h"
#include "CBoundingBox.h"

/**
* @brief ������ӿռ��Ӿ�����
*/
class AVATAR_EXPORT CFrustum {
public:
	//! Ĭ�Ϲ��캯��
	CFrustum();
	//! ���ƹ��캯��
	CFrustum(const CFrustum& frustum);
	//! ��6ƽ�淽�̲������鹹���Ӿ���
	CFrustum(const float frustum[6][4]);
	//! ����ͼͶӰ������
	CFrustum(const CMatrix4& clipMat);

	//! ��չ�Ӿ���
	CFrustum& Enlarge(float horizontal, float vertical, float depth);
	//! ����ͼͶӰ����
	CFrustum& FromViewProj(const CMatrix4& viewProj);

	//! �жϵ��Ƿ����Ӿ�����
	bool IsContain(const CVector3& point) const;
	//! �ж����Ƿ����Ӿ�����
	bool IsSphereInside(const CVector3& center, float radius) const;
	//! �жϰ�Χ���Ƿ����Ӿ�����
	bool IsAABBInside(const CBoundingBox& aabb) const;

public:
	//! ��������� ==
	bool operator == (const CFrustum& frustum) const;
	//! ��������� !=
	bool operator != (const CFrustum& frustum) const;

public:
	/**
	* �Ӿ��� 6 ��ƽ��һ�㷽�̲�����
	* ����Ϊ �ң����ϣ��£�Զ����
	*/
	float m_fPlane[6][4];
};

#endif