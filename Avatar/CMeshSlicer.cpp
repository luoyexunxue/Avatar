//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CMeshSlicer.h"
#include "CSceneNodeLine.h"
#include <cmath>

/**
* ���캯��
*/
CMeshSlicer::CMeshSlicer() {
}

/**
* ʹ��ָ�����������
*/
CMeshSlicer::CMeshSlicer(const CMesh* mesh) {
	m_vecMeshes.push_back(mesh);
}

/**
* �������
*/
void CMeshSlicer::AddMesh(const CMesh* mesh) {
	m_vecMeshes.push_back(mesh);
}

/**
* ��ָ��ƽ���и�
*/
bool CMeshSlicer::Slice(const CPlane& plane) {
	for (size_t i = 0; i < m_vecMeshes.size(); i++) {
		CMesh* pMesh = const_cast<CMesh*>(m_vecMeshes[i]);
		unsigned int triangleCount = (unsigned int)pMesh->GetTriangleCount();
		for (unsigned int t = 0; t < triangleCount; t++) {
			CVector3 a(pMesh->GetVertex(t, 0)->m_fPosition);
			CVector3 b(pMesh->GetVertex(t, 1)->m_fPosition);
			CVector3 c(pMesh->GetVertex(t, 2)->m_fPosition);
			SliceTriangle(plane, a, b, c);
		}
	}
	BuildTrackLine();
	return m_vecTrackLines.size() > 0;
}

/**
* ��ָ���������и�
* @param axis ������ 1,2,3 �ֱ�Ϊ X,Y,Z ��
* @param value ����ֵ
*/
bool CMeshSlicer::Slice(int axis, float value) {
	CVector3 normal;
	switch (axis) {
	case 1: normal[0] = 1.0f; break;
	case 2: normal[1] = 1.0f; break;
	case 3: normal[2] = 1.0f; break;
	case -1: normal[0] = -1.0f; break;
	case -2: normal[1] = -1.0f; break;
	case -3: normal[2] = -1.0f; break;
	default: normal[2] = 1.0f; break;
	}
	return Slice(CPlane(normal, value));
}

/**
* ��ȡ�켣����
* @note �и����ܰ�����ֹһ���켣�����пսṹ��ģ��
*/
int CMeshSlicer::GetTrackCount() const {
	return m_vecTrackLines.size();
}

/**
* ��ȡһ���켣�ĵ����
* @note �켣�ϵĵ����и�ƽ���ϰ���ʱ������������У�
*	��Ϊ˳ʱ�����У����ʾ�˹켣Ϊ�ڲ���ǻ
*/
int CMeshSlicer::GetTrackPointCount(int track) const {
	return m_vecTrackLines[track].size();
}

/**
* ��ȡ�켣��ָ����
*/
const CVector3& CMeshSlicer::GetTrackPoint(int track, int point) const {
	list<CVector3>::const_iterator iter = m_vecTrackLines[track].begin();
	while (point--) ++iter;
	return *iter;
}

/**
* ��������Ⱦ�ĳ����ڵ�
*/
CSceneNode* CMeshSlicer::CreateRenderLine(CSceneNode* src, int track, bool depth, bool point) {
	if (!src) {
		src = new CSceneNodeLine("slicer", CColor::Red, 1.0f);
	}
	CSceneNodeLine* line = reinterpret_cast<CSceneNodeLine*>(src);
	line->ClearPoint();
	line->ShowPoints(point, 4.0f);
	line->DisableDepth(!depth);
	list<CVector3>::iterator iter = m_vecTrackLines[track].begin();
	while (iter != m_vecTrackLines[track].end()) {
		line->AddPoint(*iter);
		++iter;
	}
	line->AddPoint(*m_vecTrackLines[track].begin());
	return src;
}

/**
* �и�������
*/
void CMeshSlicer::SliceTriangle(const CPlane& plane, const CVector3& a, const CVector3& b, const CVector3& c) {
	CVector3 ab = b - a;
	CVector3 bc = c - b;
	CVector3 ca = a - c;
	CVector3 norm(plane.m_fNormal);
	// ��������������ɵ��߶���ƽ����
	float t1 = plane.m_fDistance - a.DotProduct(norm);
	float d1 = ab.DotProduct(norm);
	float t2 = plane.m_fDistance - b.DotProduct(norm);
	float d2 = bc.DotProduct(norm);
	float t3 = plane.m_fDistance - c.DotProduct(norm);
	float d3 = ca.DotProduct(norm);
	if (d1 < 0.0f) { t1 = -t1; d1 = -d1; }
	if (d2 < 0.0f) { t2 = -t2; d2 = -d2; }
	if (d3 < 0.0f) { t3 = -t3; d3 = -d3; }
	int count = 0;
	CVector3 intersect[3];
	if (d1 >= 1e-6 && t1 >= 0.0f && t1 <= d1) intersect[count++] = a + ab * (t1 / d1);
	if (d2 >= 1e-6 && t2 >= 0.0f && t2 <= d2) intersect[count++] = b + bc * (t2 / d2);
	if (d3 >= 1e-6 && t3 >= 0.0f && t3 <= d3) intersect[count++] = c + ca * (t3 / d3);
	if (count < 2) return;
	// �ཻ�������ε�һ�����㣬����
	if (count == 2 && IsEqual(intersect[0], intersect[1])) return;
	// ��������һ�������ཻ���Ա��ཻ
	if (count == 3 && IsEqual(intersect[0], intersect[1])) intersect[1] = intersect[2];
	// ��鷽��
	CVector3 dir = norm.CrossProduct(ab.CrossProduct(bc));
	if (dir.DotProduct(intersect[1] - intersect[0]) > 0) {
		m_lstTrackSet.push_back(STrackSegment(intersect[0], intersect[1]));
	} else {
		m_lstTrackSet.push_back(STrackSegment(intersect[1], intersect[0]));
	}
}

/**
* ���и����߶���������
* @attention ÿ������ĵ�һ����������һ�����㲻�ظ�
*/
void CMeshSlicer::BuildTrackLine() {
	m_vecTrackLines.clear();
	if (m_lstTrackSet.empty()) return;

	// ѭ������ÿ���߶Σ�ʹ��ͷβ���
	while (!m_lstTrackSet.empty()) {
		list<STrackSegment>::iterator iter = m_lstTrackSet.begin();
		CVector3 from = iter->a;
		CVector3 to = iter->b;
		m_lstTrackSet.erase(iter);
		m_vecTrackLines.push_back(list<CVector3>());
		for (iter = m_lstTrackSet.begin(); iter != m_lstTrackSet.end();) {
			if (IsEqual(to, iter->a)) {
				to = iter->b;
				m_vecTrackLines.back().push_back(iter->a);
				m_lstTrackSet.erase(iter);
				iter = m_lstTrackSet.begin();
				continue;
			}
			if (IsEqual(iter->b, from)) {
				from = iter->a;
				m_vecTrackLines.back().push_front(iter->b);
				m_lstTrackSet.erase(iter);
				iter = m_lstTrackSet.begin();
				continue;
			}
			++iter;
		}
		m_vecTrackLines.back().push_front(from);
		if (from != to) m_vecTrackLines.back().push_back(to);
	}
}

/**
* �ж������Ƿ���ͬ
*/
bool CMeshSlicer::IsEqual(const CVector3& a, const CVector3& b) const {
	float x = a.m_fValue[0] - b.m_fValue[0];
	float y = a.m_fValue[1] - b.m_fValue[1];
	float z = a.m_fValue[2] - b.m_fValue[2];
	if (fabs(x) < 1e-6 && fabs(y) < 1e-6 && fabs(z) < 1e-6) return true;
	return false;
}