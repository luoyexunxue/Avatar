//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSSNOW_H_
#define _CPOSTPROCESSSNOW_H_
#include "CPostProcess.h"

/**
* @brief 下雪效果后处理
*/
class CPostProcessSnow: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
	virtual void Apply(CTexture* target, CMesh* mesh);
};

#endif