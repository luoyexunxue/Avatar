//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPostProcessRain.h"
#include "CEngine.h"
#include "CTimer.h"
#include <cstdlib>

/**
* 初始化后处理对象
*/
bool CPostProcessRain::Init(int width, int height) {
	const char* fragShader = "\
		uniform sampler2D uTexture;\
		uniform sampler2D uTextureNoise;\
		uniform float uElapsedTime;\
		uniform bool uEnableLightning;\
		uniform float uVelocity;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		void main()\
		{\
			float d = 1.0;\
			float c = 0.0;\
			float time = uElapsedTime * 0.1;\
			for (int i = 0; i < 8; i++)\
			{\
				float s = vTexCoord.x * 1.5 + vTexCoord.y * uVelocity - time;\
				float t = vTexCoord.y * 0.05 + time;\
				vec2 st = (pow(d, 0.45) + 0.25) * vec2(s, t);\
				float f = texture(uTextureNoise, st * 0.5).x + texture(uTextureNoise, st * 0.28).y;\
				c += 0.25 * clamp(pow(abs(f) * 0.5, 29.0) * 140.0, 0.0, 1.0);\
				d += 25.0;\
			}\
			c = clamp(c, 0.0, 1.0);\
			vec3 color = vec3(c) + texture(uTexture, vTexCoord).rgb;\
			if (uEnableLightning)\
			{\
				float tm = (uElapsedTime + 3.0) * 0.5;\
				float lightning = sin(sin(tm * 10.0) * 13.0 + tm * 0.33) * pow(max(0.0, sin(tm + sin(tm))), 10.0);\
				color *= mix(vec3(1.0), vec3(0.8, 0.9, 1.3), sin(tm * 0.2) * 0.5 + 0.5);\
				color *= 1.0 + clamp(lightning, -0.8, 0.8);\
			}\
			fragColor = vec4(color, 1.0);\
		}";
	// 创建着色器与纹理
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	m_pPostProcessShader = pShaderMgr->Create("postprocess_rain", GetVertexShader(), fragShader);
	m_pPostProcessShader->SetUniform("uTexture", 0);
	m_pPostProcessShader->SetUniform("uTextureNoise", 1);
	m_pPostProcessShader->SetUniform("uElapsedTime", 0.0f);
	m_pPostProcessShader->SetUniform("uVelocity", 0.2f);
	m_pPostProcessShader->SetUniform("uEnableLightning", true);
	unsigned char noise[256 * 256 * 3];
	for (int i = 0; i < 196608; i++) noise[i] = (unsigned char)(rand() & 0xFF);
	m_pRenderTexture = pTextureMgr->Create("postprocess_rain", width, height, false, true, false);
	m_pNoiseTexture = pTextureMgr->Create("postprocess_rain_noise", 256, 256, 3, noise, false);
	return m_pPostProcessShader->IsValid();
}

/**
* 渲染区域大小改变
*/
void CPostProcessRain::Resize(int width, int height) {
	CEngine::GetTextureManager()->Resize(m_pRenderTexture, width, height);
}

/**
* 销毁后处理对象
*/
void CPostProcessRain::Destroy() {
	CEngine::GetTextureManager()->Drop(m_pNoiseTexture);
	CEngine::GetTextureManager()->Drop(m_pRenderTexture);
	CEngine::GetShaderManager()->Drop(m_pPostProcessShader);
}

/**
* 应用当前后处理
*/
void CPostProcessRain::Apply(CTexture* target, CMesh* mesh) {
	CEngine::GetGraphicsManager()->SetRenderTarget(target, 0, true, true, true);
	m_pPostProcessShader->UseShader();
	m_pPostProcessShader->SetUniform("uElapsedTime", CTimer::Reset("postprocess_rain", false));
	m_pNoiseTexture->UseTexture(1);
	m_pRenderTexture->UseTexture(0);
	mesh->Render(false);
}