//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPostProcessGray.h"
#include "CEngine.h"

/**
* ��ʼ���������
*/
bool CPostProcessGray::Init(int width, int height) {
	const char* fragShader = "\
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
	m_pPostProcessShader = pShaderMgr->Create("postprocess_gray", GetVertexShader(), fragShader);
	m_pPostProcessShader->SetUniform("uTexture", 0);
	m_pRenderTexture = pTextureMgr->Create("postprocess_gray", width, height, false, true, false);
	return m_pPostProcessShader->IsValid();
}

/**
* ��Ⱦ�����С�ı�
*/
void CPostProcessGray::Resize(int width, int height) {
	CEngine::GetTextureManager()->Resize(m_pRenderTexture, width, height);
}

/**
* ���ٺ������
*/
void CPostProcessGray::Destroy() {
	CEngine::GetTextureManager()->Drop(m_pRenderTexture);
	CEngine::GetShaderManager()->Drop(m_pPostProcessShader);
}