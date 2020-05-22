//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSPASS_H_
#define _CPOSTPROCESSPASS_H_
#include "CPostProcess.h"

/**
* @brief 图像复制后处理
*/
class CPostProcessPass: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
};

#endif