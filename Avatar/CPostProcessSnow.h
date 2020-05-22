//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSSNOW_H_
#define _CPOSTPROCESSSNOW_H_
#include "CPostProcess.h"

/**
* @brief ��ѩЧ������
*/
class CPostProcessSnow: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
	virtual void Apply(CTexture* target, CMesh* mesh);
};

#endif