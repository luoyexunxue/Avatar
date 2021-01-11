//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CScriptManager.h"
#include "AvatarConfig.h"
#include "CEngine.h"
#include "CTimer.h"
#include "CLog.h"
#include "CStringUtil.h"
#include "CPluginLoader.h"
#include "CCameraGaze.h"
#include "CCameraFree.h"
#include "CCameraSmooth.h"
#include "CCameraViewer.h"
#include "CCameraGeographic.h"
#include "CGuiEnvironment.h"
#include "CSceneNodeGeometry.h"
#include "CSceneNodeSkybox.h"
#include "CSceneNodeCloud.h"
#include "CSceneNodeStatic.h"
#include "CSceneNodeAnimation.h"
#include "CSceneNodeSound.h"
#include "CSceneNodeFlame.h"
#include "CSceneNodeBoard.h"
#include "CSceneNodeWater.h"
#include "CSceneNodeTerrain.h"
#include "CSceneNodeFresnel.h"
#include "CSceneNodeScreen.h"
#include "CSceneNodeAxis.h"
#include "CSceneNodeLensflare.h"
#include "CSceneNodePlanet.h"
#include "CSceneNodePlant.h"
#include "CSceneNodeParticles.h"
#include "CSceneNodeDecal.h"
#include "CSceneNodeBlast.h"
#include "CSceneNodeText.h"
#include "CSceneNodeLine.h"
#include "CPostProcessUser.h"
#include "thirdparty/lua/lua.hpp"
#include <cstring>

#ifdef AVATAR_WINDOWS
#pragma comment(lib, "thirdparty/lua/lua.lib")
#endif

/**
* 定义表操作
*/
#define TABLE_BEGIN() lua_newtable(lua)
#define TABLE_FUNCTION(NAME, FUNC) lua_pushstring(lua, NAME);lua_pushcfunction(lua, FUNC);lua_rawset(lua, -3)
#define TABLE_END(NAME) lua_setglobal(lua, NAME)

/**
* 获取表中字段值
*/
inline bool TableValue(lua_State* lua, int table, const char* name, bool def) {
	lua_getfield(lua, table, name);
	return lua_isboolean(lua, -1)? lua_toboolean(lua, -1) != 0: def;
}
inline int TableValue(lua_State* lua, int table, const char* name, int def) {
	lua_getfield(lua, table, name);
	return lua_isinteger(lua, -1)? (int)lua_tointeger(lua, -1): def;
}
inline float TableValue(lua_State* lua, int table, const char* name, float def) {
	lua_getfield(lua, table, name);
	return lua_isnumber(lua, -1)? (float)lua_tonumber(lua, -1): def;
}
inline const char* TableValue(lua_State* lua, int table, const char* name, const char* def) {
	lua_getfield(lua, table, name);
	return lua_isstring(lua, -1)? (const char*)lua_tostring(lua, -1): def;
}

/**
* 构造函数
*/
CScriptManager::CScriptManager() {
	m_pContext = new CScriptContext();
	m_pLuaState = luaL_newstate();
	luaL_openlibs(m_pLuaState);
	RegisterInterface(m_pLuaState);
}

/**
* 析构函数
*/
CScriptManager::~CScriptManager() {
	m_pInstance = 0;
}

/**
* 单例实例
*/
CScriptManager* CScriptManager::m_pInstance = 0;

/**
* 销毁脚本管理器实例
*/
void CScriptManager::Destroy() {
	lua_close(m_pLuaState);
	delete m_pContext;
	delete this;
}

/**
* 打开入口脚本
*/
void CScriptManager::OpenScript(const string& script, void* engine) {
	CFileManager::CBinaryFile file;
	CEngine::GetFileManager()->ReadFile(script, &file);
	// 跳过 UTF8 的 BOM
	int offset = 0;
	const unsigned char* header = file.contents;
	if (file.size >= 3 && header[0] == 0xEF && header[1] == 0xBB && header[2] == 0xBF) {
		offset = 3;
	}
	if (luaL_loadbuffer(m_pLuaState, (char*)file.contents + offset, file.size - offset, script.c_str()) ||
		lua_pcall(m_pLuaState, 0, LUA_MULTRET, 0)) {
		CLog::Error("Open script file '%s' error", script.c_str());
		CLog::Error(lua_tostring(m_pLuaState, -1));
	}
	// 记录引擎对象指针
	lua_pushstring(m_pLuaState, "ENGINE_INSTANCE");
	lua_pushlightuserdata(m_pLuaState, engine);
	lua_settable(m_pLuaState, LUA_REGISTRYINDEX);
}

/**
* 获取接口上下文
*/
CScriptContext* CScriptManager::GetContext(lua_State* lua) {
	if (!lua) lua = m_pLuaState;
	return m_pContext->GetCurrent(lua);
}

/**
* 初始化完成事件
*/
void CScriptManager::OnReady() {
	lua_getglobal(m_pLuaState, "OnReady");
	lua_pcall(m_pLuaState, 0, 0, 0);
}

/**
* 更新内部逻辑
*/
void CScriptManager::OnUpdate(float dt) {
	// 脚本命令执行
	while (!m_queScriptQueue.empty()) {
		string script = std::move(m_queScriptQueue.front());
		m_queScriptQueue.pop();
		if (LUA_OK != luaL_dostring(m_pLuaState, script.c_str())) {
			CLog::Warn("Execute script '%s' error", script.c_str());
			CLog::Warn(lua_tostring(m_pLuaState, -1));
		}
	}
	// 调用 OnUpdate 脚本函数
	lua_getglobal(m_pLuaState, "OnUpdate");
	lua_pushnumber(m_pLuaState, dt);
	lua_pcall(m_pLuaState, 1, 0, 0);
}

/**
* 引擎退出事件
*/
void CScriptManager::OnExit() {
	lua_getglobal(m_pLuaState, "OnExit");
	lua_pcall(m_pLuaState, 0, 0, 0);
}

/**
* 窗体大小改变事件
*/
void CScriptManager::OnSize(int width, int height) {
	lua_getglobal(m_pLuaState, "OnSize");
	lua_pushinteger(m_pLuaState, width);
	lua_pushinteger(m_pLuaState, height);
	lua_pcall(m_pLuaState, 2, 0, 0);
}

/**
* 输入事件
*/
void CScriptManager::OnInput(const string& name, int value, int arg1, int arg2, int arg3) {
	lua_getglobal(m_pLuaState, "OnInput");
	lua_pushstring(m_pLuaState, name.c_str());
	lua_pushinteger(m_pLuaState, value);
	lua_pushinteger(m_pLuaState, arg1);
	lua_pushinteger(m_pLuaState, arg2);
	lua_pushinteger(m_pLuaState, arg3);
	lua_pcall(m_pLuaState, 5, 0, 0);
}

/**
* 输入事件
*/
void CScriptManager::OnInput(const string& name, int value, float arg1, float arg2, float arg3) {
	lua_getglobal(m_pLuaState, "OnInput");
	lua_pushstring(m_pLuaState, name.c_str());
	lua_pushinteger(m_pLuaState, value);
	lua_pushnumber(m_pLuaState, arg1);
	lua_pushnumber(m_pLuaState, arg2);
	lua_pushnumber(m_pLuaState, arg3);
	lua_pcall(m_pLuaState, 5, 0, 0);
}

/**
* 脚本命令
*/
void CScriptManager::Script(const char* script) {
	m_queScriptQueue.push(script);
	CLog::Info("[script] %s", script);
}

/**
* 注册自定义方法回调
* @remark 参数 callback 方法原型为 int func(const char*)
*/
void CScriptManager::Register(const string& function, void* callback) {
	if (callback) m_mapFunction[function] = callback;
	else m_mapFunction.erase(function);
}

/**
* GUI 界面事件
*/
void CScriptManager::GuiEvent(const string& name, int evt, int arg1, int arg2) {
	map<string, int>::iterator iter = m_mapGuiEvent.find(name);
	if (iter != m_mapGuiEvent.end()) {
		m_vecEventQueue.push_back(SGuiEvent(iter->second, evt, arg1, arg2));
	}
}

/**
* 物理碰撞事件
*/
void CScriptManager::CollideEnter(int callback, const string& name, const CVector3& pos) {
	lua_rawgeti(m_pLuaState, LUA_REGISTRYINDEX, callback);
	lua_pushboolean(m_pLuaState, 1);
	lua_pushstring(m_pLuaState, name.c_str());
	lua_pushnumber(m_pLuaState, pos.m_fValue[0]);
	lua_pushnumber(m_pLuaState, pos.m_fValue[1]);
	lua_pushnumber(m_pLuaState, pos.m_fValue[2]);
	lua_pcall(m_pLuaState, 5, 0, 0);
}

/**
* 物理碰撞取消事件
*/
void CScriptManager::CollideLeave(int callback, const string& name) {
	lua_rawgeti(m_pLuaState, LUA_REGISTRYINDEX, callback);
	lua_pushboolean(m_pLuaState, 0);
	lua_pushstring(m_pLuaState, name.c_str());
	lua_pcall(m_pLuaState, 2, 0, 0);
}

/**
* GUI 脚本事件回调
*/
void CScriptManager::HandleEvent() {
	for (size_t i = 0; i < m_vecEventQueue.size(); i++) {
		const SGuiEvent& evt = m_vecEventQueue[i];
		lua_rawgeti(m_pLuaState, LUA_REGISTRYINDEX, evt.callback);
		lua_pushinteger(m_pLuaState, evt.event);
		lua_pushinteger(m_pLuaState, evt.arg1);
		lua_pushinteger(m_pLuaState, evt.arg2);
		lua_pcall(m_pLuaState, 3, 0, 0);
	}
	m_vecEventQueue.clear();
}

/**
* 注册脚本调用接口
*/
void CScriptManager::RegisterInterface(lua_State* lua) {
	// 系统接口
	TABLE_BEGIN();
	TABLE_FUNCTION("info", DoEngineInfo);
	TABLE_FUNCTION("fps", DoEngineFps);
	TABLE_FUNCTION("log", DoEngineLog);
	TABLE_FUNCTION("speed", DoEngineSpeed);
	TABLE_FUNCTION("timer", DoEngineTimer);
	TABLE_FUNCTION("directory", DoEngineDirectory);
	TABLE_FUNCTION("input", DoEngineInput);
	TABLE_FUNCTION("script", DoEngineScript);
	TABLE_FUNCTION("execute", DoEngineExecute);
	TABLE_FUNCTION("read", DoEngineRead);
	TABLE_FUNCTION("write", DoEngineWrite);
	TABLE_FUNCTION("plugin", DoEnginePlugin);
	TABLE_END("engine");
	// GUI接口
	TABLE_BEGIN();
	TABLE_FUNCTION("enable", DoGuiEnable);
	TABLE_FUNCTION("size", DoGuiSize);
	TABLE_FUNCTION("scale", DoGuiScale);
	TABLE_FUNCTION("create", DoGuiCreate);
	TABLE_FUNCTION("modify", DoGuiModify);
	TABLE_FUNCTION("delete", DoGuiDelete);
	TABLE_FUNCTION("attrib", DoGuiAttrib);
	TABLE_END("gui");
	// 相机接口
	TABLE_BEGIN();
	TABLE_FUNCTION("type", DoCameraType);
	TABLE_FUNCTION("control", DoCameraControl);
	TABLE_FUNCTION("fov", DoCameraFov);
	TABLE_FUNCTION("clip", DoCameraClip);
	TABLE_FUNCTION("projection", DoCameraProjection);
	TABLE_FUNCTION("position", DoCameraPosition);
	TABLE_FUNCTION("target", DoCameraTarget);
	TABLE_FUNCTION("angle", DoCameraAngle);
	TABLE_FUNCTION("direction", DoCameraDirection);
	TABLE_FUNCTION("bind", DoCameraBind);
	TABLE_END("camera");
	// 场景节点接口
	TABLE_BEGIN();
	TABLE_FUNCTION("insert", DoSceneInsert);
	TABLE_FUNCTION("delete", DoSceneDelete);
	TABLE_FUNCTION("clear", DoSceneClear);
	TABLE_FUNCTION("list", DoSceneList);
	TABLE_FUNCTION("pick", DoScenePick);
	TABLE_FUNCTION("scale", DoSceneScale);
	TABLE_FUNCTION("angle", DoSceneAngle);
	TABLE_FUNCTION("orientation", DoSceneOrientation);
	TABLE_FUNCTION("position", DoScenePosition);
	TABLE_FUNCTION("visible", DoSceneVisible);
	TABLE_FUNCTION("update", DoSceneUpdate);
	TABLE_FUNCTION("vertex", DoSceneVertex);
	TABLE_FUNCTION("material", DoSceneMaterial);
	TABLE_FUNCTION("renderMode", DoSceneRenderMode);
	TABLE_FUNCTION("boundingBox", DoSceneBoundingBox);
	TABLE_FUNCTION("handle", DoSceneHandle);
	TABLE_END("scene");
	// 后处理接口
	TABLE_BEGIN();
	TABLE_FUNCTION("list", DoPostList);
	TABLE_FUNCTION("enable", DoPostEnable);
	TABLE_FUNCTION("register", DoPostRegister);
	TABLE_FUNCTION("param", DoPostParam);
	TABLE_END("post");
	// 图形管理接口
	TABLE_BEGIN();
	TABLE_FUNCTION("screenshot", DoGraphicsScreenshot);
	TABLE_FUNCTION("stereo", DoGraphicsStereo);
	TABLE_FUNCTION("windowSize", DoGraphicsWindowSize);
	TABLE_FUNCTION("lightPosition", DoGraphicsLightPosition);
	TABLE_FUNCTION("directionLight", DoGraphicsDirectionLight);
	TABLE_FUNCTION("pointLight", DoGraphicsPointLight);
	TABLE_FUNCTION("spotLight", DoGraphicsSpotLight);
	TABLE_FUNCTION("shadow", DoGraphicsShadow);
	TABLE_FUNCTION("fog", DoGraphicsFog);
	TABLE_FUNCTION("environmentMap", DoGraphicsEnvironmentMap);
	TABLE_FUNCTION("background", DoGraphicsBackground);
	TABLE_FUNCTION("pickingRay", DoGraphicsPickingRay);
	TABLE_FUNCTION("project", DoGraphicsProject);
	TABLE_FUNCTION("renderTarget", DoGraphicsRenderTarget);
	TABLE_END("graphics");
	// 纹理管理接口
	TABLE_BEGIN();
	TABLE_FUNCTION("create", DoTextureCreate);
	TABLE_FUNCTION("delete", DoTextureDelete);
	TABLE_FUNCTION("update", DoTextureUpdate);
	TABLE_FUNCTION("data", DoTextureData);
	TABLE_END("texture");
	// 着色器管理接口
	TABLE_BEGIN();
	TABLE_FUNCTION("create", DoShaderCreate);
	TABLE_FUNCTION("delete", DoShaderDelete);
	TABLE_FUNCTION("update", DoShaderUpdate);
	TABLE_FUNCTION("define", DoShaderDefine);
	TABLE_FUNCTION("param", DoShaderParam);
	TABLE_END("shader");
	// 字体管理接口
	TABLE_BEGIN();
	TABLE_FUNCTION("list", DoFontList);
	TABLE_FUNCTION("load", DoFontLoad);
	TABLE_FUNCTION("clear", DoFontClear);
	TABLE_FUNCTION("use", DoFontUse);
	TABLE_END("font");
	// 声音管理接口
	TABLE_BEGIN();
	TABLE_FUNCTION("create", DoSoundCreate);
	TABLE_FUNCTION("delete", DoSoundDelete);
	TABLE_FUNCTION("play", DoSoundPlay);
	TABLE_FUNCTION("pause", DoSoundPause);
	TABLE_FUNCTION("stop", DoSoundStop);
	TABLE_FUNCTION("position", DoSoundPosition);
	TABLE_FUNCTION("volume", DoSoundVolume);
	TABLE_FUNCTION("update", DoSoundUpdate);
	TABLE_END("sound");
	// 物理引擎接口
	TABLE_BEGIN();
	TABLE_FUNCTION("bind", DoPhysicsBind);
	TABLE_FUNCTION("unbind", DoPhysicsUnbind);
	TABLE_FUNCTION("collide", DoPhysicsCollide);
	TABLE_FUNCTION("reset", DoPhysicsReset);
	TABLE_FUNCTION("velocity", DoPhysicsVelocity);
	TABLE_FUNCTION("applyForce", DoPhysicsApplyForce);
	TABLE_FUNCTION("applyImpulse", DoPhysicsApplyImpulse);
	TABLE_FUNCTION("gravity", DoPhysicsGravity);
	TABLE_FUNCTION("joint", DoPhysicsJoint);
	TABLE_END("physics");
	// 动画接口
	TABLE_BEGIN();
	TABLE_FUNCTION("scale", DoAnimationScale);
	TABLE_FUNCTION("rotation", DoAnimationRotation);
	TABLE_FUNCTION("translation", DoAnimationTranslation);
	TABLE_FUNCTION("start", DoAnimationStart);
	TABLE_FUNCTION("stop", DoAnimationStop);
	TABLE_END("animation");
}

/**
* 获取引擎信息
*/
int CScriptManager::DoEngineInfo(lua_State* lua) {
#ifdef AVATAR_WINDOWS
	const char* platform = "windows";
#elif defined AVATAR_LINUX
	const char* platform = "linux";
#elif defined AVATAR_ANDROID
	const char* platform = "android";
#else
	#error "No platform defined."
#endif
	lua_newtable(lua);
	lua_pushstring(lua, "version");
	lua_pushstring(lua, AVATAR_VERSION);
	lua_settable(lua, -3);
	lua_pushstring(lua, "platform");
	lua_pushstring(lua, platform);
	lua_settable(lua, -3);
	return 1;
}

/**
* 获取当前帧率
*/
int CScriptManager::DoEngineFps(lua_State* lua) {
	lua_pushnumber(lua, CEngine::GetGraphicsManager()->GetFrameRate());
	return 1;
}

/**
* 输出日志消息
*/
int CScriptManager::DoEngineLog(lua_State* lua) {
	string info;
	const int args = lua_gettop(lua);
	for (int n = 1; n <= args; n++) {
		if (lua_isboolean(lua, n)) info.append(lua_toboolean(lua, n) ? "true" : "false");
		else if (lua_isstring(lua, n)) info.append(lua_tostring(lua, n));
		else if (lua_istable(lua, n)) {
			int count = 0;
			info.append("{");
			lua_pushnil(lua);
			while (lua_next(lua, n)) {
				if (lua_type(lua, -2) == LUA_TNUMBER) info.append(" [").append(std::to_string(lua_tointeger(lua, -2))).append("]=");
				if (lua_type(lua, -2) == LUA_TSTRING) info.append(" ").append(lua_tostring(lua, -2)).append("=");
				int valueType = lua_type(lua, -1);
				if (valueType == LUA_TBOOLEAN) info.append(lua_toboolean(lua, -1) != 0 ? "true" : "false");
				else if (valueType == LUA_TNUMBER) info.append(lua_tostring(lua, -1));
				else if (valueType == LUA_TSTRING) info.append("'").append(lua_tostring(lua, -1)).append("'");
				else info.append(lua_typename(lua, lua_type(lua, -1)));
				info.append(",");
				lua_pop(lua, 1);
				count += 1;
			}
			if (count > 0) info.pop_back();
			info.append(" }");
		} else info.append(lua_typename(lua, lua_type(lua, n)));
		if (n != args) info.append(", ");
	}
	CLog::Debug("%s", info.c_str());
	return 0;
}

/**
* 设置运行速度
*/
int CScriptManager::DoEngineSpeed(lua_State* lua) {
	if (lua_isnumber(lua, 1)) {
		float speed = (float)lua_tonumber(lua, 1);
		bool fixed = lua_isboolean(lua, 2) ? lua_toboolean(lua, 2) != 0 : false;
		lua_pushstring(lua, "ENGINE_INSTANCE");
		lua_gettable(lua, LUA_REGISTRYINDEX);
		CEngine* pEngine = (CEngine*)lua_touserdata(lua, -1);
		lua_pop(lua, 1);
		pEngine->SetTimeSpeed(speed, fixed);
	}
	return 0;
}

/**
* 获取定时器定时
*/
int CScriptManager::DoEngineTimer(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		bool reset = lua_isboolean(lua, 2) ? lua_toboolean(lua, 2) != 0 : true;
		lua_pushnumber(lua, CTimer::Reset(lua_tostring(lua, 1), reset));
		return 1;
	}
	return 0;
}

/**
* 获取或设置数据目录
*/
int CScriptManager::DoEngineDirectory(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CEngine::GetFileManager()->SetDataDirectory(lua_tostring(lua, 1));
		return 0;
	}
	lua_pushstring(lua, CEngine::GetFileManager()->GetDataDirectory().c_str());
	return 1;
}

/**
* 输入控制信号
*/
int CScriptManager::DoEngineInput(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CInputManager* inputMgr = CEngine::GetInputManager();
		const char* method = lua_tostring(lua, 1);
		if (!strcmp(method, "fire")) inputMgr->Fire();
		else if (!strcmp(method, "jump")) inputMgr->Jump();
		else if (!strcmp(method, "start")) inputMgr->Start();
		else if (!strcmp(method, "pause")) inputMgr->Pause();
		else if (!strcmp(method, "quit")) inputMgr->Quit();
		else if (!strcmp(method, "function")) {
			if (lua_isinteger(lua, 2)) {
				inputMgr->Function((int)lua_tointeger(lua, 2));
			}
		} else if (!strcmp(method, "move")) {
			if (lua_isnumber(lua, 2) && lua_isnumber(lua, 3) && lua_isnumber(lua, 4)) {
				inputMgr->RightLeft((float)lua_tonumber(lua, 2));
				inputMgr->ForthBack((float)lua_tonumber(lua, 3));
				inputMgr->UpDown((float)lua_tonumber(lua, 4));
			}
		} else if (!strcmp(method, "turn")) {
			if (lua_isnumber(lua, 2) && lua_isnumber(lua, 3) && lua_isnumber(lua, 4)) {
				inputMgr->Yaw((float)lua_tonumber(lua, 2));
				inputMgr->Pitch((float)lua_tonumber(lua, 3));
				inputMgr->Roll((float)lua_tonumber(lua, 4));
			}
		} else if (!strcmp(method, "map")) {
			if (lua_isinteger(lua, 2) && lua_isinteger(lua, 3) && lua_isinteger(lua, 4)) {
				int left = (int)lua_tointeger(lua, 2);
				int right = (int)lua_tointeger(lua, 3);
				int middle = (int)lua_tointeger(lua, 4);
				inputMgr->MapMouseButton(left, right, middle);
			}
		} else if (!strcmp(method, "scale")) {
			if (lua_isnumber(lua, 2) && lua_isnumber(lua, 3)) {
				float move = (float)lua_tonumber(lua, 2);
				float turn = (float)lua_tonumber(lua, 3);
				inputMgr->MouseSensitivity(true, move, turn);
			}
		} else if (!strcmp(method, "disable")) {
			if (lua_isboolean(lua, 2) && lua_isboolean(lua, 3) && lua_isboolean(lua, 4)) {
				bool mouse = lua_toboolean(lua, 2) != 0;
				bool keyboard = lua_toboolean(lua, 3) != 0;
				bool gravity = lua_toboolean(lua, 4) != 0;
				inputMgr->DisableInput(mouse, keyboard, gravity);
			}
		} else if (!strcmp(method, "mouse")) {
			if (lua_isinteger(lua, 2) && lua_isinteger(lua, 3) && lua_isinteger(lua, 4) && lua_isinteger(lua, 5)) {
				int x = (int)lua_tointeger(lua, 2);
				int y = (int)lua_tointeger(lua, 3);
				int button = (int)lua_tointeger(lua, 4);
				int delta = (int)lua_tointeger(lua, 5);
				inputMgr->MouseInput(x, y, button, delta);
			}
		} else if (!strcmp(method, "key")) {
			if (lua_isinteger(lua, 2)) {
				inputMgr->KeyboardInput((int)lua_tointeger(lua, 2));
			}
		} else if (!strcmp(method, "gravity")) {
			if (lua_isnumber(lua, 2) && lua_isnumber(lua, 3) && lua_isnumber(lua, 4)) {
				float sx = (float)lua_tonumber(lua, 2);
				float sy = (float)lua_tonumber(lua, 3);
				float sz = (float)lua_tonumber(lua, 4);
				inputMgr->GravityScale(sx, sy, sz);
			}
		}
	}
	return 0;
}

/**
* 执行 lua 脚本
*/
int CScriptManager::DoEngineScript(lua_State* lua) {
	bool success = false;
	if (lua_isstring(lua, 1)) {
		string filename = lua_tostring(lua, 1);
		CFileManager::CBinaryFile file;
		if (!CEngine::GetFileManager()->ReadFile(filename, &file) && !CFileManager::IsFullPath(filename)) {
			lua_getglobal(lua, "__engine_script_path__");
			if (lua_isstring(lua, -1)) {
				filename = lua_tostring(lua, -1) + filename;
				CEngine::GetFileManager()->ReadFile(filename, &file);
			}
			lua_pop(lua, 1);
		}
		string path = CFileManager::GetDirectory(filename);
		string setpath = CStringUtil::Format("__engine_script_path__ = '%s'", path.c_str());
		luaL_dostring(lua, setpath.c_str());
		// 跳过 UTF8 的 BOM
		int offset = 0;
		const unsigned char* header = file.contents;
		if (file.size >= 3 && header[0] == 0xEF && header[1] == 0xBB && header[2] == 0xBF) offset = 3;
		if (luaL_loadbuffer(lua, (char*)file.contents + offset, file.size - offset, filename.c_str()) ||
			lua_pcall(lua, 0, LUA_MULTRET, 0)) {
			CLog::Error("Open script file '%s' error", filename.c_str());
			CLog::Error(lua_tostring(lua, -1));
		} else success = true;
	}
	lua_pushboolean(lua, success);
	return 1;
}

/**
* 执行回调事件
*/
int CScriptManager::DoEngineExecute(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		string function = lua_tostring(lua, 1);
		const char* argument = lua_isstring(lua, 2) ? lua_tostring(lua, 2) : 0;
		map<string, void*>::iterator iter = m_pInstance->m_mapFunction.find(function);
		if (iter != m_pInstance->m_mapFunction.end()) {
			typedef int(*pfn_callback)(const char*);
			lua_pushinteger(lua, reinterpret_cast<pfn_callback>(iter->second)(argument));
			return 1;
		}
		CLog::Warn("Function '%s' not exist", function.c_str());
		lua_pushnil(lua);
		return 1;
	}
	return 0;
}

/**
* 读取指定文件数据
*/
int CScriptManager::DoEngineRead(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CFileManager::CBinaryFile file;
		if (CEngine::GetFileManager()->ReadFile(lua_tostring(lua, 1), &file)) {
			lua_pushlstring(lua, (char*)file.contents, file.size);
			return 1;
		}
	}
	lua_pushnil(lua);
	return 1;
}

/**
* 写入数据至指定文件
*/
int CScriptManager::DoEngineWrite(lua_State* lua) {
	if (lua_isstring(lua, 1) && !lua_isnoneornil(lua, 2)) {
		size_t length = 0;
		const char* content = lua_tolstring(lua, 2, &length);
		CFileManager::CBinaryFile file(length);
		memcpy(file.contents, content, file.size);
		CEngine::GetFileManager()->WriteFile(&file, lua_tostring(lua, 1));
	}
	return 0;
}

/**
* 加载引擎插件
*/
int CScriptManager::DoEnginePlugin(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		string path = lua_tostring(lua, 1);
		lua_pushstring(lua, "ENGINE_INSTANCE");
		lua_gettable(lua, LUA_REGISTRYINDEX);
		CEngine* pEngine = (CEngine*)lua_touserdata(lua, -1);
		lua_pop(lua, 1);
		if (CPluginLoader::Load(path, pEngine)) {
			string desc = CPluginLoader::GetDescription(CFileManager::GetFileName(path, false));
			lua_pushstring(lua, desc.c_str());
			return 1;
		}
	}
	return 0;
}

/**
* GUI 使能或禁止
*/
int CScriptManager::DoGuiEnable(lua_State* lua) {
	bool enable = lua_isboolean(lua, 1) ? lua_toboolean(lua, 1) != 0 : true;
	CGuiEnvironment::GetInstance()->SetEnable(enable);
	return 0;
}

/**
* 获取 GUI 画布大小
*/
int CScriptManager::DoGuiSize(lua_State* lua) {
	int width, height;
	CGuiEnvironment::GetInstance()->GetSize(&width, &height);
	lua_pushinteger(lua, width);
	lua_pushinteger(lua, height);
	return 2;
}

/**
* 设置 GUI 缩放
*/
int CScriptManager::DoGuiScale(lua_State* lua) {
	if (lua_isnumber(lua, 1)) {
		float scale = (float)lua_tonumber(lua, 1);
		CGuiEnvironment::GetInstance()->SetScale(scale);
	}
	return 0;
}

/**
* 创建 GUI 元素
*/
int CScriptManager::DoGuiCreate(lua_State* lua) {
	if (lua_isstring(lua, 1) && lua_isstring(lua, 2) && lua_isstring(lua, 3)) {
		string name = lua_tostring(lua, 1);
		string type = lua_tostring(lua, 2);
		string desc = lua_tostring(lua, 3);
		// 添加事件
		if (CGuiEnvironment::GetInstance()->GuiCreate(name, type, desc) && lua_isfunction(lua, 4)) {
			int callback = luaL_ref(lua, LUA_REGISTRYINDEX);
			m_pInstance->m_mapGuiEvent.insert(std::pair<string, int>(name, callback));
		}
	}
	return 0;
}

/**
* 修改 GUI 元素
*/
int CScriptManager::DoGuiModify(lua_State* lua) {
	if (lua_isstring(lua, 1) && lua_isstring(lua, 2)) {
		string name = lua_tostring(lua, 1);
		string desc = lua_tostring(lua, 2);
		CGuiEnvironment::GetInstance()->GuiModify(name, desc);
	}
	return 0;
}

/**
* 删除 GUI 元素
*/
int CScriptManager::DoGuiDelete(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		string name = lua_tostring(lua, 1);
		CGuiEnvironment::GetInstance()->GuiDelete(name);
		// 删除事件
		map<string, int>::iterator iter = m_pInstance->m_mapGuiEvent.find(name);
		if (iter != m_pInstance->m_mapGuiEvent.end()) {
			luaL_unref(lua, LUA_REGISTRYINDEX, iter->second);
			m_pInstance->m_mapGuiEvent.erase(iter);
		}
	}
	return 0;
}

/**
* 获取 GUI 元素属性
*/
int CScriptManager::DoGuiAttrib(lua_State* lua) {
	if (lua_isstring(lua, 1) && lua_isstring(lua, 2)) {
		string name = lua_tostring(lua, 1);
		string attrib = lua_tostring(lua, 2);
		CGuiEnvironment::CGuiElement* element = CGuiEnvironment::GetInstance()->GetElement(name);
		if (element) {
			lua_pushstring(lua, element->GetAttribute(attrib).c_str());
			return 1;
		}
	}
	lua_pushnil(lua);
	return 1;
}

/**
* 获取或设置相机类型
*/
int CScriptManager::DoCameraType(lua_State* lua) {
	CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
	if (lua_isstring(lua, 1)) {
		const char* cameraType = lua_tostring(lua, 1);
		if (pCamera) delete pCamera;
		if (!strcmp(cameraType, "default")) pCamera = new CCamera();
		else if (!strcmp(cameraType, "free")) pCamera = new CCameraFree();
		else if (!strcmp(cameraType, "gaze")) pCamera = new CCameraGaze();
		else if (!strcmp(cameraType, "geographic")) pCamera = new CCameraGeographic();
		else if (!strcmp(cameraType, "smooth")) pCamera = new CCameraSmooth();
		else if (!strcmp(cameraType, "viewer")) pCamera = new CCameraViewer();
		else {
			pCamera = new CCamera();
			CLog::Warn("Unknow camera type '%s'", cameraType);
		}
		if (lua_istable(lua, 2)) {
			float damping = TableValue(lua, 2, "damping", -1.0f);
			float minDistance = TableValue(lua, 2, "minDistance", -1.0f);
			float maxDistance = TableValue(lua, 2, "maxDistance", -1.0f);
			if (damping > 0.0f) {
				if (!strcmp(cameraType, "free")) static_cast<CCameraFree*>(pCamera)->SetDamping(damping);
				else if (!strcmp(cameraType, "gaze")) static_cast<CCameraGaze*>(pCamera)->SetDamping(damping);
				else if (!strcmp(cameraType, "geographic")) static_cast<CCameraGeographic*>(pCamera)->SetDamping(damping);
				else if (!strcmp(cameraType, "smooth")) static_cast<CCameraSmooth*>(pCamera)->SetDamping(damping);
				else if (!strcmp(cameraType, "viewer")) static_cast<CCameraViewer*>(pCamera)->SetDamping(damping);
			}
			if (minDistance > 0.0f) {
				if (!strcmp(cameraType, "gaze")) static_cast<CCameraGaze*>(pCamera)->SetMinDistance(minDistance);
				else if (!strcmp(cameraType, "geographic")) static_cast<CCameraGeographic*>(pCamera)->SetMinDistance(minDistance);
			}
			if (maxDistance > 0.0f) {
				if (!strcmp(cameraType, "gaze")) static_cast<CCameraGaze*>(pCamera)->SetMaxDistance(maxDistance);
				else if (!strcmp(cameraType, "geographic")) static_cast<CCameraGeographic*>(pCamera)->SetMaxDistance(maxDistance);
			}
			if (!strcmp(cameraType, "gaze")) {
				float pitchMin = TableValue(lua, 2, "pitchMin", 3.141593f);
				float pitchMax = TableValue(lua, 2, "pitchMax", -3.141593f);
				if (pitchMin < pitchMax) static_cast<CCameraGaze*>(pCamera)->SetPitchRange(pitchMin, pitchMax);
			} else if (!strcmp(cameraType, "free")) {
				string track_line = TableValue(lua, 2, "track_line", "");
				float track_speed = TableValue(lua, 2, "track_speed", 1.0f);
				bool track_follow = TableValue(lua, 2, "track_follow", true);
				bool track_loop = TableValue(lua, 2, "track_loop", true);
				float offset_x = TableValue(lua, 2, "track_offset_x", 0.0f);
				float offset_y = TableValue(lua, 2, "track_offset_y", 0.0f);
				float offset_z = TableValue(lua, 2, "track_offset_z", 0.0f);
				if (!track_line.empty()) {
					CSceneNodeLine* pLine = static_cast<CSceneNodeLine*>(CEngine::GetSceneManager()->GetNodeByName(track_line));
					if (pLine) {
						vector<CVector3> track;
						for (size_t i = 0; i < pLine->GetPointCount(); i++) track.push_back(pLine->GetPoint(i)->m_fPosition);
						static_cast<CCameraFree*>(pCamera)->StartLineTrack(track, track_speed, track_follow, track_loop);
						static_cast<CCameraFree*>(pCamera)->OffsetLineTrack(CVector3(offset_x, offset_y, offset_z));
					}
				}
			}
		}
		CEngine::GetGraphicsManager()->SetCamera(pCamera);
		return 0;
	}
	lua_pushstring(lua, pCamera ? pCamera->GetName() : "");
	return 1;
}

/**
* 绑定用户输入至相机
*/
int CScriptManager::DoCameraControl(lua_State* lua) {
	bool enable = lua_isboolean(lua, 1) ? lua_toboolean(lua, 1) != 0 : true;
	CEngine::GetGraphicsManager()->GetCamera()->Control(enable);
	return 0;
}

/**
* 获取或设置相机视角
*/
int CScriptManager::DoCameraFov(lua_State* lua) {
	CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
	if (lua_isnumber(lua, 1)) {
		pCamera->SetFieldOfView((float)lua_tonumber(lua, 1));
		return 0;
	}
	lua_pushnumber(lua, pCamera->GetFieldOfView());
	return 1;
}

/**
* 获取或设置相机裁剪面
*/
int CScriptManager::DoCameraClip(lua_State* lua) {
	CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
	if (lua_isnumber(lua, 1) && lua_isnumber(lua, 2)) {
		float zNear = (float)lua_tonumber(lua, 1);
		float zFar = (float)lua_tonumber(lua, 2);
		pCamera->SetClipDistance(zNear, zFar);
		return 0;
	}
	lua_pushnumber(lua, pCamera->GetNearClipDistance());
	lua_pushnumber(lua, pCamera->GetFarClipDistance());
	return 2;
}

/**
* 获取或设置相机投影方式
*/
int CScriptManager::DoCameraProjection(lua_State* lua) {
	CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
	if (lua_isboolean(lua, 1)) {
		bool ortho = lua_toboolean(lua, 1) != 0;
		pCamera->SetOrthoProjection(ortho);
		if (lua_isnumber(lua, 2) && lua_isnumber(lua, 3)) {
			float width = (float)lua_tonumber(lua, 2);
			float height = (float)lua_tonumber(lua, 3);
			pCamera->SetViewSize(width, height, ortho);
		}
		return 0;
	}
	lua_pushboolean(lua, pCamera->IsOrthoProjection());
	return 1;
}

/**
* 获取或设置相机位置
*/
int CScriptManager::DoCameraPosition(lua_State* lua) {
	CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
	if (lua_isnumber(lua, 1) && lua_isnumber(lua, 2) && lua_isnumber(lua, 3)) {
		float x = (float)lua_tonumber(lua, 1);
		float y = (float)lua_tonumber(lua, 2);
		float z = (float)lua_tonumber(lua, 3);
		pCamera->SetPosition(CVector3(x, y, z));
		return 0;
	}
	lua_pushnumber(lua, pCamera->m_cPosition[0]);
	lua_pushnumber(lua, pCamera->m_cPosition[1]);
	lua_pushnumber(lua, pCamera->m_cPosition[2]);
	return 3;
}

/**
* 获取或设置相机目标点
*/
int CScriptManager::DoCameraTarget(lua_State* lua) {
	CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
	if (lua_isnumber(lua, 1) && lua_isnumber(lua, 2) && lua_isnumber(lua, 3)) {
		float x = (float)lua_tonumber(lua, 1);
		float y = (float)lua_tonumber(lua, 2);
		float z = (float)lua_tonumber(lua, 3);
		pCamera->SetTarget(CVector3(x, y, z));
		return 0;
	}
	lua_pushnumber(lua, pCamera->m_cLookVector[0]);
	lua_pushnumber(lua, pCamera->m_cLookVector[1]);
	lua_pushnumber(lua, pCamera->m_cLookVector[2]);
	return 3;
}

/**
* 获取或设置相机角度
*/
int CScriptManager::DoCameraAngle(lua_State* lua) {
	CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
	if (lua_isnumber(lua, 1) && lua_isnumber(lua, 2) && lua_isnumber(lua, 3)) {
		float x = (float)lua_tonumber(lua, 1);
		float y = (float)lua_tonumber(lua, 2);
		float z = (float)lua_tonumber(lua, 3);
		pCamera->SetAngle(x, y, z);
		return 0;
	}
	float yaw = 0.0f;
	float pitch = 0.0f;
	float roll = 0.0f;
	CCamera::FromVectorToAngle(pCamera->m_cLookVector, pCamera->m_cUpVector, &yaw, &pitch, &roll);
	lua_pushnumber(lua, yaw);
	lua_pushnumber(lua, pitch);
	lua_pushnumber(lua, roll);
	return 3;
}

/**
* 获取相机方向向量正交基
*/
int CScriptManager::DoCameraDirection(lua_State* lua) {
	CMatrix4& view = CEngine::GetGraphicsManager()->GetCamera()->GetViewMatrix();
	lua_pushnumber(lua, -view(2, 0));
	lua_pushnumber(lua, -view(2, 1));
	lua_pushnumber(lua, -view(2, 2));
	lua_pushnumber(lua, view(0, 0));
	lua_pushnumber(lua, view(0, 1));
	lua_pushnumber(lua, view(0, 2));
	lua_pushnumber(lua, view(1, 0));
	lua_pushnumber(lua, view(1, 1));
	lua_pushnumber(lua, view(1, 2));
	return 9;
}

/**
* 绑定指定场景节点或解除绑定
*/
int CScriptManager::DoCameraBind(lua_State* lua) {
	CVector3 pos;
	CQuaternion orient;
	CSceneNode* pNode = 0;
	if (lua_isstring(lua, 1)) {
		pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (lua_isnumber(lua, 2) && lua_isnumber(lua, 3) && lua_isnumber(lua, 4)) {
			pos[0] = (float)lua_tonumber(lua, 2);
			pos[1] = (float)lua_tonumber(lua, 3);
			pos[2] = (float)lua_tonumber(lua, 4);
		}
		if (lua_isnumber(lua, 5) && lua_isnumber(lua, 6) && lua_isnumber(lua, 7)) {
			float ax = (float)lua_tonumber(lua, 5);
			float ay = (float)lua_tonumber(lua, 6);
			float az = (float)lua_tonumber(lua, 7);
			orient.FromEulerAngles(ax, ay, az);
		}
	}
	CEngine::GetGraphicsManager()->GetCamera()->Bind(pNode, pos, orient);
	return 0;
}

/**
* 插入场景节点
*/
int CScriptManager::DoSceneInsert(lua_State* lua) {
	CSceneNode* pNode = 0;
	if (lua_isstring(lua, 2) && lua_isstring(lua, 3)) {
		CSceneNode* pParent = 0;
		if (lua_isstring(lua, 1)) {
			// 通过名称获取父节点
			pParent = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		}
		// 节点类型和名称
		const char* type = lua_tostring(lua, 2);
		const char* name = lua_tostring(lua, 3);
		// 根据场景节点类型创建
		if (!strcmp("animation", type)) {
			const char* meshFile = TableValue(lua, 4, "meshFile", "");
			bool start = TableValue(lua, 4, "start", true);
			bool skeleton = TableValue(lua, 4, "skeleton", false);
			pNode = new CSceneNodeAnimation(name, meshFile, start, skeleton);
		} else if (!strcmp("axis", type)) {
			pNode = new CSceneNodeAxis(name);
		} else if (!strcmp("blast", type)) {
			const char* texture = TableValue(lua, 4, "texture", "");
			int row = TableValue(lua, 4, "row", 8);
			int column = TableValue(lua, 4, "column", 8);
			pNode = new CSceneNodeBlast(name, texture, row, column);
		} else if (!strcmp("board", type)) {
			const char* texture = TableValue(lua, 4, "texture", "");
			float width = TableValue(lua, 4, "width", 1.0f);
			float height = TableValue(lua, 4, "height", 1.0f);
			int billboard = TableValue(lua, 4, "billboard", 0);
			pNode = new CSceneNodeBoard(name, texture, width, height, billboard);
		} else if (!strcmp("cloud", type)) {
			const char* color1 = TableValue(lua, 4, "skyColor", "#666699FF");
			const char* color2 = TableValue(lua, 4, "cloudColor", "#666699FF");
			float cloudSize = TableValue(lua, 4, "cloudSize", 100.0f);
			pNode = new CSceneNodeCloud(name, CColor(color1), CColor(color2), cloudSize);
		} else if (!strcmp("decal", type)) {
			CMatrix4 proj;
			const char* texture = TableValue(lua, 4, "texture", "");
			float fov = TableValue(lua, 4, "fov", 60.0f);
			pNode = new CSceneNodeDecal(name, texture, proj.Perspective(fov, 1.0f, 0.5f, 1000.0f));
		} else if (!strcmp("flame", type)) {
			const char* texture = TableValue(lua, 4, "texture", "");
			const char* distortionMap = TableValue(lua, 4, "distortionMap", "");
			const char* alphaMask = TableValue(lua, 4, "alphaMask", "");
			float width = TableValue(lua, 4, "width", 1.0f);
			float height = TableValue(lua, 4, "height", 1.0f);
			pNode = new CSceneNodeFlame(name, texture, distortionMap, alphaMask, width, height);
		} else if (!strcmp("fresnel", type)) {
			const char* meshFile = TableValue(lua, 4, "meshFile", "");
			pNode = new CSceneNodeFresnel(name, meshFile);
		} else if (!strcmp("geometry", type)) {
			const char* shape = TableValue(lua, 4, "shape", "");
			const char* texture = TableValue(lua, 4, "texture", "");
			SGeometry geometry;
			geometry.reversed = TableValue(lua, 4, "reversed", false);
			geometry.slices = TableValue(lua, 4, "slices", 32);
			if (!strcmp(shape, "box")) geometry.shape = SGeometry::BOX;
			else if (!strcmp(shape, "sphere")) geometry.shape = SGeometry::SPHERE;
			else if (!strcmp(shape, "capsule")) geometry.shape = SGeometry::CAPSULE;
			else if (!strcmp(shape, "cylinder")) geometry.shape = SGeometry::CYLINDER;
			else if (!strcmp(shape, "torus")) geometry.shape = SGeometry::TORUS;
			else if (!strcmp(shape, "cone")) geometry.shape = SGeometry::CONE;
			else if (!strcmp(shape, "plane")) geometry.shape = SGeometry::PLANE;
			else if (!strcmp(shape, "circle")) geometry.shape = SGeometry::CIRCLE;
			// 几何体参数设置
			switch (geometry.shape) {
			case SGeometry::BOX:
				geometry.box.x = TableValue(lua, 4, "x", 1.0f);
				geometry.box.y = TableValue(lua, 4, "y", 1.0f);
				geometry.box.z = TableValue(lua, 4, "z", 1.0f);
				break;
			case SGeometry::SPHERE:
				geometry.sphere.r = TableValue(lua, 4, "r", 1.0f);
				break;
			case SGeometry::CAPSULE:
				geometry.capsule.r = TableValue(lua, 4, "r", 1.0f);
				geometry.capsule.h = TableValue(lua, 4, "h", 1.0f);
				break;
			case SGeometry::CYLINDER:
				geometry.cylinder.r = TableValue(lua, 4, "r", 1.0f);
				geometry.cylinder.h = TableValue(lua, 4, "h", 1.0f);
				break;
			case SGeometry::TORUS:
				geometry.torus.r = TableValue(lua, 4, "r", 1.0f);
				geometry.torus.c = TableValue(lua, 4, "c", 1.0f);
				break;
			case SGeometry::CONE:
				geometry.cone.r = TableValue(lua, 4, "r", 1.0f);
				geometry.cone.h = TableValue(lua, 4, "h", 1.0f);
				break;
			case SGeometry::PLANE:
				geometry.plane.w = TableValue(lua, 4, "w", 1.0f);
				geometry.plane.h = TableValue(lua, 4, "h", 1.0f);
				break;
			case SGeometry::CIRCLE:
				geometry.circle.r = TableValue(lua, 4, "r", 1.0f);
				break;
			default: break;
			}
			pNode = new CSceneNodeGeometry(name, texture, geometry);
		} else if (!strcmp("lensflare", type)) {
			pNode = new CSceneNodeLensflare(name);
		} else if (!strcmp("line", type)) {
			const char* color = TableValue(lua, 4, "color", "red");
			float width = TableValue(lua, 4, "width", 1.0f);
			pNode = new CSceneNodeLine(name, CColor(color), width);
		} else if (!strcmp("particles", type)) {
			float speed[3];
			const char* texture = TableValue(lua, 4, "texture", "");
			const char* color = TableValue(lua, 4, "color", "#FFC080FF");
			int count = TableValue(lua, 4, "count", 100);
			float size = TableValue(lua, 4, "size", 0.5f);
			bool dark = TableValue(lua, 4, "dark", false);
			float spread = TableValue(lua, 4, "spread", 0.5f);
			float fade = TableValue(lua, 4, "fade", 1.0f);
			speed[0] = TableValue(lua, 4, "dx", 0.0f);
			speed[1] = TableValue(lua, 4, "dy", 0.0f);
			speed[2] = TableValue(lua, 4, "dz", 0.0f);
			pNode = new CSceneNodeParticles(name, texture, size, count, dark, CColor(color), speed, spread, fade, true);
		} else if (!strcmp("planet", type)) {
			const char* texture = TableValue(lua, 4, "texture", "");
			const char* textureNight = TableValue(lua, 4, "textureNight", "");
			float radius = TableValue(lua, 4, "radius", 100.0f);
			int slices = TableValue(lua, 4, "slices", 64);
			pNode = new CSceneNodePlanet(name, texture, textureNight, radius, slices);
		} else if (!strcmp("plant", type)) {
			const char* texture = TableValue(lua, 4, "texture", "");
			float width = TableValue(lua, 4, "width", 1.0f);
			float height = TableValue(lua, 4, "height", 1.0f);
			const char* distributionMap = TableValue(lua, 4, "distributionMap", "");
			int count = TableValue(lua, 4, "count", 100);
			float density = TableValue(lua, 4, "density", 0.3f);
			float range = TableValue(lua, 4, "range", 1000.0f);
			pNode = new CSceneNodePlant(name, texture, width, height, distributionMap, count, density, range);
		} else if (!strcmp("screen", type)) {
			const char* texture = TableValue(lua, 4, "texture", "");
			int width = TableValue(lua, 4, "width", 0);
			int height = TableValue(lua, 4, "height", 0);
			pNode = new CSceneNodeScreen(name, texture, width, height);
		} else if (!strcmp("skybox", type)) {
			string textureArray[6];
			const char* texture = TableValue(lua, 4, "texture", "");
			CStringUtil::Split(textureArray, 6, texture, ",", true);
			pNode = new CSceneNodeSkybox(name, textureArray);
		} else if (!strcmp("sound", type)) {
			const char* soundFile = TableValue(lua, 4, "soundFile", "");
			bool loop = TableValue(lua, 4, "loop", true);
			bool start = TableValue(lua, 4, "start", true);
			pNode = new CSceneNodeSound(name, soundFile, loop, start);
		} else if (!strcmp("static", type)) {
			const char* meshFile = TableValue(lua, 4, "meshFile", "");
			pNode = new CSceneNodeStatic(name, meshFile);
		} else if (!strcmp("terrain", type)) {
			string textureArray[4];
			const char* texture = TableValue(lua, 4, "texture", "");
			const char* heightMap = TableValue(lua, 4, "heightMap", "");
			const char* blendMap = TableValue(lua, 4, "blendMap", "");
			float heightScale = TableValue(lua, 4, "heightScale", 100.0f);
			float mapScale = TableValue(lua, 4, "mapScale", 5000.0f);
			CStringUtil::Split(textureArray, 4, texture, ",", true);
			pNode = new CSceneNodeTerrain(name, heightMap, mapScale, heightScale, textureArray, blendMap);
		} else if (!strcmp("text", type)) {
			wchar_t text[128];
			const char* src = TableValue(lua, 4, "text", "?");
			CStringUtil::Utf8ToWideCharArray(src, text, 128);
			const char* color = TableValue(lua, 4, "color", "#FF0000FF");
			const char* outline = TableValue(lua, 4, "outline", "#FFFF00FF");
			const char* font = TableValue(lua, 4, "font", "default");
			int fontSize = TableValue(lua, 4, "fontSize", 16);
			pNode = new CSceneNodeText(name, text, CColor(color), CColor(outline), font, fontSize);
		} else if (!strcmp("water", type)) {
			const char* normalMap = TableValue(lua, 4, "normalMap", "");
			float depth = TableValue(lua, 4, "depth", 0.0f);
			pNode = new CSceneNodeWater(name, normalMap, depth);
		}
		if (pNode && !CEngine::GetSceneManager()->InsertNode(pNode, pParent)) {
			pNode = 0;
		}
	}
	lua_pushboolean(lua, pNode != 0);
	return 1;
}

/**
* 删除场景节点
*/
int CScriptManager::DoSceneDelete(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (pNode) {
			CRigidBody* pBody = CEngine::GetPhysicsManager()->GetRigidBody(pNode);
			if (pBody) {
				int callback = pBody->CollideCallback();
				if (callback >= 0) luaL_unref(lua, LUA_REGISTRYINDEX, callback);
				CEngine::GetPhysicsManager()->DelRigidBody(pBody);
			}
			CEngine::GetAnimationManager()->Stop(pNode, false);
			CEngine::GetSceneManager()->DeleteNode(pNode);
		}
	}
	return 0;
}

/**
* 清除所有场景节点
*/
int CScriptManager::DoSceneClear(lua_State* lua) {
	vector<CRigidBody*> rigidList;
	CEngine::GetPhysicsManager()->GetRigidBodyList(rigidList);
	for (size_t i = 0; i < rigidList.size(); i++) {
		int callback = rigidList[i]->CollideCallback();
		if (callback >= 0) luaL_unref(lua, LUA_REGISTRYINDEX, callback);
	}
	CEngine::GetPhysicsManager()->ClearRigidBody();
	CEngine::GetAnimationManager()->Clear();
	CEngine::GetSceneManager()->ClearNode();
	return 0;
}

/**
* 获取场景节点列表
*/
int CScriptManager::DoSceneList(lua_State* lua) {
	vector<CSceneNode*> nodeList;
	CEngine::GetSceneManager()->GetNodeList(nodeList);
	lua_newtable(lua);
	for (size_t i = 0; i < nodeList.size(); i++) {
		lua_pushinteger(lua, i + 1);
		lua_pushstring(lua, nodeList[i]->GetName().c_str());
		lua_settable(lua, -3);
	}
	return 1;
}

/**
* 选择场景节点
*/
int CScriptManager::DoScenePick(lua_State* lua) {
	if (lua_isnumber(lua, 1) && lua_isnumber(lua, 2) && lua_isnumber(lua, 3) &&
		lua_isnumber(lua, 4) && lua_isnumber(lua, 5) && lua_isnumber(lua, 6)) {
		float x = (float)lua_tonumber(lua, 1);
		float y = (float)lua_tonumber(lua, 2);
		float z = (float)lua_tonumber(lua, 3);
		float dx = (float)lua_tonumber(lua, 4);
		float dy = (float)lua_tonumber(lua, 5);
		float dz = (float)lua_tonumber(lua, 6);
		CVector3 hit;
		size_t mesh, face;
		CRay ray(CVector3(x, y, z), CVector3(dx, dy, dz));
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByRay(ray, hit, &mesh, &face);
		if (pNode) {
			lua_pushstring(lua, pNode->GetName().c_str());
			lua_pushnumber(lua, hit.m_fValue[0]);
			lua_pushnumber(lua, hit.m_fValue[1]);
			lua_pushnumber(lua, hit.m_fValue[2]);
			lua_pushinteger(lua, mesh);
			lua_pushinteger(lua, face);
			return 6;
		}
	}
	lua_pushnil(lua);
	return 1;
}

/**
* 获取或设置场景节点缩放
*/
int CScriptManager::DoSceneScale(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (lua_isnumber(lua, 2) && lua_isnumber(lua, 3) && lua_isnumber(lua, 4)) {
			if (pNode) {
				float x = (float)lua_tonumber(lua, 2);
				float y = (float)lua_tonumber(lua, 3);
				float z = (float)lua_tonumber(lua, 4);
				pNode->SetScale(CVector3(x, y, z));
			}
			return 0;
		}
		if (pNode) {
			lua_pushnumber(lua, pNode->m_cScale[0]);
			lua_pushnumber(lua, pNode->m_cScale[1]);
			lua_pushnumber(lua, pNode->m_cScale[2]);
		} else {
			lua_pushnumber(lua, 0.0f);
			lua_pushnumber(lua, 0.0f);
			lua_pushnumber(lua, 0.0f);
		}
		return 3;
	}
	return 0;
}

/**
* 获取或设置场景节点角度
*/
int CScriptManager::DoSceneAngle(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (lua_isnumber(lua, 2) && lua_isnumber(lua, 3) && lua_isnumber(lua, 4)) {
			if (pNode) {
				CQuaternion orient;
				float x = (float)lua_tonumber(lua, 2);
				float y = (float)lua_tonumber(lua, 3);
				float z = (float)lua_tonumber(lua, 4);
				pNode->SetOrientation(orient.FromEulerAngles(x, y, z));
			}
			return 0;
		}
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		if (pNode) pNode->m_cOrientation.ToEulerAngles(&x, &y, &z);
		lua_pushnumber(lua, x);
		lua_pushnumber(lua, y);
		lua_pushnumber(lua, z);
		return 3;
	}
	return 0;
}

/**
* 获取或设置场景节点方位
*/
int CScriptManager::DoSceneOrientation(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (lua_isnumber(lua, 2) && lua_isnumber(lua, 3) &&
			lua_isnumber(lua, 4) && lua_isnumber(lua, 5)) {
			if (pNode) {
				float x = (float)lua_tonumber(lua, 2);
				float y = (float)lua_tonumber(lua, 3);
				float z = (float)lua_tonumber(lua, 4);
				float w = (float)lua_tonumber(lua, 5);
				pNode->SetOrientation(CQuaternion(x, y, z, w));
			}
			return 0;
		}
		if (pNode) {
			lua_pushnumber(lua, pNode->m_cOrientation[0]);
			lua_pushnumber(lua, pNode->m_cOrientation[1]);
			lua_pushnumber(lua, pNode->m_cOrientation[2]);
			lua_pushnumber(lua, pNode->m_cOrientation[3]);
		} else {
			lua_pushnumber(lua, 0.0f);
			lua_pushnumber(lua, 0.0f);
			lua_pushnumber(lua, 0.0f);
			lua_pushnumber(lua, 1.0f);
		}
		return 4;
	}
	return 0;
}

/**
* 获取或设置场景节点位置
*/
int CScriptManager::DoScenePosition(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (lua_isnumber(lua, 2) && lua_isnumber(lua, 3) && lua_isnumber(lua, 4)) {
			if (pNode) {
				float x = (float)lua_tonumber(lua, 2);
				float y = (float)lua_tonumber(lua, 3);
				float z = (float)lua_tonumber(lua, 4);
				pNode->SetPosition(CVector3(x, y, z));
			}
			return 0;
		}
		if (pNode) {
			lua_pushnumber(lua, pNode->m_cPosition[0]);
			lua_pushnumber(lua, pNode->m_cPosition[1]);
			lua_pushnumber(lua, pNode->m_cPosition[2]);
		} else {
			lua_pushnumber(lua, 0.0f);
			lua_pushnumber(lua, 0.0f);
			lua_pushnumber(lua, 0.0f);
		}
		return 3;
	}
	return 0;
}

/**
* 获取或设置场景节点可见性
*/
int CScriptManager::DoSceneVisible(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (lua_isboolean(lua, 2)) {
			if (pNode) pNode->m_bVisible = lua_toboolean(lua, 2) != 0;
			return 0;
		}
		lua_pushboolean(lua, pNode ? pNode->m_bVisible: false);
		return 1;
	}
	return 0;
}

/**
* 更新场景节点
* 使用方法 scene.update(node, mesh, {{s=0,t=0}, [3]={r=0.5,g=0.5,b=0.5}, [4]={s=1,t=1}})
*/
int CScriptManager::DoSceneUpdate(lua_State* lua) {
	if (lua_isstring(lua, 1) && lua_isinteger(lua, 2) && lua_istable(lua, 3)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		CMeshData* pMeshData = pNode ? pNode->GetMeshData() : 0;
		if (!pMeshData) return 0;
		int meshIndex = (int)lua_tointeger(lua, 2);
		if (meshIndex < 0 || meshIndex >= static_cast<int>(pMeshData->GetMeshCount())) return 0;
		CMesh* pMesh = pMeshData->GetMesh(meshIndex);
		int vertexCount = static_cast<int>(pMesh->GetVertexCount());
		lua_pushnil(lua);
		while (lua_next(lua, 3)) {
			if (lua_isinteger(lua, -2) && lua_istable(lua, -1)) {
				int index = (int)lua_tointeger(lua, -2);
				if (index > vertexCount || index <= 0) continue;
				int t = lua_gettop(lua);
				CVertex* v = pMesh->GetVertex(index - 1);
				if (lua_getfield(lua, t, "x") == LUA_TNUMBER) v->m_fPosition[0] = (float)lua_tonumber(lua, -1);
				if (lua_getfield(lua, t, "y") == LUA_TNUMBER) v->m_fPosition[1] = (float)lua_tonumber(lua, -1);
				if (lua_getfield(lua, t, "z") == LUA_TNUMBER) v->m_fPosition[2] = (float)lua_tonumber(lua, -1);
				if (lua_getfield(lua, t, "s") == LUA_TNUMBER) v->m_fTexCoord[0] = (float)lua_tonumber(lua, -1);
				if (lua_getfield(lua, t, "t") == LUA_TNUMBER) v->m_fTexCoord[1] = (float)lua_tonumber(lua, -1);
				if (lua_getfield(lua, t, "nx") == LUA_TNUMBER) v->m_fNormal[0] = (float)lua_tonumber(lua, -1);
				if (lua_getfield(lua, t, "ny") == LUA_TNUMBER) v->m_fNormal[1] = (float)lua_tonumber(lua, -1);
				if (lua_getfield(lua, t, "nz") == LUA_TNUMBER) v->m_fNormal[2] = (float)lua_tonumber(lua, -1);
				if (lua_getfield(lua, t, "r") == LUA_TNUMBER) v->m_fColor[0] = (float)lua_tonumber(lua, -1);
				if (lua_getfield(lua, t, "g") == LUA_TNUMBER) v->m_fColor[1] = (float)lua_tonumber(lua, -1);
				if (lua_getfield(lua, t, "b") == LUA_TNUMBER) v->m_fColor[2] = (float)lua_tonumber(lua, -1);
				if (lua_getfield(lua, t, "a") == LUA_TNUMBER) v->m_fColor[3] = (float)lua_tonumber(lua, -1);
				lua_settop(lua, t);
			}
			lua_pop(lua, 1);
		}
		pMesh->Update(1);
	}
	return 0;
}

/**
* 获取场景节点网格顶点信息
*/
int CScriptManager::DoSceneVertex(lua_State* lua) {
	if (lua_isstring(lua, 1) && lua_isinteger(lua, 2) && lua_isinteger(lua, 3) && lua_isinteger(lua, 4)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		CMeshData* pMeshData = pNode ? pNode->GetMeshData() : 0;
		if (pMeshData) {
			int mesh = (int)lua_tointeger(lua, 2);
			int face = (int)lua_tointeger(lua, 3);
			int vert = (int)lua_tointeger(lua, 4);
			if (vert < 0 || vert >= 3) return 0;
			if (mesh < 0 || mesh >= static_cast<int>(pMeshData->GetMeshCount())) return 0;
			if (face < 0 || face >= static_cast<int>(pMeshData->GetMesh(mesh)->GetTriangleCount())) return 0;
			CVertex* pVert = pMeshData->GetMesh(mesh)->GetVertex(face, vert);
			lua_pushnumber(lua, pVert->m_fPosition[0]);
			lua_pushnumber(lua, pVert->m_fPosition[1]);
			lua_pushnumber(lua, pVert->m_fPosition[2]);
			lua_pushnumber(lua, pVert->m_fTexCoord[0]);
			lua_pushnumber(lua, pVert->m_fTexCoord[1]);
			lua_pushnumber(lua, pVert->m_fNormal[0]);
			lua_pushnumber(lua, pVert->m_fNormal[1]);
			lua_pushnumber(lua, pVert->m_fNormal[2]);
			lua_pushnumber(lua, pVert->m_fColor[0]);
			lua_pushnumber(lua, pVert->m_fColor[1]);
			lua_pushnumber(lua, pVert->m_fColor[2]);
			lua_pushnumber(lua, pVert->m_fColor[3]);
			return 12;
		}
	}
	return 0;
}

/**
* 获取或设置场景节点网格材质
*/
int CScriptManager::DoSceneMaterial(lua_State* lua) {
	if (lua_isstring(lua, 1) && lua_isinteger(lua, 2)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		CMeshData* pMeshData = pNode ? pNode->GetMeshData() : 0;
		if (!pMeshData) return 0;
		int meshIndex = (int)lua_tointeger(lua, 2);
		if (meshIndex >= static_cast<int>(pMeshData->GetMeshCount())) return 0;
		if (lua_istable(lua, 3)) {
			const char* name = TableValue(lua, 3, "name", "");
			float roughness = TableValue(lua, 3, "roughness", -1.0f);
			float metalness = TableValue(lua, 3, "metalness", -1.0f);
			const char* color = TableValue(lua, 3, "color", "");
			const char* shader = TableValue(lua, 3, "shader", "");
			const char* texture = TableValue(lua, 3, "texture", "");
			vector<string> textures;
			if (strlen(texture) > 0) CStringUtil::Split(textures, texture, ",", true);
			if (meshIndex < 0) {
				for (size_t i = 0; i < pMeshData->GetMeshCount(); i++) {
					CMaterial* pMaterial = pMeshData->GetMesh(i)->GetMaterial();
					if (roughness >= 0.0f) pMaterial->m_fRoughness = roughness;
					if (metalness >= 0.0f) pMaterial->m_fMetalness = metalness;
					if (strlen(color) > 0) CColor(color).GetValue(pMaterial->m_fColor);
					if (strlen(shader) > 0) pMaterial->SetShader(shader);
					for (size_t t = 0; t < textures.size(); t++) pMaterial->SetTexture(textures[t], static_cast<int>(t));
				}
			} else {
				CMaterial* pMaterial = pMeshData->GetMesh(meshIndex)->GetMaterial();
				if (strlen(name) > 0) pMaterial->GetName() = name;
				if (roughness >= 0.0f) pMaterial->m_fRoughness = roughness;
				if (metalness >= 0.0f) pMaterial->m_fMetalness = metalness;
				if (strlen(color) > 0) CColor(color).GetValue(pMaterial->m_fColor);
				if (strlen(shader) > 0) pMaterial->SetShader(shader);
				for (size_t t = 0; t < textures.size(); t++) pMaterial->SetTexture(textures[t], static_cast<int>(t));
			}
		} else {
			CMaterial* pMaterial = pMeshData->GetMesh(meshIndex < 0 ? 0 : meshIndex)->GetMaterial();
			lua_newtable(lua);
			lua_pushstring(lua, "name");
			lua_pushstring(lua, pMaterial->GetName().c_str());
			lua_settable(lua, -3);
			lua_pushstring(lua, "roughness");
			lua_pushnumber(lua, pMaterial->m_fRoughness);
			lua_settable(lua, -3);
			lua_pushstring(lua, "metalness");
			lua_pushnumber(lua, pMaterial->m_fMetalness);
			lua_settable(lua, -3);
			lua_pushstring(lua, "color");
			lua_pushstring(lua, CColor(pMaterial->m_fColor).ToString().c_str());
			lua_settable(lua, -3);
			lua_pushstring(lua, "shader");
			if (pMaterial->GetShader()) lua_pushstring(lua, pMaterial->GetShader()->GetName().c_str());
			else lua_pushnil(lua);
			lua_settable(lua, -3);
			vector<string> textures;
			for (int t = 0; t < pMaterial->GetTextureCount(); t++) textures.push_back(pMaterial->GetTexture(t)->GetFilePath());
			lua_pushstring(lua, "texture");
			lua_pushstring(lua, CStringUtil::Join(textures, ",").c_str());
			lua_settable(lua, -3);
			return 1;
		}
	}
	return 0;
}

/**
* 设置场景节点渲染模式
*/
int CScriptManager::DoSceneRenderMode(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		CMeshData* pMeshData = pNode ? pNode->GetMeshData() : 0;
		if (!pMeshData) return 0;
		if (lua_isboolean(lua, 2) && lua_isboolean(lua, 3) && lua_isboolean(lua, 4)) {
			bool cullFace = lua_toboolean(lua, 2) != 0;
			bool useDepth = lua_toboolean(lua, 3) != 0;
			bool addColor = lua_toboolean(lua, 4) != 0;
			for (size_t i = 0; i < pMeshData->GetMeshCount(); i++) {
				pMeshData->GetMesh(i)->GetMaterial()->SetRenderMode(cullFace, useDepth, addColor);
			}
		}
	}
	return 0;
}

/**
* 获取场景节点的包围盒
*/
int CScriptManager::DoSceneBoundingBox(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (pNode) {
			CBoundingBox aabb = pNode->GetBoundingBox();
			lua_pushnumber(lua, aabb.m_cMin[0]);
			lua_pushnumber(lua, aabb.m_cMin[1]);
			lua_pushnumber(lua, aabb.m_cMin[2]);
			lua_pushnumber(lua, aabb.m_cMax[0]);
			lua_pushnumber(lua, aabb.m_cMax[1]);
			lua_pushnumber(lua, aabb.m_cMax[2]);
			return 6;
		}
	}
	return 0;
}

/**
* 调用场景节点方法
*/
int CScriptManager::DoSceneHandle(lua_State* lua) {
	if (lua_isstring(lua, 1) && lua_isstring(lua, 2)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (!pNode) return 0;
		const string& type = pNode->GetType();
		const char* function = lua_tostring(lua, 2);
		if (type == "animation") {
			CSceneNodeAnimation* that = static_cast<CSceneNodeAnimation*>(pNode);
			if (!strcmp(function, "StartAnimation")) {
				if (!lua_istable(lua, 3)) return 0;
				const char* name = TableValue(lua, 3, "name", "");
				bool loop = TableValue(lua, 3, "loop", true);
				float transition = TableValue(lua, 3, "transition", -1.0f);
				that->StartAnimation(name, loop, transition);
			} else if (!strcmp(function, "BlendAnimation")) {
				if (!lua_istable(lua, 3)) return 0;
				const char* anim1 = TableValue(lua, 3, "anim1", "");
				const char* anim2 = TableValue(lua, 3, "anim2", "");
				float k = TableValue(lua, 3, "k", 0.5f);
				that->BlendAnimation(anim1, anim2, k);
			} else if (!strcmp(function, "StopAnimation")) {
				that->StopAnimation();
			} else if (!strcmp(function, "SetAnimationFrameRate")) {
				if (!lua_istable(lua, 3)) return 0;
				that->SetAnimationFrameRate(TableValue(lua, 3, "fps", 30.0f));
			} else if (!strcmp(function, "ShowSkeleton")) {
				if (!lua_istable(lua, 3)) return 0;
				bool visible = TableValue(lua, 3, "visible", true);
				bool skeletonOnly = TableValue(lua, 3, "skeletonOnly", false);
				that->ShowSkeleton(visible, skeletonOnly);
			} else if (!strcmp(function, "GetAnimationName")) {
				if (!lua_istable(lua, 3)) return 0;
				int index = TableValue(lua, 3, "index", 0);
				lua_pushstring(lua, that->GetAnimationName(index).c_str());
				return 1;
			} else if (!strcmp(function, "PointFacing")) {
				if (!lua_istable(lua, 3)) return 0;
				CVector3 front, point;
				const char* joint = TableValue(lua, 3, "joint", "");
				front[0] = TableValue(lua, 3, "front_x", 0.0f);
				front[1] = TableValue(lua, 3, "front_y", -1.0f);
				front[2] = TableValue(lua, 3, "front_z", 0.0f);
				point[0] = TableValue(lua, 3, "point_x", 0.0f);
				point[1] = TableValue(lua, 3, "point_y", 0.0f);
				point[2] = TableValue(lua, 3, "point_z", 0.0f);
				float angle = TableValue(lua, 3, "angle", 1.047198f);
				that->PointFacing(joint, front, point, angle);
			} else if (!strcmp(function, "SetTransform")) {
				if (!lua_istable(lua, 3)) return 0;
				CVector3 translation, rotation, scale;
				const char* joint = TableValue(lua, 3, "joint", "");
				translation[0] = TableValue(lua, 3, "translation_x", 0.0f);
				translation[1] = TableValue(lua, 3, "translation_y", 0.0f);
				translation[2] = TableValue(lua, 3, "translation_z", 0.0f);
				rotation[0] = TableValue(lua, 3, "rotation_x", 0.0f);
				rotation[1] = TableValue(lua, 3, "rotation_y", 0.0f);
				rotation[2] = TableValue(lua, 3, "rotation_z", 0.0f);
				scale[0] = TableValue(lua, 3, "scale_x", 1.0f);
				scale[1] = TableValue(lua, 3, "scale_y", 1.0f);
				scale[2] = TableValue(lua, 3, "scale_z", 1.0f);
				that->SetTransform(joint, translation, rotation, scale);
			}
		} else if (type == "blast") {
			CSceneNodeBlast* that = static_cast<CSceneNodeBlast*>(pNode);
			if (!strcmp(function, "InitBlast")) {
				if (!lua_istable(lua, 3)) return 0;
				float duration = TableValue(lua, 3, "duration", 1.0f);
				float size = TableValue(lua, 3, "size", 1.0f);
				that->InitBlast(duration, size);
			} else if (!strcmp(function, "IsFinished")) {
				lua_pushboolean(lua, that->IsFinished());
				return 1;
			}
		} else if (type == "board") {
			CSceneNodeBoard* that = static_cast<CSceneNodeBoard*>(pNode);
			if (!strcmp(function, "MediaInfo")) {
				int width = 0, height = 0; float length = 0.0f;
				that->MediaInfo(&width, &height, &length);
				lua_pushinteger(lua, width);
				lua_pushinteger(lua, height);
				lua_pushnumber(lua, length);
				return 3;
			} else if (!strcmp(function, "SetAxis")) {
				if (!lua_istable(lua, 3)) return 0;
				float x = TableValue(lua, 3, "x", 0.0f);
				float y = TableValue(lua, 3, "y", 0.0f);
				float z = TableValue(lua, 3, "z", 1.0f);
				that->SetAxis(CVector3(x, y, z));
			} else if (!strcmp(function, "SetPlayTime")) {
				if (!lua_istable(lua, 3)) return 0;
				that->SetPlayTime(TableValue(lua, 3, "time", 0.0f));
			}
		} else if (type == "line") {
			CSceneNodeLine* that = static_cast<CSceneNodeLine*>(pNode);
			if (!strcmp(function, "AddPoint")) {
				if (!lua_istable(lua, 3)) return 0;
				float x = TableValue(lua, 3, "x", 0.0f);
				float y = TableValue(lua, 3, "y", 0.0f);
				float z = TableValue(lua, 3, "z", 0.0f);
				that->AddPoint(CVector3(x, y, z));
			} else if (!strcmp(function, "ClearPoint")) {
				that->ClearPoint();
			} else if (!strcmp(function, "SmoothLine")) {
				if (!lua_istable(lua, 3)) return 0;
				that->SmoothLine(TableValue(lua, 3, "ds", 0.1f));
			} else if (!strcmp(function, "ShowPoints")) {
				if (!lua_istable(lua, 3)) return 0;
				bool show = TableValue(lua, 3, "show", true);
				float pointSize = TableValue(lua, 3, "pointSize", 1.0f);
				that->ShowPoints(show, pointSize);
			} else if (!strcmp(function, "DisableDepth")) {
				if (!lua_istable(lua, 3)) return 0;
				that->DisableDepth(TableValue(lua, 3, "disable", true));
			} else if (!strcmp(function, "ScreenSpace")) {
				if (!lua_istable(lua, 3)) return 0;
				that->ScreenSpace(TableValue(lua, 3, "enable", true));
			} else if (!strcmp(function, "Segment")) {
				if (!lua_istable(lua, 3)) return 0;
				that->Segment(TableValue(lua, 3, "enable", true));
			} else if (!strcmp(function, "SetShader")) {
				if (!lua_istable(lua, 3)) return 0;
				that->SetShader(TableValue(lua, 3, "shader", ""));
			} else if (!strcmp(function, "GetPointCount")) {
				lua_pushinteger(lua, that->GetPointCount());
				return 1;
			}
		} else if (type == "particles") {
			CSceneNodeParticles* that = static_cast<CSceneNodeParticles*>(pNode);
			if (!strcmp(function, "InitParticles")) {
				if (!lua_istable(lua, 3)) return 0;
				CVector3 initSpeed;
				initSpeed[0] = TableValue(lua, 3, "initSpeed_x", 0.0f);
				initSpeed[1] = TableValue(lua, 3, "initSpeed_y", 0.0f);
				initSpeed[2] = TableValue(lua, 3, "initSpeed_z", 0.0f);
				float spreadSpeed = TableValue(lua, 3, "spreadSpeed", 1.0f);
				float fadeSpeed = TableValue(lua, 3, "fadeSpeed", 1.0f);
				that->InitParticles(initSpeed, spreadSpeed, fadeSpeed);
			}
		} else if (type == "sound") {
			CSceneNodeSound* that = static_cast<CSceneNodeSound*>(pNode);
			if (!strcmp(function, "Play")) {
				if (!lua_istable(lua, 3)) return 0;
				that->Play(TableValue(lua, 3, "enable", true));
			}
		} else if (type == "terrain") {
			CSceneNodeTerrain* that = static_cast<CSceneNodeTerrain*>(pNode);
			if (!strcmp(function, "GetHeight")) {
				if (!lua_istable(lua, 3)) return 0;
				float x = TableValue(lua, 3, "x", 0.0f);
				float y = TableValue(lua, 3, "y", 0.0f);
				lua_pushnumber(lua, that->GetHeight(x, y));
				return 1;
			} else if (!strcmp(function, "GetNormal")) {
				if (!lua_istable(lua, 3)) return 0;
				float x = TableValue(lua, 3, "x", 0.0f);
				float y = TableValue(lua, 3, "y", 0.0f);
				CVector3 normal = that->GetNormal(x, y);
				lua_pushnumber(lua, normal[0]);
				lua_pushnumber(lua, normal[1]);
				lua_pushnumber(lua, normal[2]);
				return 3;
			} else if (!strcmp(function, "SetErrorTolerance")) {
				if (!lua_istable(lua, 3)) return 0;
				that->SetErrorTolerance(TableValue(lua, 3, "tolerance", 20.0f));
			}
		} else if (type == "text") {
			CSceneNodeText* that = static_cast<CSceneNodeText*>(pNode);
			if (!strcmp(function, "SetText")) {
				if (!lua_istable(lua, 3)) return 0;
				wchar_t buffer[1024];
				const char* text = TableValue(lua, 3, "text", "");
				CStringUtil::Utf8ToWideCharArray(text, buffer, 1024);
				that->SetText(buffer);
			} else if (!strcmp(function, "SetFont")) {
				if (!lua_istable(lua, 3)) return 0;
				const char* font = TableValue(lua, 3, "font", "default");
				int fontSize = TableValue(lua, 3, "fontSize", 16);
				that->SetFont(font, fontSize);
			} else if (!strcmp(function, "SetColor")) {
				if (!lua_istable(lua, 3)) return 0;
				const char* color = TableValue(lua, 3, "color", "red");
				const char* outline = TableValue(lua, 3, "outline", "yellow");
				that->SetColor(CColor(color), CColor(outline));
			}
		}
	}
	return 0;
}

/**
* 获取已注册的后处理列表
*/
int CScriptManager::DoPostList(lua_State* lua) {
	vector<string> postList;
	CEngine::GetPostProcessManager()->GetPostProcessList(postList);
	lua_newtable(lua);
	for (size_t i = 0; i < postList.size(); i++) {
		lua_pushinteger(lua, i + 1);
		lua_pushstring(lua, postList[i].c_str());
		lua_settable(lua, -3);
	}
	return 1;
}

/**
* 获取或设置指定后处理
*/
int CScriptManager::DoPostEnable(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CPostProcessManager* postMgr = CEngine::GetPostProcessManager();
		if (lua_isboolean(lua, 2)) {
			bool enable = lua_toboolean(lua, 2) != 0;
			postMgr->Enable(lua_tostring(lua, 1), enable);
			return 0;
		}
		lua_pushboolean(lua, postMgr->IsEnabled(lua_tostring(lua, 1)));
		return 1;
	}
	return 0;
}

/**
* 注册用户自定义后处理
*/
int CScriptManager::DoPostRegister(lua_State* lua) {
	bool success = false;
	if (lua_isstring(lua, 1) && lua_isstring(lua, 2)) {
		const char* name = lua_tostring(lua, 1);
		const char* shader = lua_tostring(lua, 2);
		const char* texture = lua_isstring(lua, 3) ? lua_tostring(lua, 3) : 0;
		bool cube = lua_isboolean(lua, 4) ? lua_toboolean(lua, 4) != 0 : false;
		CPostProcess* postProcess = new CPostProcessUser(name, shader, texture, cube);
		success = CEngine::GetPostProcessManager()->Register(name, postProcess);
	}
	lua_pushboolean(lua, success);
	return 1;
}

/**
* 设置后处理参数
*/
int CScriptManager::DoPostParam(lua_State* lua) {
	int paramCount = lua_gettop(lua) - 2;
	if (paramCount > 0 && lua_isstring(lua, 1) && lua_isstring(lua, 2)) {
		const char* name = lua_tostring(lua, 1);
		const char* key = lua_tostring(lua, 2);
		CPostProcessManager* pPostProcessMgr = CEngine::GetPostProcessManager();
		if (paramCount == 1) {
			if (lua_isinteger(lua, 3)) pPostProcessMgr->SetProcessParam(name, key, (int)lua_tointeger(lua, 3));
			else if (lua_isnumber(lua, 3)) pPostProcessMgr->SetProcessParam(name, key, (float)lua_tonumber(lua, 3));
		} else {
			float value[16] = { 0.0f };
			for (int i = 0; i < paramCount; i++) if (lua_isnumber(lua, i + 3)) value[i] = (float)lua_tonumber(lua, i + 3);
			pPostProcessMgr->SetProcessParam(name, key, value, paramCount);
		}
	}
	return 0;
}

/**
* 屏幕截图
*/
int CScriptManager::DoGraphicsScreenshot(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		string filepath = lua_tostring(lua, 1);
		bool redraw = lua_isboolean(lua, 2) ? lua_toboolean(lua, 2) != 0 : true;
		CEngine::GetGraphicsManager()->Screenshot(filepath, redraw);
	} else if (lua_isnil(lua, 1)) {
		bool redraw = lua_isboolean(lua, 2) ? lua_toboolean(lua, 2) != 0 : true;
		CFileManager::CImageFile file(CFileManager::PNG);
		CEngine::GetGraphicsManager()->Screenshot(file, redraw);
		CFileManager::CBinaryFile buffer(file.size);
		CEngine::GetFileManager()->WriteFile(&file, buffer.contents, &buffer.size);
		lua_pushlstring(lua, (const char*)buffer.contents, buffer.size);
		return 1;
	}
	return 0;
}

/**
* 获取或设置立体显示
*/
int CScriptManager::DoGraphicsStereo(lua_State* lua) {
	if (lua_isboolean(lua, 1)) {
		CEngine::GetGraphicsManager()->SetStereoMode(lua_toboolean(lua, 1) != 0);
		return 0;
	}
	lua_pushboolean(lua, CEngine::GetGraphicsManager()->GetStereoMode());
	return 1;
}

/**
* 获取窗口大小
*/
int CScriptManager::DoGraphicsWindowSize(lua_State* lua) {
	int width;
	int height;
	CEngine::GetGraphicsManager()->GetWindowSize(&width, &height);
	lua_pushinteger(lua, width);
	lua_pushinteger(lua, height);
	return 2;
}

/**
* 获取光源位置
*/
int CScriptManager::DoGraphicsLightPosition(lua_State* lua) {
	CVector3 lightPos;
	CVector3 lightDir;
	CEngine::GetGraphicsManager()->GetLight(lightPos, lightDir);
	lua_pushnumber(lua, lightPos[0]);
	lua_pushnumber(lua, lightPos[1]);
	lua_pushnumber(lua, lightPos[2]);
	lua_pushnumber(lua, lightPos[3]);
	lua_pushnumber(lua, lightDir[0]);
	lua_pushnumber(lua, lightDir[1]);
	lua_pushnumber(lua, lightDir[2]);
	lua_pushnumber(lua, lightDir[3]);
	return 8;
}

/**
* 设置方向光光源
*/
int CScriptManager::DoGraphicsDirectionLight(lua_State* lua) {
	if (lua_isnumber(lua, 1) && lua_isnumber(lua, 2) && lua_isnumber(lua, 3)) {
		float dx = (float)lua_tonumber(lua, 1);
		float dy = (float)lua_tonumber(lua, 2);
		float dz = (float)lua_tonumber(lua, 3);
		CVector3 color(1.0f, 1.0f, 1.0f, 0.0f);
		if (lua_isnumber(lua, 4) && lua_isnumber(lua, 5) && lua_isnumber(lua, 6)) {
			color[0] = (float)lua_tonumber(lua, 4);
			color[1] = (float)lua_tonumber(lua, 5);
			color[2] = (float)lua_tonumber(lua, 6);
		}
		CEngine::GetGraphicsManager()->SetLight(CVector3(dx, dy, dz, 0.0f), CVector3::Zero, color);
	}
	return 0;
}

/**
* 设置点光源
*/
int CScriptManager::DoGraphicsPointLight(lua_State* lua) {
	if (lua_isnumber(lua, 1) && lua_isnumber(lua, 2) && lua_isnumber(lua, 3)) {
		float x = (float)lua_tonumber(lua, 1);
		float y = (float)lua_tonumber(lua, 2);
		float z = (float)lua_tonumber(lua, 3);
		CVector3 color(1.0f, 1.0f, 1.0f, 100.0f);
		if (lua_isnumber(lua, 4)) color[3] = (float)lua_tonumber(lua, 4);
		if (lua_isnumber(lua, 5) && lua_isnumber(lua, 6) && lua_isnumber(lua, 7)) {
			color[0] = (float)lua_tonumber(lua, 5);
			color[1] = (float)lua_tonumber(lua, 6);
			color[2] = (float)lua_tonumber(lua, 7);
		}
		CEngine::GetGraphicsManager()->SetLight(CVector3(x, y, z, 1.0f), CVector3::Zero, color);
	}
	return 0;
}

/**
* 设置聚光灯光源
*/
int CScriptManager::DoGraphicsSpotLight(lua_State* lua) {
	if (lua_isnumber(lua, 1) && lua_isnumber(lua, 2) && lua_isnumber(lua, 3)) {
		float x = (float)lua_tonumber(lua, 1);
		float y = (float)lua_tonumber(lua, 2);
		float z = (float)lua_tonumber(lua, 3);
		CVector3 color(1.0f, 1.0f, 1.0f, 100.0f);
		CVector3 dir(0.0f, 0.0f, -1.0f, 0.7854f);
		if (lua_isnumber(lua, 4) && lua_isnumber(lua, 5) && lua_isnumber(lua, 6)) {
			dir[0] = (float)lua_tonumber(lua, 4);
			dir[1] = (float)lua_tonumber(lua, 5);
			dir[2] = (float)lua_tonumber(lua, 6);
		}
		if (lua_isnumber(lua, 7)) dir[3] = (float)lua_tonumber(lua, 7);
		if (lua_isnumber(lua, 8)) color[3] = (float)lua_tonumber(lua, 8);
		if (lua_isnumber(lua, 9) && lua_isnumber(lua, 10) && lua_isnumber(lua, 11)) {
			color[0] = (float)lua_tonumber(lua, 9);
			color[1] = (float)lua_tonumber(lua, 10);
			color[2] = (float)lua_tonumber(lua, 11);
		}
		CEngine::GetGraphicsManager()->SetLight(CVector3(x, y, z, 1.0f), dir, color);
	}
	return 0;
}

/**
* 获取或设置阴影是否显示
*/
int CScriptManager::DoGraphicsShadow(lua_State* lua) {
	if (lua_isboolean(lua, 1)) {
		CEngine::GetGraphicsManager()->SetShadowEnable(lua_toboolean(lua, 1) != 0);
		return 0;
	}
	lua_pushboolean(lua, CEngine::GetGraphicsManager()->GetShadowEnable());
	return 1;
}

/**
* 获取或设置雾是否显示
*/
int CScriptManager::DoGraphicsFog(lua_State* lua) {
	if (lua_isboolean(lua, 1)) {
		if (lua_toboolean(lua, 1)) {
			if (lua_isnumber(lua, 2) && lua_isnumber(lua, 3) && lua_isnumber(lua, 4) &&
				lua_isnumber(lua, 5) && lua_isnumber(lua, 6)) {
				float r = (float)lua_tonumber(lua, 2);
				float g = (float)lua_tonumber(lua, 3);
				float b = (float)lua_tonumber(lua, 4);
				float start = (float)lua_tonumber(lua, 5);
				float end = (float)lua_tonumber(lua, 6);
				CEngine::GetGraphicsManager()->SetFogEnable(true, start, end, CColor(r, g, b));
			}
		} else CEngine::GetGraphicsManager()->SetFogEnable(false, 0.0f, 0.0f, CColor::White);
		return 0;
	}
	lua_pushboolean(lua, CEngine::GetGraphicsManager()->GetFogEnable());
	return 1;
}

/**
* 获取或设置环境贴图
*/
int CScriptManager::DoGraphicsEnvironmentMap(lua_State* lua) {
	if (lua_isboolean(lua, 1)) {
		if (lua_toboolean(lua, 1)) {
			if (lua_isstring(lua, 2)) {
				CEngine::GetGraphicsManager()->SetEnvironmentMapEnable(true, lua_tostring(lua, 2));
			}
		} else CEngine::GetGraphicsManager()->SetEnvironmentMapEnable(false, "");
		return 0;
	}
	lua_pushboolean(lua, CEngine::GetGraphicsManager()->GetEnvironmentMapEnable());
	return 1;
}

/**
* 设置绘图背景颜色
*/
int CScriptManager::DoGraphicsBackground(lua_State* lua) {
	if (lua_isnumber(lua, 1) && lua_isnumber(lua, 2) && lua_isnumber(lua, 3) && lua_isnumber(lua, 4)) {
		float r = (float)lua_tonumber(lua, 1);
		float g = (float)lua_tonumber(lua, 2);
		float b = (float)lua_tonumber(lua, 3);
		float a = (float)lua_tonumber(lua, 4);
		CEngine::GetGraphicsManager()->SetBackgroundColor(CColor(r, g, b, a));
	}
	return 0;
}

/**
* 从屏幕上的点获取空间射线
*/
int CScriptManager::DoGraphicsPickingRay(lua_State* lua) {
	if (lua_isinteger(lua, 1) && lua_isinteger(lua, 2)) {
		int x = (int)lua_tointeger(lua, 1);
		int y = (int)lua_tointeger(lua, 2);
		CRay ray;
		CEngine::GetGraphicsManager()->PickingRay(x, y, ray);
		lua_pushnumber(lua, ray.m_cOrigin.m_fValue[0]);
		lua_pushnumber(lua, ray.m_cOrigin.m_fValue[1]);
		lua_pushnumber(lua, ray.m_cOrigin.m_fValue[2]);
		lua_pushnumber(lua, ray.m_cDirection.m_fValue[0]);
		lua_pushnumber(lua, ray.m_cDirection.m_fValue[1]);
		lua_pushnumber(lua, ray.m_cDirection.m_fValue[2]);
		return 6;
	}
	return 0;
}

/**
* 获取空间点投影到屏幕上的位置
*/
int CScriptManager::DoGraphicsProject(lua_State* lua) {
	if (lua_isnumber(lua, 1) && lua_isnumber(lua, 2) && lua_isnumber(lua, 3)) {
		float x = (float)lua_tonumber(lua, 1);
		float y = (float)lua_tonumber(lua, 2);
		float z = (float)lua_tonumber(lua, 3);
		CVector3 screen;
		CEngine::GetGraphicsManager()->PointToScreen(CVector3(x, y, z), screen);
		lua_pushnumber(lua, screen.m_fValue[0]);
		lua_pushnumber(lua, screen.m_fValue[1]);
		lua_pushnumber(lua, screen.m_fValue[2]);
		return 3;
	}
	return 0;
}

/**
* 设置离线渲染目标
*/
int CScriptManager::DoGraphicsRenderTarget(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		const char* name = lua_tostring(lua, 1);
		bool clear = lua_isboolean(lua, 2) ? lua_toboolean(lua, 2) != 0 : false;
		CTexture* pTexture = CEngine::GetTextureManager()->GetTexture(name);
		CGraphicsManager* pGraphicsMgr = CEngine::GetGraphicsManager();
		CTexture* pRenderTarget = pGraphicsMgr->GetRenderTarget();
		pGraphicsMgr->SetRenderTarget(pTexture, 0, clear, clear);
		lua_pushstring(lua, pRenderTarget ? pRenderTarget->GetFilePath().c_str() : "null");
		return 1;
	}
	return 0;
}

/**
* 创建或载入纹理
*/
int CScriptManager::DoTextureCreate(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CTexture* pTexture = 0;
		string name = lua_tostring(lua, 1);
		if (lua_isinteger(lua, 2) && lua_isinteger(lua, 3)) {
			int width = (int)lua_tointeger(lua, 2);
			int height = (int)lua_tointeger(lua, 3);
			if (lua_isstring(lua, 4)) {
				string shader = lua_tostring(lua, 4);
				pTexture = CEngine::GetTextureManager()->Create(name, width, height, shader);
			} else {
				bool depth = lua_isboolean(lua, 4) ? lua_toboolean(lua, 4) != 0: false;
				pTexture = CEngine::GetTextureManager()->Create(name, width, height, false, depth, false);
			}
		} else {
			string textureArray[6];
			if (CStringUtil::Split(textureArray, 6, name, ",", true) == 6) {
				pTexture = CEngine::GetTextureManager()->Create("", textureArray);
			} else pTexture = CEngine::GetTextureManager()->Create(name);
		}
		lua_pushstring(lua, pTexture->GetFilePath().c_str());
		return 1;
	}
	return 0;
}

/**
* 删除纹理资源
*/
int CScriptManager::DoTextureDelete(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CTexture* pTexture = CEngine::GetTextureManager()->GetTexture(lua_tostring(lua, 1));
		CEngine::GetTextureManager()->Drop(pTexture);
	}
	return 0;
}

/**
* 更新纹理数据
*/
int CScriptManager::DoTextureUpdate(lua_State* lua) {
	bool success = false;
	if (lua_isstring(lua, 1) && lua_isstring(lua, 2)) {
		const char* file = lua_tostring(lua, 2);
		CTexture* pTexture = CEngine::GetTextureManager()->GetTexture(lua_tostring(lua, 1));
		if (pTexture) {
			CColor color;
			if (color.FromName(file)) {
				CRectangle region(0, 0, pTexture->GetWidth(), pTexture->GetHeight());
				success = CEngine::GetTextureManager()->Update(pTexture, color, region);
			} else success = CEngine::GetTextureManager()->Update(pTexture, file);
		}
	}
	lua_pushboolean(lua, success);
	return 1;
}

/**
* 读取纹理数据
*/
int CScriptManager::DoTextureData(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CTexture* pTexture = CEngine::GetTextureManager()->GetTexture(lua_tostring(lua, 1));
		if (pTexture) {
			int face = lua_isinteger(lua, 2) ? (int)lua_tointeger(lua, 2) : 0;
			int level = lua_isinteger(lua, 3) ? (int)lua_tointeger(lua, 3) : 0;
			CFileManager::CImageFile image(CFileManager::PNG, 4, pTexture->GetWidth(), pTexture->GetHeight());
			CFileManager::CBinaryFile buffer(image.size);
			CEngine::GetTextureManager()->ReadData(pTexture, face, level, image.contents);
			CEngine::GetFileManager()->WriteFile(&image, buffer.contents, &buffer.size);
			lua_pushlstring(lua, (const char*)buffer.contents, buffer.size);
			return 1;
		}
	}
	lua_pushnil(lua);
	return 1;
}

/**
* 创建着色器
*/
int CScriptManager::DoShaderCreate(lua_State* lua) {
	if (lua_isstring(lua, 1) && lua_isstring(lua, 2) && lua_isstring(lua, 3)) {
		CShader* pShader = 0;
		const char* name = lua_tostring(lua, 1);
		const char* vert = lua_tostring(lua, 2);
		const char* frag = lua_tostring(lua, 3);
		if (CEngine::GetFileManager()->FileExists(vert) && CEngine::GetFileManager()->FileExists(frag)) {
			string file[] = { vert, frag };
			pShader = CEngine::GetShaderManager()->Create(name, file);
		} else {
			pShader = CEngine::GetShaderManager()->Create(name, vert, frag);
		}
		lua_pushstring(lua, pShader->GetName().c_str());
		return 1;
	}
	return 0;
}

/**
* 删除着色器资源
*/
int CScriptManager::DoShaderDelete(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CShaderManager* pShaderMgr = CEngine::GetShaderManager();
		pShaderMgr->Drop(pShaderMgr->GetShader(lua_tostring(lua, 1)));
	}
	return 0;
}

/**
* 更新着色器程序
*/
int CScriptManager::DoShaderUpdate(lua_State* lua) {
	bool success = false;
	if (lua_isstring(lua, 1) && lua_isstring(lua, 2) && lua_isstring(lua, 3)) {
		const char* vert = lua_tostring(lua, 2);
		const char* frag = lua_tostring(lua, 3);
		CShaderManager* pShaderMgr = CEngine::GetShaderManager();
		CShader* pShader = pShaderMgr->GetShader(lua_tostring(lua, 1));
		success = pShaderMgr->Update(pShader, vert, frag);
	}
	lua_pushboolean(lua, success);
	return 1;
}

/**
* 着色器程序宏定义
*/
int CScriptManager::DoShaderDefine(lua_State* lua) {
	bool success = false;
	if (lua_isstring(lua, 1) && lua_isstring(lua, 2) && lua_isstring(lua, 3)) {
		string append = lua_tostring(lua, 2);
		string remove = lua_tostring(lua, 3);
		CShaderManager* pShaderMgr = CEngine::GetShaderManager();
		CShader* pShader = pShaderMgr->GetShader(lua_tostring(lua, 1));
		success = pShaderMgr->Update(pShader, append, remove, 0);
	}
	lua_pushboolean(lua, success);
	return 1;
}

/**
* 设置着色器参数
*/
int CScriptManager::DoShaderParam(lua_State* lua) {
	int paramCount = lua_gettop(lua) - 2;
	if (paramCount > 0 && lua_isstring(lua, 1) && lua_isstring(lua, 2)) {
		CShader* pShader = CEngine::GetShaderManager()->GetShader(lua_tostring(lua, 1));
		if (!pShader) return 0;
		pShader->UseShader();
		string key = lua_tostring(lua, 2);
		if (paramCount == 1) {
			if (lua_isinteger(lua, 3)) pShader->SetUniform(key, (int)lua_tointeger(lua, 3));
			else if (lua_isnumber(lua, 3)) pShader->SetUniform(key, (float)lua_tonumber(lua, 3));
		} else {
			float value[16] = { 0.0f };
			for (int i = 0; i < paramCount; i++) if (lua_isnumber(lua, i + 3)) value[i] = (float)lua_tonumber(lua, i + 3);
			pShader->SetUniform(key, value, paramCount, 1);
		}
	}
	return 0;
}

/**
* 获取已加载的字体列表
*/
int CScriptManager::DoFontList(lua_State* lua) {
	vector<string> fontList;
	CEngine::GetFontManager()->GetFontList(fontList);
	lua_newtable(lua);
	for (size_t i = 0; i < fontList.size(); i++) {
		lua_pushinteger(lua, i + 1);
		lua_pushstring(lua, fontList[i].c_str());
		lua_settable(lua, -3);
	}
	return 1;
}

/**
* 加载字体
*/
int CScriptManager::DoFontLoad(lua_State* lua) {
	bool success = false;
	if (lua_isstring(lua, 1) && lua_isstring(lua, 2)) {
		const char* name = lua_tostring(lua, 1);
		const char* file = lua_tostring(lua, 2);
		success = CEngine::GetFontManager()->Load(file, name);
	}
	lua_pushboolean(lua, success);
	return 1;
}

/**
* 清空全部已加载的字体
*/
int CScriptManager::DoFontClear(lua_State* lua) {
	CEngine::GetFontManager()->Clear();
	return 0;
}

/**
* 设置当前字体
*/
int CScriptManager::DoFontUse(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		const char* name = lua_tostring(lua, 1);
		CEngine::GetFontManager()->UseFont(name);
	}
	return 0;
}

/**
* 创建音源
*/
int CScriptManager::DoSoundCreate(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CSoundManager* pSoundMgr = CEngine::GetSoundManager();
		int soundId = pSoundMgr->Create(lua_tostring(lua, 1));
		bool background = lua_isboolean(lua, 2) ? lua_toboolean(lua, 2) != 0 : false;
		bool loop = lua_isboolean(lua, 3) ? lua_toboolean(lua, 3) != 0 : false;
		float gain = lua_isnumber(lua, 4) ? (float)lua_tonumber(lua, 4) : 1.0f;
		float pitch = lua_isnumber(lua, 5) ? (float)lua_tonumber(lua, 5) : 1.0f;
		if (background) pSoundMgr->SetBackground(soundId);
		pSoundMgr->SetAttrib(soundId, loop, gain, pitch);
		lua_pushinteger(lua, soundId);
		return 1;
	}
	lua_pushinteger(lua, -1);
	return 1;
}

/**
* 删除音源
*/
int CScriptManager::DoSoundDelete(lua_State* lua) {
	if (lua_isinteger(lua, 1)) {
		CEngine::GetSoundManager()->Drop((int)lua_tointeger(lua, 1));
	}
	return 0;
}

/**
* 开始播放声音
*/
int CScriptManager::DoSoundPlay(lua_State* lua) {
	if (lua_isinteger(lua, 1)) {
		CEngine::GetSoundManager()->Play((int)lua_tointeger(lua, 1));
	}
	return 0;
}

/**
* 暂停播放声音
*/
int CScriptManager::DoSoundPause(lua_State* lua) {
	CSoundManager* pSoundMgr = CEngine::GetSoundManager();
	if (lua_isinteger(lua, 1)) pSoundMgr->Pause((int)lua_tointeger(lua, 1));
	else if (lua_isboolean(lua, 1)) pSoundMgr->Pause(lua_toboolean(lua, 1) != 0);
	return 0;
}

/**
* 停止播放声音
*/
int CScriptManager::DoSoundStop(lua_State* lua) {
	if (lua_isinteger(lua, 1)) {
		CEngine::GetSoundManager()->Stop((int)lua_tointeger(lua, 1));
	}
	return 0;
}

/**
* 设置音源位置
*/
int CScriptManager::DoSoundPosition(lua_State* lua) {
	if (lua_isinteger(lua, 1) && lua_isnumber(lua, 2) && lua_isnumber(lua, 3) && lua_isnumber(lua, 4)) {
		int soundId = (int)lua_tointeger(lua, 1);
		float x = (float)lua_tonumber(lua, 2);
		float y = (float)lua_tonumber(lua, 3);
		float z = (float)lua_tonumber(lua, 4);
		CEngine::GetSoundManager()->SetPosition(soundId, CVector3(x, y, z));
	}
	return 0;
}

/**
* 获取或设置音量
*/
int CScriptManager::DoSoundVolume(lua_State* lua) {
	if (lua_isinteger(lua, 1)) {
		CEngine::GetSoundManager()->SetVolume((int)lua_tointeger(lua, 1));
		return 0;
	}
	lua_pushinteger(lua, CEngine::GetSoundManager()->GetVolume());
	return 1;
}

/**
* 更新音频数据
*/
int CScriptManager::DoSoundUpdate(lua_State* lua) {
	bool success = false;
	if (lua_isstring(lua, 1) && lua_isstring(lua, 2)) {
		const char* original = lua_tostring(lua, 1);
		const char* file = lua_tostring(lua, 2);
		success = CEngine::GetSoundManager()->Update(original, file);
	}
	lua_pushboolean(lua, success);
	return 1;
}

/**
* 绑定场景节点到刚体对象
*/
int CScriptManager::DoPhysicsBind(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (!pNode) return 0;
		// 将刚体大小初始设为 1.0
		SGeometry geometry;
		if (pNode->GetType() == "geometry") {
			geometry = static_cast<CSceneNodeGeometry*>(pNode)->GetGeometry(true);
		} else {
			geometry.box.x = 1.0f;
			geometry.box.y = 1.0f;
			geometry.box.z = 1.0f;
		}
		const char* shape = TableValue(lua, 2, "shape", "");
		if (!strcmp(shape, "box")) geometry.shape = SGeometry::BOX;
		else if (!strcmp(shape, "sphere")) geometry.shape = SGeometry::SPHERE;
		else if (!strcmp(shape, "capsule")) geometry.shape = SGeometry::CAPSULE;
		else if (!strcmp(shape, "cylinder")) geometry.shape = SGeometry::CYLINDER;
		else if (!strcmp(shape, "torus")) geometry.shape = SGeometry::TORUS;
		else if (!strcmp(shape, "plane")) geometry.shape = SGeometry::PLANE;
		switch (geometry.shape) {
		case SGeometry::BOX:
			geometry.box.x = TableValue(lua, 2, "x", geometry.box.x);
			geometry.box.y = TableValue(lua, 2, "y", geometry.box.y);
			geometry.box.z = TableValue(lua, 2, "z", geometry.box.z);
			break;
		case SGeometry::SPHERE:
			geometry.sphere.r = TableValue(lua, 2, "r", geometry.sphere.r);
			break;
		case SGeometry::CAPSULE:
			geometry.capsule.r = TableValue(lua, 2, "r", geometry.capsule.r);
			geometry.capsule.h = TableValue(lua, 2, "h", geometry.capsule.h);
			break;
		case SGeometry::CYLINDER:
			geometry.cylinder.r = TableValue(lua, 2, "r", geometry.cylinder.r);
			geometry.cylinder.h = TableValue(lua, 2, "h", geometry.cylinder.h);
			break;
		case SGeometry::TORUS:
			geometry.torus.r = TableValue(lua, 2, "r", geometry.torus.r);
			geometry.torus.c = TableValue(lua, 2, "c", geometry.torus.c);
			break;
		default: break;
		}
		float mass = TableValue(lua, 2, "mass", 1.0f);
		float linearDamping = TableValue(lua, 2, "linearDamping", 0.0f);
		float angularDamping = TableValue(lua, 2, "angularDamping", 0.0f);
		float friction = TableValue(lua, 2, "friction", 0.5f);
		float restitution = TableValue(lua, 2, "restitution", 0.0f);
		bool trigger = TableValue(lua, 2, "trigger", false);
		bool allowSleep = TableValue(lua, 2, "allowSleep", true);
		CRigidBody* pRigidBody = new CRigidBody(pNode);
		pRigidBody->SetAttribute(mass, geometry, trigger, allowSleep);
		pRigidBody->SetDamping(linearDamping, angularDamping);
		pRigidBody->SetFriction(friction);
		pRigidBody->SetRestitution(restitution);
		CEngine::GetPhysicsManager()->AddRigidBody(pRigidBody);
	}
	return 0;
}

/**
* 取消场景节点的物理模拟
*/
int CScriptManager::DoPhysicsUnbind(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		CRigidBody* pRigidBody = CEngine::GetPhysicsManager()->GetRigidBody(pNode);
		if (pRigidBody) {
			int callback = pRigidBody->CollideCallback();
			if (callback >= 0) luaL_unref(lua, LUA_REGISTRYINDEX, callback);
			CEngine::GetPhysicsManager()->DelRigidBody(pRigidBody);
		}
	}
	return 0;
}

/**
* 设置场景节点碰撞事件回调
*/
int CScriptManager::DoPhysicsCollide(lua_State* lua) {
	if (lua_isstring(lua, 1) && lua_isfunction(lua, 2)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		CRigidBody* pRigidBody = CEngine::GetPhysicsManager()->GetRigidBody(pNode);
		if (pRigidBody) {
			int callback = pRigidBody->CollideCallback();
			if (callback >= 0) luaL_unref(lua, LUA_REGISTRYINDEX, callback);
			pRigidBody->CollideCallback() = luaL_ref(lua, LUA_REGISTRYINDEX);
		}
	}
	return 0;
}

/**
* 重置刚体状态，即将刚体线速度和角速度都设为零
*/
int CScriptManager::DoPhysicsReset(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		CRigidBody* pRigidBody = CEngine::GetPhysicsManager()->GetRigidBody(pNode);
		if (pRigidBody) pRigidBody->Reset();
	}
	return 0;
}

/**
* 设置刚体线速度和角速度
*/
int CScriptManager::DoPhysicsVelocity(lua_State* lua) {
	if (lua_isstring(lua, 1) && lua_isnumber(lua, 2) && lua_isnumber(lua, 3) && lua_isnumber(lua, 4)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		CRigidBody* pRigidBody = CEngine::GetPhysicsManager()->GetRigidBody(pNode);
		if (pRigidBody) {
			CVector3 linearVel;
			CVector3 angularVel;
			linearVel[0] = (float)lua_tonumber(lua, 2);
			linearVel[1] = (float)lua_tonumber(lua, 3);
			linearVel[2] = (float)lua_tonumber(lua, 4);
			if (lua_isnumber(lua, 5) && lua_isnumber(lua, 6) && lua_isnumber(lua, 7)) {
				angularVel[0] = (float)lua_tonumber(lua, 5);
				angularVel[1] = (float)lua_tonumber(lua, 6);
				angularVel[2] = (float)lua_tonumber(lua, 7);
			}
			pRigidBody->SetVelocity(linearVel, angularVel);
		}
	}
	return 0;
}

/**
* 向刚体施加力
*/
int CScriptManager::DoPhysicsApplyForce(lua_State* lua) {
	if (lua_isstring(lua, 1) && lua_isnumber(lua, 2) && lua_isnumber(lua, 3) && lua_isnumber(lua, 4)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		CRigidBody* pRigidBody = CEngine::GetPhysicsManager()->GetRigidBody(pNode);
		if (pRigidBody) {
			CVector3 force;
			force[0] = (float)lua_tonumber(lua, 2);
			force[1] = (float)lua_tonumber(lua, 3);
			force[2] = (float)lua_tonumber(lua, 4);
			if (lua_isnumber(lua, 5) && lua_isnumber(lua, 6) && lua_isnumber(lua, 7)) {
				CVector3 relPos;
				relPos[0] = (float)lua_tonumber(lua, 5);
				relPos[1] = (float)lua_tonumber(lua, 6);
				relPos[2] = (float)lua_tonumber(lua, 7);
				CEngine::GetPhysicsManager()->ApplyForce(pRigidBody, force, relPos);
			}
			CEngine::GetPhysicsManager()->ApplyForce(pRigidBody, force);
		}
	}
	return 0;
}

/**
* 向刚体施加冲量
*/
int CScriptManager::DoPhysicsApplyImpulse(lua_State* lua) {
	if (lua_isstring(lua, 1) && lua_isnumber(lua, 2) && lua_isnumber(lua, 3) && lua_isnumber(lua, 4)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		CRigidBody* pRigidBody = CEngine::GetPhysicsManager()->GetRigidBody(pNode);
		if (pRigidBody) {
			CVector3 impulse;
			impulse[0] = (float)lua_tonumber(lua, 2);
			impulse[1] = (float)lua_tonumber(lua, 3);
			impulse[2] = (float)lua_tonumber(lua, 4);
			if (lua_isnumber(lua, 5) && lua_isnumber(lua, 6) && lua_isnumber(lua, 7)) {
				CVector3 pos;
				pos[0] = (float)lua_tonumber(lua, 5);
				pos[1] = (float)lua_tonumber(lua, 6);
				pos[2] = (float)lua_tonumber(lua, 7);
				CEngine::GetPhysicsManager()->ApplyImpulse(pRigidBody, impulse, pos);
			}
			CEngine::GetPhysicsManager()->ApplyImpulse(pRigidBody, impulse);
		}
	}
	return 0;
}

/**
* 设置物理引擎重力
*/
int CScriptManager::DoPhysicsGravity(lua_State* lua) {
	if (lua_gettop(lua) == 1 && lua_isnumber(lua, 1)) {
		float gravity = (float)lua_tonumber(lua, 3);
		CEngine::GetPhysicsManager()->SetGravity(CVector3(0, 0, gravity));
	} else if (lua_isnumber(lua, 1) && lua_isnumber(lua, 2) && lua_isnumber(lua, 3)) {
		float x = (float)lua_tonumber(lua, 1);
		float y = (float)lua_tonumber(lua, 2);
		float z = (float)lua_tonumber(lua, 3);
		CEngine::GetPhysicsManager()->SetGravity(CVector3(x, y, z));
	}
	return 0;
}

/**
* 为关节设置物理模拟
*/
int CScriptManager::DoPhysicsJoint(lua_State* lua) {
	if (lua_isstring(lua, 1) && lua_isstring(lua, 2)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (pNode && pNode->GetType() == "animation") {
			CMeshData* meshData = reinterpret_cast<CSceneNodeAnimation*>(pNode)->GetMeshData();
			if (meshData) {
				float mass = 1.0f;
				float bendFactor = 200.0f;
				float stretchFactor = 100.0f;
				float damping = -0.1f;
				if (lua_isnumber(lua, 3) && lua_isnumber(lua, 4) && lua_isnumber(lua, 5) && lua_isnumber(lua, 6)) {
					mass = (float)lua_tonumber(lua, 3);
					bendFactor = (float)lua_tonumber(lua, 4);
					stretchFactor = (float)lua_tonumber(lua, 5);
					damping = (float)lua_tonumber(lua, 6);
				}
				meshData->SetPhysics(lua_tostring(lua, 2), mass, bendFactor, stretchFactor, damping);
			}
		}
	}
	return 0;
}

/**
* 添加缩放动画
*/
int CScriptManager::DoAnimationScale(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (pNode && lua_isnumber(lua, 2) && lua_isnumber(lua, 3) && lua_isnumber(lua, 4) && lua_isnumber(lua, 5)) {
			float x = (float)lua_tonumber(lua, 2);
			float y = (float)lua_tonumber(lua, 3);
			float z = (float)lua_tonumber(lua, 4);
			float duration = (float)lua_tonumber(lua, 5);
			const char* interpolator = lua_isstring(lua, 6) ? lua_tostring(lua, 6) : "";
			CAnimationManager::Interpolator type = CAnimationManager::LINEAR;
			if (!strcmp(interpolator, "linear")) type = CAnimationManager::LINEAR;
			else if (!strcmp(interpolator, "accelerate")) type = CAnimationManager::ACCELERATE;
			else if (!strcmp(interpolator, "decelerate")) type = CAnimationManager::DECELERATE;
			else if (!strcmp(interpolator, "acceleratedecelerate")) type = CAnimationManager::ACCELERATEDECELERATE;
			CEngine::GetAnimationManager()->AddScale(pNode, CVector3(x, y, z), duration, type);
		}
	}
	return 0;
}

/**
* 添加旋转动画
*/
int CScriptManager::DoAnimationRotation(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (pNode && lua_isnumber(lua, 2) && lua_isnumber(lua, 3) && lua_isnumber(lua, 4) && lua_isnumber(lua, 5)) {
			float x = (float)lua_tonumber(lua, 2);
			float y = (float)lua_tonumber(lua, 3);
			float z = (float)lua_tonumber(lua, 4);
			float duration = (float)lua_tonumber(lua, 5);
			const char* interpolator = lua_isstring(lua, 6) ? lua_tostring(lua, 6) : "";
			CAnimationManager::Interpolator type = CAnimationManager::LINEAR;
			if (!strcmp(interpolator, "linear")) type = CAnimationManager::LINEAR;
			else if (!strcmp(interpolator, "accelerate")) type = CAnimationManager::ACCELERATE;
			else if (!strcmp(interpolator, "decelerate")) type = CAnimationManager::DECELERATE;
			else if (!strcmp(interpolator, "acceleratedecelerate")) type = CAnimationManager::ACCELERATEDECELERATE;
			CEngine::GetAnimationManager()->AddRotation(pNode, CQuaternion().FromEulerAngles(x, y, z), duration, type);
		}
	}
	return 0;
}

/**
* 添加位移动画
*/
int CScriptManager::DoAnimationTranslation(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (pNode && lua_isnumber(lua, 2) && lua_isnumber(lua, 3) && lua_isnumber(lua, 4) && lua_isnumber(lua, 5)) {
			float x = (float)lua_tonumber(lua, 2);
			float y = (float)lua_tonumber(lua, 3);
			float z = (float)lua_tonumber(lua, 4);
			float duration = (float)lua_tonumber(lua, 5);
			const char* interpolator = lua_isstring(lua, 6) ? lua_tostring(lua, 6) : "";
			CAnimationManager::Interpolator type = CAnimationManager::LINEAR;
			if (!strcmp(interpolator, "linear")) type = CAnimationManager::LINEAR;
			else if (!strcmp(interpolator, "accelerate")) type = CAnimationManager::ACCELERATE;
			else if (!strcmp(interpolator, "decelerate")) type = CAnimationManager::DECELERATE;
			else if (!strcmp(interpolator, "acceleratedecelerate")) type = CAnimationManager::ACCELERATEDECELERATE;
			CEngine::GetAnimationManager()->AddTranslation(pNode, CVector3(x, y, z), duration, type);
		}
	}
	return 0;
}

/**
* 开始执行动画
*/
int CScriptManager::DoAnimationStart(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (pNode) {
			int repeat = lua_isinteger(lua, 2) ? (int)lua_tointeger(lua, 2) : 1;
			bool swing = lua_isboolean(lua, 3) ? lua_toboolean(lua, 3) != 0 : false;
			bool relative = lua_isboolean(lua, 4) ? lua_toboolean(lua, 4) != 0 : false;
			float delay = lua_isnumber(lua, 5) ? (float)lua_tonumber(lua, 5) : 0.0f;
			CEngine::GetAnimationManager()->Start(pNode, repeat, swing, relative, delay);
		}
	}
	return 0;
}

/**
* 停止执行动画
*/
int CScriptManager::DoAnimationStop(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (pNode) {
			bool reset = lua_isboolean(lua, 2) ? lua_toboolean(lua, 2) != 0 : false;
			CEngine::GetAnimationManager()->Stop(pNode, reset);
		}
	}
	return 0;
}