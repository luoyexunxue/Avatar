//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CBOUNDINGBOX_H_
#define _CBOUNDINGBOX_H_
#include "AvatarConfig.h"
#include "CVector3.h"

/**
* @brief 轴对齐包围盒类
*/
class AVATAR_EXPORT CBoundingBox {
public:
	//! 默认构造函数
	CBoundingBox();
	//! 复制构造函数
	CBoundingBox(const CBoundingBox& aabb);
	//! 使用极值坐标构造包围盒
	CBoundingBox(const CVector3& min, const CVector3& max);
	//! 使用极值坐标构造包围盒
	CBoundingBox(float minx, float miny, float minz, float maxx, float maxy, float maxz);

	//! 设置包围盒值
	void SetValue(const CBoundingBox& aabb);
	//! 设置包围盒值
	void SetValue(const CVector3& min, const CVector3& max);
	//! 设置包围盒值
	void SetValue(float minx, float miny, float minz, float maxx, float maxy, float maxz);
	//! 设置为无效包围盒
	void SetInvalid();
	//! 更新包围盒
	void Update(const CVector3& point);
	//! 更新包围盒
	void Update(const CBoundingBox& aabb);

	//! 判断是否为有效包围盒
	bool IsValid() const;
	//! 是否和另一个包围盒重叠
	bool IsOverlap(const CBoundingBox& aabb) const;
	//! 是否包含指定包围盒
	bool IsContain(const CBoundingBox& aabb) const;
	//! 是否包含指定点
	bool IsContain(const CVector3& point) const;
	//! 计算表面积
	float SurfaceArea() const;
	//! 计算各轴的大小
	float Size(int axis) const;
	//! 计算中心点位置
	CVector3 Center() const;
	//! 对包围盒进行缩放
	CBoundingBox& Scale(const CVector3& scale);
	//! 对包围盒进行偏移
	CBoundingBox& Offset(const CVector3& offset);

public:
	//! 重载运算符 ==
	bool operator == (const CBoundingBox& aabb) const;
	//! 重载运算符 !=
	bool operator != (const CBoundingBox& aabb) const;
	//! 并集运算符 +
	CBoundingBox operator + (const CBoundingBox& aabb) const;
	//! 并集运算符 +=
	CBoundingBox& operator += (const CBoundingBox& aabb);

public:
	//! 最小(x,y,z)坐标
	CVector3 m_cMin;
	//! 最大(x,y,z)坐标
	CVector3 m_cMax;
};

#endif