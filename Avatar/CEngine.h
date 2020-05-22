//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CENGINE_H_
#define _CENGINE_H_
#include "CDevice.h"
#include "CGraphicsManager.h"
#include "CFileManager.h"
#include "CInputManager.h"
#include "CSoundManager.h"
#include "CSceneManager.h"
#include "CTextureManager.h"
#include "CPhysicsManager.h"
#include "CFontManager.h"
#include "CShaderManager.h"
#include "CPostProcessManager.h"
#include "CAnimationManager.h"
#include "CScriptManager.h"
#include <list>

/**
* @brief ��������
*/
struct SEngineConfig {
	//! ��ԴĿ¼
	string directory;
	//! ͼ������
	struct SGraphic {
		string title;
		int width;
		int height;
		bool resizable;
		bool fullscreen;
		bool antialias;
	} graphic;
	//! ��־����
	struct SLog {
		string name;
		bool enable;
		int level;
		bool console;
		bool file;
		bool time;
	} log;
	//! Ĭ��ֵ
	SEngineConfig() {
		directory = "media";
		graphic.title = "AVATAR";
		graphic.width = 960;
		graphic.height = 640;
		graphic.resizable = true;
		graphic.fullscreen = false;
		graphic.antialias = false;
		log.name = "AVATAR.LOG";
		log.enable = false;
		log.level = 0;
		log.console = false;
		log.file = false;
		log.time = false;
	}
};

/**
* @brief ������
*/
class AVATAR_EXPORT CEngine {
public:
	//! Ĭ�Ϲ��캯��
	CEngine();

	//! ��ʼ������
	bool Init(const SEngineConfig& config);
	//! ��������
	void Destroy();
	//! ����
	void Update();
	//! ��Ⱦ
	void Render();
	//! �ж������Ƿ�
	bool IsActive() { return m_bActive; }
	//! �ж������Ƿ�����
	bool IsRunning() { return m_bRunning; }

	//! ���������ٶ�
	void SetTimeSpeed(float speed, bool fixed);
	//! ��ȡ����汾
	int GetVersion(const char** version);
	//! ��ȡʱ����
	float GetTimeSpan();

	//! ��ȡͼ�ι�����ʵ��
	static CGraphicsManager* GetGraphicsManager();
	//! ��ȡ�ļ�������ʵ��
	static CFileManager* GetFileManager();
	//! ��ȡ���������ʵ��
	static CInputManager* GetInputManager();
	//! ��ȡ����������ʵ��
	static CSoundManager* GetSoundManager();
	//! ��ȡ����������ʵ��
	static CSceneManager* GetSceneManager();
	//! ��ȡ���������ʵ��
	static CFontManager* GetFontManager();
	//! ��ȡ���������ʵ��
	static CTextureManager* GetTextureManager();
	//! ��ȡ��ɫ��������ʵ��
	static CShaderManager* GetShaderManager();
	//! ��ȡ���������ʵ��
	static CPhysicsManager* GetPhysicsManager();
	//! ��ȡ�ű�������ʵ��
	static CScriptManager* GetScriptManager();
	//! ��ȡ���������ʵ��
	static CPostProcessManager* GetPostProcessManager();
	//! ��ȡ����������ʵ��
	static CAnimationManager* GetAnimationManager();

public:
	//! �¼�֪ͨ�ӿ�
	class CNotifyHandler {
	public:
		//! ����������
		virtual ~CNotifyHandler() {}
		//! �¼�֪ͨ����
		virtual void Handle() = 0;
	};

	//! ע���¼��ص�
	bool RegisterEvent(const string& eventType, CNotifyHandler* handle);
	//! ȡ���¼�ע��
	void UnRegisterEvent(const string& eventType, CNotifyHandler* handle, bool release);

private:
	//! ��ȡ�豸ʵ��
	CDevice* GetDevice();
	//! ����ϵͳ��ɫ��
	void LoadShader();
	//! ����ϵͳ���
	void LoadPlugin();

private:
	//! �Ƿ�״̬
	bool m_bActive;
	//! �Ƿ�����״̬
	bool m_bRunning;
	//! �Ƿ�̶���ʱ�䲽��
	bool m_bFixedTimeSpan;
	//! ��������ʱ�䲽��
	float m_fTimeSpan;
	//! ����ʱ���ٶ�
	float m_fSpeedOfTime;
	//! �����¼�֪ͨ�����б�
	std::list<CNotifyHandler*> m_lstUpdateListener;
	//! ��Ⱦ�¼�֪ͨ�����б�
	std::list<CNotifyHandler*> m_lstRenderListener;
};

#endif