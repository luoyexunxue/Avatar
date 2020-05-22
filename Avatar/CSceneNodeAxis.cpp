//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeAxis.h"
#include "CGeometryCreator.h"
#include "CEngine.h"
#include <cmath>

/**
* ���캯��
*/
CSceneNodeAxis::CSceneNodeAxis(const string& name): CSceneNode("axis", name) {
	m_pMesh = 0;
}

/**
* ��ʼ�������ڵ�
*/
bool CSceneNodeAxis::Init() {
	const float r1 = 0.01f;
	const float h1 = 0.08f;
	const float r2 = 0.02f;
	const float h2 = 0.08f;
	// �������ͷ
	CMesh* xAxis = CGeometryCreator::CreateArrow(r1, r2, h1, h2, 16, false, CColor::Red);
	CMesh* yAxis = CGeometryCreator::CreateArrow(r1, r2, h1, h2, 16, false, CColor::Green);
	CMesh* zAxis = CGeometryCreator::CreateArrow(r1, r2, h1, h2, 16, false, CColor::Blue);
	xAxis->Transform(CMatrix4().RotateY(1.57f));
	yAxis->Transform(CMatrix4().RotateX(-1.57f));
	m_pMesh = CGeometryCreator::CreateSphere(r2, 16, 16);
	m_pMesh->Append(xAxis);
	m_pMesh->Append(yAxis);
	m_pMesh->Append(zAxis);
	m_pMesh->Update(0);
	m_pMesh->GetMaterial()->SetTexture("white");
	delete xAxis;
	delete yAxis;
	delete zAxis;
	return true;
}

/**
* ���ٳ����ڵ�
*/
void CSceneNodeAxis::Destroy() {
	delete m_pMesh;
}

/**
* ��Ⱦ�����ڵ�
*/
void CSceneNodeAxis::Render() {
	m_pMesh->Render();
}

/**
* ���³����ڵ�
*/
void CSceneNodeAxis::Update(float dt) {
	// ������ʾ�����½�
	CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
	float fov = pCamera->GetFieldOfView() * 0.017453f;
	float r = tanf(fov) * 0.2f;
	float d = r / pCamera->GetAspectRatio();
	CVector3 vec_r = pCamera->m_cLookVector.CrossProduct(pCamera->m_cUpVector) * r;
	CVector3 vec_u = pCamera->m_cUpVector * d;
	m_cPosition = pCamera->m_cPosition + pCamera->m_cLookVector;
	m_cPosition += vec_r - vec_u;
	Transform();
}