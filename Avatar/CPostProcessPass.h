//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSPASS_H_
#define _CPOSTPROCESSPASS_H_
#include "CPostProcess.h"

/**
* @brief ͼ���ƺ���
*/
class CPostProcessPass: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
};

#endif