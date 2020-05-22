//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeMesh.h"
#include "CEngine.h"

/**
* ���캯��
*/
CSceneNodeMesh::CSceneNodeMesh(const string& name, CMesh* mesh) : CSceneNode("mesh", name) {
	m_pMesh = mesh;
	m_pMeshData = 0;
}

/**
* ��ʼ�������ڵ�
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
* ���ٳ����ڵ�
*/
void CSceneNodeMesh::Destroy() {
	if (m_pMeshData) delete m_pMeshData;
}

/**
* ��Ⱦ�����ڵ�
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
* ��ȡ��������
*/
CMeshData* CSceneNodeMesh::GetMeshData() {
	return m_pMeshData;
}