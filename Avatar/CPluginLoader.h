//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* @brief ���������
*/
class AVATAR_EXPORT CPluginLoader {
public:
	//! ���ز��
	static bool Load(const string& filename, CEngine* engine);
	//! ж�ز��
	static bool Unload(const string& name);
	//! ���ټ�����
	static void Destroy();

	//! ��ȡ�������
	static int GetCount();
	//! ��ȡ�Ѽ��صĲ���б�
	static void GetPluginList(vector<string>& pluginList);
	//! ��ȡ���·��
	static string GetPath(const string& name);
	//! ��ȡ�������
	static string GetDescription(const string& name);

private:
	//! �������ӿ�
	typedef int(*pfn_avatar_init)(CEngine* engine);
	typedef int(*pfn_avatar_desc)(char* buffer);
	typedef int(*pfn_avatar_destroy)();
	//! �������
	typedef struct _SPlugin {
		string name;
		string path;
		string description;
		void* module;
		pfn_avatar_init avatar_init;
		pfn_avatar_desc avatar_desc;
		pfn_avatar_destroy avatar_destroy;
	} SPlugin;
	//! ����б�
	static map<string, SPlugin*> m_mapPlugin;

private:
	//! ���ض�̬��
	static SPlugin* LoadDynamicLibrary(const string& file);
	//! ж�ض�̬��
	static void ReleaseDynamicLibrary(SPlugin* plugin);
};

#endif