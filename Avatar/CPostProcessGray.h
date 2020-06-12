//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSGRAY_H_
#define _CPOSTPROCESSGRAY_H_
#include "CPostProcess.h"

/**
* @brief 灰度化后处理
*/
class CPostProcessGray: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
};

#endif