//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSEMBOSS_H_
#define _CPOSTPROCESSEMBOSS_H_
#include "CPostProcess.h"

/**
* @brief ����Ч������
*/
class CPostProcessEmboss: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
};

#endif