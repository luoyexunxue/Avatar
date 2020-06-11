//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPostProcessUser.h"
#include "CEngine.h"
#include "CTimer.h"
#include "CStringUtil.h"

/**
* 构造函数
*/
CPostProcessUser::CPostProcessUser(const string& name, const char* shader, const char* texture, bool cube) {
	m_strName = name;
	m_strIdentifier = "postprocess_" + name;
	m_strFragShader = shader;
	m_strTextures = texture ? texture : "";
	m_bRenderCube = cube;
}

/**
* 初始化后处理对象
*/
bool CPostProcessUser::Init(int width, int height) {
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	const char* vertShader = GetVertexShader();
	const char* fragShader = m_strFragShader.c_str();
	m_pPostProcessShader = pShaderMgr->Create(m_strIdentifier, vertShader, fragShader);
	m_pPostProcessShader->SetUniform("uTexture", 0);
	m_pPostProcessShader->SetUniform("uElapsedTime", 0.0f);
	m_pPostProcessShader->SetUniform("uResolution", CVector2(width, height));
	if (!m_bRenderCube) m_pRenderTexture = pTextureMgr->Create(m_strIdentifier, width, height, false, true, false);
	else m_pRenderTexture = pTextureMgr->Create(m_strIdentifier, width > height ? height : width, false, true, false);
	vector<string> textureArray;
	CStringUtil::Split(textureArray, m_strTextures, ",", true);
	for (size_t i = 0; i < textureArray.size(); i++) {
		if (!textureArray[i].empty()) {
			string uniform = CStringUtil::Format("uSamples[%d]", i);
			m_vecTextures.push_back(pTextureMgr->Create(textureArray[i]));
			m_pPostProcessShader->SetUniform(uniform, static_cast<int>(i + 1));
		}
	}
	return m_pPostProcessShader->IsValid();
}

/**
* 渲染区域大小改变
*/
void CPostProcessUser::Resize(int width, int height) {
	CEngine::GetTextureManager()->Resize(m_pRenderTexture, width, height);
	m_pPostProcessShader->UseShader();
	m_pPostProcessShader->SetUniform("uResolution", CVector2(width, height));
}

/**
* 销毁后处理对象
*/
void CPostProcessUser::Destroy() {
	CEngine::GetTextureManager()->Drop(m_pRenderTexture);
	CEngine::GetShaderManager()->Drop(m_pPostProcessShader);
	for (size_t i = 0; i < m_vecTextures.size(); i++) {
		CEngine::GetTextureManager()->Drop(m_vecTextures[i]);
	}
}

/**
* 应用当前后处理
*/
void CPostProcessUser::Apply(CTexture* target, CMesh* mesh) {
	CEngine::GetGraphicsManager()->SetRenderTarget(target, 0, true, true, true);
	m_pPostProcessShader->UseShader();
	m_pPostProcessShader->SetUniform("uElapsedTime", CTimer::Reset(m_strIdentifier, false));
	if (!m_vecTextures.empty()) {
		for (size_t i = m_vecTextures.size(); i > 0; i--) m_vecTextures[i - 1]->UseTexture(i);
		m_pRenderTexture->UseTexture(0);
	} else m_pRenderTexture->UseTexture();
	mesh->Render(false);
}