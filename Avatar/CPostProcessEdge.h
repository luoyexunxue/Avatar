//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSEDGE_H_
#define _CPOSTPROCESSEDGE_H_
#include "CPostProcess.h"

/**
* @brief 边缘增强后处理
*/
class CPostProcessEdge: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
};

#endif