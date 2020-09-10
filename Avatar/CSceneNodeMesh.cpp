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
	m_pMeshData = new CMeshData();
	m_pMeshData->AddMesh(mesh);
	m_cLocalBoundingBox = mesh->GetBoundingBox();
}

/**
* 初始化场景节点
*/
bool CSceneNodeMesh::Init() {
	return true;
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
	int meshCount = m_pMeshData->GetMeshCount();
	if (CEngine::GetGraphicsManager()->IsDepthRender()) {
		for (int i = 0; i < meshCount; i++) {
			m_pMeshData->GetMesh(i)->GetMaterial()->GetTexture()->UseTexture();
			m_pMeshData->GetMesh(i)->Render(false);
		}
	} else {
		for (int i = 0; i < meshCount; i++) {
			CMesh* mesh = m_pMeshData->GetMesh(i);
			if (mesh->GetMaterial()->GetShader()) {
				CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
				mesh->GetMaterial()->PassUniform("uProjMatrix", pCamera->GetProjMatrix());
				mesh->GetMaterial()->PassUniform("uViewMatrix", pCamera->GetViewMatrix());
				mesh->GetMaterial()->PassUniform("uCameraPos", pCamera->m_cPosition);
				mesh->GetMaterial()->PassUniform("uModelMatrix", m_cWorldMatrix);
			}
			mesh->Render();
		}
	}
}

/**
* 获取网格数据
*/
CMeshData* CSceneNodeMesh::GetMeshData() {
	return m_pMeshData;
}

/**
* 添加网格对象
*/
void CSceneNodeMesh::AddMesh(CMesh* mesh, bool replace) {
	if (replace) m_pMeshData->ClearMesh(true);
	m_pMeshData->AddMesh(mesh);
	m_cLocalBoundingBox = m_pMeshData->GetBoundingBox();
}