//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSFXAA_H_
#define _CPOSTPROCESSFXAA_H_
#include "CPostProcess.h"

/**
* @brief FXAA ���ٿ���ݺ���
*/
class CPostProcessFxaa: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
};

#endif