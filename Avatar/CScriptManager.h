//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCRIPTMANAGER_H_
#define _CSCRIPTMANAGER_H_
#include "AvatarConfig.h"
#include "CScriptContext.h"
#include "CVector3.h"
#include <string>
#include <map>
#include <vector>
#include <queue>
using std::string;
using std::map;
using std::vector;
using std::queue;
typedef struct lua_State lua_State;

/**
* @brief 脚本管理器
*/
class AVATAR_EXPORT CScriptManager {
public:
	//! 获取管理器实例
	static CScriptManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CScriptManager();
		return m_pInstance;
	}
	//! 实例销毁
	void Destroy();
	//! 打开入口脚本
	void OpenScript(const string& script, void* engine);
	//! 获取接口上下文
	CScriptContext* GetContext(lua_State* lua);

	//! 初始化完成事件
	void OnReady();
	//! 逻辑更新事件
	void OnUpdate(float dt);
	//! 引擎退出事件
	void OnExit();
	//! 窗体大小改变事件
	void OnSize(int width, int height);
	//! 输入事件
	void OnInput(const string& name, int value, int arg1, int arg2, int arg3);
	//! 输入事件
	void OnInput(const string& name, int value, float arg1, float arg2, float arg3);

	//! 脚本命令
	void Script(const char* script);
	//! 注册方法回调
	void Register(const string& function, void* callback);
	//! GUI界面事件
	void GuiEvent(const string& name, int evt, int arg1, int arg2);
	//! 物理碰撞事件
	void CollideEnter(int callback, const string& name, const CVector3& pos);
	//! 物理碰撞取消事件
	void CollideLeave(int callback, const string& name);
	//! 处理事件
	void HandleEvent();

private:
	CScriptManager();
	~CScriptManager();

	//! 注册脚本调用接口
	void RegisterInterface(lua_State* lua);

	//! 系统接口
	static int DoEngineInfo(lua_State* lua);
	static int DoEngineFps(lua_State* lua);
	static int DoEngineLog(lua_State* lua);
	static int DoEngineSpeed(lua_State* lua);
	static int DoEngineTimer(lua_State* lua);
	static int DoEngineDirectory(lua_State* lua);
	static int DoEngineInput(lua_State* lua);
	static int DoEngineScript(lua_State* lua);
	static int DoEngineExecute(lua_State* lua);
	static int DoEngineRead(lua_State* lua);
	static int DoEngineWrite(lua_State* lua);
	static int DoEnginePlugin(lua_State* lua);
	//! GUI 接口
	static int DoGuiEnable(lua_State* lua);
	static int DoGuiSize(lua_State* lua);
	static int DoGuiScale(lua_State* lua);
	static int DoGuiCreate(lua_State* lua);
	static int DoGuiModify(lua_State* lua);
	static int DoGuiDelete(lua_State* lua);
	static int DoGuiAttrib(lua_State* lua);
	//! 相机接口
	static int DoCameraType(lua_State* lua);
	static int DoCameraControl(lua_State* lua);
	static int DoCameraFov(lua_State* lua);
	static int DoCameraClip(lua_State* lua);
	static int DoCameraPosition(lua_State* lua);
	static int DoCameraTarget(lua_State* lua);
	static int DoCameraAngle(lua_State* lua);
	static int DoCameraDirection(lua_State* lua);
	static int DoCameraBind(lua_State* lua);
	//! 场景节点接口
	static int DoSceneInsert(lua_State* lua);
	static int DoSceneDelete(lua_State* lua);
	static int DoSceneClear(lua_State* lua);
	static int DoSceneList(lua_State* lua);
	static int DoScenePick(lua_State* lua);
	static int DoSceneScale(lua_State* lua);
	static int DoSceneAngle(lua_State* lua);
	static int DoSceneOrientation(lua_State* lua);
	static int DoScenePosition(lua_State* lua);
	static int DoSceneVisible(lua_State* lua);
	static int DoSceneUpdate(lua_State* lua);
	static int DoSceneVertex(lua_State* lua);
	static int DoSceneMaterial(lua_State* lua);
	static int DoSceneRenderMode(lua_State* lua);
	static int DoSceneBoundingBox(lua_State* lua);
	static int DoSceneHandle(lua_State* lua);
	//! 后处理接口
	static int DoPostList(lua_State* lua);
	static int DoPostEnable(lua_State* lua);
	static int DoPostRegister(lua_State* lua);
	static int DoPostParam(lua_State* lua);
	//! 图形接口
	static int DoGraphicsScreenshot(lua_State* lua);
	static int DoGraphicsStereo(lua_State* lua);
	static int DoGraphicsWindowSize(lua_State* lua);
	static int DoGraphicsLightPosition(lua_State* lua);
	static int DoGraphicsDirectionLight(lua_State* lua);
	static int DoGraphicsPointLight(lua_State* lua);
	static int DoGraphicsSpotLight(lua_State* lua);
	static int DoGraphicsShadow(lua_State* lua);
	static int DoGraphicsFog(lua_State* lua);
	static int DoGraphicsEnvironmentMap(lua_State* lua);
	static int DoGraphicsBackground(lua_State* lua);
	static int DoGraphicsPickingRay(lua_State* lua);
	static int DoGraphicsProject(lua_State* lua);
	static int DoGraphicsRenderTarget(lua_State* lua);
	//! 纹理接口
	static int DoTextureCreate(lua_State* lua);
	static int DoTextureDelete(lua_State* lua);
	static int DoTextureUpdate(lua_State* lua);
	//! 着色器接口
	static int DoShaderCreate(lua_State* lua);
	static int DoShaderDelete(lua_State* lua);
	static int DoShaderUpdate(lua_State* lua);
	static int DoShaderParam(lua_State* lua);
	//! 字体接口
	static int DoFontList(lua_State* lua);
	static int DoFontLoad(lua_State* lua);
	static int DoFontClear(lua_State* lua);
	static int DoFontUse(lua_State* lua);
	//! 声音接口
	static int DoSoundCreate(lua_State* lua);
	static int DoSoundDelete(lua_State* lua);
	static int DoSoundPlay(lua_State* lua);
	static int DoSoundPause(lua_State* lua);
	static int DoSoundStop(lua_State* lua);
	static int DoSoundPosition(lua_State* lua);
	static int DoSoundVolume(lua_State* lua);
	static int DoSoundUpdate(lua_State* lua);
	//! 物理引擎接口
	static int DoPhysicsBind(lua_State* lua);
	static int DoPhysicsUnbind(lua_State* lua);
	static int DoPhysicsCollide(lua_State* lua);
	static int DoPhysicsReset(lua_State* lua);
	static int DoPhysicsVelocity(lua_State* lua);
	static int DoPhysicsApplyForce(lua_State* lua);
	static int DoPhysicsApplyImpulse(lua_State* lua);
	static int DoPhysicsGravity(lua_State* lua);
	static int DoPhysicsJoint(lua_State* lua);
	//! 动画接口
	static int DoAnimationParam(lua_State* lua);
	static int DoAnimationScale(lua_State* lua);
	static int DoAnimationRotation(lua_State* lua);
	static int DoAnimationTranslation(lua_State* lua);
	static int DoAnimationStart(lua_State* lua);
	static int DoAnimationStop(lua_State* lua);

private:
	//! GUI事件定义
	typedef struct _SGuiEvent {
		int callback;
		int event;
		int arg1;
		int arg2;
		_SGuiEvent(int callback, int event, int arg1, int arg2) {
			this->callback = callback;
			this->event = event;
			this->arg1 = arg1;
			this->arg2 = arg2;
		}
	} SGuiEvent;

private:
	//! Lua 虚拟机
	lua_State* m_pLuaState;
	//! 脚本接口上下文
	CScriptContext* m_pContext;
	//! 自定义方法回调
	map<string, void*> m_mapFunction;
	//! GUI 界面事件绑定
	map<string, int> m_mapGuiEvent;
	//! GUI 事件队列
	vector<SGuiEvent> m_vecEventQueue;
	//! 脚本命令队列
	queue<string> m_queScriptQueue;
	//! 脚本管理器实例
	static CScriptManager* m_pInstance;
};

#endif