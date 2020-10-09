//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CMeshBlend.h"
#include <cmath>

/**
* 默认构造函数
*/
CMeshBlend::CMeshBlend() {
	m_pTargetMesh = 0;
}

/**
* 默认析构函数
*/
CMeshBlend::~CMeshBlend() {
	if (m_pTargetMesh) delete m_pTargetMesh;
}

/**
* 添加网格
*/
void CMeshBlend::AddMesh(const CMesh* mesh) {
	m_vecMeshes.push_back(const_cast<CMesh*>(mesh));
	m_vecWeights.push_back(1.0f);
}

/**
* 添加网格并指定权重
*/
void CMeshBlend::AddMesh(const CMesh* mesh, float weight) {
	m_vecMeshes.push_back(const_cast<CMesh*>(mesh));
	m_vecWeights.push_back(weight);
}

/**
* 混合
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
* 按指定权重混合
* @param weights 权重列表，为空表示所有网格权重相同
* @param count 参数 weights 的长度，如果长度小于网格数量，则之后的网格权重都为零
*/
CMesh* CMeshBlend::Blend(float* weights, size_t count) {
	if (m_vecMeshes.empty()) return 0;
	if (!weights) {
		for (size_t i = 0; i < m_vecWeights.size(); i++) m_vecWeights[i] = 1.0f;
	} else {
		if (count > m_vecMeshes.size()) count = m_vecMeshes.size();
		for (size_t i = 0; i < count; i++) m_vecWeights[i] = weights[i];
		for (size_t i = count; i < m_vecWeights.size(); i++) m_vecWeights[i] = 0.0f;
	}
	return Blend();
}

/**
* 初始化目标网格
*/
void CMeshBlend::SetupMesh() {
	CMesh* temp = m_vecMeshes[0];
	m_pTargetMesh = new CMesh();
	m_pTargetMesh->SetVertexUsage(m_vecMeshes[0]->GetVertexCount());
	m_pTargetMesh->GetMaterial()->CopyFrom(temp->GetMaterial());
	size_t triangles = temp->GetTriangleCount();
	size_t vertices = temp->GetVertexCount();
	for (size_t i = 0; i < triangles; i++) {
		unsigned int index[3];
		temp->GetTriangle(i, index);
		m_pTargetMesh->AddTriangle(index[0], index[1], index[2]);
	}
	for (size_t i = 0; i < vertices; i++) {
		CVertex* vert = temp->GetVertex(i);
		m_pTargetMesh->AddVertex(*vert);
	}
	m_pTargetMesh->Create(true);
}

/**
* 对顶点插值
*/
void CMeshBlend::Interpolate() {
	size_t vertexCount = m_pTargetMesh->GetVertexCount();
	for (size_t i = 0; i < vertexCount; i++) {
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