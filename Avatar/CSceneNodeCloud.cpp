//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeCloud.h"
#include "CGeometryCreator.h"
#include "CEngine.h"
#include "CVector3.h"

/**
* ���캯��
*/
CSceneNodeCloud::CSceneNodeCloud(const string& name, const CColor& skyColor, const CColor& cloudColor, float cloudSize)
	: CSceneNode("cloud", name) {
	m_fTimeElapse = 0.0f;
	m_pMesh = 0;
	m_cPosition[2] = 200.0f;
	m_cSkyColor.SetValue(skyColor);
	m_cCloudColor.SetValue(cloudColor);
	m_fCloudSize = cloudSize;
}

/**
* ��ʼ�������ڵ�
*/
bool CSceneNodeCloud::Init() {
	m_pMesh = CGeometryCreator::CreatePlane(2000.0f, 2000.0f, 3, true);
	CShader* pShader = CEngine::GetShaderManager()->GetShader("cloud");
	pShader->UseShader();
	pShader->SetUniform("uSkyColor", m_cSkyColor.m_fValue, 3, 1);
	pShader->SetUniform("uCloudColor", m_cCloudColor.m_fValue, 3, 1);
	pShader->SetUniform("uCloudSize", m_fCloudSize);
	m_pMesh->GetMaterial()->SetShader(pShader);
	return true;
}

/**
* ���ٳ����ڵ�
*/
void CSceneNodeCloud::Destroy() {
	delete m_pMesh;
}

/**
* ��Ⱦ�����ڵ�
*/
void CSceneNodeCloud::Render() {
	m_pMesh->GetMaterial()->PassUniform("uTime", m_fTimeElapse);
	m_pMesh->Render();
}

/**
* �����Ʋ�λ�ã����������������
*/
void CSceneNodeCloud::Update(float dt) {
	m_fTimeElapse += dt * 0.1f;
	CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
	m_cPosition[0] = pCamera->m_cPosition[0];
	m_cPosition[1] = pCamera->m_cPosition[1];
	Transform();
}