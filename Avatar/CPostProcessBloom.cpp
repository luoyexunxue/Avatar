//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPostProcessBloom.h"
#include "CEngine.h"
#include "CStringUtil.h"

/**
* 初始化后处理对象
*/
bool CPostProcessBloom::Init(int width, int height) {
	// Bloom
	const char* fragShader = "\
		uniform sampler2D uTexture;\
		uniform sampler2D uBlurTexture;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		void main()\
		{\
			vec3 color = texture(uTexture, vTexCoord).rgb;\
			vec3 glow = texture(uBlurTexture, vTexCoord).rgb;\
			fragColor = vec4(color * 0.5 + glow * 0.5, 1.0);\
		}";
	// 模糊处理
	const char* blurShader = "\
		uniform sampler2D uTexture;\
		uniform vec2 uScreenSize;\
		uniform vec2 uDirection;\
		uniform float uWeights[11];\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		void main()\
		{\
			vec2 texelStep = vec2(1.0 / uScreenSize.x, 1.0 / uScreenSize.y) * uDirection;\
			vec2 start = vTexCoord - 5.0 * texelStep;\
			vec4 baseColor = vec4(0.0, 0.0, 0.0, 0.0);\
			vec2 texelOffset = vec2(0.0, 0.0);\
			for (int i = 0; i < 11; i++)\
			{\
				baseColor += texture(uTexture, start + texelOffset) * uWeights[i];\
				texelOffset += texelStep;\
			}\
			fragColor = vec4(baseColor.rgb, 1.0);\
		}";
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	m_pPostProcessShader = pShaderMgr->Create("postprocess_bloom", GetVertexShader(), fragShader);
	m_pPostProcessShader->SetUniform("uTexture", 0);
	m_pPostProcessShader->SetUniform("uBlurTexture", 1);
	m_pBlurShader = pShaderMgr->Create("postprocess_bloom_blur", GetVertexShader(), blurShader);
	m_pBlurShader->SetUniform("uTexture", 0);
	m_pBlurShader->SetUniform("uScreenSize", CVector2(width, height));
	SetGaussKernel(m_pBlurShader, 11, 3.0f);
	m_pRenderTexture = pTextureMgr->Create("postprocess_bloom", width, height, false, true, false);
	m_pHBlurTexture = pTextureMgr->Create("postprocess_bloom_blur_h", width >> 1, height >> 1, false, false, false);
	m_pVBlurTexture = pTextureMgr->Create("postprocess_bloom_blur_v", width >> 1, height >> 1, false, false, false);
	m_pHBlurTexture->SetWrapModeClampToEdge(true, true);
	m_pVBlurTexture->SetWrapModeClampToEdge(true, true);
	return m_pPostProcessShader->IsValid() && m_pBlurShader->IsValid();
}

/**
* 渲染区域大小改变
*/
void CPostProcessBloom::Resize(int width, int height) {
	const int hwidth = width >> 1;
	const int hheight = height >> 1;
	CEngine::GetTextureManager()->Resize(m_pRenderTexture, width, height);
	CEngine::GetTextureManager()->Resize(m_pHBlurTexture, hwidth, hheight);
	CEngine::GetTextureManager()->Resize(m_pVBlurTexture, hwidth, hheight);
	m_pBlurShader->UseShader();
	m_pBlurShader->SetUniform("uScreenSize", CVector2(hwidth, hheight));
}

/**
* 销毁后处理对象
*/
void CPostProcessBloom::Destroy() {
	CEngine::GetTextureManager()->Drop(m_pRenderTexture);
	CEngine::GetTextureManager()->Drop(m_pHBlurTexture);
	CEngine::GetTextureManager()->Drop(m_pVBlurTexture);
	CEngine::GetShaderManager()->Drop(m_pPostProcessShader);
	CEngine::GetShaderManager()->Drop(m_pBlurShader);
}

/**
* 应用当前后处理
*/
void CPostProcessBloom::Apply(CTexture* target, CMesh* mesh) {
	CGraphicsManager* pGraphicsMgr = CEngine::GetGraphicsManager();
	// 水平模糊
	pGraphicsMgr->SetRenderTarget(m_pHBlurTexture, 0, false, false);
	m_pBlurShader->UseShader();
	m_pBlurShader->SetUniform("uDirection", CVector2(3.0f, 0.0f));
	m_pRenderTexture->UseTexture();
	mesh->Render(false);
	// 垂直模糊
	pGraphicsMgr->SetRenderTarget(m_pVBlurTexture, 0, false, false);
	m_pBlurShader->UseShader();
	m_pBlurShader->SetUniform("uDirection", CVector2(0.0f, 3.0f));
	m_pHBlurTexture->UseTexture();
	mesh->Render(false);
	// 合成最终场景
	pGraphicsMgr->SetRenderTarget(target, 0, false, false);
	m_pPostProcessShader->UseShader();
	m_pVBlurTexture->UseTexture(1);
	m_pRenderTexture->UseTexture(0);
	mesh->Render(false);
}

/**
* 计算高斯卷积核
*/
void CPostProcessBloom::SetGaussKernel(CShader* blurShader, int kernelSize, float sigma) {
	float kernel[64];
	const int padding = (kernelSize - 1) / 2;
	const float sig2 = -1.0f / (2.0f * sigma * sigma);
	float sum = 0.0f;
	kernel[padding] = 1.0f;
	for (int i = 1; i <= padding; i++) kernel[padding - i] = kernel[i + padding] = expf(i * i * sig2);
	for (int i = 0; i < kernelSize; i++) sum += kernel[i];
	for (int i = 0; i < kernelSize; i++) kernel[i] /= sum;
	for (int i = 0; i < kernelSize; i++) blurShader->SetUniform(CStringUtil::Format("uWeights[%d]", i), kernel[i]);
}