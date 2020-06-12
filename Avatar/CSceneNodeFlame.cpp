//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeFlame.h"
#include "CGeometryCreator.h"
#include "CEngine.h"
#include <cmath>

/**
* 构造函数
*/
CSceneNodeFlame::CSceneNodeFlame(const string& name, const string& texture, const string& distortionMap,
	const string& alphaMask, float width, float height): CSceneNode("flame", name) {
	m_fTimeElapse = 0.0f;
	m_pMesh = 0;
	m_strTexture = texture;
	m_strDistortionMap = distortionMap;
	m_strAlphaMask = alphaMask;
	m_fWidth = width;
	m_fHeight = height;
}

/**
* 初始化场景节点
*/
bool CSceneNodeFlame::Init() {
	m_pMesh = CGeometryCreator::CreatePlane(m_fWidth, m_fHeight, 2);
	m_pMesh->GetMaterial()->SetRenderMode(true, true, true);
	m_pMesh->GetMaterial()->SetTexture(m_strTexture, 0);
	m_pMesh->GetMaterial()->SetTexture(m_strDistortionMap, 1);
	m_pMesh->GetMaterial()->SetTexture(m_strAlphaMask, 2);
	m_pMesh->GetMaterial()->GetTexture(2)->SetWrapModeClampToEdge(true, true);
	CShader* pShader = CEngine::GetShaderManager()->GetShader("flame");
	pShader->UseShader();
	pShader->SetUniform("uSpeed", 2.0f);
	pShader->SetUniform("uDistortion", 1);
	pShader->SetUniform("uAlphaMask", 2);
	m_pMesh->GetMaterial()->SetShader(pShader);
	return true;
}

/**
* 销毁场景节点
*/
void CSceneNodeFlame::Destroy() {
	delete m_pMesh;
}

/**
* 渲染场景节点
*/
void CSceneNodeFlame::Render() {
	m_pMesh->GetMaterial()->PassUniform("uTime", m_fTimeElapse);
	m_pMesh->Render();
}

/***
* 更新场景节点
*/
void CSceneNodeFlame::Update(float dt) {
	m_fTimeElapse += dt;
	CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
	CVector3 position = GetWorldPosition();
	float dx = pCamera->m_cPosition[0] - position[0];
	float dy = pCamera->m_cPosition[1] - position[1];
	// 使其始终朝向观察者
	if (dx != 0.0f || dy != 0.0f) {
		float angle = acosf(dy / sqrt(dx * dx + dy * dy));
		if (dx > 0.0f) angle = -angle;
		m_cOrientation.FromAxisAngle(CVector3::Z, angle);
		Transform();
	}
}