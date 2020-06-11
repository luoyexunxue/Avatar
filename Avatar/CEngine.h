//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
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
* @brief 引擎配置
*/
struct SEngineConfig {
	//! 资源目录
	string directory;
	//! 图形配置
	struct SGraphic {
		string title;
		int width;
		int height;
		bool resizable;
		bool fullscreen;
		bool antialias;
	} graphic;
	//! 日志配置
	struct SLog {
		string name;
		bool enable;
		int level;
		bool console;
		bool file;
		bool time;
	} log;
	//! 默认值
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
* @brief 引擎类
*/
class AVATAR_EXPORT CEngine {
public:
	//! 默认构造函数
	CEngine();

	//! 初始化引擎
	bool Init(const SEngineConfig& config);
	//! 销毁引擎
	void Destroy();
	//! 更新
	void Update();
	//! 渲染
	void Render();
	//! 判断引擎是否活动
	bool IsActive() { return m_bActive; }
	//! 判断引擎是否运行
	bool IsRunning() { return m_bRunning; }

	//! 设置运行速度
	void SetTimeSpeed(float speed, bool fixed);
	//! 获取引擎版本
	int GetVersion(const char** version);
	//! 获取时间间隔
	float GetTimeSpan();

	//! 获取图形管理器实例
	static CGraphicsManager* GetGraphicsManager();
	//! 获取文件管理器实例
	static CFileManager* GetFileManager();
	//! 获取输入管理器实例
	static CInputManager* GetInputManager();
	//! 获取声音管理器实例
	static CSoundManager* GetSoundManager();
	//! 获取场景管理器实例
	static CSceneManager* GetSceneManager();
	//! 获取字体管理器实例
	static CFontManager* GetFontManager();
	//! 获取纹理管理器实例
	static CTextureManager* GetTextureManager();
	//! 获取着色器管理器实例
	static CShaderManager* GetShaderManager();
	//! 获取物理管理器实例
	static CPhysicsManager* GetPhysicsManager();
	//! 获取脚本管理器实例
	static CScriptManager* GetScriptManager();
	//! 获取后处理管理器实例
	static CPostProcessManager* GetPostProcessManager();
	//! 获取动画管理器实例
	static CAnimationManager* GetAnimationManager();

public:
	//! 事件通知接口
	class CNotifyHandler {
	public:
		//! 虚析构函数
		virtual ~CNotifyHandler() {}
		//! 事件通知方法
		virtual void Handle() = 0;
	};

	//! 注册事件回调
	bool RegisterEvent(const string& eventType, CNotifyHandler* handle);
	//! 取消事件注册
	void UnRegisterEvent(const string& eventType, CNotifyHandler* handle, bool release);

private:
	//! 获取设备实例
	CDevice* GetDevice();
	//! 加载系统着色器
	void LoadShader();
	//! 加载系统插件
	void LoadPlugin();

private:
	//! 是否活动状态
	bool m_bActive;
	//! 是否运行状态
	bool m_bRunning;
	//! 是否固定的时间步长
	bool m_bFixedTimeSpan;
	//! 引擎运行时间步长
	float m_fTimeSpan;
	//! 引擎时间速度
	float m_fSpeedOfTime;
	//! 更新事件通知监听列表
	std::list<CNotifyHandler*> m_lstUpdateListener;
	//! 渲染事件通知监听列表
	std::list<CNotifyHandler*> m_lstRenderListener;
};

#endif