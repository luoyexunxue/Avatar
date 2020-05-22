//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
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
* @brief 空间射线类
*/
class AVATAR_EXPORT CRay {
public:
	//! 默认构造函数
	CRay();
	//! 复制构造函数
	CRay(const CRay& ray);
	//! 使用起点和方向向量构造射线
	CRay(const CVector3& origin, const CVector3& direction);

	//! 设置射线值
	void SetValue(const CRay& ray);
	//! 设置射线值
	void SetValue(const float origin[3], const float direction[3]);

	//! 使用矩阵变换射线
	CRay& Transform(const CMatrix4& matrix);
	//! 将射线反向
	CRay& Reverse();

	//! 获取射线与平面的交点
	bool IntersectPlane(const CPlane& plane, CVector3& point) const;
	//! 获取射线与球体的交点
	bool IntersectSphere(const CVector3& center, float radius, CVector3& point) const;
	//! 获取射线与包围盒的交点
	bool IntersectAABB(const CBoundingBox& aabb, CVector3& point) const;

public:
	//! 重载运算符 ==
	bool operator == (const CRay& ray) const;
	//! 运算符重载 !=
	bool operator != (const CRay& ray) const;

public:
	//! 射线起点
	CVector3 m_cOrigin;
	//! 射线方向(单位向量)
	CVector3 m_cDirection;
};

#endif