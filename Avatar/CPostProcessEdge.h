//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSEDGE_H_
#define _CPOSTPROCESSEDGE_H_
#include "CPostProcess.h"

/**
* @brief ��Ե��ǿ����
*/
class CPostProcessEdge: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
};

#endif