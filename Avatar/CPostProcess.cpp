//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPostProcess.h"
#include "CEngine.h"

/**
* ���캯��
*/
CPostProcess::CPostProcess() {
	m_pPostProcessShader = 0;
	m_pRenderTexture = 0;
	m_iSavedScreenSize[0] = 0;
	m_iSavedScreenSize[1] = 0;
}

/**
* Ӧ�õ�ǰ����
*/
void CPostProcess::Apply(CTexture* target, CMesh* mesh) {
	CEngine::GetGraphicsManager()->SetRenderTarget(target, 0, true, true, true);
	m_pPostProcessShader->UseShader();
	m_pRenderTexture->UseTexture();
	mesh->Render(false);
}

/**
* ��ȡ��������ɫ��
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