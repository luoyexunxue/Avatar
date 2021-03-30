//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeStatic.h"
#include "CEngine.h"
#include "CMeshLoader.h"

/**
* 构造函数
*/
CSceneNodeStatic::CSceneNodeStatic(const string& name, const string& meshFile, bool cache): CSceneNode("static", name) {
	m_strFile = meshFile;
	m_bUseCache = cache;
	m_pMeshData = 0;
}

/**
* 初始化场景节点
*/
bool CSceneNodeStatic::Init() {
	m_pMeshData = CMeshLoader::Load(m_strFile, true, m_bUseCache);
	if (!m_pMeshData) return false;
	m_cLocalBoundingBox = m_pMeshData->GetBoundingBox();
	return true;
}

/**
* 销毁场景节点
*/
void CSceneNodeStatic::Destroy() {
	CMeshLoader::Remove(m_pMeshData);
}

/**
* 渲染场景节点
*/
void CSceneNodeStatic::Render() {
	size_t count = m_pMeshData->GetMeshCount();
	if (CEngine::GetGraphicsManager()->IsDepthRender()) {
		for (size_t i = 0; i < count; i++) {
			CMesh* pMesh = m_pMeshData->GetMesh(i);
			pMesh->GetMaterial()->GetTexture()->UseTexture();
			pMesh->Render(false);
		}
	} else {
		CShader* currentShader = CEngine::GetShaderManager()->GetCurrentShader();
		for (size_t i = 0; i < count; i++) {
			CMesh* pMesh = m_pMeshData->GetMesh(i);
			if (pMesh->GetMaterial()->GetShader()) {
				CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
				pMesh->GetMaterial()->PassUniform("uProjMatrix", pCamera->GetProjMatrix());
				pMesh->GetMaterial()->PassUniform("uViewMatrix", pCamera->GetViewMatrix());
				pMesh->GetMaterial()->PassUniform("uCameraPos", pCamera->m_cPosition);
				pMesh->GetMaterial()->PassUniform("uModelMatrix", m_cWorldMatrix);
			} else currentShader->UseShader();
			pMesh->Render();
		}
	}
}

/**
* 获取网格数据
*/
CMeshData* CSceneNodeStatic::GetMeshData() {
	return m_pMeshData;
}