//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSOCULUS_H_
#define _CPOSTPROCESSOCULUS_H_
#include "CPostProcess.h"

/**
* @brief 立体视觉镜头矫正后处理
*/
class CPostProcessOculus: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
};

#endif