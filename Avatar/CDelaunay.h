//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* @brief Delaunay ������������
*/
class AVATAR_EXPORT CDelaunay {
public:
	//! Ĭ�Ϲ��캯��
	CDelaunay();
	//! ���ڶ����б���
	CDelaunay(const vector<CVector3>& points);

	//! ��ӵ�
	void AddPoint(const CVector3& point);
	//! ��������
	void AddPoint(float x, float y, float z);
	//! �������ӵ�
	void ClearPoint();
	//! ����������
	bool Generate();

	//! ��ȡ��������
	int GetPointCount() const;
	//! ��ȡһ������
	CVector3 GetPoint(int index) const;
	//! ��ȡ����������
	int GetTriangleCount() const;
	//! ��ȡһ��������
	void GetTriangle(int index, int vertices[3]) const;

	//! �����������
	CMesh* CreateMesh(bool dynamic, bool gradient);

private:
	//! �����α߶���
	typedef struct _SEdge {
		size_t v1;
		size_t v2;
		_SEdge(size_t a, size_t b): v1(a), v2(b) {}
	} SEdge;
	//! �����ζ���
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
	//! �����ж�����X������
	void SortPointsAlongX(vector<CVector3>& vertices, vector<size_t>& indices);
	//! ��ӳ��������ε������б�ĩβ
	void AddSuperTriangle(vector<CVector3>& vertices);
	//! �������������Բ
	STriangle& Circumcircle(STriangle& triangle);
	//! �Ƴ����б��е��ظ���
	void RemoveDoublyEdge(list<SEdge>& edges);
	//! �����������б�
	void BuildTriangles(list<STriangle>& triangles);

private:
	//! ��������
	vector<CVector3> m_vecPoints;
	//! �������б�
	vector<STriangle> m_vecTriangles;
};

#endif