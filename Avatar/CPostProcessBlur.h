﻿//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSBLUR_H_
#define _CPOSTPROCESSBLUR_H_
#include "CPostProcess.h"

/**
* @brief 模糊后处理
*/
class CPostProcessBlur: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
	virtual void Apply(CTexture* target, CMesh* mesh);

	//! 设置高斯模糊参数
	void SetGaussKernel(int size, float sigma);

private:
	CTexture* m_pBlurTexture;
};

#endif