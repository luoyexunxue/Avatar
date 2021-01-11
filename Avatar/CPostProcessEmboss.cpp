//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPostProcessEmboss.h"
#include "CEngine.h"

/**
* 初始化后处理对象
*/
bool CPostProcessEmboss::Init(int width, int height) {
	const char* emboss = "\
		uniform sampler2D uTexture;\
		uniform vec2 uTextureSize;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		void main()\
		{\
			vec2 onePixel = vec2(1.0 / uTextureSize.x, 1.0 / uTextureSize.y);\
			vec4 color = vec4(0.5, 0.5, 0.5, 1.0);\
			color -= texture(uTexture, vTexCoord - onePixel) * 4.0;\
			color += texture(uTexture, vTexCoord + onePixel) * 4.0;\
			color.rgb = vec3((color.r + color.g + color.b) / 3.0);\
			fragColor = vec4(color.rgb, 1.0);\
		}";
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	m_pPostProcessShader = pShaderMgr->Create("postprocess_emboss", GetVertexShader(), emboss);
	m_pPostProcessShader->SetUniform("uTexture", 0);
	m_pPostProcessShader->SetUniform("uTextureSize", CVector2(width, height));
	m_pRenderTexture = pTextureMgr->Create("postprocess_emboss", width, height, false, true, false);
	return m_pPostProcessShader->IsValid();
}

/**
* 渲染区域大小改变
*/
void CPostProcessEmboss::Resize(int width, int height) {
	CEngine::GetTextureManager()->Resize(m_pRenderTexture, width, height);
	m_pPostProcessShader->UseShader();
	m_pPostProcessShader->SetUniform("uTextureSize", CVector2(width, height));
}

/**
* 销毁后处理对象
*/
void CPostProcessEmboss::Destroy() {
	CEngine::GetTextureManager()->Drop(m_pRenderTexture);
	CEngine::GetShaderManager()->Drop(m_pPostProcessShader);
}