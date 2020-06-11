//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSBEAM_H_
#define _CPOSTPROCESSBEAM_H_
#include "CPostProcess.h"

/**
* @brief 体积光后处理
*/
class CPostProcessBeam: public CPostProcess {
public:
	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
	virtual void Apply(CTexture* target, CMesh* mesh);

private:
	CShader* m_pVolumeLightShader;
	CTexture* m_pVolumeLightTexture;
};

#endif