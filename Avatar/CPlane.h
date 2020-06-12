//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPLANE_H_
#define _CPLANE_H_
#include "AvatarConfig.h"
#include "CVector3.h"
#include "CMatrix4.h"

/**
* @brief 空间平面类
*/
class AVATAR_EXPORT CPlane {
public:
	//! 默认构造函数
	CPlane();
	//! 复制构造函数
	CPlane(const CPlane& plane);
	//! 由数组形式的xyzd构造平面
	CPlane(const float plane[4]);
	//! 由xyzd构造平面
	CPlane(float x, float y, float z, float d);
	//! 由法向量和d构造平面
	CPlane(const CVector3& normal, float d);
	//! 由平面上的一点和法向量构造平面
	CPlane(const CVector3& point, const CVector3& normal);

	//! 设置平面一般方程
	void SetValue(const float plane[4], bool normalized);
	//! 设置平面一般方程
	void SetValue(float x, float y, float z, float d, bool normalized);

	//! 得到平面上的一点
	CVector3 GetPlanePoint() const;
	//! 获取镜像点
	CVector3 GetMirrorPoint(const CVector3& point) const;
	//! 获取平面对应的反射矩阵
	CMatrix4 GetReflectMatrix() const;
	//! 使用矩阵变换平面
	CPlane& Transform(const CMatrix4& matrix);
	//! 翻转法向量
	CPlane& FlipNormal();

	//! 是否面向观察者
	bool IsFrontFacing(const CVector3& lookDir) const;
	//! 获取平面与平面的交线
	bool IntersectPlane(const CPlane& plane, CVector3& linePoint, CVector3& lineDir) const;
	//! 获取平面与直线的交点
	bool IntersectLine(const CVector3& linePoint, const CVector3& lineDir, CVector3& point) const;
	//! 获取平面与线段的交点
	bool IntersectLineSegment(const CVector3& p1, const CVector3& p2, CVector3& point) const;
	//! 获取点到平面的距离
	float Distance(const CVector3& point) const;

public:
	//! 重载运算符 ==
	bool operator == (const CPlane& plane) const;
	//! 重载运算符 !=
	bool operator != (const CPlane& plane) const;

public:
	/**
	* 平面方程 nx * X + ny * Y + nz * Z + d = 0;
	* m_fNormal = [nx, ny, nz] 单位向量
	* m_fDistance = d;
	*/
	float m_fNormal[3];
	float m_fDistance;
};

#endif