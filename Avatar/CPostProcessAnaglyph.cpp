//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPostProcessAnaglyph.h"
#include "CEngine.h"

/**
* 初始化后处理对象
*/
bool CPostProcessAnaglyph::Init(int width, int height) {
	const char* fragShader = "\
		uniform sampler2D uTexture;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		void main()\
		{\
			vec2 texCoord = vec2(vTexCoord.x * 0.5, vTexCoord.y);\
			vec4 left = texture(uTexture, texCoord);\
			vec4 right = texture(uTexture, texCoord + vec2(0.5, 0.0));\
			fragColor = vec4(left.r, right.g, right.b, 1.0);\
		}";
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	m_pPostProcessShader = pShaderMgr->Create("postprocess_anaglyph", GetVertexShader(), fragShader);
	m_pPostProcessShader->SetUniform("uTexture", 0);
	m_pRenderTexture = pTextureMgr->Create("postprocess_anaglyph", width << 1, height, false, true, false);
	return m_pPostProcessShader->IsValid();
}

/**
* 渲染区域大小改变
*/
void CPostProcessAnaglyph::Resize(int width, int height) {
	CEngine::GetTextureManager()->Resize(m_pRenderTexture, width << 1, height);
}

/**
* 销毁后处理对象
*/
void CPostProcessAnaglyph::Destroy() {
	CEngine::GetTextureManager()->Drop(m_pRenderTexture);
	CEngine::GetShaderManager()->Drop(m_pPostProcessShader);
}