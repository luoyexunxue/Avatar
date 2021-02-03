//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CEngine.h"
#include "AvatarConfig.h"
#include "CTimer.h"
#include "CLog.h"
#include "CDeviceWindows.h"
#include "CDeviceLinux.h"
#include "CDeviceAndroid.h"
#include "CGuiEnvironment.h"
#include "CMeshLoader.h"
#include "CCameraSmooth.h"
#include "CPluginLoader.h"
#include "CStringUtil.h"

/**
* 构造函数
*/
CEngine::CEngine() {
	m_bActive = false;
	m_bRunning = false;
	m_bFixedTimeSpan = false;
	m_fTimeSpan = 0.0f;
	m_fSpeedOfTime = 1.0f;
}

/**
* 引擎初始化
* @param config 引擎初始化参数
* @return 引擎初始化结果
*/
bool CEngine::Init(const SEngineConfig& config) {
	// 初始化定时器
	CTimer::Init();
	// 创建日志系统
	if (config.log.enable) {
		CLog::Create(config.log.console, config.log.file, config.log.time, config.log.name);
		CLog::SetLevel((CLog::Level)config.log.level);
	}
	// 创建设备
	const SEngineConfig::SGraphic& g = config.graphic;
	if (!GetDevice()->Create(g.title, g.width, g.height, g.resizable, g.fullscreen, g.antialias)) {
		CLog::Error("Create graphic window failed");
		return false;
	}
	// 设置资源文件路径
	GetFileManager()->SetDataDirectory(config.directory);
	// 加载系统着色器
	LoadShader();
	CLog::Info("Loading system shader completed.");
	// 加载引擎插件
	LoadPlugin();
	CLog::Info("Loading engine plugins completed.");
	// 设置默认光照及相机
	GetGraphicsManager()->SetLight(CVector3(0, 1, -1, 0), CVector3::Zero, CVector3(1.0f, 1.0f, 1.0f, 100.0f));
	GetGraphicsManager()->SetCamera(new CCameraSmooth());
	// 加载系统内置字体
	GetFontManager()->Load("system/default.ttf", "default");
	CLog::Info("Loading default font completed.");
	// 加载脚本程序
	GetScriptManager()->OpenScript("avatar.lua", this);
	CLog::Info("Loading script completed.");
	GetScriptManager()->OnReady();
	// 设置状态
	m_bActive = true;
	m_bRunning = true;
	return true;
}

/**
* 引擎销毁
*/
void CEngine::Destroy() {
	m_bActive = false;
	m_bRunning = false;
	UnRegisterEvent("update", 0, true);
	UnRegisterEvent("render", 0, true);
	GetSceneManager()->ClearNode();
	CPluginLoader::Destroy();
	GetScriptManager()->OnExit();
	GetScriptManager()->Destroy();
	GetSceneManager()->Destroy();
	GetAnimationManager()->Destroy();
	GetPhysicsManager()->Destroy();
	GetGraphicsManager()->Destroy();
	CGuiEnvironment::GetInstance()->Destroy();
	GetPostProcessManager()->Destroy();
	GetTextureManager()->Destroy();
	GetShaderManager()->Destroy();
	GetDevice()->Destroy();
	GetSoundManager()->Destroy();
	GetFileManager()->Destroy();
	GetFontManager()->Destroy();
	GetInputManager()->Destroy();
	CMeshLoader::Destroy();
	CLog::Destroy();
}

/**
* 逻辑更新
*/
void CEngine::Update() {
	// 时间间隔
	if (!m_bFixedTimeSpan) {
		m_fTimeSpan = CTimer::Reset("__engine__") * m_fSpeedOfTime;
		if (m_fTimeSpan > 1.0f) m_fTimeSpan = 1.0f;
	}
	// 获取系统输入
	CInputManager* pInputMgr = GetInputManager();
	GetDevice()->Handle(m_fTimeSpan);
	pInputMgr->Update();
	CInputManager::SInput* pInput = pInputMgr->GetInput();
	// 判断是否退出
	if (pInput->bQuit) {
		CLog::Info("Input exit message");
		m_bRunning = false;
		return;
	}
	// 判断开始暂停
	if (m_bActive == pInput->bPause) {
		CLog::Info("Input %s message", pInput->bPause ? "pause" : "start");
		CTimer::Pause(pInput->bPause);
		GetSoundManager()->Pause(pInput->bPause);
		m_bActive = !m_bActive;
	}
	if (!m_bActive) return;
	// 重力输入
	if (pInput->bGravity) GetPhysicsManager()->SetGravity(CVector3(pInput->fGravity));
	// 判断窗口是否改变大小
	if (pInput->iWidth > 0 || pInput->iHeight > 0) {
		GetGraphicsManager()->SetWindowSize(pInput->iWidth, pInput->iHeight);
		GetScriptManager()->OnSize(pInput->iWidth, pInput->iHeight);
	}
	// 脚本输入及更新
	CScriptManager* pScriptMgr = GetScriptManager();
	pScriptMgr->OnInput(pInput);
	pScriptMgr->OnUpdate(m_fTimeSpan);
	// 更新相机状态
	CCamera* pCamera = GetGraphicsManager()->GetCamera();
	pCamera->Input(pInput);
	pCamera->Update(m_fTimeSpan);
	// 处理注册的事件回调
	list<CNotifyHandler*>::iterator iter = m_lstUpdateListener.begin();
	while (iter != m_lstUpdateListener.end()) {
		(*iter)->Handle();
		++iter;
	}
	// 更新系统组件
	GetPhysicsManager()->Update(m_fTimeSpan);
	GetAnimationManager()->Update(m_fTimeSpan);
	GetSceneManager()->Update(m_fTimeSpan);
	// 更新听众位置，即摄像机位置
	CSoundManager* pSoundMgr = GetSoundManager();
	pSoundMgr->ListenerPos(pCamera->m_cPosition);
	pSoundMgr->ListenerOri(pCamera->m_cLookVector, pCamera->m_cUpVector);
}

/**
* 图形渲染
*/
void CEngine::Render() {
	// 绘制场景
	GetGraphicsManager()->Render();
	// 处理注册的事件回调
	list<CNotifyHandler*>::iterator iter = m_lstRenderListener.begin();
	while (iter != m_lstRenderListener.end()) {
		(*iter)->Handle();
		++iter;
	}
	// 绘制到设备上
	GetDevice()->Render();
}

/**
* 设置运行速度
* @param speed 时间系数，大于1则时间变快，小于1则变慢
* @param fixed 是否固定的时间步长
* @attention 当 fixed = true 时 speed 表示时间步长，即每帧时间间隔
*/
void CEngine::SetTimeSpeed(float speed, bool fixed) {
	if (speed > 0.0f) {
		m_fSpeedOfTime = speed;
		m_bFixedTimeSpan = fixed;
		if (fixed) {
			m_fTimeSpan = speed;
		}
	}
}

/**
* 获取引擎版本
* @param version 输出的版本号字符串指针
* @return 返回主版本号
*/
int CEngine::GetVersion(const char** version) {
	if (version) *version = AVATAR_VERSION;
	return (int)atof(AVATAR_VERSION);
}

/**
* 计算时间间隔
* @return 当前帧的时间长度
*/
float CEngine::GetTimeSpan() {
	return m_fTimeSpan;
}

/**
* 获取图形管理器实例
*/
CGraphicsManager* CEngine::GetGraphicsManager() {
	return CGraphicsManager::GetInstance();
}

/**
* 获取文件管理器实例
*/
CFileManager* CEngine::GetFileManager() {
	return CFileManager::GetInstance();
}

/**
* 获取输入管理器实例
*/
CInputManager* CEngine::GetInputManager() {
	return CInputManager::GetInstance();
}

/**
* 获取声音管理器实例
*/
CSoundManager* CEngine::GetSoundManager() {
	return CSoundManager::GetInstance();
}

/**
* 获取场景管理器实例
*/
CSceneManager* CEngine::GetSceneManager() {
	return CSceneManager::GetInstance();
}

/**
* 获取字体管理器实例
*/
CFontManager* CEngine::GetFontManager() {
	return CFontManager::GetInstance();
}

/**
* 获取纹理管理器实例
*/
CTextureManager* CEngine::GetTextureManager() {
	return CTextureManager::GetInstance();
}

/**
* 获取着色器管理器实例
*/
CShaderManager* CEngine::GetShaderManager() {
	return CShaderManager::GetInstance();
}

/**
* 获取物理管理器实例
*/
CPhysicsManager* CEngine::GetPhysicsManager() {
	return CPhysicsManager::GetInstance();
}

/**
* 获取脚本管理器实例
*/
CScriptManager* CEngine::GetScriptManager() {
	return CScriptManager::GetInstance();
}

/**
* 获取后处理管理器实例
*/
CPostProcessManager* CEngine::GetPostProcessManager() {
	return CPostProcessManager::GetInstance();
}

/**
* 获取动画管理器实例
*/
CAnimationManager* CEngine::GetAnimationManager() {
	return CAnimationManager::GetInstance();
}

/**
* 注册事件回调
* @param eventType 事件类型，可为 update, render
* @param handle 回调处理对象指针
* @return 注册是否成功
*/
bool CEngine::RegisterEvent(const string& eventType, CNotifyHandler* handle) {
	if (eventType == "update") {
		m_lstUpdateListener.push_back(handle);
		return true;
	} else if (eventType == "render") {
		m_lstRenderListener.push_back(handle);
		return true;
	}
	return false;
}

/**
* 取消事件注册
* @param eventType 事件类型，可为 update, render
* @param handle 回调处理对象指针
* @param release 是否释放 handle 指向的对象
*/
void CEngine::UnRegisterEvent(const string& eventType, CNotifyHandler* handle, bool release) {
	list<CNotifyHandler*>* listeners = 0;
	if (eventType == "update") listeners = &m_lstUpdateListener;
	else if (eventType == "render") listeners = &m_lstRenderListener;
	else return;
	list<CNotifyHandler*>::iterator iter = listeners->begin();
	while (iter != listeners->end()) {
		if (handle) {
			if (*iter == handle) {
				listeners->erase(iter);
				if (release) delete *iter;
				break;
			}
		} else if (release) {
			delete *iter;
		}
		++iter;
	}
	if (!handle) listeners->clear();
}

/**
* 获取设备实例
*/
CDevice* CEngine::GetDevice() {
#ifdef AVATAR_WINDOWS
	return CDeviceWindows::GetInstance();
#elif defined AVATAR_LINUX
	return CDeviceLinux::GetInstance();
#elif defined AVATAR_ANDROID
	return CDeviceAndroid::GetInstance();
#else
	#error "No platform defined."
#endif
}

/**
* 加载系统内置着色器
*/
void CEngine::LoadShader() {
	const string shaderName[] = {
		"default", "texture", "texturelight", "water", "terrain", "cloud",
		"planetsky", "planetground", "light", "hemilight", "fresnel", "lensflare",
		"depthmap", "shadowmap", "shadowmapfilter", "screen", "skybox", "flame",
		"blast", "irradiancemap", "environmentmap", "integrationmap"
	};
	const int shaderCount = sizeof(shaderName) / sizeof(string);
	CShaderManager* pShaderMgr = GetShaderManager();
	for (int i = 0; i < shaderCount; i++) {
		string shaderFile[] = {
			"system/shader/" + shaderName[i] + ".vert",
			"system/shader/" + shaderName[i] + ".frag"
		};
		CShader* pShader = pShaderMgr->Create(shaderName[i], shaderFile);
		// 默认的纹理单元
		if (pShader->IsUniform("uTexture")) pShader->SetUniform("uTexture", 0);
		if (pShader->IsUniform("uAlphaMap")) pShader->SetUniform("uAlphaMap", 0);
		if (pShader->IsUniform("uPointSize")) pShader->SetUniform("uPointSize", 4.0f);
	}
}

/**
* 加载系统插件
*/
void CEngine::LoadPlugin() {
	vector<string> plugins;
	string path = CFileManager::GetAppDirectory() + "plugins/";
	if (!GetFileManager()->DirectoryExists(path)) {
		path = GetFileManager()->GetDataDirectory() + "plugins/";
	}
	GetFileManager()->GetFileList(path, plugins);
	for (size_t i = 0; i < plugins.size(); i++) {
		string ext = CStringUtil::UpperCase(CFileManager::GetExtension(plugins[i]));
		if (ext == "DLL" || ext == "SO") {
			CPluginLoader::Load(path + plugins[i], this);
		}
	}
}