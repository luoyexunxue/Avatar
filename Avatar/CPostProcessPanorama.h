//================================================
// Copyright (c) 2019 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSPANORAMA_H_
#define _CPOSTPROCESSPANORAMA_H_
#include "CPostProcess.h"

/**
* @brief ȫ����Ⱦ����
*/
class CPostProcessPanorama : public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
};

#endif