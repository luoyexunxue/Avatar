//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSANAGLYPH_H_
#define _CPOSTPROCESSANAGLYPH_H_
#include "CPostProcess.h"

/**
* @brief �����������
*/
class CPostProcessAnaglyph: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
};

#endif