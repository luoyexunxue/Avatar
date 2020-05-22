//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodePlanet.h"
#include "CGeometryCreator.h"
#include "CEngine.h"
#include <cmath>

/**
* 构造函数
*/
CSceneNodePlanet::CSceneNodePlanet(const string& name, const string& texture, const string& textureNight,
	float radius, int slices): CSceneNode("planet", name) {
	m_fRadius = radius;
	m_iTessellation = slices;
	m_strTexture = texture;
	m_strTextureNight = textureNight;
	m_pMeshSky = 0;
	m_pMeshData = 0;
}

/**
* 初始化场景节点
*/
bool CSceneNodePlanet::Init() {
	const float Kr = 0.0025f;
	const float Km = 0.001f;
	const float ESun = 20.0f;
	const float g = -0.99f;
	const float innerRadius = m_fRadius;
	const float outerRadius = m_fRadius * 1.05f;
	const CVector3 invWavelength(1.0f / powf(0.65f, 4), 1.0f / powf(0.57f, 4), 1.0f / powf(0.475f, 4));
	const float scaleDepth = 0.25f;
	const CVector3 cameraPos = m_cPosition - CVector3(0.0f, m_fRadius, 0.0f);
	const CVector3 lightPos = CVector3(1.0f, -1.0f, 1.0f).Normalize();
	const float cameraHeight = (cameraPos - m_cPosition).Length();

	m_pMeshSky = CGeometryCreator::CreateSphere(outerRadius, m_iTessellation, m_iTessellation, true);
	m_pMeshSky->GetMaterial()->SetShader("planetsky");
	m_pMeshSky->GetMaterial()->SetRenderMode(true, true, true);
	CShader* pShader = m_pMeshSky->GetMaterial()->GetShader();
	pShader->UseShader();
	pShader->SetUniform("uCameraPos", cameraPos);
	pShader->SetUniform("uLightPos", lightPos);
	pShader->SetUniform("uInvWavelength", invWavelength);
	pShader->SetUniform("uCameraHeight", cameraHeight);
	pShader->SetUniform("uCameraHeight2", cameraHeight * cameraHeight);
	pShader->SetUniform("uOuterRadius", outerRadius);
	pShader->SetUniform("uOuterRadius2", outerRadius * outerRadius);
	pShader->SetUniform("uInnerRadius", innerRadius);
	pShader->SetUniform("uKrESun", Kr * ESun);
	pShader->SetUniform("uKmESun", Km * ESun);
	pShader->SetUniform("uKr4PI", Kr * 4.0f * 3.14159f);
	pShader->SetUniform("uKm4PI", Km * 4.0f * 3.14159f);
	pShader->SetUniform("uScale", 1.0f / (outerRadius - innerRadius));
	pShader->SetUniform("uScaleDepth", scaleDepth);
	pShader->SetUniform("uScaleOverScaleDepth", 1.0f / (outerRadius - innerRadius) / scaleDepth);
	pShader->SetUniform("uG", g);
	pShader->SetUniform("uG2", g * g);

	CMesh* ground = CGeometryCreator::CreateSphere(innerRadius, m_iTessellation, m_iTessellation);
	ground->GetMaterial()->SetShader("planetground");
	ground->GetMaterial()->SetTexture(m_strTexture, 0);
	ground->GetMaterial()->SetTexture(m_strTextureNight, 1);
	pShader = ground->GetMaterial()->GetShader();
	pShader->UseShader();
	pShader->SetUniform("uTexture[0]", 0);
	pShader->SetUniform("uTexture[1]", 1);
	pShader->SetUniform("uCameraPos", cameraPos);
	pShader->SetUniform("uLightPos", lightPos);
	pShader->SetUniform("uInvWavelength", invWavelength);
	pShader->SetUniform("uCameraHeight2", cameraHeight * cameraHeight);
	pShader->SetUniform("uOuterRadius", outerRadius);
	pShader->SetUniform("uOuterRadius2", outerRadius * outerRadius);
	pShader->SetUniform("uInnerRadius", innerRadius);
	pShader->SetUniform("uKrESun", Kr * ESun);
	pShader->SetUniform("uKmESun", Km * ESun);
	pShader->SetUniform("uKr4PI", Kr * 4.0f * 3.14159f);
	pShader->SetUniform("uKm4PI", Km * 4.0f * 3.14159f);
	pShader->SetUniform("uScale", 1.0f / (outerRadius - innerRadius));
	pShader->SetUniform("uScaleDepth", scaleDepth);
	pShader->SetUniform("uScaleOverScaleDepth", 1.0f / (outerRadius - innerRadius) / scaleDepth);
	pShader->SetUniform("uNightScale", ground->GetMaterial()->GetTexture(1)->IsValid() ? 1.0f : 0.0f);

	m_pMeshData = new CMeshData();
	m_pMeshData->AddMesh(ground);
	m_cLocalBoundingBox.SetValue(CVector3(-outerRadius), CVector3(outerRadius));
	return true;
}

/**
* 销毁场景节点
*/
void CSceneNodePlanet::Destroy() {
	delete m_pMeshData;
	delete m_pMeshSky;
}

/**
* 渲染场景节点
*/
void CSceneNodePlanet::Render() {
	CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
	float cameraHeight = (pCamera->m_cPosition - m_cPosition).Length();
	// 设置着色器参数
	CMaterial* pMaterialGround = m_pMeshData->GetMesh(0)->GetMaterial();
	pMaterialGround->PassUniform("uProjMatrix", pCamera->GetProjMatrix());
	pMaterialGround->PassUniform("uViewMatrix", pCamera->GetViewMatrix());
	pMaterialGround->PassUniform("uModelMatrix", m_cWorldMatrix);
	pMaterialGround->PassUniform("uCameraPos", pCamera->m_cPosition);
	pMaterialGround->PassUniform("uCameraHeight", cameraHeight);
	pMaterialGround->PassUniform("uCameraHeight2", cameraHeight * cameraHeight);
	CMaterial* pMaterialSky = m_pMeshSky->GetMaterial();
	pMaterialSky->PassUniform("uProjMatrix", pCamera->GetProjMatrix());
	pMaterialSky->PassUniform("uViewMatrix", pCamera->GetViewMatrix());
	pMaterialSky->PassUniform("uModelMatrix", m_cWorldMatrix);
	pMaterialSky->PassUniform("uCameraPos", pCamera->m_cPosition);
	pMaterialSky->PassUniform("uCameraHeight", cameraHeight);
	pMaterialSky->PassUniform("uCameraHeight2", cameraHeight * cameraHeight);
	// 渲染地面和天空
	m_pMeshData->GetMesh(0)->Render();
	m_pMeshSky->Render();
}

/**
* 获取网格数据
*/
CMeshData* CSceneNodePlanet::GetMeshData() {
	return m_pMeshData;
}