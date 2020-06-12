//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPLUGINLOADER_H_
#define _CPLUGINLOADER_H_
#include "AvatarConfig.h"
#include "CEngine.h"
#include <string>
#include <map>
using std::string;
using std::map;

/**
* @brief 插件加载类
*/
class AVATAR_EXPORT CPluginLoader {
public:
	//! 加载插件
	static bool Load(const string& filename, CEngine* engine);
	//! 卸载插件
	static bool Unload(const string& name);
	//! 销毁加载器
	static void Destroy();

	//! 获取插件数量
	static int GetCount();
	//! 获取已加载的插件列表
	static void GetPluginList(vector<string>& pluginList);
	//! 获取插件路径
	static string GetPath(const string& name);
	//! 获取插件描述
	static string GetDescription(const string& name);

private:
	//! 定义插件接口
	typedef int(*pfn_avatar_init)(CEngine* engine);
	typedef int(*pfn_avatar_desc)(char* buffer);
	typedef int(*pfn_avatar_destroy)();
	//! 插件定义
	typedef struct _SPlugin {
		string name;
		string path;
		string description;
		void* module;
		pfn_avatar_init avatar_init;
		pfn_avatar_desc avatar_desc;
		pfn_avatar_destroy avatar_destroy;
	} SPlugin;
	//! 插件列表
	static map<string, SPlugin*> m_mapPlugin;

private:
	//! 加载动态库
	static SPlugin* LoadDynamicLibrary(const string& file);
	//! 卸载动态库
	static void ReleaseDynamicLibrary(SPlugin* plugin);
};

#endif