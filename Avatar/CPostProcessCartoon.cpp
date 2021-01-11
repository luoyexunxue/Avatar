//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPostProcessCartoon.h"
#include "CEngine.h"

/**
* 初始化后处理对象
*/
bool CPostProcessCartoon::Init(int width, int height) {
	const char* cartoon = "\
		uniform sampler2D uTexture;\
		uniform vec3 uBaseColor;\
		uniform float uNumShades;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		void main()\
		{\
			float luminance = dot(texture(uTexture, vTexCoord).rgb, vec3(0.3, 0.59, 0.11));\
			float intensity = ceil(luminance * uNumShades) / uNumShades;\
			fragColor = vec4(uBaseColor * vec3(intensity, intensity, intensity), 1.0);\
		}";
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	m_pPostProcessShader = pShaderMgr->Create("postprocess_cartoon", GetVertexShader(), cartoon);
	m_pPostProcessShader->SetUniform("uTexture", 0);
	m_pPostProcessShader->SetUniform("uBaseColor", CVector3(1.0f, 0.9f, 0.9f));
	m_pPostProcessShader->SetUniform("uNumShades", 4.0f);
	m_pRenderTexture = pTextureMgr->Create("postprocess_cartoon", width, height, false, true, false);
	return m_pPostProcessShader->IsValid();
}

/**
* 渲染区域大小改变
*/
void CPostProcessCartoon::Resize(int width, int height) {
	CEngine::GetTextureManager()->Resize(m_pRenderTexture, width, height);
}

/**
* 销毁后处理对象
*/
void CPostProcessCartoon::Destroy() {
	CEngine::GetTextureManager()->Drop(m_pRenderTexture);
	CEngine::GetShaderManager()->Drop(m_pPostProcessShader);
}