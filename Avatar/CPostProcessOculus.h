//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSOCULUS_H_
#define _CPOSTPROCESSOCULUS_H_
#include "CPostProcess.h"

/**
* @brief �����Ӿ���ͷ��������
*/
class CPostProcessOculus: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
};

#endif