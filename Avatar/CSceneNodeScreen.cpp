//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeScreen.h"
#include "CGeometryCreator.h"
#include "CEngine.h"

/**
* 构造函数
*/
CSceneNodeScreen::CSceneNodeScreen(const string& name, const string& texture, int width, int height)
	: CSceneNode("screen", name) {
	m_strTexture = texture;
	m_iWidth = width;
	m_iHeight = height;
	m_pMeshData = 0;
}

/**
* 初始化场景节点
*/
bool CSceneNodeScreen::Init() {
	CTexture* pTexture = CEngine::GetTextureManager()->Create(m_strTexture);
	// 宽高为0表示以图片为准，为负数表示以屏幕为准
	int texture_w = pTexture->GetWidth();
	int texture_h = pTexture->GetHeight();
	if (!pTexture->IsValid()) {
		m_iWidth = 0;
		m_iHeight = 0;
	} else if (m_iWidth == 0 && m_iHeight == 0) {
		m_iWidth = texture_w;
		m_iHeight = texture_h;
	} else if (m_iWidth < 0 || m_iHeight < 0) {
		int screen_w;
		int screen_h;
		CEngine::GetGraphicsManager()->GetWindowSize(&screen_w, &screen_h);
		int ratio1 = screen_w * texture_h;
		int ratio2 = screen_h * texture_w;
		if (m_iWidth < 0 && m_iHeight < 0) {
			if (ratio1 < ratio2) {
				m_iWidth = screen_w;
				m_iHeight = ratio1 / texture_w;
			} else {
				m_iHeight = screen_h;
				m_iWidth = ratio2 / texture_h;
			}
		} else if (m_iWidth < 0) {
			m_iWidth = screen_w;
			if (m_iHeight == 0) m_iHeight = ratio1 / texture_w;
		} else if (m_iHeight < 0) {
			m_iHeight = screen_h;
			if (m_iWidth == 0) m_iWidth = ratio2 / texture_h;
		}
	}
	m_pMeshData = new CMeshData();
	m_pMeshData->AddMesh(CGeometryCreator::CreatePlane(static_cast<float>(m_iWidth), static_cast<float>(m_iHeight), 3));
	m_pMeshData->GetMesh(0)->GetMaterial()->SetTexture(pTexture);
	m_pMeshData->GetMesh(0)->GetMaterial()->GetTexture()->SetWrapModeClampToEdge(true, true);
	CEngine::GetTextureManager()->Drop(pTexture);
	return true;
}

/**
* 销毁场景节点
*/
void CSceneNodeScreen::Destroy() {
	delete m_pMeshData;
}

/**
* 渲染场景节点
*/
void CSceneNodeScreen::Render() {
	m_pMeshData->GetMesh(0)->Render();
}

/**
* 获取网格数据
*/
CMeshData* CSceneNodeScreen::GetMeshData() {
	return m_pMeshData;
}