//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSEMBOSS_H_
#define _CPOSTPROCESSEMBOSS_H_
#include "CPostProcess.h"

/**
* @brief 浮雕效果后处理
*/
class CPostProcessEmboss: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
};

#endif