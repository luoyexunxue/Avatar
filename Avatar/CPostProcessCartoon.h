//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSCARTOON_H_
#define _CPOSTPROCESSCARTOON_H_
#include "CPostProcess.h"

/**
* @brief ��ͨЧ������
*/
class CPostProcessCartoon: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
};

#endif