//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CMESHLOADERBASIC_H_
#define _CMESHLOADERBASIC_H_
#include "CMeshLoader.h"

/**
* @brief ����ģ�ͼ�����
*/
class CMeshLoaderBasic: public CMeshLoader {
protected:
	//! ����ģ��
	virtual CMeshData* LoadFile(const string& filename, const string& type);

private:
	//! ���� STL �ļ�
	CMeshData* LoadStlFile(const string& filename);
	//! ���� OBJ �ļ�
	CMeshData* LoadObjFile(const string& filename);
	//! ���� MS3D �ļ�
	CMeshData* LoadMs3dFile(const string& filename);
	//! ���� BVH �ļ�
	CMeshData* LoadBvhFile(const string& filename);

private:
	//! ���� OBJ �����ļ�
	void LoadObjMaterial(const string& filename, CMeshData* meshData);
	//! ���� OBJ �ļ��涨��
	int ParseObjFace(const string& face, int index[3]);
};

#endif