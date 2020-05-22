//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CGeometryCreator.h"
#include "CQuaternion.h"
#include "CPlane.h"
#include <cmath>

/**
* 创建立方体
* @param dx X 轴方向的长度
* @param dy Y 轴方向的长度
* @param dz Z 轴方向的长度
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateBox(float dx, float dy, float dz) {
	return CreateBox(dx, dy, dz, false);
}

/**
* 创建立方体并指定法向
* @param dx X 轴方向的长度
* @param dy Y 轴方向的长度
* @param dz Z 轴方向的长度
* @param reversed 面法向反向
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateBox(float dx, float dy, float dz, bool reversed) {
	return CreateBox(dx, dy, dz, reversed, CColor::White);
}

/**
* 创建立方体并指定法向和颜色
* @param dx X 轴方向的长度
* @param dy Y 轴方向的长度
* @param dz Z 轴方向的长度
* @param reversed 面法向反向
* @param color 颜色
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateBox(float dx, float dy, float dz, bool reversed, const CColor& color) {
	CMesh* pMesh = new CMesh();
	pMesh->SetVertexUsage(24);
	dx *= 0.5f;
	dy *= 0.5f;
	dz *= 0.5f;
	// 前
	pMesh->AddVertex(CVertex(-dx, -dy, -dz, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex( dx, -dy, -dz, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex(-dx, -dy,  dz, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex( dx, -dy,  dz, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, color.m_fValue));
	// 右
	pMesh->AddVertex(CVertex( dx, -dy, -dz, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex( dx,  dy, -dz, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex( dx, -dy,  dz, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex( dx,  dy,  dz, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, color.m_fValue));
	// 后
	pMesh->AddVertex(CVertex( dx,  dy, -dz, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex(-dx,  dy, -dz, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex( dx,  dy,  dz, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex(-dx,  dy,  dz, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, color.m_fValue));
	// 左
	pMesh->AddVertex(CVertex(-dx,  dy, -dz, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex(-dx, -dy, -dz, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex(-dx,  dy,  dz, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex(-dx, -dy,  dz, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, color.m_fValue));
	// 上
	pMesh->AddVertex(CVertex(-dx, -dy,  dz, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, color.m_fValue));
	pMesh->AddVertex(CVertex( dx, -dy,  dz, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, color.m_fValue));
	pMesh->AddVertex(CVertex(-dx,  dy,  dz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, color.m_fValue));
	pMesh->AddVertex(CVertex( dx,  dy,  dz, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, color.m_fValue));
	// 下
	pMesh->AddVertex(CVertex(-dx,  dy, -dz, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, color.m_fValue));
	pMesh->AddVertex(CVertex( dx,  dy, -dz, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, color.m_fValue));
	pMesh->AddVertex(CVertex(-dx, -dy, -dz, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, color.m_fValue));
	pMesh->AddVertex(CVertex( dx, -dy, -dz, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, color.m_fValue));
	// 面三角形
	for (int i = 0; i < 24; i += 4) {
		pMesh->AddTriangle(i + 0, i + 1, i + 2);
		pMesh->AddTriangle(i + 1, i + 3, i + 2);
	}
	pMesh->Reverse(reversed, reversed, false);
	pMesh->Create(false);
	return pMesh;
}

/**
* 创建球体
* @param radius 球体半径
* @param slices 横切细分数
* @param stacks 纵切细分数
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateSphere(float radius, int slices, int stacks) {
	return CreateSphere(radius, slices, stacks, false);
}

/**
* 创建球体并指定法向
* @param radius 球体半径
* @param slices 横切细分数
* @param stacks 纵切细分数
* @param reversed 面法向反向
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateSphere(float radius, int slices, int stacks, bool reversed) {
	return CreateSphere(radius, slices, stacks, reversed, CColor::White);
}

/**
* 创建球体并指定法向和颜色
* @param radius 球体半径
* @param slices 横切细分数
* @param stacks 纵切细分数
* @param reversed 面法向反向
* @param color 颜色
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateSphere(float radius, int slices, int stacks, bool reversed, const CColor& color) {
	CMesh* pMesh = new CMesh();
	pMesh->SetVertexUsage((slices + 1) * (stacks + 2));
	const float PI = 3.14159265f;
	const float stepZ = PI / (stacks + 1);
	const float stepXY = 2 * PI / slices;

	for (int i = 0; i < stacks + 2; i++) {
		float nxy = sinf(i * stepZ);
		float nz = -cosf(i * stepZ);
		float z = nz * radius;
		float t = 1.0f - 1.0f * i / (stacks + 1);
		for (int j = 0; j <= slices; j++) {
			float nx = cosf(j * stepXY) * nxy;
			float ny = sinf(j * stepXY) * nxy;
			float s = 1.0f * j / slices;
			pMesh->AddVertex(CVertex(nx * radius, ny * radius, z, s, t, nx, ny, nz, color.m_fValue));
		}
	}
	// 计算三角形索引
	for (int i = 0; i <= stacks; i++) {
		for (int j = 0; j < slices; j++) {
			int index1 = (slices + 1) * i + j;
			int index2 = index1 + 1;
			int index3 = (slices + 1) * (i + 1) + j;
			int index4 = index3 + 1;
			pMesh->AddTriangle(index1, index2, index3);
			pMesh->AddTriangle(index2, index4, index3);
		}
	}
	pMesh->Reverse(reversed, reversed, false);
	pMesh->Create(false);
	return pMesh;
}

/**
* 创建胶囊体
* @param radius 胶囊体底面半径
* @param height 胶囊体高度
* @param slices 细分参数
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateCapsule(float radius, float height, int slices) {
	return CreateCapsule(radius, height, slices, false);
}

/**
* 创建胶囊体并指定法向
* @param radius 胶囊体底面半径
* @param height 胶囊体高度
* @param slices 细分参数
* @param reversed 面法向反向
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateCapsule(float radius, float height, int slices, bool reversed) {
	return CreateCapsule(radius, height, slices, reversed, CColor::White);
}

/**
* 创建胶囊体并指定法向和颜色
* @param radius 胶囊体底面半径
* @param height 胶囊体高度
* @param slices 细分参数
* @param reversed 面法向反向
* @param color 颜色
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateCapsule(float radius, float height, int slices, bool reversed, const CColor& color) {
	CMesh* pMesh = new CMesh();
	pMesh->SetVertexUsage(2 * (slices + 1) * slices);
	const float PI = 3.14159265f;
	const float stepZ = PI / (2 * slices);
	const float stepXY = 2 * PI / slices;

	for (int i = 0; i <= slices; i++) {
		float nxy = sinf(stepZ * i);
		float nz = cosf(stepZ * i);
		float z = radius * nz + 0.5f * height;
		float t = radius * (1.0f - nz) / (height + 2.0f * radius);
		for (int j = 0; j <= slices; j++) {
			float nx = cosf(stepXY * j) * nxy;
			float ny = sinf(stepXY * j) * nxy;
			float s = 1.0f * j / slices;
			pMesh->AddVertex(CVertex(nx * radius, ny * radius, z, s, t, nx, ny, nz, color.m_fValue));
			pMesh->AddVertex(CVertex(nx * radius, ny * radius, -z, s, 1.0f - t, nx, ny, -nz, color.m_fValue));
		}
	}
	// 计算三角形索引
	for (int i = 0; i < slices; i++) {
		for (int j = 0; j < slices; j++) {
			int index1 = i * (slices + 1) * 2 + j * 2;
			int index2 = (slices + 1) * 2 + index1;
			pMesh->AddTriangle(index1, index2, index2 + 2);
			pMesh->AddTriangle(index1, index2 + 2, index1 + 2);
			pMesh->AddTriangle(index2 + 1, index1 + 1, index1 + 3);
			pMesh->AddTriangle(index2 + 1, index1 + 3, index2 + 3);
		}
		// 侧面三角形
		int idx = 2 * slices * (slices + 1) + i * 2;
		pMesh->AddTriangle(idx, idx + 1, idx + 3);
		pMesh->AddTriangle(idx, idx + 3, idx + 2);
	}
	pMesh->Reverse(reversed, reversed, false);
	pMesh->Create(false);
	return pMesh;
}

/**
* 创建圆柱体
* @param radius 圆柱体底面半径
* @param height 圆柱体高度
* @param slices 细分参数
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateCylinder(float radius, float height, int slices) {
	return CreateCylinder(radius, height, slices, false);
}

/**
* 创建圆柱体并指定法向
* @param radius 圆柱体底面半径
* @param height 圆柱体高度
* @param slices 细分参数
* @param reversed 面法向反向
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateCylinder(float radius, float height, int slices, bool reversed) {
	return CreateCylinder(radius, height, slices, reversed, CColor::White);
}

/**
* 创建圆柱体并指定法向和颜色
* @param radius 圆柱体底面半径
* @param height 圆柱体高度
* @param slices 细分参数
* @param reversed 面法向反向
* @param color 颜色
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateCylinder(float radius, float height, int slices, bool reversed, const CColor& color) {
	CMesh* pMesh = new CMesh();
	pMesh->SetVertexUsage((slices + 1) * 4);
	const float PI = 3.14159265f;
	const float step = 2 * PI / slices;
	const float z = height / 2;

	for (int i = 0; i <= slices; i++) {
		float nx = cosf(i * step);
		float ny = sinf(i * step);
		float x = nx * radius;
		float y = ny * radius;
		float s = 1.0f * i / slices;
		// 侧面上顶点和下顶点
		pMesh->AddVertex(CVertex(x, y, z, s, 0, nx, ny, 0, color.m_fValue));
		pMesh->AddVertex(CVertex(x, y, -z, s, 1, nx, ny, 0, color.m_fValue));
		// 顶部和底部面顶点
		if (i != slices) {
			pMesh->AddVertex(CVertex(x, y, z, 0, 0, 0, 0, 1, color.m_fValue));
			pMesh->AddVertex(CVertex(x, y, -z, 0, 0, 0, 0, -1, color.m_fValue));
		}
	}
	// 顶部和底部中心顶点
	int baseIndex = pMesh->GetVertexCount();
	pMesh->AddVertex(CVertex(0, 0, z, 0, 0, 0, 0, 1, color.m_fValue));
	pMesh->AddVertex(CVertex(0, 0, -z, 0, 0, 0, 0, -1, color.m_fValue));

	// 侧面三角形
	for (int i = 0; i < slices; i++) {
		int index = i << 2;
		pMesh->AddTriangle(index + 1, index + 5, index);
		pMesh->AddTriangle(index + 5, index + 4, index);
	}
	// 底部和顶部三角形
	for (int i = 0; i < slices; i++) {
		int index = (i << 2) + 2;
		if (i + 1 == slices) {
			pMesh->AddTriangle(2, baseIndex, index);
			pMesh->AddTriangle(index + 1, baseIndex + 1, 3);
		} else {
			pMesh->AddTriangle(index + 4, baseIndex, index);
			pMesh->AddTriangle(index + 1, baseIndex + 1, index + 5);
		}
	}
	pMesh->Reverse(reversed, reversed, false);
	pMesh->Create(false);
	return pMesh;
}

/**
* 创建圆环
* @param radius 圆环半径
* @param thickness 截面半径
* @param slices 细分参数
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateTorus(float radius, float thickness, int slices) {
	return CreateTorus(radius, thickness, slices, false);
}

/**
* 创建圆环并指定法向
* @param radius 圆环半径
* @param thickness 截面半径
* @param slices 细分参数
* @param reversed 面法向反向
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateTorus(float radius, float thickness, int slices, bool reversed) {
	return CreateTorus(radius, thickness, slices, reversed, CColor::White);
}

/**
* 创建圆环并指定法向和颜色
* @param radius 圆环半径
* @param thickness 截面半径
* @param slices 细分参数
* @param reversed 面法向反向
* @param color 颜色
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateTorus(float radius, float thickness, int slices, bool reversed, const CColor& color) {
	CMesh* pMesh = new CMesh();
	pMesh->SetVertexUsage((slices + 1) * (slices + 1));
	const float PI = 3.14159265f;

	for (int i = 0; i <= slices; i++) {
		float s = 1.0f * i / slices;
		CMatrix4 transform;
		transform.Translate(radius, 0, 0);
		transform.RotateZ(PI * 2.0f * i / slices);
		// 截面顶点通过变换圆周顶点坐标得到
		for (int j = 0; j <= slices; j++) {
			float t = 1.0f - 1.0f * j / slices;
			float theta = PI * 2.0f * j / slices + PI;
			CVector3 n(cosf(theta), 0, sinf(theta), 0);
			CVector3 p(n[0] * thickness, 0, n[2] * thickness, 1);
			// 对截面法相和坐标进行变换
			n = transform * n;
			p = transform * p;
			pMesh->AddVertex(CVertex(p[0], p[1], p[2], s, t, n[0], n[1], n[2], color.m_fValue));
		}
	}
	// 计算三角形索引
	for (int i = 0; i < slices; i++) {
		for (int j = 0; j < slices; j++) {
			unsigned int index = (slices + 1) * i + j;
			pMesh->AddTriangle(index, index + slices + 1, index + 1);
			pMesh->AddTriangle(index + slices + 1, index + slices + 2, index + 1);
		}
	}
	pMesh->Reverse(reversed, reversed, false);
	pMesh->Create(false);
	return pMesh;
}

/**
* 创建椎体
* @param radius 椎体底面半径
* @param height 椎体高度
* @param slices 细分参数
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateCone(float radius, float height, int slices) {
	return CreateCone(radius, height, slices, false);
}

/**
* 创建椎体并指定法向
* @param radius 椎体底面半径
* @param height 椎体高度
* @param slices 细分参数
* @param reversed 面法向反向
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateCone(float radius, float height, int slices, bool reversed) {
	return CreateCone(radius, height, slices, reversed, CColor::White);
}

/**
* 创建椎体并指定法向和颜色
* @param radius 椎体底面半径
* @param height 椎体高度
* @param slices 细分参数
* @param reversed 面法向反向
* @param color 颜色
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateCone(float radius, float height, int slices, bool reversed, const CColor& color) {
	CMesh* pMesh = new CMesh();
	pMesh->SetVertexUsage((slices + 1) * (slices + 2) + 1);
	const float PI = 3.14159265f;
	const float step = 2 * PI / slices;
	const float nz = atan2f(radius, height);
	const float offset = -height / 4.0f;
	// 分为侧面和底面
	for (int i = 0; i <= slices; i++) {
		float nx = cosf(i * step);
		float ny = sinf(i * step);
		float x = nx * radius;
		float y = ny * radius;
		float s = 1.0f * i / slices;
		for (int j = 0; j <= slices; j++) {
			float t = 1.0f - 1.0f * j / slices;
			float z = height * (1.0f - t);
			pMesh->AddVertex(CVertex(x * t, y * t, z + offset, s, t, nx, ny, nz, color.m_fValue));
		}
		pMesh->AddVertex(CVertex(x, y, offset, 0, 0, 0, 0, -1, color.m_fValue));
	}
	// 底面中心点
	pMesh->AddVertex(CVertex(0, 0, offset, 0, 0, 0, 0, -1, color.m_fValue));
	// 添加三角形
	unsigned int bottomCenter = (slices + 1) * (slices + 2);
	for (int i = 0; i < slices; i++) {
		int baseIndex1 = i * (slices + 2);
		int baseIndex2 = (i + 1) * (slices + 2);
		for (int j = 0; j < slices; j++) {
			int index1 = baseIndex1 + j;
			int index2 = baseIndex2 + j;
			pMesh->AddTriangle(index1, index2, index1 + 1);
			pMesh->AddTriangle(index1 + 1, index2, index2 + 1);
		}
		pMesh->AddTriangle(bottomCenter, baseIndex2 + slices + 1, baseIndex1 + slices + 1);
	}
	pMesh->Reverse(reversed, reversed, false);
	pMesh->Create(false);
	return pMesh;
}

/**
* 创建平面
* @param width 平面宽度
* @param height 平面高度
* @param axis 平面法向沿坐标轴的方向（1,2,3,-1,-2,-3 分别表示 XYZ 轴正方向和负方向）
* @return 网格对象
*/
CMesh* CGeometryCreator::CreatePlane(float width, float height, int axis) {
	return CreatePlane(width, height, axis, false);
}

/**
* 创建平面并指定法向
* @param width 平面宽度
* @param height 平面高度
* @param axis 平面法向沿坐标轴的方向（1,2,3,-1,-2,-3 分别表示 XYZ 轴正方向和负方向）
* @param reversed 面法向反向
* @return 网格对象
*/
CMesh* CGeometryCreator::CreatePlane(float width, float height, int axis, bool reversed) {
	return CreatePlane(width, height, axis, reversed, CColor::White);
}

/**
* 创建平面并指定法向和颜色
* @param width 平面宽度
* @param height 平面高度
* @param axis 平面法向沿坐标轴的方向（1,2,3,-1,-2,-3 分别表示 XYZ 轴正方向和负方向）
* @param reversed 面法向反向
* @param color 颜色
* @return 网格对象
*/
CMesh* CGeometryCreator::CreatePlane(float width, float height, int axis, bool reversed, const CColor& color) {
	CMesh* pMesh = new CMesh();
	pMesh->SetVertexUsage(4);
	width *= 0.5f;
	height *= 0.5f;
	switch (axis) {
	case -1: case 1:
		pMesh->AddVertex(CVertex(0, -width, -height, 0, 1, 1, 0, 0, color.m_fValue));
		pMesh->AddVertex(CVertex(0,  width, -height, 1, 1, 1, 0, 0, color.m_fValue));
		pMesh->AddVertex(CVertex(0, -width,  height, 0, 0, 1, 0, 0, color.m_fValue));
		pMesh->AddVertex(CVertex(0,  width,  height, 1, 0, 1, 0, 0, color.m_fValue));
		break;
	case -2: case 2:
		pMesh->AddVertex(CVertex( width, 0, -height, 0, 1, 0, 1, 0, color.m_fValue));
		pMesh->AddVertex(CVertex(-width, 0, -height, 1, 1, 0, 1, 0, color.m_fValue));
		pMesh->AddVertex(CVertex( width, 0,  height, 0, 0, 0, 1, 0, color.m_fValue));
		pMesh->AddVertex(CVertex(-width, 0,  height, 1, 0, 0, 1, 0, color.m_fValue));
		break;
	case -3: case 3:
		pMesh->AddVertex(CVertex(-width, -height, 0, 0, 1, 0, 0, 1, color.m_fValue));
		pMesh->AddVertex(CVertex( width, -height, 0, 1, 1, 0, 0, 1, color.m_fValue));
		pMesh->AddVertex(CVertex(-width,  height, 0, 0, 0, 0, 0, 1, color.m_fValue));
		pMesh->AddVertex(CVertex( width,  height, 0, 1, 0, 0, 0, 1, color.m_fValue));
		break;
	}
	pMesh->AddTriangle(0, 1, 2);
	pMesh->AddTriangle(1, 3, 2);
	if (axis < 0) reversed = !reversed;
	pMesh->Reverse(reversed, reversed, false);
	pMesh->Create(false);
	return pMesh;
}

/**
* 创建圆
* @param radius 圆半径
* @param axis 平面法向沿坐标轴的方向（1,2,3,-1,-2,-3 分别表示 XYZ 轴正方向和负方向）
* @param slices 圆弧细分数
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateCircle(float radius, int axis, int slices) {
	return CreateCircle(radius, axis, slices, false);
}

/**
* 创建圆并指定法向
* @param radius 圆半径
* @param axis 平面法向沿坐标轴的方向（1,2,3,-1,-2,-3 分别表示 XYZ 轴正方向和负方向）
* @param slices 圆弧细分数
* @param reversed 面法向反向
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateCircle(float radius, int axis, int slices, bool reversed) {
	return CreateCircle(radius, axis, slices, reversed, CColor::White);
}

/**
* 创建圆并指定法向和颜色
* @param radius 圆半径
* @param axis 平面法向沿坐标轴的方向（1,2,3,-1,-2,-3 分别表示 XYZ 轴正方向和负方向）
* @param slices 圆弧细分数
* @param reversed 面法向反向
* @param color 颜色
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateCircle(float radius, int axis, int slices, bool reversed, const CColor& color) {
	CMesh* pMesh = new CMesh();
	pMesh->SetVertexUsage(slices + 2);
	const float PI = 3.14159265f;
	const float step = 2 * PI / slices;
	// 添加圆心
	switch (axis) {
	case -1: case 1: pMesh->AddVertex(CVertex(0, 0, 0, 0.5f, 0.5f, 1, 0, 0, color.m_fValue)); break;
	case -2: case 2: pMesh->AddVertex(CVertex(0, 0, 0, 0.5f, 0.5f, 0, 1, 0, color.m_fValue)); break;
	case -3: case 3: pMesh->AddVertex(CVertex(0, 0, 0, 0.5f, 0.5f, 0, 0, 1, color.m_fValue)); break;
	}
	// 圆弧切分
	for (int i = 0; i <= slices; i++) {
		float x = cosf(i * step);
		float y = sinf(i * step);
		float s = 0.5f + x * 0.5f;
		float t = 0.5f - y * 0.5f;
		switch (axis) {
		case -1: case 1: pMesh->AddVertex(CVertex(0, x * radius, y * radius, s, t, 1, 0, 0, color.m_fValue)); break;
		case -2: case 2: pMesh->AddVertex(CVertex(x * radius, 0, y * radius, s, t, 0, 1, 0, color.m_fValue)); break;
		case -3: case 3: pMesh->AddVertex(CVertex(x * radius, y * radius, 0, s, t, 0, 0, 1, color.m_fValue)); break;
		}
	}
	// 添加三角形
	for (int i = 1; i <= slices; i++) {
		pMesh->AddTriangle(i, i + 1, 0);
	}
	if (axis < 0) reversed = !reversed;
	pMesh->Reverse(reversed, reversed, false);
	pMesh->Create(false);
	return pMesh;
}

/**
* 创建箭头，箭头由柱体和椎体组合而成
* @param r1 柱体半径
* @param r2 椎体半径
* @param h1 柱体高
* @param h2 椎体高
* @param slices 细分参数
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateArrow(float r1, float r2, float h1, float h2, int slices) {
	return CreateArrow(r1, r2, h1, h2, slices, false);
}

/**
* 创建箭头并指定法向
* @param r1 柱体半径
* @param r2 椎体半径
* @param h1 柱体高
* @param h2 椎体高
* @param slices 细分参数
* @param reversed 面法向反向
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateArrow(float r1, float r2, float h1, float h2, int slices, bool reversed) {
	return CreateArrow(r1, r2, h1, h2, slices, reversed, CColor::White);
}

/**
* 创建箭头并指定法向和颜色
* @param r1 柱体半径
* @param r2 椎体半径
* @param h1 柱体高
* @param h2 椎体高
* @param slices 细分参数
* @param reversed 面法向反向
* @param color 颜色
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateArrow(float r1, float r2, float h1, float h2, int slices, bool reversed, const CColor& color) {
	CMesh* pMesh = new CMesh();
	pMesh->SetVertexUsage((slices + 1) * 6 + 2);
	const float PI = 3.14159265f;
	const float step = 2 * PI / slices;
	const float nz = atan2f(r2, h2);
	const float t = h2 / (h1 + h2);

	for (int i = 0; i <= slices; i++) {
		float nx = cosf(i * step);
		float ny = sinf(i * step);
		float x1 = nx * r1;
		float y1 = ny * r1;
		float x2 = nx * r2;
		float y2 = ny * r2;
		float s = 1.0f * i / slices;
		// 柱体顶点
		pMesh->AddVertex(CVertex(x1, y1, 0, s, 1, nx, ny, 0, color.m_fValue));
		pMesh->AddVertex(CVertex(x1, y1, h1, s, t, nx, ny, 0, color.m_fValue));
		pMesh->AddVertex(CVertex(x1, y1, 0, 0, 0, 0, 0, -1, color.m_fValue));
		// 椎体顶点
		pMesh->AddVertex(CVertex(x2, y2, h1, s, t, nx, ny, nz, color.m_fValue));
		pMesh->AddVertex(CVertex(0, 0, h1 + h2, s, 0, nx, ny, nz, color.m_fValue));
		pMesh->AddVertex(CVertex(x2, y2, h1, 0, 0, 0, 0, -1, color.m_fValue));
	}
	// 柱体底部中心顶点
	pMesh->AddVertex(CVertex(0, 0, 0, 0, 0, 0, 0, -1, color.m_fValue));
	// 椎体底面中心顶点
	pMesh->AddVertex(CVertex(0, 0, h1, 0, 0, 0, 0, -1, color.m_fValue));

	// 添加三角形
	unsigned int base1 = (slices + 1) * 6;
	unsigned int base2 = base1 + 1;
	for (int i = 0; i < slices; i++) {
		unsigned int index = i * 6;
		pMesh->AddTriangle(index, index + 6, index + 1);
		pMesh->AddTriangle(index + 6, index + 7, index + 1);
		pMesh->AddTriangle(index + 3, index + 9, index + 4);
		pMesh->AddTriangle(index + 9, index + 10, index + 4);
		// 底面
		pMesh->AddTriangle(index + 2, base1, index + 8);
		pMesh->AddTriangle(index + 5, base2, index + 11);
	}
	pMesh->Reverse(reversed, reversed, false);
	pMesh->Create(false);
	return pMesh;
}

/**
* 创建拉伸体
* @param polygon 横截面顶点，必须闭合
* @param count 横截面顶点个数
* @param line 拉伸线段
* @param length 拉伸线段定点数
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateExtrude(const CVector2* polygon, int count, const CVector3* line, int length) {
	return CreateExtrude(polygon, count, line, length, false);
}

/**
* 创建拉伸体，并决定是否平滑表面
* @param polygon 横截面顶点，必须闭合
* @param count 横截面顶点个数
* @param line 拉伸线段
* @param length 拉伸线段定点数
* @param smooth 是否平滑表面
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateExtrude(const CVector2* polygon, int count, const CVector3* line, int length, bool smooth) {
	return CreateExtrude(polygon, count, line, length, smooth, CColor::White);
}

/**
* 创建拉伸体并指定颜色
* @param polygon 横截面顶点，必须闭合
* @param count 横截面顶点个数
* @param line 拉伸线段
* @param length 拉伸线段顶点数
* @param smooth 是否平滑表面
* @param color 颜色
* @return 网格对象
*/
CMesh* CGeometryCreator::CreateExtrude(const CVector2* polygon, int count, const CVector3* line, int length, bool smooth, const CColor& color) {
	// 去除线段首部相同的点
	while (length > 1 && line[1] == line[0]) { ++line; --length; }
	if (count < 2 || length < 2) return 0;

	CMesh* pMesh = new CMesh();
	pMesh->SetVertexUsage(smooth ? count * length * 2 : (count - 2) * length * 4);
	// 计算截面旋转
	CVector3 dir = line[1] - line[0];
	CVector3 axis = CVector3::Z.CrossProduct(dir);
	if (axis.Length() == 0.0f) axis.SetValue(dir.Tangent());
	float cosa = CVector3::Z.DotProduct(dir.Normalize());
	if (cosa < -1.0f) cosa = -1.0f;
	else if (cosa > 1.0f) cosa = 1.0f;
	CQuaternion rotate;
	rotate.FromAxisAngle(axis, acosf(cosa));
	// 计算截面周长和纹理坐标U
	float* perimeter = new float[count];
	float* texCoordU = new float[count];
	perimeter[0] = 0.0f;
	texCoordU[0] = 0.0f;
	for (int i = 1; i < count; i++) perimeter[i] = perimeter[i - 1] + (polygon[i] - polygon[i - 1]).Length();
	for (int i = 1; i < count; i++) texCoordU[i] = perimeter[i] / perimeter[count - 1];
	// 开始循环计算拐点截面顶点
	float lineLengthSum = 0.0f;
	for (int i = 0; i < length; i++) {
		// 计算切面，切面法向为前一段线段向量和下一段线段向量之和
		CVector3 next = line[i + 1 == length? i: i + 1] - line[i];
		float distance = next.Length();
		if (distance == 0.0f) next.SetValue(dir);
		CPlane clipPlane(line[i], dir + next.Normalize());
		CPlane projPlane1(line[i], dir);
		CPlane projPlane2(line[i], distance == 0.0f? dir: next);
		float cosin = dir.DotProduct(next);
		if (cosin < -1.0f) cosin = -1.0f;
		else if (cosin > 1.0f) cosin = 1.0f;
		const float rotateAngle = acosf(cosin);
		for (int j = 0; j < count; j++) {
			// 切面交点通过计算变换后的截面顶点沿线段方向和切面求交得到
			CVector3 prj;
			CVector3 org = line[i] + rotate * CVector3(polygon[j].m_fValue[0], polygon[j].m_fValue[1], 0.0f);
			clipPlane.IntersectLine(org, dir, prj);
			// 以线段长度为基准，以横截面周长为比例，正确计算拉伸方向的贴图坐标
			float texCoordV1 = (lineLengthSum + projPlane1.Distance(prj)) / perimeter[count - 1];
			float texCoordV2 = (lineLengthSum + projPlane2.Distance(prj)) / perimeter[count - 1];
			pMesh->AddVertex(CVertex(prj[0], prj[1], prj[2], texCoordU[j], texCoordV1, 0, 0, 0, color.m_fValue));
			pMesh->AddVertex(CVertex(prj[0], prj[1], prj[2], texCoordU[j], texCoordV2, 0, 0, 0, color.m_fValue));
			if (!smooth && j > 0 && j < count - 1) {
				pMesh->AddVertex(CVertex(prj[0], prj[1], prj[2], texCoordU[j], texCoordV1, 0, 0, 0, color.m_fValue));
				pMesh->AddVertex(CVertex(prj[0], prj[1], prj[2], texCoordU[j], texCoordV2, 0, 0, 0, color.m_fValue));
			}
		}
		lineLengthSum += distance;
		// 计算到下一个拐点的旋转
		if (fabs(rotateAngle) > 1E-9) {
			CQuaternion rot;
			rotate = rot.FromAxisAngle(dir.CrossProduct(next), rotateAngle) * rotate;
			rotate.Normalize();
		}
		dir.SetValue(next);
	}
	// 添加三角形
	for (int i = 0; i < length - 1; i++) {
		for (int j = 0; j < count - 1; j++) {
			int index1 = 2 * (count * i + j) + 1;
			int index2 = 2 * (count * (i + 1) + j);
			if (!smooth) {
				index1 = 4 * ((count - 1) * i + j) + 1;
				index2 = 4 * ((count - 1) * (i + 1) + j);
			}
			pMesh->AddTriangle(index1, index1 + 2, index2);
			pMesh->AddTriangle(index1 + 2, index2 + 2, index2);
		}
	}
	delete[] perimeter;
	delete[] texCoordU;
	pMesh->SetupNormal();
	// 平滑焊缝
	if (smooth) {
		for (int i = 0; i < length; i++) {
			int m = 2 * count * i;
			int n = 2 * count * (i + 1);
			CVertex* v11 = pMesh->GetVertex(m);
			CVertex* v12 = pMesh->GetVertex(n - 2);
			CVertex* v21 = pMesh->GetVertex(m + 1);
			CVertex* v22 = pMesh->GetVertex(n - 1);
			CVector3 n1 = CVector3(v11->m_fNormal) + CVector3(v12->m_fNormal);
			CVector3 n2 = CVector3(v21->m_fNormal) + CVector3(v22->m_fNormal);
			v11->SetNormal(n1.Normalize());
			v12->SetNormal(n1);
			v21->SetNormal(n2.Normalize());
			v22->SetNormal(n2);
		}
	}
	pMesh->Create(false);
	return pMesh;
}