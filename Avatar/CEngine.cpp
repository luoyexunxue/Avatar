//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
#include "CCameraChase.h"
#include "CPluginLoader.h"
#include "CStringUtil.h"

/**
* ���캯��
*/
CEngine::CEngine() {
	m_bActive = false;
	m_bRunning = false;
	m_bFixedTimeSpan = false;
	m_fTimeSpan = 0.0f;
	m_fSpeedOfTime = 1.0f;
}

/**
* �����ʼ��
* @param config �����ʼ������
* @return �����ʼ�����
*/
bool CEngine::Init(const SEngineConfig& config) {
	// ��ʼ����ʱ��
	CTimer::Init();
	// ������־ϵͳ
	if (config.log.enable) {
		CLog::Create(config.log.console, config.log.file, config.log.time, config.log.name);
		CLog::SetLevel((CLog::Level)config.log.level);
	}
	// �����豸
	const SEngineConfig::SGraphic& g = config.graphic;
	if (!GetDevice()->Create(g.title, g.width, g.height, g.resizable, g.fullscreen, g.antialias)) {
		CLog::Error("Create graphic window failed");
		return false;
	}
	// ������Դ�ļ�·��
	GetFileManager()->SetDataDirectory(config.directory);
	// ����ϵͳ��ɫ��
	LoadShader();
	CLog::Info("Loading system shader completed.");
	// ����������
	LoadPlugin();
	CLog::Info("Loading engine plugins completed.");
	// ����Ĭ�Ϲ��ռ����
	GetGraphicsManager()->SetLight(CVector3(0, 1, -1, 0), CVector3::Zero, CVector3(1.0f, 1.0f, 1.0f, 100.0f));
	GetGraphicsManager()->SetCamera(new CCameraChase());
	// ����ϵͳ��������
	GetFontManager()->Load("system/default.ttf", "default");
	CLog::Info("Loading default font completed.");
	// ���ؽű�����
	GetScriptManager()->OpenScript("avatar.lua", this);
	CLog::Info("Loading script completed.");
	GetScriptManager()->OnReady();
	// ����״̬
	m_bActive = true;
	m_bRunning = true;
	return true;
}

/**
* ��������
*/
void CEngine::Destroy() {
	m_bActive = false;
	m_bRunning = false;
	UnRegisterEvent("update", 0, true);
	UnRegisterEvent("render", 0, true);
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
* �߼�����
*/
void CEngine::Update() {
	// ʱ����
	if (!m_bFixedTimeSpan) {
		m_fTimeSpan = CTimer::Reset("__engine__") * m_fSpeedOfTime;
		if (m_fTimeSpan > 1.0f) m_fTimeSpan = 1.0f;
	}
	// ��ȡϵͳ����
	CInputManager* pInputMgr = GetInputManager();
	CScriptManager* pScriptMgr = GetScriptManager();
	CSoundManager* pSoundMgr = GetSoundManager();
	GetDevice()->Handle(m_fTimeSpan);
	pInputMgr->Update();
	pScriptMgr->HandleEvent();
	// ����������Ϣ
	CInputManager::SInput* pInput = pInputMgr->GetInput();
	// �жϴ����Ƿ�ı��С
	if (pInput->iWidth > 0 || pInput->iHeight > 0) {
		GetGraphicsManager()->SetWindowSize(pInput->iWidth, pInput->iHeight);
		pScriptMgr->OnSize(pInput->iWidth, pInput->iHeight);
	}
	// �ж��Ƿ��˳�
	if (pInput->bQuit) {
		CLog::Info("Input exit message");
		m_bRunning = false;
		return;
	}
	// �жϿ�ʼ��ͣ
	if (m_bActive == pInput->bPause) {
		CLog::Info("Input start/pause message");
		CTimer::Pause(pInput->bPause);
		pSoundMgr->Pause(pInput->bPause);
		m_bActive = !m_bActive;
	}
	if (!m_bActive) return;

	// �ű����봦��
	if (pInput->bGravity) GetPhysicsManager()->SetGravity(CVector3(pInput->fGravity));
	if (pInput->iFunction) pScriptMgr->OnInput("function", pInput->iFunction, 0, 0, 0);
	if (pInput->iInputKey) pScriptMgr->OnInput("key", pInput->iInputKey, 0, 0, 0);
	if (pInput->bFire) pScriptMgr->OnInput("fire", 1, pInput->iInputX, pInput->iInputY, 0);
	if (pInput->bMove) pScriptMgr->OnInput("move", 1, pInput->fRightLeft, pInput->fForthBack, pInput->fUpDown);
	if (pInput->bTurn) pScriptMgr->OnInput("turn", 1, pInput->fYaw, pInput->fPitch, pInput->fRoll);
	// �������״̬
	CCamera* pCamera = GetGraphicsManager()->GetCamera();
	pCamera->Input(pInput);
	pCamera->Update(m_fTimeSpan);
	// ����ϵͳ���
	GetPhysicsManager()->Update(m_fTimeSpan);
	GetAnimationManager()->Update(m_fTimeSpan);
	GetSceneManager()->Update(m_fTimeSpan);
	pScriptMgr->OnUpdate(m_fTimeSpan);
	// ����ע����¼��ص�
	list<CNotifyHandler*>::iterator iter = m_lstUpdateListener.begin();
	while (iter != m_lstUpdateListener.end()) {
		(*iter)->Handle();
		++iter;
	}
	// ��������λ�ã��������λ��
	pSoundMgr->ListenerPos(pCamera->m_cPosition);
	pSoundMgr->ListenerOri(pCamera->m_cLookVector, pCamera->m_cUpVector);
}

/**
* ͼ����Ⱦ
*/
void CEngine::Render() {
	// ���Ƴ���
	GetGraphicsManager()->Render();
	// ����ע����¼��ص�
	list<CNotifyHandler*>::iterator iter = m_lstRenderListener.begin();
	while (iter != m_lstRenderListener.end()) {
		(*iter)->Handle();
		++iter;
	}
	// ���Ƶ��豸��
	GetDevice()->Render();
}

/**
* ���������ٶ�
* @param speed ʱ��ϵ��������1��ʱ���죬С��1�����
* @param fixed �Ƿ�̶���ʱ�䲽��
* @attention �� fixed = true ʱ speed ��ʾʱ�䲽������ÿ֡ʱ����
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
* ��ȡ����汾
* @param version ����İ汾���ַ���ָ��
* @return �������汾��
*/
int CEngine::GetVersion(const char** version) {
	if (version) *version = AVATAR_VERSION;
	return (int)atof(AVATAR_VERSION);
}

/**
* ����ʱ����
* @return ��ǰ֡��ʱ�䳤��
*/
float CEngine::GetTimeSpan() {
	return m_fTimeSpan;
}

/**
* ��ȡͼ�ι�����ʵ��
*/
CGraphicsManager* CEngine::GetGraphicsManager() {
	return CGraphicsManager::GetInstance();
}

/**
* ��ȡ�ļ�������ʵ��
*/
CFileManager* CEngine::GetFileManager() {
	return CFileManager::GetInstance();
}

/**
* ��ȡ���������ʵ��
*/
CInputManager* CEngine::GetInputManager() {
	return CInputManager::GetInstance();
}

/**
* ��ȡ����������ʵ��
*/
CSoundManager* CEngine::GetSoundManager() {
	return CSoundManager::GetInstance();
}

/**
* ��ȡ����������ʵ��
*/
CSceneManager* CEngine::GetSceneManager() {
	return CSceneManager::GetInstance();
}

/**
* ��ȡ���������ʵ��
*/
CFontManager* CEngine::GetFontManager() {
	return CFontManager::GetInstance();
}

/**
* ��ȡ���������ʵ��
*/
CTextureManager* CEngine::GetTextureManager() {
	return CTextureManager::GetInstance();
}

/**
* ��ȡ��ɫ��������ʵ��
*/
CShaderManager* CEngine::GetShaderManager() {
	return CShaderManager::GetInstance();
}

/**
* ��ȡ���������ʵ��
*/
CPhysicsManager* CEngine::GetPhysicsManager() {
	return CPhysicsManager::GetInstance();
}

/**
* ��ȡ�ű�������ʵ��
*/
CScriptManager* CEngine::GetScriptManager() {
	return CScriptManager::GetInstance();
}

/**
* ��ȡ���������ʵ��
*/
CPostProcessManager* CEngine::GetPostProcessManager() {
	return CPostProcessManager::GetInstance();
}

/**
* ��ȡ����������ʵ��
*/
CAnimationManager* CEngine::GetAnimationManager() {
	return CAnimationManager::GetInstance();
}

/**
* ע���¼��ص�
* @param eventType �¼����ͣ���Ϊ update, render
* @param handle �ص��������ָ��
* @return ע���Ƿ�ɹ�
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
* ȡ���¼�ע��
* @param eventType �¼����ͣ���Ϊ update, render
* @param handle �ص��������ָ��
* @param release �Ƿ��ͷ� handle ָ��Ķ���
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
* ��ȡ�豸ʵ��
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
* ����ϵͳ������ɫ��
*/
void CEngine::LoadShader() {
	const string shaderName[] = {
		"default", "texture", "texturelight", "water", "terrain", "cloud",
		"planetsky", "planetground", "light", "fresnel", "lensflare",
		"shadowmap", "screen", "skybox", "flame", "blast", "shadowmapfilter",
		"ibl_irradiancemap", "ibl_environmentmap", "ibl_integrationmap"
	};
	const int shaderCount = sizeof(shaderName) / sizeof(string);
	CShaderManager* pShaderMgr = GetShaderManager();
	for (int i = 0; i < shaderCount; i++) {
		string shaderFile[] = {
			"system/shader/" + shaderName[i] + ".vert",
			"system/shader/" + shaderName[i] + ".frag"
		};
		CShader* pShader = pShaderMgr->Create(shaderName[i], shaderFile);
		// Ĭ�ϵ�����Ԫ
		if (pShader->IsUniform("uTexture")) pShader->SetUniform("uTexture", 0);
		if (pShader->IsUniform("uAlphaMap")) pShader->SetUniform("uAlphaMap", 0);
		if (pShader->IsUniform("uPointSize")) pShader->SetUniform("uPointSize", 4.0f);
	}
}

/**
* ����ϵͳ���
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