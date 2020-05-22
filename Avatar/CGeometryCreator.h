//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CGEOMETRYCREATOR_H_
#define _CGEOMETRYCREATOR_H_
#include "AvatarConfig.h"
#include "CMesh.h"
#include "CColor.h"
#include "CVector2.h"
#include "CVector3.h"
#include <string>
using std::string;

/**
* @brief 几何体定义
*/
struct SGeometry {
	//! 几何类型
	enum {
		UNKNOWN = 0,
		BOX = 1,
		SPHERE = 2,
		CAPSULE = 3,
		CYLINDER = 4,
		TORUS = 5,
		CONE = 6,
		PLANE = 7,
		CIRCLE = 8
	} shape;
	//! 大小参数
	union {
		struct { float x; float y; float z; } box;
		struct { float r; } sphere;
		struct { float r; float h; } capsule;
		struct { float r; float h; } cylinder;
		struct { float r; float c; } torus;
		struct { float r; float h; } cone;
		struct { float w; float h; } plane;
		struct { float r; } circle;
	};
	//! 细分控制
	int slices;
	//! 法向控制
	bool reversed;
	//! 默认构造函数
	SGeometry(): shape(UNKNOWN), slices(32), reversed(false) {}
	//! 获取字符串表示
	string ToString() const {
		if (shape == BOX) return "box";
		if (shape == SPHERE) return "sphere";
		if (shape == CAPSULE) return "capsule";
		if (shape == CYLINDER) return "cylinder";
		if (shape == TORUS) return "torus";
		if (shape == CONE) return "cone";
		if (shape == PLANE) return "plane";
		if (shape == CIRCLE) return "circle";
		return "unknown";
	}
};

/**
* @brief 几何体网格创建器
*/
class AVATAR_EXPORT CGeometryCreator {
public:
	//! 创建立方体
	static CMesh* CreateBox(float dx, float dy, float dz);
	static CMesh* CreateBox(float dx, float dy, float dz, bool reversed);
	static CMesh* CreateBox(float dx, float dy, float dz, bool reversed, const CColor& color);
	//! 创建球体
	static CMesh* CreateSphere(float radius, int slices, int stacks);
	static CMesh* CreateSphere(float radius, int slices, int stacks, bool reversed);
	static CMesh* CreateSphere(float radius, int slices, int stacks, bool reversed, const CColor& color);
	//! 创建胶囊体
	static CMesh* CreateCapsule(float radius, float height, int slices);
	static CMesh* CreateCapsule(float radius, float height, int slices, bool reversed);
	static CMesh* CreateCapsule(float radius, float height, int slices, bool reversed, const CColor& color);
	//! 创建圆柱体
	static CMesh* CreateCylinder(float radius, float height, int slices);
	static CMesh* CreateCylinder(float radius, float height, int slices, bool reversed);
	static CMesh* CreateCylinder(float radius, float height, int slices, bool reversed, const CColor& color);
	//! 创建圆环
	static CMesh* CreateTorus(float radius, float thickness, int slices);
	static CMesh* CreateTorus(float radius, float thickness, int slices, bool reversed);
	static CMesh* CreateTorus(float radius, float thickness, int slices, bool reversed, const CColor& color);
	//! 创建椎体
	static CMesh* CreateCone(float radius, float height, int slices);
	static CMesh* CreateCone(float radius, float height, int slices, bool reversed);
	static CMesh* CreateCone(float radius, float height, int slices, bool reversed, const CColor& color);
	//! 创建平面
	static CMesh* CreatePlane(float width, float height, int axis);
	static CMesh* CreatePlane(float width, float height, int axis, bool reversed);
	static CMesh* CreatePlane(float width, float height, int axis, bool reversed, const CColor& color);
	//! 创建圆形
	static CMesh* CreateCircle(float radius, int axis, int slices);
	static CMesh* CreateCircle(float radius, int axis, int slices, bool reversed);
	static CMesh* CreateCircle(float radius, int axis, int slices, bool reversed, const CColor& color);
	//! 创建箭头
	static CMesh* CreateArrow(float r1, float r2, float h1, float h2, int slices);
	static CMesh* CreateArrow(float r1, float r2, float h1, float h2, int slices, bool reversed);
	static CMesh* CreateArrow(float r1, float r2, float h1, float h2, int slices, bool reversed, const CColor& color);
	//! 创建拉伸体
	static CMesh* CreateExtrude(const CVector2* polygon, int count, const CVector3* line, int length);
	static CMesh* CreateExtrude(const CVector2* polygon, int count, const CVector3* line, int length, bool smooth);
	static CMesh* CreateExtrude(const CVector2* polygon, int count, const CVector3* line, int length, bool smooth, const CColor& color);
};

#endif