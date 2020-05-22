//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CFRUSTUM_H_
#define _CFRUSTUM_H_
#include "AvatarConfig.h"
#include "CVector3.h"
#include "CMatrix4.h"
#include "CBoundingBox.h"

/**
* @brief 相机可视空间视景体类
*/
class AVATAR_EXPORT CFrustum {
public:
	//! 默认构造函数
	CFrustum();
	//! 复制构造函数
	CFrustum(const CFrustum& frustum);
	//! 由6平面方程参数数组构造视景体
	CFrustum(const float frustum[6][4]);
	//! 由视图投影矩阵构造
	CFrustum(const CMatrix4& clipMat);

	//! 扩展视景体
	CFrustum& Enlarge(float horizontal, float vertical, float depth);
	//! 从视图投影矩阵
	CFrustum& FromViewProj(const CMatrix4& viewProj);

	//! 判断点是否在视景体内
	bool IsContain(const CVector3& point) const;
	//! 判断球是否在视景体内
	bool IsSphereInside(const CVector3& center, float radius) const;
	//! 判断包围盒是否在视景体内
	bool IsAABBInside(const CBoundingBox& aabb) const;

public:
	//! 重载运算符 ==
	bool operator == (const CFrustum& frustum) const;
	//! 重载运算符 !=
	bool operator != (const CFrustum& frustum) const;

public:
	/**
	* 视景体 6 个平面一般方程参数，
	* 依次为 右，左，上，下，远，近
	*/
	float m_fPlane[6][4];
};

#endif