//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSCARTOON_H_
#define _CPOSTPROCESSCARTOON_H_
#include "CPostProcess.h"

/**
* @brief 卡通效果后处理
*/
class CPostProcessCartoon: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
};

#endif