//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPostProcessHdr.h"
#include "CEngine.h"
#include "CTimer.h"

/**
* ��ʼ���������
*/
bool CPostProcessHdr::Init(int width, int height) {
	// HDR ������ɫ��
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
	// ��������ɫ��
	const char* passShader = "\
		uniform sampler2D uTexture;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		void main()\
		{\
			float luminance = dot(texture(uTexture, vTexCoord).rgb, vec3(0.3, 0.59, 0.11));\
			fragColor = vec4(luminance, luminance, luminance, 1.0);\
		}";
	// 1/4�²���������ɫ��
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
	// ���Ƚ�����ɫ��
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
	// ������ɫ��������
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
* ��Ⱦ�����С�ı�
*/
void CPostProcessHdr::Resize(int width, int height) {
	CEngine::GetTextureManager()->Resize(m_pRenderTexture, width, height);
}

/**
* ���ٺ������
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
* Ӧ�õ�ǰ����
*/
void CPostProcessHdr::Apply(CTexture* target, CMesh* mesh) {
	// ���Ƶ�ǰ HDR ͼ��
	CGraphicsManager* pGraphicsMgr = CEngine::GetGraphicsManager();
	pGraphicsMgr->SetRenderTarget(target, 0, true, true, true);
	m_pPostProcessShader->UseShader();
	m_pToneMapTexture[0]->UseTexture(1);
	m_pRenderTexture->UseTexture(0);
	mesh->Render(false);

	// ����ͼ���ƽ�����ȣ���������Ⱦ�� 64 x 64 �������ϣ��õ��Ҷ�ͼ
	pGraphicsMgr->SetRenderTarget(m_pToneMapTexture[4], 0, true, true, false);
	m_pPassShader->UseShader();
	m_pRenderTexture->UseTexture(0);
	mesh->Render(false);
	// �����²���
	const static float sizeArray[4] = { 1.0f, 4.0f, 16.0f, 64.0f };
	for (int i = 3; i > 0; i--) {
		pGraphicsMgr->SetRenderTarget(m_pToneMapTexture[i], 0, true, true, false);
		m_pDownScaleShader->UseShader();
		m_pDownScaleShader->SetUniform("uTextureSize", CVector2(sizeArray[i], sizeArray[i]));
		m_pToneMapTexture[i + 1]->UseTexture();
		mesh->Render(false);
	}
	// ���Ƚ��������Ե�����
	float elapsedTime = CTimer::Reset("postprocess_hdr");
	if (elapsedTime > 1.0f) elapsedTime = 1.0f;
	pGraphicsMgr->SetRenderTarget(m_pToneMapTexture[0], 0, true, false, false);
	m_pAdaptLumShader->UseShader();
	m_pAdaptLumShader->SetUniform("uElapsedTime", elapsedTime);
	m_pToneMapTexture[0]->UseTexture(1);
	m_pToneMapTexture[1]->UseTexture(0);
	mesh->Render(false);
	// �ָ���ȾĿ��
	pGraphicsMgr->SetRenderTarget(target, 0, true, false, false);
}