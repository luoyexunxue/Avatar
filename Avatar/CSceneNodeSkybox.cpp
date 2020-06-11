//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeSkybox.h"
#include "CEngine.h"

/**
* 构造函数
*/
CSceneNodeSkybox::CSceneNodeSkybox(const string& name, const string texture[6]): CSceneNode("skybox", name) {
	m_pMesh = 0;
	m_strTexture[0] = texture[0];
	m_strTexture[1] = texture[1];
	m_strTexture[2] = texture[3];
	m_strTexture[3] = texture[2];
	m_strTexture[4] = texture[4];
	m_strTexture[5] = texture[5];
}

/**
* 初始化场景节点
*/
bool CSceneNodeSkybox::Init() {
	m_pMesh = new CMesh();
	m_pMesh->SetVertexUsage(8);
	m_pMesh->AddVertex(CVertex(-1000.0f, -1000.0f, -1000.0f));
	m_pMesh->AddVertex(CVertex( 1000.0f, -1000.0f, -1000.0f));
	m_pMesh->AddVertex(CVertex( 1000.0f,  1000.0f, -1000.0f));
	m_pMesh->AddVertex(CVertex(-1000.0f,  1000.0f, -1000.0f));
	m_pMesh->AddVertex(CVertex(-1000.0f, -1000.0f,  1000.0f));
	m_pMesh->AddVertex(CVertex( 1000.0f, -1000.0f,  1000.0f));
	m_pMesh->AddVertex(CVertex( 1000.0f,  1000.0f,  1000.0f));
	m_pMesh->AddVertex(CVertex(-1000.0f,  1000.0f,  1000.0f));
	m_pMesh->AddTriangle(2, 1, 5);
	m_pMesh->AddTriangle(2, 5, 6);
	m_pMesh->AddTriangle(0, 3, 7);
	m_pMesh->AddTriangle(0, 7, 4);
	m_pMesh->AddTriangle(3, 2, 6);
	m_pMesh->AddTriangle(3, 6, 7);
	m_pMesh->AddTriangle(1, 0, 4);
	m_pMesh->AddTriangle(1, 4, 5);
	m_pMesh->AddTriangle(7, 6, 5);
	m_pMesh->AddTriangle(7, 5, 4);
	m_pMesh->AddTriangle(0, 1, 2);
	m_pMesh->AddTriangle(0, 2, 3);
	m_pMesh->GetMaterial()->SetTexture(m_strName + "_texture", m_strTexture);
	m_pMesh->Create(false);
	return true;
}

/**
* 销毁场景节点
*/
void CSceneNodeSkybox::Destroy() {
	delete m_pMesh;
}

/**
* 渲染场景节点
*/
void CSceneNodeSkybox::Render() {
	m_pMesh->Render();
}

/**
* 更新逻辑
*/
void CSceneNodeSkybox::Update(float dt) {
	m_cPosition = CEngine::GetGraphicsManager()->GetCamera()->m_cPosition;
	Transform();
}