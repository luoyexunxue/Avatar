//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CDelaunay.h"
#include "CBoundingBox.h"
#include "CColor.h"

/**
* ���캯��
*/
CDelaunay::CDelaunay() {
}

/**
* ʹ��һϵ�е㹹��
*/
CDelaunay::CDelaunay(const vector<CVector3>& points) {
	m_vecPoints = points;
}

/**
* ���ӵ�
*/
void CDelaunay::AddPoint(const CVector3& point) {
	m_vecPoints.push_back(point);
}

/**
* ���������
*/
void CDelaunay::AddPoint(float x, float y, float z) {
	m_vecPoints.push_back(CVector3(x, y, z));
}

/**
* ��������ӵĵ�����
*/
void CDelaunay::ClearPoint() {
	m_vecPoints.clear();
	m_vecTriangles.clear();
}

/**
* ����������(�����뷨)
*/
bool CDelaunay::Generate() {
	m_vecTriangles.clear();
	if (m_vecPoints.size() < 3) return false;

	// ��������
	vector<size_t> indices;
	SortPointsAlongX(m_vecPoints, indices);
	AddSuperTriangle(m_vecPoints);
	// ���㳬�����������Բ�����뵽�����б���
	list<STriangle> openList;
	list<STriangle> closedList;
	STriangle super(indices.size(), indices.size() + 1, indices.size() + 2);
	openList.push_back(Circumcircle(super));

	// �������ж���
	for (size_t i = 0; i < indices.size(); i++) {
		list<SEdge> edgeList;
		list<STriangle>::iterator iter = openList.begin();
		while (iter != openList.end()) {
			const float dx = m_vecPoints[indices[i]][0] - iter->cx;
			const float dy = m_vecPoints[indices[i]][1] - iter->cy;
			if (dx > 0.0f && dx * dx > iter->radius2) {
				list<STriangle>::iterator tmp = iter++;
				closedList.push_back(*tmp);
				openList.erase(tmp);
				continue;
			}
			if (dx * dx + dy * dy < iter->radius2) {
				list<STriangle>::iterator tmp = iter++;
				edgeList.push_back(SEdge(tmp->a, tmp->b));
				edgeList.push_back(SEdge(tmp->b, tmp->c));
				edgeList.push_back(SEdge(tmp->c, tmp->a));
				openList.erase(tmp);
				continue;
			}
			++iter;
		}
		// �Ƴ��ر߲�����ǰ�������б���������μ��뵽�����б���
		RemoveDoublyEdge(edgeList);
		for (list<SEdge>::iterator j = edgeList.begin(); j != edgeList.end(); ++j) {
			STriangle triangle(j->v1, j->v2, indices[i]);
			openList.push_back(Circumcircle(triangle));
		}
	}
	// �����������εĶ����Ƴ�
	m_vecPoints.pop_back();
	m_vecPoints.pop_back();
	m_vecPoints.pop_back();
	// �������б��������μ��뵽�պ��б���
	for (list<STriangle>::iterator i = openList.begin(); i != openList.end(); ++i) {
		closedList.push_back(*i);
	}
	BuildTriangles(closedList);
	return true;
}

/**
* ��ȡ��������
*/
int CDelaunay::GetPointCount() const {
	return m_vecPoints.size();
}

/**
* ��ȡһ������
*/
CVector3 CDelaunay::GetPoint(int index) const {
	return m_vecPoints[index];
}

/**
* ��ȡ����������
*/
int CDelaunay::GetTriangleCount() const {
	return m_vecTriangles.size();
}

/**
* ��ȡһ��������
*/
void CDelaunay::GetTriangle(int index, int vertices[3]) const {
	const STriangle& tri = m_vecTriangles[index];
	vertices[0] = tri.a;
	vertices[1] = tri.b;
	vertices[2] = tri.c;
}

/**
* �������������������
*/
CMesh* CDelaunay::CreateMesh(bool dynamic, bool gradient) {
	CBoundingBox boundingBox;
	for (size_t i = 0; i < m_vecPoints.size(); i++) {
		boundingBox.Update(m_vecPoints[i]);
	}
	const float isizex = 1.0f / (boundingBox.m_cMax[0] - boundingBox.m_cMin[0]);
	const float isizey = 1.0f / (boundingBox.m_cMax[1] - boundingBox.m_cMin[1]);
	const float isizez = 1.0f / (boundingBox.m_cMax[2] - boundingBox.m_cMin[2]);
	CColor color;
	CMesh* pMesh = new CMesh();
	pMesh->SetVertexUsage(m_vecPoints.size());
	for (size_t i = 0; i < m_vecPoints.size(); i++) {
		CVector3& v = m_vecPoints[i];
		float s = (v[0] - boundingBox.m_cMin[0]) * isizex;
		float t = (v[1] - boundingBox.m_cMin[1]) * isizey;
		if (gradient) color.FromHsla((v[2] - boundingBox.m_cMin[2]) * isizez * 240.0f, 100.0f, 50.0f, 1.0f);
		pMesh->AddVertex(CVertex(v[0], v[1], v[2], s, t, 0.0f, 0.0f, 0.0f, color.m_fValue));
	}
	for (size_t i = 0; i < m_vecTriangles.size(); i++) {
		pMesh->AddTriangle(m_vecTriangles[i].a, m_vecTriangles[i].b, m_vecTriangles[i].c);
	}
	pMesh->SetupNormal();
	pMesh->Create(dynamic);
	return pMesh;
}

/**
* �����ж�����X������
*/
void CDelaunay::SortPointsAlongX(vector<CVector3>& vertices, vector<size_t>& indices) {
	indices.resize(vertices.size());
	for (size_t i = 0; i < vertices.size(); i++) indices[i] = i;
	for (size_t i = 0; i < vertices.size(); i++) {
		for (size_t j = i + 1; j < vertices.size(); j++) {
			if (vertices[indices[j]][0] < vertices[indices[i]][0]) {
				size_t temp = indices[i];
				indices[i] = indices[j];
				indices[j] = temp;
			}
		}
	}
}

/**
* ���ӳ��������ε������б�ĩβ
*/
void CDelaunay::AddSuperTriangle(vector<CVector3>& vertices) {
	float minx = vertices[0][0];
	float miny = vertices[0][1];
	float maxx = vertices[0][0];
	float maxy = vertices[0][1];
	for (size_t i = 1; i < vertices.size(); i++) {
		if (vertices[i][0] < minx) minx = vertices[i][0];
		else if (vertices[i][0] > maxx) maxx = vertices[i][0];
		if (vertices[i][1] < miny) miny = vertices[i][1];
		else if (vertices[i][1] > maxy) maxy = vertices[i][1];
	}
	float dx = maxx - minx;
	float dy = maxy - miny;
	float midx = minx + dx * 0.5f;
	float midy = miny + dx * 0.5f;
	float maxw = dx > dy ? dx : dy;
	CVector3 a(midx - 3.0f * maxw, midy - maxw, 0.0f);
	CVector3 b(midx + 3.0f * maxw, midy - maxw, 0.0f);
	CVector3 c(midx, midy + 2.0f * maxw, 0.0f);
	vertices.push_back(a);
	vertices.push_back(b);
	vertices.push_back(c);
}

/**
* �������������Բ
*/
CDelaunay::STriangle& CDelaunay::Circumcircle(STriangle& triangle) {
	const CVector3& pa = m_vecPoints[triangle.a];
	const CVector3& pb = m_vecPoints[triangle.b];
	const CVector3& pc = m_vecPoints[triangle.c];

	float x1  =  pa.m_fValue[0];
	float x2  =  pb.m_fValue[0];
	float x3  =  pc.m_fValue[0];
	float y1  =  pa.m_fValue[1];
	float y2  =  pb.m_fValue[1];
	float y3  =  pc.m_fValue[1];

	float t1 = x1 * x1 + y1 * y1;
	float t2 = x2 * x2 + y2 * y2;
	float t3 = x3 * x3 + y3 * y3;
	float dt = 2.0f * (x1 * y2 + x2 * y3 + x3 * y1 - y1 * x2 - y2 * x3 - y3 * x1);

	triangle.cx = ((y3 - y1) * (t2 - t1) - (y2 - y1) * (t3 - t1)) / dt;
	triangle.cy = ((x2 - x1) * (t3 - t1) - (x3 - x1) * (t2 - t1)) / dt;
	triangle.radius2 = (x1 - triangle.cx) * (x1 - triangle.cx) + (y1 - triangle.cy) * (y1 - triangle.cy);
	return triangle;
}

/**
* �Ƴ����б��е��ظ���
*/
void CDelaunay::RemoveDoublyEdge(list<SEdge>& edges) {
	list<SEdge>::iterator iter = edges.begin();
	while (iter != edges.end()) {
		size_t a = iter->v1;
		size_t b = iter->v2;
		list<SEdge>::iterator temp = iter++;
		for (list<SEdge>::iterator i = iter; i != edges.end(); ++i) {
			size_t m = i->v1;
			size_t n = i->v2;
			if ((a == m && b == n) || (a == n && b == m)) {
				if (i == iter) ++iter;
				edges.erase(i);
				edges.erase(temp);
				break;
			}
		}
	}
}

/**
* �����������б�
*/
void CDelaunay::BuildTriangles(list<STriangle>& triangles) {
	size_t count = m_vecPoints.size();
	for (list<STriangle>::iterator i = triangles.begin(); i != triangles.end(); ++i) {
		if (i->a < count && i->b < count && i->c < count) {
			// ��������η���
			CVector3 ab = m_vecPoints[i->b] - m_vecPoints[i->a];
			CVector3 ac = m_vecPoints[i->c] - m_vecPoints[i->a];
			if (ab.CrossProduct(ac).DotProduct(CVector3::Z) < 0) {
				size_t temp = i->a;
				i->a = i->b;
				i->b = temp;
			}
			m_vecTriangles.push_back(*i);
		}
	}
}