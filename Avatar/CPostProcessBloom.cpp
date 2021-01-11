//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPostProcessBloom.h"
#include "CEngine.h"
#include <cmath>

/**
* 初始化后处理对象
*/
bool CPostProcessBloom::Init(int width, int height) {
	// 提取高亮度部分
	const char* bright = "\
		uniform sampler2D uTexture;\
		uniform float uThreshold;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		void main()\
		{\
			vec3 color = texture(uTexture, vTexCoord).rgb;\
			float luminance = dot(color, vec3(0.3, 0.59, 0.11));\
			if (luminance < uThreshold) color = vec3(0.0, 0.0, 0.0);\
			fragColor = vec4(color, 1.0);\
		}";
	// 高斯模糊处理
	const char* blur = "\
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
	// 合成辉光及原场景
	const char* combine = "\
		uniform sampler2D uTexture;\
		uniform sampler2D uBlurTexture;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		void main()\
		{\
			vec3 color = texture(uTexture, vTexCoord).rgb;\
			vec3 glow = texture(uBlurTexture, vTexCoord).rgb;\
			fragColor = vec4(color + glow, 1.0);\
		}";
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	m_pHighlightShader = pShaderMgr->Create("postprocess_bloom_bright", GetVertexShader(), bright);
	m_pHighlightShader->SetUniform("uTexture", 0);
	m_pHighlightShader->SetUniform("uThreshold", 0.3f);
	m_pGaussBlurShader = pShaderMgr->Create("postprocess_bloom_blur", GetVertexShader(), blur);
	m_pGaussBlurShader->SetUniform("uTexture", 0);
	m_pGaussBlurShader->SetUniform("uScreenSize", CVector2(width, height));
	m_pPostProcessShader = pShaderMgr->Create("postprocess_bloom", GetVertexShader(), combine);
	m_pPostProcessShader->SetUniform("uTexture", 0);
	m_pPostProcessShader->SetUniform("uBlurTexture", 1);
	m_pRenderTexture = pTextureMgr->Create("postprocess_bloom", width, height, false, true, false);
	m_pHBlurTexture = pTextureMgr->Create("postprocess_bloom_blur_h", width >> 1, height >> 1, false, false, false);
	m_pVBlurTexture = pTextureMgr->Create("postprocess_bloom_blur_v", width >> 1, height >> 1, false, false, false);
	m_pHBlurTexture->SetWrapModeClampToEdge(true, true);
	m_pVBlurTexture->SetWrapModeClampToEdge(true, true);
	SetGaussKernel(11, 3.0f);
	return m_pHighlightShader->IsValid() && m_pGaussBlurShader->IsValid() && m_pPostProcessShader->IsValid();
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
	m_pGaussBlurShader->UseShader();
	m_pGaussBlurShader->SetUniform("uScreenSize", CVector2(hwidth, hheight));
}

/**
* 销毁后处理对象
*/
void CPostProcessBloom::Destroy() {
	CEngine::GetTextureManager()->Drop(m_pRenderTexture);
	CEngine::GetTextureManager()->Drop(m_pHBlurTexture);
	CEngine::GetTextureManager()->Drop(m_pVBlurTexture);
	CEngine::GetShaderManager()->Drop(m_pHighlightShader);
	CEngine::GetShaderManager()->Drop(m_pGaussBlurShader);
	CEngine::GetShaderManager()->Drop(m_pPostProcessShader);
}

/**
* 应用当前后处理
*/
void CPostProcessBloom::Apply(CTexture* target, CMesh* mesh) {
	CGraphicsManager* pGraphicsMgr = CEngine::GetGraphicsManager();
	// 提取高亮部分
	pGraphicsMgr->SetRenderTarget(m_pVBlurTexture, 0, false, false);
	m_pHighlightShader->UseShader();
	m_pRenderTexture->UseTexture();
	mesh->Render(false);
	// 水平模糊
	pGraphicsMgr->SetRenderTarget(m_pHBlurTexture, 0, false, false);
	m_pGaussBlurShader->UseShader();
	m_pGaussBlurShader->SetUniform("uDirection", CVector2(3.0f, 0.0f));
	m_pVBlurTexture->UseTexture();
	mesh->Render(false);
	// 垂直模糊
	pGraphicsMgr->SetRenderTarget(m_pVBlurTexture, 0, false, false);
	m_pGaussBlurShader->UseShader();
	m_pGaussBlurShader->SetUniform("uDirection", CVector2(0.0f, 3.0f));
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
void CPostProcessBloom::SetGaussKernel(int size, float sigma) {
	float kernel[64];
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
	m_pGaussBlurShader->UseShader();
	m_pGaussBlurShader->SetUniform("uWeights", kernel, 1, size);
}