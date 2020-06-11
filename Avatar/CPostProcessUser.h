//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSUSER_H_
#define _CPOSTPROCESSUSER_H_
#include "CPostProcess.h"
#include <vector>
#include <string>
using std::vector;
using std::string;

/**
* @brief 用户自定义后处理
*/
class CPostProcessUser: public CPostProcess {
public:
	CPostProcessUser(const string& name, const char* shader, const char* texture, bool cube);

	virtual bool Init(int width, int height);
	virtual void Resize(int width, int height);
	virtual void Destroy();
	virtual void Apply(CTexture* target, CMesh* mesh);

private:
	bool m_bRenderCube;
	string m_strName;
	string m_strIdentifier;
	string m_strFragShader;
	string m_strTextures;
	vector<CTexture*> m_vecTextures;
};

#endif