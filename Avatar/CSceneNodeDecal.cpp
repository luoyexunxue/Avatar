//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeDecal.h"
#include "CEngine.h"
#include "CPlane.h"
#include "CBoundingBox.h"
#include "CRay.h"

/**
* ���캯��
*/
CSceneNodeDecal::CSceneNodeDecal(const string& name, const string& texture, const CMatrix4& proj)
	: CSceneNode("decal", name) {
	m_strTexture = texture;
	m_cDecalMatrix.SetValue(proj);
	m_pMesh = 0;
}

/**
* ��ʼ�������ڵ�
*/
bool CSceneNodeDecal::Init() {
	if (m_pParent && m_pParent->GetMeshData()) {
		m_pMesh = new CMesh();
		m_pMesh->GetMaterial()->SetTexture(m_strTexture);
		m_pMesh->GetMaterial()->GetTexture()->SetWrapModeClampToEdge(true, true);
		return true;
	}
	return false;
}

/**
* ���ٳ����ڵ�
*/
void CSceneNodeDecal::Destroy() {
	delete m_pMesh;
}

/**
* ��Ⱦ�����ڵ�
*/
void CSceneNodeDecal::Render() {
	CGraphicsManager* pGraphicsMgr = CEngine::GetGraphicsManager();
	pGraphicsMgr->SetDepthOffsetEnable(true, -1.0f);
	m_pMesh->Render();
	pGraphicsMgr->SetDepthOffsetEnable(false, 0.0f);
}

/**
* ���±任����
*/
void CSceneNodeDecal::Transform() {
	SetupFrustum();
	UpdateMesh();
}

/**
* ��������ͶӰ�Ӿ���
* �˴�����Ԥ���㣬������õ�
*/
void CSceneNodeDecal::SetupFrustum() {
	const CVector3 dir = m_cOrientation * CVector3::Y;
	const CVector3 up = m_cOrientation * CVector3::Z;
	CMatrix4 viewProj = m_cDecalMatrix * CMatrix4().LookAt(m_cPosition, dir, up);
	m_cDecalFrustum.FromViewProj(viewProj);
	m_cClipPlane[0].SetValue(m_cDecalFrustum.m_fPlane[0], true);
	m_cClipPlane[1].SetValue(m_cDecalFrustum.m_fPlane[1], true);
	m_cClipPlane[2].SetValue(m_cDecalFrustum.m_fPlane[2], true);
	m_cClipPlane[3].SetValue(m_cDecalFrustum.m_fPlane[3], true);
	m_cClipPlane[4].SetValue(m_cDecalFrustum.m_fPlane[4], true);
	m_cClipPlane[5].SetValue(m_cDecalFrustum.m_fPlane[5], true);
	// ��������ͶӰ����
	const float biasMat[16] = {
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	};
	m_cTexCoordProj = CMatrix4(biasMat) * viewProj;
}

/**
* ����ཻ������
* ���㷽����
*    ��ÿ��������
*        ���㾭���Ӿ���ü�������򶥵㼯�ϣ��������꣩
*        �����µĶ�����������
*        ͨ���������������µ�������
*/
void CSceneNodeDecal::UpdateMesh() {
	m_pMesh->RemoveVertex(0, -1);
	m_pMesh->RemoveTriangle(0, -1);
	CMeshData* pMeshData = m_pParent->GetMeshData();
	for (int m = 0; m < pMeshData->GetMeshCount(); m++) {
		CMesh* mesh = pMeshData->GetMesh(m);
		// �����ж�����б任
		CBoundingBox boundingBox;
		vector<CVertex> vertices;
		vertices.resize(mesh->GetVertexCount());
		for (size_t i = 0; i < vertices.size(); i++) {
			CVertex* vert = mesh->GetVertex(i);
			CVector3 pos = m_pParent->m_cWorldMatrix * CVector3(vert->m_fPosition);
			CVector3 nor = m_pParent->m_cWorldMatrix * CVector3(vert->m_fNormal, 0.0f);
			vertices[i].SetPosition(pos[3] != 0.0f ? pos.Scale(1.0f / pos[3]) : pos);
			vertices[i].SetNormal(nor);
			boundingBox.Update(pos);
		}
		// �Բ���ͶӰ�Ӿ����ڵ���������
		if (!m_cDecalFrustum.IsAABBInside(boundingBox)) continue;
		// �����������ν��вü�
		for (int i = 0; i < mesh->GetTriangleCount(); i++) {
			unsigned int vertexIndices[3];
			mesh->GetTriangle(i, vertexIndices);
			CVector3 p1 = CVector3(vertices[vertexIndices[0]].m_fPosition);
			CVector3 p2 = CVector3(vertices[vertexIndices[1]].m_fPosition);
			CVector3 p3 = CVector3(vertices[vertexIndices[2]].m_fPosition);
			// �����������Ӿ���ü�
			float intersect[12];
			int count = IntersectWithFrustum(p1, p2, p3, intersect);
			if (count < 3) continue;
			// �����η�����
			CVector3 n1 = CVector3(vertices[vertexIndices[0]].m_fNormal);
			CVector3 n2 = CVector3(vertices[vertexIndices[1]].m_fNormal);
			CVector3 n3 = CVector3(vertices[vertexIndices[2]].m_fNormal);
			// ����������
			int baseIndex = m_pMesh->GetVertexCount();
			for (int n = 0; n < count; n++) {
				const float bu = intersect[n * 2];
				const float bv = intersect[n * 2 + 1];
				const float bw = 1.0f - bu - bv;
				// ͨ�����������ֵ���㽻�������ͷ��򣬲�������������
				CVector3 vp = p1 * bw + p2 * bu + p3 * bv;
				CVector3 vn = n1 * bw + n2 * bu + n3 * bv;
				CVector3 st = m_cTexCoordProj * vp;
				st[0] = st[0] / st[3];
				st[1] = 1.0f - st[1] / st[3];
				m_pMesh->AddVertex(CVertex(vp[0], vp[1], vp[2], st[0], st[1], vn[0], vn[1], vn[2]));
			}
			for (int n = 1; n < count - 1; n++) {
				m_pMesh->AddTriangle(baseIndex, baseIndex + n, baseIndex + n + 1);
			}
		}
	}
	m_pMesh->Create(false);
}

/**
* ����������ͶӰ���ڵĽ���
* @param p1 �����ε�һ������
* @param p2 �����εڶ�������
* @param p3 �����ε���������
* @param result ��������ϵ
* @return �������
* @note �������� = (1 - u - v) * p1 + u * p2 + v * p3 ���� (u,v) Ϊ��������
* ���㷽����
*    ��ʼ�����㼯��1��������������������붥�㼯��1
*    ���Ӿ���ÿ���ü������
*        �Լ���1��ÿ�����㣬�����Ƿ������ڲ�����������붥�㼯��2
*        ��������������¸�������ɵ��߶�����Ľ��㲢���붥�㼯��2
*        �������㼯��
*    �����յõ��Ķ��㼯��ת��Ϊ�������꣬�������
*/
int CSceneNodeDecal::IntersectWithFrustum(const CVector3& p1, const CVector3& p2, const CVector3& p3, float* result) {
	CBoundingBox aabb;
	aabb.Update(p1);
	aabb.Update(p2);
	aabb.Update(p3);
	if (!m_cDecalFrustum.IsAABBInside(aabb)) return 0;
	CVector3 vt = (p2 - p1).CrossProduct(p3 - p1);
	// ����������Ƿ�����ͶӰ��
	if (vt.DotProduct(m_cPosition - p1) <= 0.0f) return 0;
	// ���Ӿ���ÿ��ƽ������ཻ����
	int current = 0;
	int vertexCount[2] = {0, 0};
	CVector3 vertexBuffer[2][6];
	CVector3 intersectPoint;
	vertexBuffer[current][vertexCount[current]++].SetValue(p1);
	vertexBuffer[current][vertexCount[current]++].SetValue(p2);
	vertexBuffer[current][vertexCount[current]++].SetValue(p3);
	for (int j = 0; j < 6; j++) {
		for (int v = 0; v < vertexCount[current]; v++) {
			int next = (v + 1) % vertexCount[current];
			if (m_cClipPlane[j].Distance(vertexBuffer[current][v]) > 0.0f) {
				vertexBuffer[1 - current][vertexCount[1 - current]++].SetValue(vertexBuffer[current][v]);
			}
			if (m_cClipPlane[j].IntersectLineSegment(vertexBuffer[current][v], vertexBuffer[current][next], intersectPoint)) {
				vertexBuffer[1 - current][vertexCount[1 - current]++].SetValue(intersectPoint);
			}
		}
		vertexCount[current] = 0;
		current = 1 - current;
	}
	// see wiki Barycentric coordinate system
	float area = vt.Length();
	for (int i = 0; i < vertexCount[current]; i++) {
		const CVector3 a = p1 - vertexBuffer[current][i];
		const CVector3 b = p2 - vertexBuffer[current][i];
		const CVector3 c = p3 - vertexBuffer[current][i];
		result[i * 2 + 0] = c.CrossProduct(a).Length() / area;
		result[i * 2 + 1] = a.CrossProduct(b).Length() / area;
	}
	return vertexCount[current];
}