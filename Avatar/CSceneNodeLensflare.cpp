//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeLensflare.h"
#include "CGeometryCreator.h"
#include "CEngine.h"

/**
* ���캯��
*/
CSceneNodeLensflare::CSceneNodeLensflare(const string& name): CSceneNode("lensflare", name) {
	m_pMesh = 0;
	m_cPosition.SetValue(0.0f, 100.0f, 100.0f);
	m_cSunPosition = m_cPosition;
}

/**
* ��ʼ�������ڵ�
*/
bool CSceneNodeLensflare::Init() {
	// ���ع�������
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	m_pTextureFlare = pTextureMgr->Create("system/flare.jpg");
	m_pTextureGlow = pTextureMgr->Create("system/glow.jpg");
	m_pTextureHalo = pTextureMgr->Create("system/halo.jpg");
	// ̫��
	m_sLensFlares[0].SetParameter(0, 0.5f, 0.6f, 0.6f, 0.8f, 1.0f, m_pTextureFlare);
	// �����ν���λ�õĹ���
	m_sLensFlares[1].SetParameter(0.1f, 0.05f, 0.9f, 0.6f, 0.4f, 0.5f, m_pTextureGlow);
	m_sLensFlares[2].SetParameter(0.5f, 0.15f, 0.7f, 0.6f, 0.5f, 0.5f, m_pTextureHalo);
	m_sLensFlares[3].SetParameter(0.48f, 0.15f, 0.7f, 0.6f, 0.4f, 0.5f, m_pTextureGlow);
	m_sLensFlares[4].SetParameter(0.8f, 0.08f, 0.7f, 0.6f, 0.5f, 0.5f, m_pTextureHalo);
	m_sLensFlares[5].SetParameter(0.78f, 0.07f, 0.8f, 0.5f, 0.1f, 0.5f, m_pTextureGlow);

	// ������λ�������
	m_pMesh = CGeometryCreator::CreatePlane(1, 1, 3);
	m_pMesh->GetMaterial()->SetRenderMode(true, false, true);
	m_pMesh->GetMaterial()->SetShader("lensflare");
	return true;
}

/**
* ���ٳ����ڵ�
*/
void CSceneNodeLensflare::Destroy() {
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	pTextureMgr->Drop(m_pTextureFlare);
	pTextureMgr->Drop(m_pTextureGlow);
	pTextureMgr->Drop(m_pTextureHalo);
	delete m_pMesh;
}

/**
* ��Ⱦ�����ڵ�
*/
void CSceneNodeLensflare::Render() {
	CGraphicsManager* pGraphicsMgr = CEngine::GetGraphicsManager();
	CCamera* pCamera = pGraphicsMgr->GetCamera();
	// ���̫�����Ӿ���Χ����ɼ�
	if (pCamera->GetFrustum().IsContain(m_cSunPosition)) {
		// �ڵ����
		CVector3 hitPoint;
		CRay pickRay(pCamera->m_cPosition, m_cPosition);
		if (CEngine::GetSceneManager()->GetNodeByRay(pickRay, hitPoint)) return;

		// ��ȡ��Ļ����λ��
		int screen_w = pCamera->GetViewWidth();
		int screen_h = pCamera->GetViewHeight();
		m_cCenterPos[0] = static_cast<float>(screen_w >> 1);
		m_cCenterPos[1] = static_cast<float>(screen_h >> 1);

		// ��̫���������Ļ���ĵ�λ��
		pGraphicsMgr->PointToScreen(m_cSunPosition, m_cScreenPos);
		m_cScreenPos[0] -= m_cCenterPos[0];
		m_cScreenPos[1] -= m_cCenterPos[1];
		if (!pGraphicsMgr->IsLeftEyeRender()) m_cScreenPos[0] -= screen_w;
		CVector2 screenPos(m_cScreenPos[0], m_cScreenPos[1]);

		// ��Ⱦ����
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
* ���³����ڵ�
*/
void CSceneNodeLensflare::Update(float dt) {
	// m_cPosition ��̫������������������
	m_cSunPosition = CEngine::GetGraphicsManager()->GetCamera()->m_cPosition + m_cPosition;
}