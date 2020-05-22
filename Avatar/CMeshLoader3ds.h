//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CMESHLOADER3DS_H_
#define _CMESHLOADER3DS_H_
#include "CMeshLoader.h"
#include <vector>
using std::vector;

/**
* @brief 3DS ģ�ͼ�����
*/
class CMeshLoader3ds: public CMeshLoader {
protected:
	//! ����ģ��
	virtual CMeshData* LoadFile(const string& filename, const string& type);

private:
	//! �ݹ��ȡ��
	void ReadChunk(unsigned char* buffer, unsigned int size);
	//! ��ȡ�����
	void ReadMeshChunk(unsigned char* buffer, unsigned int size, CMesh* mesh);
	//! ��ȡ���ʿ�
	void ReadMaterialChunk(unsigned char* buffer, unsigned int size, CMaterial* material);
	//! ��ȡ��ɫ��
	void ReadColorChunk(unsigned char* buffer, unsigned int size, float color[3]);
	//! ��ȡ�����
	void ReadTextureChunk(unsigned char* buffer, unsigned int size, string& texture);

private:
	string m_strBaseDir;
	vector<CMesh*> m_vecMeshes;
	vector<CMaterial*> m_vecMaterials;
};

#endif