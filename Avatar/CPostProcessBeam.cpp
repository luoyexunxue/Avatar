//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPostProcessBeam.h"
#include "CLog.h"
#include "CEngine.h"

/**
* 初始化后处理对象
*/
bool CPostProcessBeam::Init(int width, int height) {
	// 体积光生成着色器
	const char* fragShader = "\
		uniform sampler2D uTexture;\
		uniform float uDensity;\
		uniform float uExposure;\
		uniform vec2 uLightPos;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		float random(vec2 n)\
		{\
			return fract(cos(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);\
		}\
		void main()\
		{\
			const float numSamples = 30.0;\
			vec2 delta = (uLightPos - vTexCoord) * uDensity;\
			vec4 color = vec4(0.0);\
			float disturb = random(gl_FragCoord.xy);\
			float total = 0.0;\
			for (float t = 0.0; t < numSamples; t++)\
			{\
				float percent = (t + disturb) / numSamples;\
				float weight = 4.0 * (percent - percent * percent);\
				vec2 offset = clamp(delta * percent, -1.0, 1.0);\
				color += texture(uTexture, vTexCoord + offset) * weight;\
				total += weight;\
			}\
			fragColor = vec4(color.rgb * uExposure / total, 1.0);\
		}";
	// 使用体积光照生成最终场景
	const char* combineShader = "\
		uniform sampler2D uTexture;\
		uniform sampler2D uTextureLight;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		void main()\
		{\
			vec4 lightColor = texture(uTextureLight, vTexCoord);\
			vec4 sceneColor = texture(uTexture, vTexCoord);\
			fragColor = lightColor + sceneColor;\
		}";
	// 创建着色器和纹理
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	m_pPostProcessShader = pShaderMgr->Create("postprocess_beam", GetVertexShader(), combineShader);
	m_pPostProcessShader->SetUniform("uTexture", 0);
	m_pPostProcessShader->SetUniform("uTextureLight", 1);
	m_pVolumeLightShader = pShaderMgr->Create("postprocess_beam_light", GetVertexShader(), fragShader);
	m_pVolumeLightShader->SetUniform("uTexture", 0);
	m_pVolumeLightShader->SetUniform("uDensity", 0.8f);
	m_pVolumeLightShader->SetUniform("uExposure", 1.0f);
	m_pVolumeLightShader->SetUniform("uLightPos", CVector2());
	m_pRenderTexture = pTextureMgr->Create("postprocess_beam", width, height, false, true, false);
	m_pVolumeLightTexture = pTextureMgr->Create("postprocess_beam_light", width, height, false, false, false);
	return m_pPostProcessShader->IsValid() && m_pVolumeLightShader->IsValid();
}

/**
* 渲染区域大小改变
*/
void CPostProcessBeam::Resize(int width, int height) {
	CEngine::GetTextureManager()->Resize(m_pRenderTexture, width, height);
	CEngine::GetTextureManager()->Resize(m_pVolumeLightTexture, width, height);
}

/**
* 销毁后处理对象
*/
void CPostProcessBeam::Destroy() {
	CEngine::GetTextureManager()->Drop(m_pRenderTexture);
	CEngine::GetTextureManager()->Drop(m_pVolumeLightTexture);
	CEngine::GetShaderManager()->Drop(m_pPostProcessShader);
	CEngine::GetShaderManager()->Drop(m_pVolumeLightShader);
}

/**
* 应用当前后处理
*/
void CPostProcessBeam::Apply(CTexture* target, CMesh* mesh) {
	// 获取光源在屏幕上的位置
	CGraphicsManager* pGraphicsMgr = CEngine::GetGraphicsManager();
	CCamera* pCamera = pGraphicsMgr->GetCamera();
	CVector3 lightPos = pGraphicsMgr->GetLight();
	// 对方向光进行特殊处理
	if (lightPos[3] == 0.0f) lightPos.Scale(10000.0f);
	CVector3 lightPosScreen;
	pGraphicsMgr->PointToScreen(lightPos, lightPosScreen);
	float screenX = lightPosScreen[0] / m_pRenderTexture->GetWidth();
	float screenY = lightPosScreen[1] / m_pRenderTexture->GetHeight();
	float exposure = pCamera->m_cLookVector.DotProduct((lightPos - pCamera->m_cPosition).Normalize()) * 1.1f;
	if (exposure < 0.1f) {
		if (exposure < 0.0f) exposure = 0.0f;
		screenX = 0.0f;
		screenY = 0.0f;
	}
	// 渲染体积光
	pGraphicsMgr->SetRenderTarget(m_pVolumeLightTexture, 0, true, true, true);
	m_pVolumeLightShader->UseShader();
	m_pVolumeLightShader->SetUniform("uLightPos", CVector2(screenX, screenY));
	m_pVolumeLightShader->SetUniform("uExposure", exposure);
	m_pRenderTexture->UseTexture();
	mesh->Render(false);
	// 合成最终场景
	pGraphicsMgr->SetRenderTarget(target, 0, true, false, false);
	m_pPostProcessShader->UseShader();
	m_pVolumeLightTexture->UseTexture(1);
	m_pRenderTexture->UseTexture(0);
	mesh->Render(false);
}