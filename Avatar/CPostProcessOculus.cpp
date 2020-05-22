//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPostProcessOculus.h"
#include "CEngine.h"

/**
* 初始化后处理对象
*/
bool CPostProcessOculus::Init(int width, int height) {
	const char* fragShader = "\
		uniform sampler2D uTexture;\
		uniform vec2 uLensCenter;\
		uniform vec2 uScaleIn;\
		uniform vec2 uScaleOut;\
		uniform vec2 uDistortionK;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		vec2 HmdWarp(vec2 texCoord)\
		{\
			vec2 center = uLensCenter;\
			if (texCoord.x > 0.5) center.x = 1.0 - uLensCenter.x;\
			vec2 radial = (texCoord - center) * uScaleIn;\
			float rSq = radial.x * radial.x + radial.y * radial.y;\
			vec2 distortion = radial * (1.0 + uDistortionK.x * rSq + uDistortionK.y * rSq * rSq);\
			return center + uScaleOut * distortion;\
		}\
		void main()\
		{\
			vec2 tc = HmdWarp(vTexCoord);\
			if (tc.x < 0.0 || tc.x > 1.0 || tc.y < 0.0 || tc.y > 1.0 ||\
				(vTexCoord.x < 0.5 && tc.x > 0.5) ||\
				(vTexCoord.x > 0.5 && tc.x < 0.5))\
				fragColor = vec4(0.0, 0.0, 0.0, 1.0);\
			else\
				fragColor = vec4(texture(uTexture, tc).rgb, 1.0);\
		}";
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	m_pPostProcessShader = pShaderMgr->Create("postprocess_oculus", GetVertexShader(), fragShader);
	m_pPostProcessShader->SetUniform("uTexture", 0);
	m_pPostProcessShader->SetUniform("uScaleIn", CVector2(2.0f, 2.0f));
	m_pPostProcessShader->SetUniform("uScaleOut", CVector2(0.45f, 0.45f));
	m_pPostProcessShader->SetUniform("uLensCenter", CVector2(0.3f, 0.5f));
	m_pPostProcessShader->SetUniform("uDistortionK", CVector2(0.219999999f, 0.239999995f));
	m_pRenderTexture = pTextureMgr->Create("postprocess_oculus", width, height, false, true, false);
	return m_pPostProcessShader->IsValid();
}

/**
* 渲染区域大小改变
*/
void CPostProcessOculus::Resize(int width, int height) {
	CEngine::GetTextureManager()->Resize(m_pRenderTexture, width, height);
}

/**
* 销毁后处理对象
*/
void CPostProcessOculus::Destroy() {
	CEngine::GetTextureManager()->Drop(m_pRenderTexture);
	CEngine::GetShaderManager()->Drop(m_pPostProcessShader);
}