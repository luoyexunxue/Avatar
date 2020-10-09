//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CDELAUNAY_H_
#define _CDELAUNAY_H_
#include "AvatarConfig.h"
#include "CVector3.h"
#include "CMesh.h"
#include <vector>
#include <list>
using std::vector;
using std::list;

/**
* @brief Delaunay 三角网生成器
*/
class AVATAR_EXPORT CDelaunay {
public:
	//! 默认构造函数
	CDelaunay();
	//! 基于顶点列表构造
	CDelaunay(const vector<CVector3>& points);

	//! 添加点
	void AddPoint(const CVector3& point);
	//! 添加坐标点
	void AddPoint(float x, float y, float z);
	//! 清空已添加点
	void ClearPoint();
	//! 生成三角网
	bool Generate();

	//! 获取顶点数量
	size_t GetPointCount() const;
	//! 获取一个顶点
	CVector3 GetPoint(size_t index) const;
	//! 获取三角形数量
	size_t GetTriangleCount() const;
	//! 获取一个三角形
	void GetTriangle(size_t index, unsigned int vertices[3]) const;

	//! 生成网格对象
	CMesh* CreateMesh(bool dynamic, bool gradient);

private:
	//! 三角形边定义
	typedef struct _SEdge {
		size_t v1;
		size_t v2;
		_SEdge(size_t a, size_t b): v1(a), v2(b) {}
	} SEdge;
	//! 三角形定义
	typedef struct _STriangle {
		size_t a;
		size_t b;
		size_t c;
		float cx;
		float cy;
		float radius2;
		_STriangle(size_t i, size_t j, size_t k): a(i), b(j), c(k) {}
	} STriangle;

private:
	//! 将所有顶点沿X轴排序
	void SortPointsAlongX(vector<CVector3>& vertices, vector<size_t>& indices);
	//! 添加超级三角形到顶点列表末尾
	void AddSuperTriangle(vector<CVector3>& vertices);
	//! 计算三角形外接圆
	STriangle& Circumcircle(STriangle& triangle);
	//! 移除边列表中的重复边
	void RemoveDoublyEdge(list<SEdge>& edges);
	//! 生成三角形列表
	void BuildTriangles(list<STriangle>& triangles);

private:
	//! 顶点数组
	vector<CVector3> m_vecPoints;
	//! 三角形列表
	vector<STriangle> m_vecTriangles;
};

#endif