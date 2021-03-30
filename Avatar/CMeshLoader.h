//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CMESHLOADER_H_
#define _CMESHLOADER_H_
#include "AvatarConfig.h"
#include "CMeshData.h"
#include <cstdint>
#include <map>
using std::map;

/**
* @brief 模型加载类
*/
class AVATAR_EXPORT CMeshLoader {
public:
	//! 注册模型加载器
	static bool Register(const string& type, CMeshLoader* loader);
	//! 销毁已注册的加载器
	static void Destroy();

	//! 加载网格模型
	static CMeshData* Load(const string& filename, bool create, bool cache);
	//! 移除已缓存的模型
	static void Remove(CMeshData* meshData);
	//! 保存网格模型
	static bool Save(const string& filename, CMeshData* meshData);

public:
	//! 默认构造函数
	CMeshLoader() {}
	//! 虚析构函数
	virtual ~CMeshLoader() {}
	//! 加载模型文件
	virtual CMeshData* LoadFile(const string& filename, uint8_t* data, size_t size) = 0;

private:
	//! 注册内置加载器
	static void RegisterLoader();
	//! 加载 AVT 文件
	static CMeshData* LoadAvatar(const string& filename);
	//! 保存 AVT 文件
	static bool SaveAvatar(const string& filename, CMeshData* meshData);
	//! 模型加载器列表
	static map<string, CMeshLoader*> m_mapMeshLoader;
	//! 已缓存的网格模型
	static map<string, CMeshData*> m_mapMeshDataCache;
	//! 已缓存的网格模型引用
	static map<CMeshData*, size_t> m_mapCacheRefCount;
};

#endif