//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPostProcessGray.h"
#include "CEngine.h"

/**
* 初始化后处理对象
*/
bool CPostProcessGray::Init(int width, int height) {
	const char* gray = "\
		uniform sampler2D uTexture;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		void main() \
		{\
			float luminance = dot(texture(uTexture, vTexCoord).rgb, vec3(0.3, 0.59, 0.11));\
			fragColor = vec4(luminance, luminance, luminance, 1.0);\
		}";
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	m_pPostProcessShader = pShaderMgr->Create("postprocess_gray", GetVertexShader(), gray);
	m_pPostProcessShader->SetUniform("uTexture", 0);
	m_pRenderTexture = pTextureMgr->Create("postprocess_gray", width, height, false, true, false);
	return m_pPostProcessShader->IsValid();
}

/**
* 渲染区域大小改变
*/
void CPostProcessGray::Resize(int width, int height) {
	CEngine::GetTextureManager()->Resize(m_pRenderTexture, width, height);
}

/**
* 销毁后处理对象
*/
void CPostProcessGray::Destroy() {
	CEngine::GetTextureManager()->Drop(m_pRenderTexture);
	CEngine::GetShaderManager()->Drop(m_pPostProcessShader);
}