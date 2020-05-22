//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSRAIN_H_
#define _CPOSTPROCESSRAIN_H_
#include "CPostProcess.h"

/**
* @brief ����Ч������
*/
class CPostProcessRain: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
	virtual void Apply(CTexture* target, CMesh* mesh);

private:
	CTexture* m_pNoiseTexture;
};

#endif