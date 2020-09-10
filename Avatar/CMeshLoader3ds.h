//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CMESHLOADER3DS_H_
#define _CMESHLOADER3DS_H_
#include "CMeshLoader.h"
#include <vector>
using std::vector;

/**
* @brief 3DS 模型加载类
*/
class AVATAR_EXPORT CMeshLoader3ds : public CMeshLoader {
public:
	//! 加载模型
	virtual CMeshData* LoadFile(const string& filename, uint8_t* data, uint32_t size);

private:
	//! 递归读取块
	void ReadChunk(unsigned char* buffer, unsigned int size);
	//! 读取网格块
	void ReadMeshChunk(unsigned char* buffer, unsigned int size, CMesh* mesh);
	//! 读取材质块
	void ReadMaterialChunk(unsigned char* buffer, unsigned int size, CMaterial* material);
	//! 读取颜色块
	void ReadColorChunk(unsigned char* buffer, unsigned int size, float color[3]);
	//! 读取纹理块
	void ReadTextureChunk(unsigned char* buffer, unsigned int size, string& texture);

private:
	string m_strBaseDir;
	vector<CMesh*> m_vecMeshes;
	vector<CMaterial*> m_vecMaterials;
};

#endif