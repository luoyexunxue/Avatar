//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPostProcessBlur.h"
#include "CEngine.h"
#include <cmath>

/**
* 初始化后处理对象
*/
bool CPostProcessBlur::Init(int width, int height) {
	const char* blur = "\
		uniform sampler2D uTexture;\
		uniform vec2 uScreenSize;\
		uniform vec2 uDirection;\
		uniform int uKernelPadding;\
		uniform int uKernelSize;\
		uniform float uWeights[21];\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		void main()\
		{\
			vec2 texelSize = vec2(1.0 / uScreenSize.x, 1.0 / uScreenSize.y);\
			vec2 texelStep = texelSize * uDirection;\
			vec2 start = vTexCoord - texelStep * vec2(uKernelPadding, uKernelPadding);\
			vec4 baseColor = vec4(0.0, 0.0, 0.0, 0.0);\
			vec2 texelOffset = vec2(0.0, 0.0);\
			for (int i = 0; i < uKernelSize; i++)\
			{\
				baseColor += texture(uTexture, start + texelOffset) * uWeights[i];\
				texelOffset += texelStep;\
			}\
			fragColor = vec4(baseColor.rgb, 1.0);\
		}";
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	m_pPostProcessShader = pShaderMgr->Create("postprocess_blur", GetVertexShader(), blur);
	m_pPostProcessShader->SetUniform("uTexture", 0);
	m_pPostProcessShader->SetUniform("uScreenSize", CVector2(width, height));
	m_pPostProcessShader->SetUniform("uDirection", CVector2(0.0f, 0.0f));
	m_pRenderTexture = pTextureMgr->Create("postprocess_blur", width, height, false, true, false);
	m_pBlurTexture = pTextureMgr->Create("postprocess_blur_dir", width, height, false, false, false);
	SetGaussKernel(7, 2.5f);
	return m_pPostProcessShader->IsValid();
}

/**
* 渲染区域大小改变
*/
void CPostProcessBlur::Resize(int width, int height) {
	CEngine::GetTextureManager()->Resize(m_pRenderTexture, width, height);
	CEngine::GetTextureManager()->Resize(m_pBlurTexture, width, height);
	m_pPostProcessShader->UseShader();
	m_pPostProcessShader->SetUniform("uScreenSize", CVector2(width, height));
}

/**
* 销毁后处理对象
*/
void CPostProcessBlur::Destroy() {
	CEngine::GetTextureManager()->Drop(m_pRenderTexture);
	CEngine::GetTextureManager()->Drop(m_pBlurTexture);
	CEngine::GetShaderManager()->Drop(m_pPostProcessShader);
}

/**
* 应用当前后处理
*/
void CPostProcessBlur::Apply(CTexture* target, CMesh* mesh) {
	CGraphicsManager* pGraphicsMgr = CEngine::GetGraphicsManager();
	// 水平模糊
	pGraphicsMgr->SetRenderTarget(m_pBlurTexture, 0, false, false);
	m_pPostProcessShader->UseShader();
	m_pPostProcessShader->SetUniform("uDirection", CVector2(3.0f, 0.0f));
	m_pRenderTexture->UseTexture();
	mesh->Render(false);
	// 垂直模糊
	pGraphicsMgr->SetRenderTarget(target, 0, false, false);
	m_pPostProcessShader->UseShader();
	m_pPostProcessShader->SetUniform("uDirection", CVector2(0.0f, 3.0f));
	m_pBlurTexture->UseTexture();
	mesh->Render(false);
}

/**
* 计算高斯卷积核
*/
void CPostProcessBlur::SetGaussKernel(int size, float sigma) {
	if (size > 21) size = 21;
	float kernel[21];
	const int padding = (size - 1) / 2;
	const float sig2 = -1.0f / (2.0f * sigma * sigma);
	float sum = 0.0f;
	kernel[padding] = 1.0f;
	for (int i = 1; i <= padding; i++) {
		kernel[padding + i] = expf(i * i * sig2);
		kernel[padding - i] = kernel[padding + i];
	}
	for (int i = 0; i < size; i++) sum += kernel[i];
	for (int i = 0; i < size; i++) kernel[i] /= sum;
	m_pPostProcessShader->UseShader();
	m_pPostProcessShader->SetUniform("uKernelPadding", padding);
	m_pPostProcessShader->SetUniform("uKernelSize", size);
	m_pPostProcessShader->SetUniform("uWeights", kernel, 1, size);
}