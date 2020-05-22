//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPostProcessBlur.h"
#include "CEngine.h"

/**
* 初始化后处理对象
*/
bool CPostProcessBlur::Init(int width, int height) {
	const char* fragShader = "\
		uniform sampler2D uTexture;\
		uniform vec2 uScreenSize;\
		uniform vec2 uDirection;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		void main()\
		{\
			float weights[7];\
			weights[0] = 0.05;\
			weights[1] = 0.1;\
			weights[2] = 0.2;\
			weights[3] = 0.3;\
			weights[4] = 0.2;\
			weights[5] = 0.1;\
			weights[6] = 0.05;\
			vec2 texelSize = vec2(1.0 / uScreenSize.x, 1.0 / uScreenSize.y);\
			vec2 texelStep = texelSize * uDirection;\
			vec2 start = vTexCoord - 3.0 * texelStep;\
			vec4 baseColor = vec4(0.0, 0.0, 0.0, 0.0);\
			vec2 texelOffset = vec2(0.0, 0.0);\
			for (int i = 0; i < 7; i++)\
			{\
				baseColor += texture(uTexture, start + texelOffset) * weights[i];\
				texelOffset += texelStep;\
			}\
			fragColor = vec4(baseColor.rgb, 1.0);\
		}";
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	m_pPostProcessShader = pShaderMgr->Create("postprocess_blur", GetVertexShader(), fragShader);
	m_pPostProcessShader->SetUniform("uTexture", 0);
	m_pPostProcessShader->SetUniform("uScreenSize", CVector2(width, height));
	m_pPostProcessShader->SetUniform("uDirection", CVector2(2.0f, 2.0f));
	m_pRenderTexture = pTextureMgr->Create("postprocess_blur", width, height, false, true, false);
	return m_pPostProcessShader->IsValid();
}

/**
* 渲染区域大小改变
*/
void CPostProcessBlur::Resize(int width, int height) {
	CEngine::GetTextureManager()->Resize(m_pRenderTexture, width, height);
	m_pPostProcessShader->UseShader();
	m_pPostProcessShader->SetUniform("uScreenSize", CVector2(width, height));
}

/**
* 销毁后处理对象
*/
void CPostProcessBlur::Destroy() {
	CEngine::GetTextureManager()->Drop(m_pRenderTexture);
	CEngine::GetShaderManager()->Drop(m_pPostProcessShader);
}