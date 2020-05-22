//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPostProcessEdge.h"
#include "CEngine.h"

/**
* 初始化后处理对象
*/
bool CPostProcessEdge::Init(int width, int height) {
	const char* fragShader = "\
		uniform sampler2D uTexture;\
		uniform vec2 uTextureSize;\
		uniform vec3 uEdgeColor;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		float lookup(vec2 p, float dx, float dy)\
		{\
			vec2 uv = p + vec2(dx, dy) / uTextureSize;\
			vec4 c = texture(uTexture, uv);\
			return 0.2126 * c.r + 0.7152 * c.g + 0.0722 * c.b;\
		}\
		void main()\
		{\
			vec2 p = gl_FragCoord.xy;\
			float m00 = lookup(vTexCoord, -1.0, -1.0);\
			float m01 = lookup(vTexCoord, -1.0,  0.0);\
			float m02 = lookup(vTexCoord, -1.0,  1.0);\
			float m10 = lookup(vTexCoord,  0.0, -1.0);\
			float m12 = lookup(vTexCoord,  0.0,  1.0);\
			float m20 = lookup(vTexCoord,  1.0, -1.0);\
			float m21 = lookup(vTexCoord,  1.0,  0.0);\
			float m22 = lookup(vTexCoord,  1.0,  1.0);\
			float gx = -1.0 * m00 - 2.0 * m01 - 1.0 * m02 + 1.0 * m20 + 2.0 * m21 + 1.0 * m22;\
			float gy = -1.0 * m00 - 2.0 * m10 - 1.0 * m20 + 1.0 * m02 + 2.0 * m12 + 1.0 * m22;\
			float glow = gx * gx + gy * gy;\
			fragColor = vec4(uEdgeColor * glow, 1.0);\
		}";
	// 创建着色器和纹理
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	m_pPostProcessShader = pShaderMgr->Create("postprocess_edge", GetVertexShader(), fragShader);
	m_pPostProcessShader->SetUniform("uTexture", 0);
	m_pPostProcessShader->SetUniform("uTextureSize", CVector2(width, height));
	m_pPostProcessShader->SetUniform("uEdgeColor", CVector3(1.0f, 1.0f, 1.0f));
	m_pRenderTexture = pTextureMgr->Create("postprocess_edge", width, height, false, true, false);
	return m_pPostProcessShader->IsValid();
}

/**
* 渲染区域大小改变
*/
void CPostProcessEdge::Resize(int width, int height) {
	CEngine::GetTextureManager()->Resize(m_pRenderTexture, width, height);
	m_pPostProcessShader->UseShader();
	m_pPostProcessShader->SetUniform("uTextureSize", CVector2(width, height));
}

/**
* 销毁后处理对象
*/
void CPostProcessEdge::Destroy() {
	CEngine::GetTextureManager()->Drop(m_pRenderTexture);
	CEngine::GetShaderManager()->Drop(m_pPostProcessShader);
}