//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeWater.h"
#include "CEngine.h"
#include "CPlane.h"
#include "CSceneNodeScreen.h"
#include <cmath>

/**
* 构造函数
*/
CSceneNodeWater::CSceneNodeWater(const string& name, const string& normalMap, float depth): CSceneNode("water", name) {
	m_strNormalMap = normalMap;
	m_cPosition[2] = depth;
	m_fTimeElapse = 0.0f;
	m_pMesh = 0;
	m_pReflectTexture = 0;
	m_pUnderwaterNode = 0;
}

/**
* 初始化场景节点
*/
bool CSceneNodeWater::Init() {
	const float waterSize = 500.0f;
	const float waterColor[3] = { 0.35f, 0.72f, 0.82f };
	const unsigned char maskColor[4] = { 0x49, 0xA8, 0xB1, 0xA0 };

	m_pReflectTexture = CEngine::GetTextureManager()->Create(m_strName + "_reflect", 1024, 1024, false, true, false);
	m_pUnderwaterNode = CEngine::GetSceneManager()->GetNodeByName("__water_mask__");
	if (!m_pUnderwaterNode) {
		CTexture* mask = CEngine::GetTextureManager()->Create("__water_mask__", 1, 1, 4, maskColor, false);
		m_pUnderwaterNode = new CSceneNodeScreen("__water_mask__", "__water_mask__", 1, 1);
		CEngine::GetSceneManager()->InsertNode(m_pUnderwaterNode);
		CEngine::GetTextureManager()->Drop(mask);
	}
	CShader* pShader = CEngine::GetShaderManager()->GetShader("water");
	m_pMesh = new CMesh();
	m_pMesh->SetVertexUsage(4);
	m_pMesh->AddVertex(CVertex(-waterSize, -waterSize, 0, 0, 1, 0, 0, 1));
	m_pMesh->AddVertex(CVertex( waterSize, -waterSize, 0, 1, 1, 0, 0, 1));
	m_pMesh->AddVertex(CVertex(-waterSize,  waterSize, 0, 0, 0, 0, 0, 1));
	m_pMesh->AddVertex(CVertex( waterSize,  waterSize, 0, 1, 0, 0, 0, 1));
	m_pMesh->AddTriangle(0, 1, 2);
	m_pMesh->AddTriangle(1, 3, 2);
	m_pMesh->Create(false);
	m_pMesh->GetMaterial()->SetShader(pShader);
	m_pMesh->GetMaterial()->SetTexture(m_strNormalMap, 0);
	m_pMesh->GetMaterial()->SetTexture(m_pReflectTexture, 1);
	m_pMesh->GetMaterial()->SetRenderMode(false, true, false);
	m_pMesh->GetMaterial()->GetTexture(0)->SetWrapModeMirroredRepeat(true, true);
	m_pMesh->GetMaterial()->GetTexture(1)->SetWrapModeClampToEdge(true, true);

	CTexture* pTexture = m_pMesh->GetMaterial()->GetTexture(0);
	float normalMapW = 1.0f / static_cast<float>(pTexture->GetWidth());
	float normalMapH = 1.0f / static_cast<float>(pTexture->GetHeight());
	pShader->UseShader();
	pShader->SetUniform("uTexture[0]", 0);
	pShader->SetUniform("uTexture[1]", 1);
	pShader->SetUniform("uWaveHeight", 0.1f);
	pShader->SetUniform("uWaterScale", CVector2(normalMapW, normalMapH));
	pShader->SetUniform("uWaterColor", CVector3(waterColor));
	return true;
}

/**
* 销毁场景节点
*/
void CSceneNodeWater::Destroy() {
	CEngine::GetSceneManager()->DeleteNode("__water_mask__");
	CEngine::GetTextureManager()->Drop(m_pReflectTexture);
	delete m_pMesh;
}

/**
* 渲染场景节点
*/
void CSceneNodeWater::Render() {
	// 防止在渲染倒影贴图的时候进入
	if (!CEngine::GetGraphicsManager()->IsReflectRender()) {
		m_pMesh->GetMaterial()->PassUniform("uModelMatrix", m_cWorldMatrix);
		m_pMesh->GetMaterial()->PassUniform("uElapsedTime", m_fTimeElapse);
		m_pMesh->Render();
		// 更新水下屏幕蒙版
		UpdateUnderwaterMask();
	}
}

/**
* 更新场景节点
*/
void CSceneNodeWater::Update(float dt) {
	m_fTimeElapse += dt * 0.1f;
	m_pUnderwaterNode->m_bVisible = m_bVisible;
}

/**
* 更新水下屏幕蒙版
*/
void CSceneNodeWater::UpdateUnderwaterMask() {
	m_pUnderwaterNode->m_bVisible = false;
	CGraphicsManager* pGraphicsMgr = CEngine::GetGraphicsManager();
	CCamera* pCamera = pGraphicsMgr->GetCamera();
	CMatrix4& viewMat = pCamera->GetViewMatrix();
	CVector3 cameraUpVec = CVector3(viewMat(1, 0), viewMat(1, 1), viewMat(1, 2));
	CVector3 cameraLookVec = CVector3(-viewMat(2, 0), -viewMat(2, 1), -viewMat(2, 2));
	CVector3 nearClipCenter = pCamera->m_cPosition + cameraLookVec * pCamera->GetNearClipDistance();
	CVector3 intersectPoint;
	CVector3 intersectNormal;
	CPlane clipPlane(nearClipCenter, cameraLookVec);
	// 得到相机近裁剪面和水面的交线
	if (clipPlane.IntersectPlane(CPlane(CVector3::Z, m_cPosition[2]), intersectPoint, intersectNormal)) {
		int screen_w = pCamera->GetViewWidth();
		int screen_h = pCamera->GetViewHeight();
		intersectNormal.Normalize();
		CVector3 centerPos = intersectPoint + intersectNormal * intersectNormal.DotProduct(nearClipCenter - intersectPoint);
		CVector3 screenPos;
		pGraphicsMgr->PointToScreen(centerPos, screenPos);
		if (!pGraphicsMgr->IsLeftEyeRender()) screenPos[0] -= screen_w;
		int halfVW = screen_w >> 1;
		int halfVH = screen_h >> 1;
		float viewDistance = static_cast<float>(halfVW + halfVH);
		float distance = fabs(screenPos[0] - halfVW) + fabs(screenPos[1] - halfVH);
		if (nearClipCenter[2] < m_cPosition[2] || distance < viewDistance) {
			m_pUnderwaterNode->m_bVisible = true;
			m_pUnderwaterNode->m_cWorldMatrix.SetIdentity().Scale(viewDistance * 2.0f, viewDistance * 2.0f, 1.0f);
			// 完全在水下的情况
			if (nearClipCenter[2] < m_cPosition[2] && distance > viewDistance) {
				m_pUnderwaterNode->m_cWorldMatrix.Translate(static_cast<float>(halfVW), static_cast<float>(halfVH), 0);
			} else {
				float cosa = intersectNormal.DotProduct(cameraUpVec);
				if (cosa < -1.0f) cosa = -1.0f;
				else if (cosa > 1.0f) cosa = 1.0f;
				float angle = cameraUpVec[2] < 0? 1.5707963f + acosf(cosa): 1.5707963f - acosf(cosa);
				m_pUnderwaterNode->m_cWorldMatrix.Translate(0.0f, -viewDistance, 0.0f);
				m_pUnderwaterNode->m_cWorldMatrix.RotateZ(angle);
				m_pUnderwaterNode->m_cWorldMatrix.Translate(screenPos[0], screenPos[1], 0);
			}
		}
	}
}