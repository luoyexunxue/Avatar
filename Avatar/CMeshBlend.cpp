//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CMeshBlend.h"
#include <cmath>

/**
* Ĭ�Ϲ��캯��
*/
CMeshBlend::CMeshBlend() {
	m_pTargetMesh = 0;
}

/**
* Ĭ����������
*/
CMeshBlend::~CMeshBlend() {
	if (m_pTargetMesh) delete m_pTargetMesh;
}

/**
* �������
*/
void CMeshBlend::AddMesh(const CMesh* mesh) {
	m_vecMeshes.push_back(const_cast<CMesh*>(mesh));
	m_vecWeights.push_back(1.0f);
}

/**
* �������ָ��Ȩ��
*/
void CMeshBlend::AddMesh(const CMesh* mesh, float weight) {
	m_vecMeshes.push_back(const_cast<CMesh*>(mesh));
	m_vecWeights.push_back(weight);
}

/**
* ���
*/
CMesh* CMeshBlend::Blend() {
	float sum = 0.0f;
	for (size_t i = 0; i < m_vecWeights.size(); i++) sum += m_vecWeights[i];
	for (size_t i = 0; i < m_vecWeights.size(); i++) m_vecWeights[i] /= sum;
	if (!m_pTargetMesh) SetupMesh();
	Interpolate();
	m_pTargetMesh->Update(1);
	return m_pTargetMesh;
}

/**
* ��ָ��Ȩ�ػ��
* @param weights Ȩ���б�Ϊ�ձ�ʾ��������Ȩ����ͬ
* @param count ���� weights �ĳ��ȣ��������С��������������֮�������Ȩ�ض�Ϊ��
*/
CMesh* CMeshBlend::Blend(float* weights, int count) {
	if (m_vecMeshes.empty()) return 0;
	if (!weights) {
		for (size_t i = 0; i < m_vecWeights.size(); i++) m_vecWeights[i] = 1.0f;
	} else {
		if ((size_t)count > m_vecMeshes.size()) count = m_vecMeshes.size();
		for (int i = 0; i < count; i++) m_vecWeights[i] = weights[i];
		for (size_t i = count; i < m_vecWeights.size(); i++) m_vecWeights[i] = 0.0f;
	}
	return Blend();
}

/**
* ��ʼ��Ŀ������
*/
void CMeshBlend::SetupMesh() {
	CMesh* temp = m_vecMeshes[0];
	m_pTargetMesh = new CMesh();
	m_pTargetMesh->SetVertexUsage(m_vecMeshes[0]->GetVertexCount());
	m_pTargetMesh->GetMaterial()->CopyFrom(temp->GetMaterial());
	int triangles = temp->GetTriangleCount();
	int vertices = temp->GetVertexCount();
	for (int i = 0; i < triangles; i++) {
		unsigned int index[3];
		temp->GetTriangle(i, index);
		m_pTargetMesh->AddTriangle(index[0], index[1], index[2]);
	}
	for (int i = 0; i < vertices; i++) {
		CVertex* vert = temp->GetVertex(i);
		m_pTargetMesh->AddVertex(*vert);
	}
	m_pTargetMesh->Create(true);
}

/**
* �Զ����ֵ
*/
void CMeshBlend::Interpolate() {
	int vertexCount = m_pTargetMesh->GetVertexCount();
	for (int i = 0; i < vertexCount; i++) {
		CVertex* target = m_pTargetMesh->GetVertex(i);
		target->m_fPosition[0] = 0.0f;
		target->m_fPosition[1] = 0.0f;
		target->m_fPosition[2] = 0.0f;
		target->m_fTexCoord[0] = 0.0f;
		target->m_fTexCoord[1] = 0.0f;
		target->m_fNormal[0] = 0.0f;
		target->m_fNormal[1] = 0.0f;
		target->m_fNormal[2] = 0.0f;
		target->m_fColor[0] = 0.0f;
		target->m_fColor[1] = 0.0f;
		target->m_fColor[2] = 0.0f;
		target->m_fColor[3] = 0.0f;
		for (size_t j = 0; j < m_vecMeshes.size(); j++) {
			CVertex* src = m_vecMeshes[j]->GetVertex(i);
			float weight = m_vecWeights[j];
			target->m_fPosition[0] += src->m_fPosition[0] * weight;
			target->m_fPosition[1] += src->m_fPosition[1] * weight;
			target->m_fPosition[2] += src->m_fPosition[2] * weight;
			target->m_fTexCoord[0] += src->m_fTexCoord[0] * weight;
			target->m_fTexCoord[1] += src->m_fTexCoord[1] * weight;
			target->m_fNormal[0] += src->m_fNormal[0] * weight;
			target->m_fNormal[1] += src->m_fNormal[1] * weight;
			target->m_fNormal[2] += src->m_fNormal[2] * weight;
			target->m_fColor[0] += src->m_fColor[0] * weight;
			target->m_fColor[1] += src->m_fColor[1] * weight;
			target->m_fColor[2] += src->m_fColor[2] * weight;
			target->m_fColor[3] += src->m_fColor[3] * weight;
		}
	}
}