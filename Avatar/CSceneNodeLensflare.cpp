//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeLensflare.h"
#include "CGeometryCreator.h"
#include "CEngine.h"

/**
* 构造函数
*/
CSceneNodeLensflare::CSceneNodeLensflare(const string& name): CSceneNode("lensflare", name) {
	m_pMesh = 0;
	m_cPosition.SetValue(0.0f, 100.0f, 100.0f);
	m_cSunPosition = m_cPosition;
}

/**
* 初始化场景节点
*/
bool CSceneNodeLensflare::Init() {
	// 加载光晕纹理
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	m_pTextureFlare = pTextureMgr->Create("system/flare.jpg");
	m_pTextureGlow = pTextureMgr->Create("system/glow.jpg");
	m_pTextureHalo = pTextureMgr->Create("system/halo.jpg");
	// 太阳
	m_sLensFlares[0].SetParameter(0, 0.5f, 0.6f, 0.6f, 0.8f, 1.0f, m_pTextureFlare);
	// 到光晕结束位置的光晕
	m_sLensFlares[1].SetParameter(0.1f, 0.05f, 0.9f, 0.6f, 0.4f, 0.5f, m_pTextureGlow);
	m_sLensFlares[2].SetParameter(0.5f, 0.15f, 0.7f, 0.6f, 0.5f, 0.5f, m_pTextureHalo);
	m_sLensFlares[3].SetParameter(0.48f, 0.15f, 0.7f, 0.6f, 0.4f, 0.5f, m_pTextureGlow);
	m_sLensFlares[4].SetParameter(0.8f, 0.08f, 0.7f, 0.6f, 0.5f, 0.5f, m_pTextureHalo);
	m_sLensFlares[5].SetParameter(0.78f, 0.07f, 0.8f, 0.5f, 0.1f, 0.5f, m_pTextureGlow);

	// 创建单位网格对象
	m_pMesh = CGeometryCreator::CreatePlane(1, 1, 3);
	m_pMesh->GetMaterial()->SetRenderMode(true, false, true);
	m_pMesh->GetMaterial()->SetShader("lensflare");
	return true;
}

/**
* 销毁场景节点
*/
void CSceneNodeLensflare::Destroy() {
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	pTextureMgr->Drop(m_pTextureFlare);
	pTextureMgr->Drop(m_pTextureGlow);
	pTextureMgr->Drop(m_pTextureHalo);
	delete m_pMesh;
}

/**
* 渲染场景节点
*/
void CSceneNodeLensflare::Render() {
	CGraphicsManager* pGraphicsMgr = CEngine::GetGraphicsManager();
	CCamera* pCamera = pGraphicsMgr->GetCamera();
	// 如果太阳在视景范围内则可见
	if (pCamera->GetFrustum().IsContain(m_cSunPosition)) {
		// 遮挡检测
		CVector3 hitPoint;
		CRay pickRay(pCamera->m_cPosition, m_cPosition);
		if (CEngine::GetSceneManager()->GetNodeByRay(pickRay, hitPoint)) return;

		// 获取屏幕中心位置
		float screen_w = pCamera->GetViewWidth();
		float screen_h = pCamera->GetViewHeight();
		m_cCenterPos[0] = screen_w * 0.5f;
		m_cCenterPos[1] = screen_h * 0.5f;

		// 求太阳相对于屏幕中心的位置
		pGraphicsMgr->PointToScreen(m_cSunPosition, m_cScreenPos);
		m_cScreenPos[0] -= m_cCenterPos[0];
		m_cScreenPos[1] -= m_cCenterPos[1];
		if (!pGraphicsMgr->IsLeftEyeRender()) m_cScreenPos[0] -= screen_w;
		CVector2 screenPos(m_cScreenPos[0], m_cScreenPos[1]);

		// 渲染光晕
		CMaterial* pMaterial = m_pMesh->GetMaterial();
		pMaterial->PassUniform("uCenterPos", m_cCenterPos);
		for (int i = 0; i < 6; i++) {
			SLensFlare* pLensFlare = &m_sLensFlares[i];
			pMaterial->PassUniform("uScreenPos", screenPos * (1.0f - pLensFlare->offset));
			pMaterial->PassUniform("uScale", pLensFlare->size);
			pMaterial->PassUniform("uColor", pLensFlare->color, 4);
			pLensFlare->texture->UseTexture();
			m_pMesh->Render();
		}
	}
}

/**
* 更新场景节点
*/
void CSceneNodeLensflare::Update(float dt) {
	// m_cPosition 是太阳相对于摄像机的向量
	m_cSunPosition = CEngine::GetGraphicsManager()->GetCamera()->m_cPosition + m_cPosition;
}