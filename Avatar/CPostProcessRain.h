//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSRAIN_H_
#define _CPOSTPROCESSRAIN_H_
#include "CPostProcess.h"

/**
* @brief 下雨效果后处理
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