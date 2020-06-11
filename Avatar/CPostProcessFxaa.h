//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSFXAA_H_
#define _CPOSTPROCESSFXAA_H_
#include "CPostProcess.h"

/**
* @brief FXAA 快速抗锯齿后处理
*/
class CPostProcessFxaa: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
};

#endif