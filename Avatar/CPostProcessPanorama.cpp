//================================================
// Copyright (c) 2019 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPostProcessPanorama.h"
#include "CEngine.h"

/**
* ��ʼ���������
*/
bool CPostProcessPanorama::Init(int width, int height) {
	const char* fragShader = "\
		uniform samplerCube uTexture;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		void main()\
		{\
			float lon = 6.283185 * (vTexCoord.x - 0.5);\
			float lat = 3.141593 * (vTexCoord.y - 0.5);\
			float x = sin(lon) * cos(lat);\
			float y = cos(lon) * cos(lat);\
			float z = sin(lat);\
			fragColor = texture(uTexture, vec3(x, y, z));\
			fragColor.a = 1.0;\
		}";
	int cubeSize = width > height ? height : width;
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	m_pPostProcessShader = pShaderMgr->Create("postprocess_panorama", GetVertexShader(), fragShader);
	m_pPostProcessShader->SetUniform("uTexture", 0);
	m_pRenderTexture = pTextureMgr->Create("postprocess_panorama", cubeSize, false, true, false);
	return m_pPostProcessShader->IsValid();
}

/**
* ��Ⱦ�����С�ı�
*/
void CPostProcessPanorama::Resize(int width, int height) {
	CEngine::GetTextureManager()->Resize(m_pRenderTexture, width, height);
}

/**
* ���ٺ������
*/
void CPostProcessPanorama::Destroy() {
	CEngine::GetTextureManager()->Drop(m_pRenderTexture);
	CEngine::GetShaderManager()->Drop(m_pPostProcessShader);
}