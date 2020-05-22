//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSSSAO_H_
#define _CPOSTPROCESSSSAO_H_
#include "CPostProcess.h"

/**
* @brief SSAO 后处理
*/
class CPostProcessSsao: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
	virtual void Apply(CTexture* target, CMesh* mesh);

private:
	CShader* m_pDepthShader;
	CShader* m_pSsaoShader;
	CShader* m_pBlurShader;
	CTexture* m_pSsaoTexture;
	CTexture* m_pBlurTexture;
	CTexture* m_pDepthTexture;
	CTexture* m_pRandomTexture;
};

#endif