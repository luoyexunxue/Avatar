//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSHDR_H_
#define _CPOSTPROCESSHDR_H_
#include "CPostProcess.h"

/**
* @brief HDR ����
*/
class CPostProcessHdr: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
	virtual void Apply(CTexture* target, CMesh* mesh);

private:
	CShader* m_pPassShader;
	CShader* m_pDownScaleShader;
	CShader* m_pAdaptLumShader;
	CTexture* m_pToneMapTexture[5];
};

#endif