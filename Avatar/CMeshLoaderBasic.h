//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CMESHLOADERBASIC_H_
#define _CMESHLOADERBASIC_H_
#include "CMeshLoader.h"

/**
* @brief 基础模型加载类
*/
class CMeshLoaderBasic: public CMeshLoader {
protected:
	//! 加载模型
	virtual CMeshData* LoadFile(const string& filename, const string& type);

private:
	//! 加载 STL 文件
	CMeshData* LoadStlFile(const string& filename);
	//! 加载 OBJ 文件
	CMeshData* LoadObjFile(const string& filename);
	//! 加载 MS3D 文件
	CMeshData* LoadMs3dFile(const string& filename);
	//! 加载 BVH 文件
	CMeshData* LoadBvhFile(const string& filename);

private:
	//! 加载 OBJ 材质文件
	void LoadObjMaterial(const string& filename, CMeshData* meshData);
	//! 解析 OBJ 文件面定义
	int ParseObjFace(const string& face, int index[3]);
};

#endif