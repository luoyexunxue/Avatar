//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeStatic.h"
#include "CEngine.h"
#include "CMeshLoader.h"

/**
* ���캯��
*/
CSceneNodeStatic::CSceneNodeStatic(const string& name, const string& meshFile): CSceneNode("static", name) {
	m_strFile = meshFile;
	m_pMeshData = 0;
}

/**
* ��ʼ�������ڵ�
*/
bool CSceneNodeStatic::Init() {
	m_pMeshData = CMeshLoader::Load(m_strFile, true);
	if (!m_pMeshData) return false;
	m_cLocalBoundingBox = m_pMeshData->GetBoundingBox();
	return true;
}

/**
* ���ٳ����ڵ�
*/
void CSceneNodeStatic::Destroy() {
	CMeshLoader::Remove(m_pMeshData);
}

/**
* ��Ⱦ�����ڵ�
*/
void CSceneNodeStatic::Render() {
	int count = m_pMeshData->GetMeshCount();
	if (CEngine::GetGraphicsManager()->IsDepthRender()) {
		for (int i = 0; i < count; i++) {
			CMesh* pMesh = m_pMeshData->GetMesh(i);
			pMesh->GetMaterial()->GetTexture()->UseTexture();
			pMesh->Render(false);
		}
	} else {
		CShader* currentShader = CEngine::GetShaderManager()->GetCurrentShader();
		for (int i = 0; i < count; i++) {
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
* ��ȡ��������
*/
CMeshData* CSceneNodeStatic::GetMeshData() {
	return m_pMeshData;
}