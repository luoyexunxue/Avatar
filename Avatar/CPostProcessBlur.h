//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSBLUR_H_
#define _CPOSTPROCESSBLUR_H_
#include "CPostProcess.h"

/**
* @brief ģ������
*/
class CPostProcessBlur: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
};

#endif