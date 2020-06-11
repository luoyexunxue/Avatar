//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPostProcessFxaa.h"
#include "CEngine.h"

/**
* 初始化后处理对象
*/
bool CPostProcessFxaa::Init(int width, int height) {
	const char* fragShader = "\
		#define FXAA_REDUCE_MIN (1.0/128.0)\n\
		#define FXAA_REDUCE_MUL (1.0/8.0)\n\
		#define FXAA_SPAN_MAX 8.0\n\
		uniform sampler2D uTexture;\
		uniform vec2 uTexelSize;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		void main()\
		{\
			vec2 localTexelSize = uTexelSize;\
			vec4 rgbNW = texture(uTexture, (vTexCoord + vec2(-1.0, -1.0) * localTexelSize));\
			vec4 rgbNE = texture(uTexture, (vTexCoord + vec2(1.0, -1.0) * localTexelSize));\
			vec4 rgbSW = texture(uTexture, (vTexCoord + vec2(-1.0, 1.0) * localTexelSize));\
			vec4 rgbSE = texture(uTexture, (vTexCoord + vec2(1.0, 1.0) * localTexelSize));\
			vec4 rgbM = texture(uTexture, vTexCoord);\
			vec4 luma = vec4(0.299, 0.587, 0.114, 1.0);\
			float lumaNW = dot(rgbNW, luma);\
			float lumaNE = dot(rgbNE, luma);\
			float lumaSW = dot(rgbSW, luma);\
			float lumaSE = dot(rgbSE, luma);\
			float lumaM = dot(rgbM, luma);\
			float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));\
			float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));\
			vec2 dir = vec2(-((lumaNW + lumaNE) - (lumaSW + lumaSE)), ((lumaNW + lumaSW) - (lumaNE + lumaSE)));\
			float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);\
			float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);\
			dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX), max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) * localTexelSize;\
			vec3 rgbA = 0.5 * (texture(uTexture, vTexCoord + dir * (1.0 / 3.0 - 0.5)).rgb + texture(uTexture, vTexCoord + dir * (2.0 / 3.0 - 0.5)).rgb);\
			vec3 rgbB = rgbA * 0.5 + 0.25 * (texture(uTexture, vTexCoord + dir * -0.5).rgb + texture(uTexture, vTexCoord + dir * 0.5).rgb);\
			float lumaB = dot(rgbB, luma.rgb);\
			if (lumaB < lumaMin || lumaB > lumaMax) fragColor = vec4(rgbA, 1.0);\
			else fragColor = vec4(rgbB, 1.0);\
		}";
	// 创建着色器与纹理
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	m_pPostProcessShader = pShaderMgr->Create("postprocess_fxaa", GetVertexShader(), fragShader);
	m_pPostProcessShader->SetUniform("uTexture", 0);
	m_pPostProcessShader->SetUniform("uTexelSize", CVector2(1.0f / width, 1.0f / height));
	m_pRenderTexture = pTextureMgr->Create("postprocess_fxaa", width, height, false, true, false);
	return m_pPostProcessShader->IsValid();
}

/**
* 渲染区域大小改变
*/
void CPostProcessFxaa::Resize(int width, int height) {
	CEngine::GetTextureManager()->Resize(m_pRenderTexture, width, height);
	m_pPostProcessShader->UseShader();
	m_pPostProcessShader->SetUniform("uTexelSize", CVector2(1.0f / width, 1.0f / height));
}

/**
* 销毁后处理对象
*/
void CPostProcessFxaa::Destroy() {
	CEngine::GetTextureManager()->Drop(m_pRenderTexture);
	CEngine::GetShaderManager()->Drop(m_pPostProcessShader);
}