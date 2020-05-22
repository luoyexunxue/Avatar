//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPostProcessSnow.h"
#include "CEngine.h"
#include "CTimer.h"

/**
* 初始化后处理对象
*/
bool CPostProcessSnow::Init(int width, int height) {
	const char* fragShader = "\
		uniform sampler2D uTexture;\
		uniform float uElapsedTime;\
		uniform vec2 uResolution;\
		uniform vec2 uVelocity;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		float snow(vec2 uv, float scale, float time)\
		{\
			float w = smoothstep(0.0, 1.0, 1.0 + uv.y * (scale / 10.0));\
			if (w < 0.1) return 0.0;\
			uv.y += time * 2.0 / scale;\
			uv.x += sin(uv.y + time * 0.5) / scale;\
			uv *= scale;\
			vec2 f = fract(uv);\
			vec2 p = 0.5 + 0.35 * sin(11.0 * fract(sin((floor(uv) + scale) * mat2(vec2(7, 3), vec2(6, 5))) * 5.0)) - f;\
			float k = smoothstep(0.0, min(length(p), 3.0), sin(f.x + f.y) * 0.01);\
			return k * w;\
		}\
		void main()\
		{\
			vec2 uv = (gl_FragCoord.xy * 2.0 - uResolution.xy) / min(uResolution.x, uResolution.y);\
			float c = 0.2;\
			uv += uVelocity * uElapsedTime;\
			c += snow(uv, 30.0, uElapsedTime) * 0.3;\
			c += snow(uv, 20.0, uElapsedTime) * 0.5;\
			c += snow(uv, 10.0, uElapsedTime);\
			c += snow(uv, 7.0, uElapsedTime);\
			c += snow(uv, 5.0, uElapsedTime);\
			c += snow(uv, 4.0, uElapsedTime);\
			fragColor = vec4(vec3(c) + texture(uTexture, vTexCoord).rgb, 1.0);\
		}";
	// 创建着色器和纹理
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	m_pPostProcessShader = pShaderMgr->Create("postprocess_snow", GetVertexShader(), fragShader);
	m_pPostProcessShader->SetUniform("uTexture", 0);
	m_pPostProcessShader->SetUniform("uElapsedTime", 0.0f);
	m_pPostProcessShader->SetUniform("uResolution", CVector2(width, height));
	m_pPostProcessShader->SetUniform("uVelocity", CVector2(0.0f, 0.0f));
	m_pRenderTexture = pTextureMgr->Create("postprocess_snow", width, height, false, true, false);
	return m_pPostProcessShader->IsValid();
}

/**
* 渲染区域大小改变
*/
void CPostProcessSnow::Resize(int width, int height) {
	CEngine::GetTextureManager()->Resize(m_pRenderTexture, width, height);
	m_pPostProcessShader->UseShader();
	m_pPostProcessShader->SetUniform("uResolution", CVector2(width, height));
}

/**
* 销毁后处理对象
*/
void CPostProcessSnow::Destroy() {
	CEngine::GetTextureManager()->Drop(m_pRenderTexture);
	CEngine::GetShaderManager()->Drop(m_pPostProcessShader);
}

/**
* 应用当前后处理
*/
void CPostProcessSnow::Apply(CTexture* target, CMesh* mesh) {
	CEngine::GetGraphicsManager()->SetRenderTarget(target, 0, true, true, true);
	m_pPostProcessShader->UseShader();
	m_pPostProcessShader->SetUniform("uElapsedTime", CTimer::Reset("postprocess_snow", false));
	m_pRenderTexture->UseTexture();
	mesh->Render(false);
}