//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSGRAY_H_
#define _CPOSTPROCESSGRAY_H_
#include "CPostProcess.h"

/**
* @brief �ҶȻ�����
*/
class CPostProcessGray: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
};

#endif