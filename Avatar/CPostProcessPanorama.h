//================================================
// Copyright (c) 2019 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSPANORAMA_H_
#define _CPOSTPROCESSPANORAMA_H_
#include "CPostProcess.h"

/**
* @brief 全景渲染后处理
*/
class CPostProcessPanorama : public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
};

#endif