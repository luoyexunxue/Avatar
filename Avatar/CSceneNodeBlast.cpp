//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeBlast.h"
#include "CGeometryCreator.h"
#include "CEngine.h"

/**
* 构造函数
*/
CSceneNodeBlast::CSceneNodeBlast(const string& name, const string& texture, int row, int column)
	: CSceneNode("blast", name) {
	m_bBlast = false;
	m_pMesh = 0;
	m_strTexture = texture;
	m_fTimeElapse = 0.0f;
	m_fDuration = 1.0f;
	m_iFrameRow = 0;
	m_iFrameCol = 0;
	m_iTotalRow = row;
	m_iTotalCol = column;
}

/**
* 初始化场景节点
*/
bool CSceneNodeBlast::Init() {
	CShader* pShader = CEngine::GetShaderManager()->GetShader("blast");
	pShader->UseShader();
	pShader->SetUniform("uTotalRow", static_cast<float>(m_iTotalRow));
	pShader->SetUniform("uTotalCol", static_cast<float>(m_iTotalCol));
	m_pMesh = CGeometryCreator::CreatePlane(1.0f, 1.0f, 3);
	m_pMesh->GetMaterial()->SetShader(pShader);
	m_pMesh->GetMaterial()->SetTexture(m_strTexture);
	m_pMesh->GetMaterial()->SetRenderMode(true, true, true);
	return true;
}

/**
* 销毁场景节点
*/
void CSceneNodeBlast::Destroy() {
	delete m_pMesh;
}

/**
* 渲染场景节点
*/
void CSceneNodeBlast::Render() {
	if (m_bBlast) {
		m_pMesh->GetMaterial()->PassUniform("uFrameRow", static_cast<float>(m_iFrameRow));
		m_pMesh->GetMaterial()->PassUniform("uFrameCol", static_cast<float>(m_iFrameCol));
		m_pMesh->Render();
	}
}

/***
* 更新场景节点
*/
void CSceneNodeBlast::Update(float dt) {
	if (m_bBlast) {
		m_fTimeElapse += dt;
		if (m_fTimeElapse > m_fDuration) m_bBlast = false;
		int index = static_cast<int>(m_iTotalRow * m_iTotalCol * (m_fTimeElapse / m_fDuration));
		m_iFrameRow = index / m_iTotalCol;
		m_iFrameCol = index % m_iTotalCol;
		// 使其始终朝向观察者
		CMatrix4 viewMat = CEngine::GetGraphicsManager()->GetCamera()->GetViewMatrix();
		m_cOrientation.FromMatrix(viewMat.Transpose());
		Transform();
	}
}

/**
* 爆炸初始化
*/
void CSceneNodeBlast::InitBlast(float duration, float size) {
	m_fDuration = duration;
	m_cScale.SetValue(size, size, 1.0f);
	m_bBlast = true;
	m_fTimeElapse = 0.0f;
}