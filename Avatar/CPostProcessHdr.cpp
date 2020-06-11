//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPostProcessHdr.h"
#include "CEngine.h"
#include "CTimer.h"

/**
* 初始化后处理对象
*/
bool CPostProcessHdr::Init(int width, int height) {
	// HDR 像素着色器
	const char* hdrShader = "\
		uniform sampler2D uTexture;\
		uniform sampler2D uLuminance;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		void main()\
		{\
			float luminance = texture(uLuminance, vec2(0.0, 0.0)).r;\
			vec3 color = texture(uTexture, vTexCoord).rgb;\
			color *= 0.72 / (luminance + 0.001);\
			color *= (1.0 + color / 1.5);\
			color /= (1.0 + color);\
			fragColor = vec4(color, 1.0);\
		}";
	// 纹理复制着色器
	const char* passShader = "\
		uniform sampler2D uTexture;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		void main()\
		{\
			float luminance = dot(texture(uTexture, vTexCoord).rgb, vec3(0.3, 0.59, 0.11));\
			fragColor = vec4(luminance, luminance, luminance, 1.0);\
		}";
	// 1/4下采样像素着色器
	const char* downShader = "\
		uniform sampler2D uTexture;\
		uniform vec2 uTextureSize;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		void main()\
		{\
			vec2 pix = vec2(1.0 / uTextureSize.x, 1.0 / uTextureSize.y);\
			vec4 color = texture(uTexture, vTexCoord);\
			color += texture(uTexture, vTexCoord + vec2(pix.x, 0.0));\
			color += texture(uTexture, vTexCoord + vec2(pix.x * 2.0, 0.0));\
			color += texture(uTexture, vTexCoord + vec2(pix.x * 3.0, 0.0));\
			color += texture(uTexture, vTexCoord + vec2(0.0, pix.y));\
			color += texture(uTexture, vTexCoord + vec2(pix.x, pix.y));\
			color += texture(uTexture, vTexCoord + vec2(pix.x * 2.0, pix.y));\
			color += texture(uTexture, vTexCoord + vec2(pix.x * 3.0, pix.y));\
			color += texture(uTexture, vTexCoord + vec2(0.0, pix.y * 2.0));\
			color += texture(uTexture, vTexCoord + vec2(pix.x, pix.y * 2.0));\
			color += texture(uTexture, vTexCoord + vec2(pix.x * 2.0, pix.y * 2.0));\
			color += texture(uTexture, vTexCoord + vec2(pix.x * 3.0, pix.y * 2.0));\
			color += texture(uTexture, vTexCoord + vec2(0.0, pix.y * 3.0));\
			color += texture(uTexture, vTexCoord + vec2(pix.x, pix.y * 3.0));\
			color += texture(uTexture, vTexCoord + vec2(pix.x * 2.0, pix.y * 3.0));\
			color += texture(uTexture, vTexCoord + vec2(pix.x * 3.0, pix.y * 3.0));\
			color /= 16.0;\
			fragColor = color;\
		}";
	// 亮度渐调着色器
	const char* adaptShader = "\
		uniform sampler2D uTexture;\
		uniform sampler2D uTextureSelf;\
		uniform float uElapsedTime;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		void main()\
		{\
			float adaptedLum = texture(uTextureSelf, vec2(0.5, 0.5)).r;\
			float currentLum = texture(uTexture, vec2(0.5, 0.5)).r;\
			float adaptation = adaptedLum + (currentLum - adaptedLum) * uElapsedTime;\
			fragColor = vec4(adaptation, adaptation, adaptation, 1.0);\
		}";
	// 创建着色器和纹理
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	m_pPostProcessShader = pShaderMgr->Create("postprocess_hdr", GetVertexShader(), hdrShader);
	m_pPostProcessShader->SetUniform("uTexture", 0);
	m_pPostProcessShader->SetUniform("uLuminance", 1);
	m_pPassShader = pShaderMgr->Create("postprocess_hdr_pass", GetVertexShader(), passShader);
	m_pPassShader->SetUniform("uTexture", 0);
	m_pDownScaleShader = pShaderMgr->Create("postprocess_hdr_down", GetVertexShader(), downShader);
	m_pDownScaleShader->SetUniform("uTexture", 0);
	m_pAdaptLumShader = pShaderMgr->Create("postprocess_hdr_adapt", GetVertexShader(), adaptShader);
	m_pAdaptLumShader->SetUniform("uTexture", 0);
	m_pAdaptLumShader->SetUniform("uTextureSelf", 1);
	m_pRenderTexture = pTextureMgr->Create("postprocess_hdr", width, height, false, true, false);
	m_pToneMapTexture[0] = pTextureMgr->Create("postprocess_hdr_adapt", 1, 1, false, false, false);
	m_pToneMapTexture[1] = pTextureMgr->Create("postprocess_hdr_1x1", 1, 1, false, false, false);
	m_pToneMapTexture[2] = pTextureMgr->Create("postprocess_hdr_4x4", 4, 4, false, false, false);
	m_pToneMapTexture[3] = pTextureMgr->Create("postprocess_hdr_16x16", 16, 16, false, false, false);
	m_pToneMapTexture[4] = pTextureMgr->Create("postprocess_hdr_64x64", 64, 64, false, false, false);
	return m_pPostProcessShader->IsValid() && m_pPassShader->IsValid()
		&& m_pDownScaleShader->IsValid() && m_pAdaptLumShader->IsValid();
}

/**
* 渲染区域大小改变
*/
void CPostProcessHdr::Resize(int width, int height) {
	CEngine::GetTextureManager()->Resize(m_pRenderTexture, width, height);
}

/**
* 销毁后处理对象
*/
void CPostProcessHdr::Destroy() {
	CEngine::GetTextureManager()->Drop(m_pRenderTexture);
	CEngine::GetTextureManager()->Drop(m_pToneMapTexture[0]);
	CEngine::GetTextureManager()->Drop(m_pToneMapTexture[1]);
	CEngine::GetTextureManager()->Drop(m_pToneMapTexture[2]);
	CEngine::GetTextureManager()->Drop(m_pToneMapTexture[3]);
	CEngine::GetTextureManager()->Drop(m_pToneMapTexture[4]);
	CEngine::GetShaderManager()->Drop(m_pPostProcessShader);
	CEngine::GetShaderManager()->Drop(m_pPassShader);
	CEngine::GetShaderManager()->Drop(m_pDownScaleShader);
	CEngine::GetShaderManager()->Drop(m_pAdaptLumShader);
}

/**
* 应用当前后处理
*/
void CPostProcessHdr::Apply(CTexture* target, CMesh* mesh) {
	// 绘制当前 HDR 图像
	CGraphicsManager* pGraphicsMgr = CEngine::GetGraphicsManager();
	pGraphicsMgr->SetRenderTarget(target, 0, true, true, true);
	m_pPostProcessShader->UseShader();
	m_pToneMapTexture[0]->UseTexture(1);
	m_pRenderTexture->UseTexture(0);
	mesh->Render(false);

	// 计算图像的平均亮度，将场景渲染到 64 x 64 的纹理上，得到灰度图
	pGraphicsMgr->SetRenderTarget(m_pToneMapTexture[4], 0, true, true, false);
	m_pPassShader->UseShader();
	m_pRenderTexture->UseTexture(0);
	mesh->Render(false);
	// 进行下采样
	const static float sizeArray[4] = { 1.0f, 4.0f, 16.0f, 64.0f };
	for (int i = 3; i > 0; i--) {
		pGraphicsMgr->SetRenderTarget(m_pToneMapTexture[i], 0, true, true, false);
		m_pDownScaleShader->UseShader();
		m_pDownScaleShader->SetUniform("uTextureSize", CVector2(sizeArray[i], sizeArray[i]));
		m_pToneMapTexture[i + 1]->UseTexture();
		mesh->Render(false);
	}
	// 亮度渐调（线性调整）
	float elapsedTime = CTimer::Reset("postprocess_hdr");
	if (elapsedTime > 1.0f) elapsedTime = 1.0f;
	pGraphicsMgr->SetRenderTarget(m_pToneMapTexture[0], 0, true, false, false);
	m_pAdaptLumShader->UseShader();
	m_pAdaptLumShader->SetUniform("uElapsedTime", elapsedTime);
	m_pToneMapTexture[0]->UseTexture(1);
	m_pToneMapTexture[1]->UseTexture(0);
	mesh->Render(false);
	// 恢复渲染目标
	pGraphicsMgr->SetRenderTarget(target, 0, true, false, false);
}