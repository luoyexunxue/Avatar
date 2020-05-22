//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPostProcess.h"
#include "CEngine.h"

/**
* 构造函数
*/
CPostProcess::CPostProcess() {
	m_pPostProcessShader = 0;
	m_pRenderTexture = 0;
	m_iSavedScreenSize[0] = 0;
	m_iSavedScreenSize[1] = 0;
}

/**
* 应用当前后处理
*/
void CPostProcess::Apply(CTexture* target, CMesh* mesh) {
	CEngine::GetGraphicsManager()->SetRenderTarget(target, 0, true, true, true);
	m_pPostProcessShader->UseShader();
	m_pRenderTexture->UseTexture();
	mesh->Render(false);
}

/**
* 获取后处理顶点着色器
*/
const char* CPostProcess::GetVertexShader() {
	static const char* strVertShader = "\
		in vec4 aPosition;\
		in vec2 aTexCoord;\
		out vec2 vTexCoord;\
		void main()\
		{\
			vTexCoord = aTexCoord;\
			gl_Position = aPosition;\
		}";
	return strVertShader;
}