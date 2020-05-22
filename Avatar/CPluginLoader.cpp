//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPluginLoader.h"
#include "AvatarConfig.h"
#include "CFileManager.h"
#include "CStringUtil.h"
#include "CLog.h"
#ifdef AVATAR_WINDOWS
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

/**
* �Ѽ��صĲ���б�
*/
map<string, CPluginLoader::SPlugin*> CPluginLoader::m_mapPlugin;

/**
* ���ز��
*/
bool CPluginLoader::Load(const string& filename, CEngine* engine) {
	string name = CFileManager::GetFileName(filename, false);
	if (m_mapPlugin.count(name) > 0) {
		CLog::Warn("Plugin '%s' already loaded", name.c_str());
		return false;
	}
	SPlugin* plugin = LoadDynamicLibrary(filename);
	if (plugin == 0 || plugin->avatar_init == 0) {
		if (plugin) CLog::Warn("Wrong library format");
		CLog::Warn("Load plugin '%s' failed", filename.c_str());
		return false;
	}
	plugin->name.assign(name);
	plugin->path.assign(filename);
	// ��ʼ�����
	int ret = plugin->avatar_init(engine);
	if (ret != 0) {
		CLog::Warn("Initialize plugin '%s' error, ret = %d", plugin->name.c_str(), ret);
		ReleaseDynamicLibrary(plugin);
		return false;
	}
	// ��ȡ�������
	char description[1024];
	plugin->avatar_desc(description);
	plugin->description.assign(description);
	m_mapPlugin.insert(std::pair<string, SPlugin*>(plugin->name, plugin));
	CLog::Info("Load plugin '%s' success", plugin->name.c_str());
	return true;
}

/**
* ж�ز��
*/
bool CPluginLoader::Unload(const string& name) {
	map<string, SPlugin*>::iterator iter = m_mapPlugin.find(name);
	if (iter != m_mapPlugin.end()) {
		SPlugin* plugin = iter->second;
		plugin->avatar_destroy();
		ReleaseDynamicLibrary(plugin);
		m_mapPlugin.erase(iter);
		return true;
	}
	return false;
}

/**
* ���ٲ��������
*/
void CPluginLoader::Destroy() {
	map<string, SPlugin*>::iterator iter = m_mapPlugin.begin();
	while (iter != m_mapPlugin.end()) {
		SPlugin* plugin = iter->second;
		plugin->avatar_destroy();
		ReleaseDynamicLibrary(plugin);
		++iter;
	}
	m_mapPlugin.clear();
}

/**
* ��ȡ�������
*/
int CPluginLoader::GetCount() {
	return m_mapPlugin.size();
}

/**
* ��ȡ�Ѽ��صĲ���б�
*/
void CPluginLoader::GetPluginList(vector<string>& pluginList) {
	pluginList.clear();
	map<string, SPlugin*>::iterator iter = m_mapPlugin.begin();
	while (iter != m_mapPlugin.end()) {
		pluginList.push_back(iter->first);
		++iter;
	}
}

/**
* ��ȡ���·��
*/
string CPluginLoader::GetPath(const string& name) {
	map<string, SPlugin*>::iterator iter = m_mapPlugin.find(name);
	if (iter != m_mapPlugin.end()) {
		return iter->second->path;
	}
	return "";
}

/**
* ��ȡ�������
*/
string CPluginLoader::GetDescription(const string& name) {
	map<string, SPlugin*>::iterator iter = m_mapPlugin.find(name);
	if (iter != m_mapPlugin.end()) {
		return iter->second->description;
	}
	return "";
}

/**
* ���ض�̬��
*/
CPluginLoader::SPlugin* CPluginLoader::LoadDynamicLibrary(const string& file) {
	string path = file;
	CFileManager* pFileMgr = CEngine::GetFileManager();
	if (!pFileMgr->FileExists(file)) {
		const string ext = CStringUtil::UpperCase(CFileManager::GetExtension(file));
		if (ext != "DLL" && pFileMgr->FileExists(file + ".dll")) path = file + ".dll";
		else if (ext != "SO" && pFileMgr->FileExists(file + ".so")) path = file + ".so";
	}
#ifdef AVATAR_WINDOWS
	HMODULE hModule = LoadLibraryA(path.c_str());
	if (!hModule) return 0;
	SPlugin* plugin = new SPlugin();
	plugin->module = hModule;
	plugin->avatar_init = reinterpret_cast<pfn_avatar_init>(GetProcAddress(hModule, "avatar_init"));
	plugin->avatar_desc = reinterpret_cast<pfn_avatar_desc>(GetProcAddress(hModule, "avatar_desc"));
	plugin->avatar_destroy = reinterpret_cast<pfn_avatar_destroy>(GetProcAddress(hModule, "avatar_destroy"));
#else
	void* hModule = dlopen(path.c_str(), RTLD_LAZY | RTLD_GLOBAL);
	if (!hModule) return 0;
	SPlugin* plugin = new SPlugin();
	plugin->module = hModule;
	plugin->avatar_init = reinterpret_cast<pfn_avatar_init>(dlsym(hModule, "avatar_init"));
	plugin->avatar_desc = reinterpret_cast<pfn_avatar_desc>(dlsym(hModule, "avatar_desc"));
	plugin->avatar_destroy = reinterpret_cast<pfn_avatar_destroy>(dlsym(hModule, "avatar_destroy"));
#endif
	return plugin;
}

/**
* ж�ض�̬��
*/
void CPluginLoader::ReleaseDynamicLibrary(CPluginLoader::SPlugin* plugin) {
#ifdef AVATAR_WINDOWS
	FreeLibrary((HMODULE)plugin->module);
#else
	dlclose(plugin->module);
#endif
	delete plugin;
}