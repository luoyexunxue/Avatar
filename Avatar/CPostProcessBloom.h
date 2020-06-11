//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSBLOOM_H_
#define _CPOSTPROCESSBLOOM_H_
#include "CPostProcess.h"

/**
* @brief Bloom 后处理
*/
class CPostProcessBloom: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
	virtual void Apply(CTexture* target, CMesh* mesh);

private:
	CShader* m_pBlurShader;
	CTexture* m_pHBlurTexture;
	CTexture* m_pVBlurTexture;
};

#endif