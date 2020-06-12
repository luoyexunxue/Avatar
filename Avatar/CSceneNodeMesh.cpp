//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeMesh.h"
#include "CEngine.h"

/**
* 构造函数
*/
CSceneNodeMesh::CSceneNodeMesh(const string& name, CMesh* mesh) : CSceneNode("mesh", name) {
	m_pMesh = mesh;
	m_pMeshData = 0;
}

/**
* 初始化场景节点
*/
bool CSceneNodeMesh::Init() {
	if (m_pMesh) {
		m_pMeshData = new CMeshData();
		m_pMeshData->AddMesh(m_pMesh);
		m_cLocalBoundingBox = m_pMesh->GetBoundingBox();
		return true;
	}
	return false;
}

/**
* 销毁场景节点
*/
void CSceneNodeMesh::Destroy() {
	if (m_pMeshData) delete m_pMeshData;
}

/**
* 渲染场景节点
*/
void CSceneNodeMesh::Render() {
	if (CEngine::GetGraphicsManager()->IsDepthRender()) {
		m_pMesh->GetMaterial()->GetTexture()->UseTexture();
		m_pMesh->Render(false);
	} else {
		if (m_pMesh->GetMaterial()->GetShader()) {
			CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
			m_pMesh->GetMaterial()->PassUniform("uProjMatrix", pCamera->GetProjMatrix());
			m_pMesh->GetMaterial()->PassUniform("uViewMatrix", pCamera->GetViewMatrix());
			m_pMesh->GetMaterial()->PassUniform("uCameraPos", pCamera->m_cPosition);
			m_pMesh->GetMaterial()->PassUniform("uModelMatrix", m_cWorldMatrix);
		}
		m_pMesh->Render();
	}
}

/**
* 获取网格数据
*/
CMeshData* CSceneNodeMesh::GetMeshData() {
	return m_pMeshData;
}