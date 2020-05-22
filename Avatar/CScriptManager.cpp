//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CScriptManager.h"
#include "AvatarConfig.h"
#include "CEngine.h"
#include "CTimer.h"
#include "CLog.h"
#include "CStringUtil.h"
#include "CPluginLoader.h"
#include "CCameraChase.h"
#include "CCameraGaze.h"
#include "CCameraFree.h"
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
#pragma comment(lib, "thirdparty/lua/liblua.lib")
#endif

/**
* ��������Ӻ���
*/
#define SET_TABLE_FUNCTION(NAME, FUNC) \
	lua_pushstring(lua, NAME); \
	lua_pushcfunction(lua, FUNC); \
	lua_rawset(lua, -3)

/**
* ��ȡ�����ֶ�ֵ
*/
inline bool LuaTableFieldValue(lua_State* lua, int table, const char* name, bool def) {
	lua_getfield(lua, table, name);
	return lua_isboolean(lua, -1)? lua_toboolean(lua, -1) != 0: def;
}
inline int LuaTableFieldValue(lua_State* lua, int table, const char* name, int def) {
	lua_getfield(lua, table, name);
	return lua_isinteger(lua, -1)? (int)lua_tointeger(lua, -1): def;
}
inline float LuaTableFieldValue(lua_State* lua, int table, const char* name, float def) {
	lua_getfield(lua, table, name);
	return lua_isnumber(lua, -1)? (float)lua_tonumber(lua, -1): def;
}
inline const char* LuaTableFieldValue(lua_State* lua, int table, const char* name, const char* def) {
	lua_getfield(lua, table, name);
	return lua_isstring(lua, -1)? (const char*)lua_tostring(lua, -1): def;
}

/**
* ���캯��
*/
CScriptManager::CScriptManager() {
	m_pContext = new CScriptContext();
	m_pLuaState = luaL_newstate();
	luaL_openlibs(m_pLuaState);
	RegisterInterface(m_pLuaState);
}

/**
* ��������
*/
CScriptManager::~CScriptManager() {
	m_pInstance = 0;
}

/**
* ����ʵ��
*/
CScriptManager* CScriptManager::m_pInstance = 0;

/**
* ���ٽű�������ʵ��
*/
void CScriptManager::Destroy() {
	lua_close(m_pLuaState);
	delete m_pContext;
	delete this;
}

/**
* ����ڽű�
*/
void CScriptManager::OpenScript(const string& script, void* engine) {
	CFileManager::CBinaryFile file;
	CEngine::GetFileManager()->ReadFile(script, &file);
	// ���� UTF8 �� BOM
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
	// ��¼�������ָ��
	lua_pushstring(m_pLuaState, "ENGINE_INSTANCE");
	lua_pushlightuserdata(m_pLuaState, engine);
	lua_settable(m_pLuaState, LUA_REGISTRYINDEX);
}

/**
* ��ȡ�ӿ�������
*/
CScriptContext* CScriptManager::GetContext(lua_State* lua) {
	if (!lua) lua = m_pLuaState;
	return m_pContext->GetCurrent(lua);
}

/**
* ��ʼ������¼�
*/
void CScriptManager::OnReady() {
	lua_getglobal(m_pLuaState, "OnReady");
	lua_pcall(m_pLuaState, 0, 0, 0);
}

/**
* �����ڲ��߼�
*/
void CScriptManager::OnUpdate(float dt) {
	// �ű�����ִ��
	while (!m_queScriptQueue.empty()) {
		string script = std::move(m_queScriptQueue.front());
		m_queScriptQueue.pop();
		if (LUA_OK != luaL_dostring(m_pLuaState, script.c_str())) {
			CLog::Warn("Execute script '%s' error", script.c_str());
			CLog::Warn(lua_tostring(m_pLuaState, -1));
		}
	}
	// ���� OnUpdate �ű�����
	lua_getglobal(m_pLuaState, "OnUpdate");
	lua_pushnumber(m_pLuaState, dt);
	lua_pcall(m_pLuaState, 1, 0, 0);
}

/**
* �����˳��¼�
*/
void CScriptManager::OnExit() {
	lua_getglobal(m_pLuaState, "OnExit");
	lua_pcall(m_pLuaState, 0, 0, 0);
}

/**
* �����С�ı��¼�
*/
void CScriptManager::OnSize(int width, int height) {
	lua_getglobal(m_pLuaState, "OnSize");
	lua_pushinteger(m_pLuaState, width);
	lua_pushinteger(m_pLuaState, height);
	lua_pcall(m_pLuaState, 2, 0, 0);
}

/**
* �����¼�
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
* �����¼�
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
* �ű�����
*/
void CScriptManager::Script(const char* script) {
	m_queScriptQueue.push(script);
	CLog::Info("[script] %s", script);
}

/**
* ע���Զ��巽���ص�
* @remark ���� callback ����ԭ��Ϊ int func(const char*)
*/
void CScriptManager::Register(const string& function, void* callback) {
	if (callback) m_mapFunction[function] = callback;
	else m_mapFunction.erase(function);
}

/**
* GUI �����¼�
*/
void CScriptManager::GuiEvent(const string& name, int evt, int arg1, int arg2) {
	map<string, int>::iterator iter = m_mapGuiEvent.find(name);
	if (iter != m_mapGuiEvent.end()) {
		m_vecEventQueue.push_back(SGuiEvent(iter->second, evt, arg1, arg2));
	}
}

/**
* ������ײ�¼�
*/
void CScriptManager::CollideEnter(int callback, const string& name, const CVector3& pos) {
	lua_rawgeti(m_pLuaState, LUA_REGISTRYINDEX, callback);
	lua_pushstring(m_pLuaState, name.c_str());
	lua_pushnumber(m_pLuaState, pos.m_fValue[0]);
	lua_pushnumber(m_pLuaState, pos.m_fValue[1]);
	lua_pushnumber(m_pLuaState, pos.m_fValue[2]);
	lua_pcall(m_pLuaState, 4, 0, 0);
}

/**
* ������ײȡ���¼�
*/
void CScriptManager::CollideLeave(int callback, const string& name) {
	lua_rawgeti(m_pLuaState, LUA_REGISTRYINDEX, callback);
	lua_pushstring(m_pLuaState, name.c_str());
	lua_pcall(m_pLuaState, 1, 0, 0);
}

/**
* GUI �ű��¼��ص�
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
* ע��ű����ýӿ�
*/
void CScriptManager::RegisterInterface(lua_State* lua) {
	// ϵͳ�ӿ�
	lua_newtable(lua);
	SET_TABLE_FUNCTION("info", DoEngineInfo);
	SET_TABLE_FUNCTION("fps", DoEngineFps);
	SET_TABLE_FUNCTION("log", DoEngineLog);
	SET_TABLE_FUNCTION("speed", DoEngineSpeed);
	SET_TABLE_FUNCTION("timer", DoEngineTimer);
	SET_TABLE_FUNCTION("directory", DoEngineDirectory);
	SET_TABLE_FUNCTION("input", DoEngineInput);
	SET_TABLE_FUNCTION("script", DoEngineScript);
	SET_TABLE_FUNCTION("execute", DoEngineExecute);
	SET_TABLE_FUNCTION("read", DoEngineRead);
	SET_TABLE_FUNCTION("write", DoEngineWrite);
	SET_TABLE_FUNCTION("plugin", DoEnginePlugin);
	lua_setglobal(lua, "engine");
	// GUI�ӿ�
	lua_newtable(lua);
	SET_TABLE_FUNCTION("enable", DoGuiEnable);
	SET_TABLE_FUNCTION("size", DoGuiSize);
	SET_TABLE_FUNCTION("scale", DoGuiScale);
	SET_TABLE_FUNCTION("create", DoGuiCreate);
	SET_TABLE_FUNCTION("modify", DoGuiModify);
	SET_TABLE_FUNCTION("delete", DoGuiDelete);
	SET_TABLE_FUNCTION("attrib", DoGuiAttrib);
	lua_setglobal(lua, "gui");
	// ����ӿ�
	lua_newtable(lua);
	SET_TABLE_FUNCTION("type", DoCameraType);
	SET_TABLE_FUNCTION("control", DoCameraControl);
	SET_TABLE_FUNCTION("fov", DoCameraFov);
	SET_TABLE_FUNCTION("clip", DoCameraClip);
	SET_TABLE_FUNCTION("position", DoCameraPosition);
	SET_TABLE_FUNCTION("target", DoCameraTarget);
	SET_TABLE_FUNCTION("angle", DoCameraAngle);
	SET_TABLE_FUNCTION("direction", DoCameraDirection);
	SET_TABLE_FUNCTION("bind", DoCameraBind);
	lua_setglobal(lua, "camera");
	// �����ڵ�ӿ�
	lua_newtable(lua);
	SET_TABLE_FUNCTION("insert", DoSceneInsert);
	SET_TABLE_FUNCTION("delete", DoSceneDelete);
	SET_TABLE_FUNCTION("clear", DoSceneClear);
	SET_TABLE_FUNCTION("list", DoSceneList);
	SET_TABLE_FUNCTION("pick", DoScenePick);
	SET_TABLE_FUNCTION("scale", DoSceneScale);
	SET_TABLE_FUNCTION("angle", DoSceneAngle);
	SET_TABLE_FUNCTION("orientation", DoSceneOrientation);
	SET_TABLE_FUNCTION("position", DoScenePosition);
	SET_TABLE_FUNCTION("visible", DoSceneVisible);
	SET_TABLE_FUNCTION("update", DoSceneUpdate);
	SET_TABLE_FUNCTION("vertex", DoSceneVertex);
	SET_TABLE_FUNCTION("material", DoSceneMaterial);
	SET_TABLE_FUNCTION("renderMode", DoSceneRenderMode);
	SET_TABLE_FUNCTION("boundingBox", DoSceneBoundingBox);
	SET_TABLE_FUNCTION("handle", DoSceneHandle);
	lua_setglobal(lua, "scene");
	// �����ӿ�
	lua_newtable(lua);
	SET_TABLE_FUNCTION("list", DoPostList);
	SET_TABLE_FUNCTION("enable", DoPostEnable);
	SET_TABLE_FUNCTION("register", DoPostRegister);
	SET_TABLE_FUNCTION("param", DoPostParam);
	lua_setglobal(lua, "post");
	// ͼ�ι����ӿ�
	lua_newtable(lua);
	SET_TABLE_FUNCTION("screenshot", DoGraphicsScreenshot);
	SET_TABLE_FUNCTION("stereo", DoGraphicsStereo);
	SET_TABLE_FUNCTION("windowSize", DoGraphicsWindowSize);
	SET_TABLE_FUNCTION("lightPosition", DoGraphicsLightPosition);
	SET_TABLE_FUNCTION("directionLight", DoGraphicsDirectionLight);
	SET_TABLE_FUNCTION("pointLight", DoGraphicsPointLight);
	SET_TABLE_FUNCTION("spotLight", DoGraphicsSpotLight);
	SET_TABLE_FUNCTION("shadow", DoGraphicsShadow);
	SET_TABLE_FUNCTION("fog", DoGraphicsFog);
	SET_TABLE_FUNCTION("environmentMap", DoGraphicsEnvironmentMap);
	SET_TABLE_FUNCTION("background", DoGraphicsBackground);
	SET_TABLE_FUNCTION("pickingRay", DoGraphicsPickingRay);
	SET_TABLE_FUNCTION("project", DoGraphicsProject);
	SET_TABLE_FUNCTION("renderTarget", DoGraphicsRenderTarget);
	lua_setglobal(lua, "graphics");
	// ���������ӿ�
	lua_newtable(lua);
	SET_TABLE_FUNCTION("create", DoTextureCreate);
	SET_TABLE_FUNCTION("delete", DoTextureDelete);
	SET_TABLE_FUNCTION("update", DoTextureUpdate);
	lua_setglobal(lua, "texture");
	// ��ɫ�������ӿ�
	lua_newtable(lua);
	SET_TABLE_FUNCTION("create", DoShaderCreate);
	SET_TABLE_FUNCTION("delete", DoShaderDelete);
	SET_TABLE_FUNCTION("update", DoShaderUpdate);
	SET_TABLE_FUNCTION("param", DoShaderParam);
	lua_setglobal(lua, "shader");
	// ��������ӿ�
	lua_newtable(lua);
	SET_TABLE_FUNCTION("list", DoFontList);
	SET_TABLE_FUNCTION("load", DoFontLoad);
	SET_TABLE_FUNCTION("clear", DoFontClear);
	SET_TABLE_FUNCTION("use", DoFontUse);
	lua_setglobal(lua, "font");
	// ���������ӿ�
	lua_newtable(lua);
	SET_TABLE_FUNCTION("create", DoSoundCreate);
	SET_TABLE_FUNCTION("delete", DoSoundDelete);
	SET_TABLE_FUNCTION("play", DoSoundPlay);
	SET_TABLE_FUNCTION("pause", DoSoundPause);
	SET_TABLE_FUNCTION("stop", DoSoundStop);
	SET_TABLE_FUNCTION("position", DoSoundPosition);
	SET_TABLE_FUNCTION("volume", DoSoundVolume);
	SET_TABLE_FUNCTION("update", DoSoundUpdate);
	lua_setglobal(lua, "sound");
	// ��������ӿ�
	lua_newtable(lua);
	SET_TABLE_FUNCTION("bind", DoPhysicsBind);
	SET_TABLE_FUNCTION("unbind", DoPhysicsUnbind);
	SET_TABLE_FUNCTION("collide", DoPhysicsCollide);
	SET_TABLE_FUNCTION("reset", DoPhysicsReset);
	SET_TABLE_FUNCTION("velocity", DoPhysicsVelocity);
	SET_TABLE_FUNCTION("applyForce", DoPhysicsApplyForce);
	SET_TABLE_FUNCTION("applyImpulse", DoPhysicsApplyImpulse);
	SET_TABLE_FUNCTION("gravity", DoPhysicsGravity);
	SET_TABLE_FUNCTION("joint", DoPhysicsJoint);
	lua_setglobal(lua, "physics");
	// �����ӿ�
	lua_newtable(lua);
	SET_TABLE_FUNCTION("param", DoAnimationParam);
	SET_TABLE_FUNCTION("scale", DoAnimationScale);
	SET_TABLE_FUNCTION("rotation", DoAnimationRotation);
	SET_TABLE_FUNCTION("translation", DoAnimationTranslation);
	SET_TABLE_FUNCTION("start", DoAnimationStart);
	SET_TABLE_FUNCTION("stop", DoAnimationStop);
	lua_setglobal(lua, "animation");
}

/**
* ��ȡ������Ϣ
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
	lua_pushstring(lua, "VERSION");
	lua_pushstring(lua, AVATAR_VERSION);
	lua_settable(lua, -3);
	lua_pushstring(lua, "PLATFORM");
	lua_pushstring(lua, platform);
	lua_settable(lua, -3);
	return 1;
}

/**
* ��ȡ��ǰ֡��
*/
int CScriptManager::DoEngineFps(lua_State* lua) {
	lua_pushnumber(lua, CEngine::GetGraphicsManager()->GetFrameRate());
	return 1;
}

/**
* ʹ����־�������־��Ϣ
*/
int CScriptManager::DoEngineLog(lua_State* lua) {
	if (lua_isboolean(lua, 1)) {
		if (lua_toboolean(lua, 1)) CLog::Create(true, false);
		else CLog::Destroy();
		if (lua_isinteger(lua, 2)) {
			CLog::SetLevel((CLog::Level)lua_tointeger(lua, 2));
		}
	} else if (lua_isstring(lua, 1)) {
		CLog::Info(lua_tostring(lua, 1));
	}
	return 0;
}

/**
* ���������ٶ�
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
* ��ȡ��ʱ����ʱ
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
* ��ȡ����������Ŀ¼
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
* ��������ź�
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
* ִ�� lua �ű�
*/
int CScriptManager::DoEngineScript(lua_State* lua) {
	bool success = false;
	if (lua_isstring(lua, 1)) {
		const char* filename = lua_tostring(lua, 1);
		CFileManager::CBinaryFile file;
		CEngine::GetFileManager()->ReadFile(filename, &file);
		// ���� UTF8 �� BOM
		int offset = 0;
		const unsigned char* header = file.contents;
		if (file.size >= 3 && header[0] == 0xEF && header[1] == 0xBB && header[2] == 0xBF) offset = 3;
		if (luaL_loadbuffer(lua, (char*)file.contents + offset, file.size - offset, filename) ||
			lua_pcall(lua, 0, LUA_MULTRET, 0)) {
			CLog::Error("Open script file '%s' error", filename);
			CLog::Error(lua_tostring(lua, -1));
		} else {
			success = true;
		}
	}
	lua_pushboolean(lua, success);
	return 1;
}

/**
* ִ�лص��¼�
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
* ��ȡָ���ļ�����
*/
int CScriptManager::DoEngineRead(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CFileManager::CTextFile file;
		if (CEngine::GetFileManager()->ReadFile(lua_tostring(lua, 1), &file)) {
			lua_pushstring(lua, (char*)file.contents);
			return 1;
		}
	}
	lua_pushnil(lua);
	return 1;
}

/**
* д��������ָ���ļ�
*/
int CScriptManager::DoEngineWrite(lua_State* lua) {
	if (lua_isstring(lua, 1) && !lua_isnoneornil(lua, 2)) {
		const char* content = lua_tostring(lua, 2);
		CFileManager::CTextFile file(strlen(content));
		memcpy(file.contents, content, file.size);
		CEngine::GetFileManager()->WriteFile(&file, lua_tostring(lua, 1));
	}
	return 0;
}

/**
* ����������
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
* GUI ʹ�ܻ��ֹ
*/
int CScriptManager::DoGuiEnable(lua_State* lua) {
	bool enable = lua_isboolean(lua, 1) ? lua_toboolean(lua, 1) != 0 : true;
	CGuiEnvironment::GetInstance()->SetEnable(enable);
	return 0;
}

/**
* ��ȡ GUI ������С
*/
int CScriptManager::DoGuiSize(lua_State* lua) {
	int width, height;
	CGuiEnvironment::GetInstance()->GetSize(&width, &height);
	lua_pushinteger(lua, width);
	lua_pushinteger(lua, height);
	return 2;
}

/**
* ���� GUI ����
*/
int CScriptManager::DoGuiScale(lua_State* lua) {
	if (lua_isnumber(lua, 1)) {
		float scale = (float)lua_tonumber(lua, 1);
		CGuiEnvironment::GetInstance()->SetScale(scale);
	}
	return 0;
}

/**
* ���� GUI Ԫ��
*/
int CScriptManager::DoGuiCreate(lua_State* lua) {
	if (lua_isstring(lua, 1) && lua_isstring(lua, 2) && lua_isstring(lua, 3)) {
		string name = lua_tostring(lua, 1);
		string type = lua_tostring(lua, 2);
		string desc = lua_tostring(lua, 3);
		// �����¼�
		if (CGuiEnvironment::GetInstance()->GuiCreate(name, type, desc) && lua_isfunction(lua, 4)) {
			int callback = luaL_ref(lua, LUA_REGISTRYINDEX);
			m_pInstance->m_mapGuiEvent.insert(std::pair<string, int>(name, callback));
		}
	}
	return 0;
}

/**
* �޸� GUI Ԫ��
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
* ɾ�� GUI Ԫ��
*/
int CScriptManager::DoGuiDelete(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		string name = lua_tostring(lua, 1);
		CGuiEnvironment::GetInstance()->GuiDelete(name);
		// ɾ���¼�
		map<string, int>::iterator iter = m_pInstance->m_mapGuiEvent.find(name);
		if (iter != m_pInstance->m_mapGuiEvent.end()) {
			luaL_unref(lua, LUA_REGISTRYINDEX, iter->second);
			m_pInstance->m_mapGuiEvent.erase(iter);
		}
	}
	return 0;
}

/**
* ��ȡ GUI Ԫ������
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
* ��ȡ�������������
*/
int CScriptManager::DoCameraType(lua_State* lua) {
	CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
	if (lua_isstring(lua, 1)) {
		const char* cameraType = lua_tostring(lua, 1);
		if (pCamera) delete pCamera;
		if (!strcmp(cameraType, "default")) pCamera = new CCamera();
		else if (!strcmp(cameraType, "chase")) pCamera = new CCameraChase();
		else if (!strcmp(cameraType, "free")) pCamera = new CCameraFree();
		else if (!strcmp(cameraType, "gaze")) pCamera = new CCameraGaze();
		else if (!strcmp(cameraType, "geographic")) pCamera = new CCameraGeographic();
		else if (!strcmp(cameraType, "viewer")) pCamera = new CCameraViewer();
		else {
			pCamera = new CCamera();
			CLog::Warn("Unknow camera type '%s'", cameraType);
		}
		if (lua_istable(lua, 2)) {
			float damping = LuaTableFieldValue(lua, 2, "damping", -1.0f);
			float minDistance = LuaTableFieldValue(lua, 2, "minDistance", -1.0f);
			float maxDistance = LuaTableFieldValue(lua, 2, "maxDistance", -1.0f);
			if (damping > 0.0f) {
				if (!strcmp(cameraType, "chase")) static_cast<CCameraChase*>(pCamera)->SetDamping(damping);
				else if (!strcmp(cameraType, "free")) static_cast<CCameraFree*>(pCamera)->SetDamping(damping);
				else if (!strcmp(cameraType, "gaze")) static_cast<CCameraGaze*>(pCamera)->SetDamping(damping);
				else if (!strcmp(cameraType, "geographic")) static_cast<CCameraGeographic*>(pCamera)->SetDamping(damping);
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
				float pitchMin = LuaTableFieldValue(lua, 2, "pitchMin", 3.141593f);
				float pitchMax = LuaTableFieldValue(lua, 2, "pitchMax", -3.141593f);
				if (pitchMin < pitchMax) static_cast<CCameraGaze*>(pCamera)->SetPitchRange(pitchMin, pitchMax);
			} else if (!strcmp(cameraType, "free")) {
				string track_line = LuaTableFieldValue(lua, 2, "track_line", "");
				float track_speed = LuaTableFieldValue(lua, 2, "track_speed", 1.0f);
				bool track_follow = LuaTableFieldValue(lua, 2, "track_follow", true);
				bool track_loop = LuaTableFieldValue(lua, 2, "track_loop", true);
				float offset_x = LuaTableFieldValue(lua, 2, "track_offset_x", 0.0f);
				float offset_y = LuaTableFieldValue(lua, 2, "track_offset_y", 0.0f);
				float offset_z = LuaTableFieldValue(lua, 2, "track_offset_z", 0.0f);
				if (!track_line.empty()) {
					CSceneNodeLine* pLine = static_cast<CSceneNodeLine*>(CEngine::GetSceneManager()->GetNodeByName(track_line));
					if (pLine) {
						vector<CVector3> track;
						for (int i = 0; i < pLine->GetPointCount(); i++) track.push_back(pLine->GetPoint(i)->m_fPosition);
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
* ���û����������
*/
int CScriptManager::DoCameraControl(lua_State* lua) {
	bool enable = lua_isboolean(lua, 1) ? lua_toboolean(lua, 1) != 0 : true;
	CEngine::GetGraphicsManager()->GetCamera()->Control(enable);
	return 0;
}

/**
* ��ȡ����������ӽ�
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
* ��ȡ����������ü���
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
* ��ȡ���������λ��
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
* ��ȡ���������Ŀ���
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
* ��ȡ����������Ƕ�
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
	pCamera->GetYawPitchRoll(pCamera->m_cLookVector, pCamera->m_cUpVector, &yaw, &pitch, &roll);
	lua_pushnumber(lua, yaw);
	lua_pushnumber(lua, pitch);
	lua_pushnumber(lua, roll);
	return 3;
}

/**
* ��ȡ�����������������
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
* ��ָ�������ڵ������
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
* ���볡���ڵ�
*/
int CScriptManager::DoSceneInsert(lua_State* lua) {
	CSceneNode* pNode = 0;
	if (lua_isstring(lua, 2) && lua_isstring(lua, 3)) {
		CSceneNode* pParent = 0;
		if (lua_isstring(lua, 1)) {
			// ͨ�����ƻ�ȡ���ڵ�
			pParent = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		}
		// �ڵ����ͺ�����
		const char* type = lua_tostring(lua, 2);
		const char* name = lua_tostring(lua, 3);
		// ���ݳ����ڵ����ʹ���
		if (!strcmp("animation", type)) {
			const char* meshFile = LuaTableFieldValue(lua, 4, "meshFile", "");
			bool start = LuaTableFieldValue(lua, 4, "start", true);
			bool skeleton = LuaTableFieldValue(lua, 4, "skeleton", false);
			pNode = new CSceneNodeAnimation(name, meshFile, start, skeleton);
		} else if (!strcmp("axis", type)) {
			pNode = new CSceneNodeAxis(name);
		} else if (!strcmp("blast", type)) {
			const char* texture = LuaTableFieldValue(lua, 4, "texture", "");
			int row = LuaTableFieldValue(lua, 4, "row", 8);
			int column = LuaTableFieldValue(lua, 4, "column", 8);
			pNode = new CSceneNodeBlast(name, texture, row, column);
		} else if (!strcmp("board", type)) {
			const char* texture = LuaTableFieldValue(lua, 4, "texture", "");
			float width = LuaTableFieldValue(lua, 4, "width", 1.0f);
			float height = LuaTableFieldValue(lua, 4, "height", 1.0f);
			int billboard = LuaTableFieldValue(lua, 4, "billboard", 0);
			pNode = new CSceneNodeBoard(name, texture, width, height, billboard);
		} else if (!strcmp("cloud", type)) {
			const char* color1 = LuaTableFieldValue(lua, 4, "skyColor", "#666699FF");
			const char* color2 = LuaTableFieldValue(lua, 4, "cloudColor", "#666699FF");
			float cloudSize = LuaTableFieldValue(lua, 4, "cloudSize", 100.0f);
			pNode = new CSceneNodeCloud(name, CColor(color1), CColor(color2), cloudSize);
		} else if (!strcmp("decal", type)) {
			CMatrix4 proj;
			const char* texture = LuaTableFieldValue(lua, 4, "texture", "");
			float fov = LuaTableFieldValue(lua, 4, "fov", 60.0f);
			pNode = new CSceneNodeDecal(name, texture, proj.Perspective(fov, 1.0f, 0.5f, 1000.0f));
		} else if (!strcmp("flame", type)) {
			const char* texture = LuaTableFieldValue(lua, 4, "texture", "");
			const char* distortionMap = LuaTableFieldValue(lua, 4, "distortionMap", "");
			const char* alphaMask = LuaTableFieldValue(lua, 4, "alphaMask", "");
			float width = LuaTableFieldValue(lua, 4, "width", 1.0f);
			float height = LuaTableFieldValue(lua, 4, "height", 1.0f);
			pNode = new CSceneNodeFlame(name, texture, distortionMap, alphaMask, width, height);
		} else if (!strcmp("fresnel", type)) {
			const char* meshFile = LuaTableFieldValue(lua, 4, "meshFile", "");
			pNode = new CSceneNodeFresnel(name, meshFile);
		} else if (!strcmp("geometry", type)) {
			const char* shape = LuaTableFieldValue(lua, 4, "shape", "");
			const char* texture = LuaTableFieldValue(lua, 4, "texture", "");
			SGeometry geometry;
			geometry.reversed = LuaTableFieldValue(lua, 4, "reversed", false);
			geometry.slices = LuaTableFieldValue(lua, 4, "slices", 32);
			if (!strcmp(shape, "box")) geometry.shape = SGeometry::BOX;
			else if (!strcmp(shape, "sphere")) geometry.shape = SGeometry::SPHERE;
			else if (!strcmp(shape, "capsule")) geometry.shape = SGeometry::CAPSULE;
			else if (!strcmp(shape, "cylinder")) geometry.shape = SGeometry::CYLINDER;
			else if (!strcmp(shape, "torus")) geometry.shape = SGeometry::TORUS;
			else if (!strcmp(shape, "cone")) geometry.shape = SGeometry::CONE;
			else if (!strcmp(shape, "plane")) geometry.shape = SGeometry::PLANE;
			else if (!strcmp(shape, "circle")) geometry.shape = SGeometry::CIRCLE;
			// �������������
			switch (geometry.shape) {
			case SGeometry::BOX:
				geometry.box.x = LuaTableFieldValue(lua, 4, "x", 1.0f);
				geometry.box.y = LuaTableFieldValue(lua, 4, "y", 1.0f);
				geometry.box.z = LuaTableFieldValue(lua, 4, "z", 1.0f);
				break;
			case SGeometry::SPHERE:
				geometry.sphere.r = LuaTableFieldValue(lua, 4, "r", 1.0f);
				break;
			case SGeometry::CAPSULE:
				geometry.capsule.r = LuaTableFieldValue(lua, 4, "r", 1.0f);
				geometry.capsule.h = LuaTableFieldValue(lua, 4, "h", 1.0f);
				break;
			case SGeometry::CYLINDER:
				geometry.cylinder.r = LuaTableFieldValue(lua, 4, "r", 1.0f);
				geometry.cylinder.h = LuaTableFieldValue(lua, 4, "h", 1.0f);
				break;
			case SGeometry::TORUS:
				geometry.torus.r = LuaTableFieldValue(lua, 4, "r", 1.0f);
				geometry.torus.c = LuaTableFieldValue(lua, 4, "c", 1.0f);
				break;
			case SGeometry::CONE:
				geometry.cone.r = LuaTableFieldValue(lua, 4, "r", 1.0f);
				geometry.cone.h = LuaTableFieldValue(lua, 4, "h", 1.0f);
				break;
			case SGeometry::PLANE:
				geometry.plane.w = LuaTableFieldValue(lua, 4, "w", 1.0f);
				geometry.plane.h = LuaTableFieldValue(lua, 4, "h", 1.0f);
				break;
			case SGeometry::CIRCLE:
				geometry.circle.r = LuaTableFieldValue(lua, 4, "r", 1.0f);
				break;
			default: break;
			}
			pNode = new CSceneNodeGeometry(name, texture, geometry);
		} else if (!strcmp("lensflare", type)) {
			pNode = new CSceneNodeLensflare(name);
		} else if (!strcmp("line", type)) {
			const char* color = LuaTableFieldValue(lua, 4, "color", "red");
			float width = LuaTableFieldValue(lua, 4, "width", 1.0f);
			pNode = new CSceneNodeLine(name, CColor(color), width);
		} else if (!strcmp("particles", type)) {
			float speed[3];
			const char* texture = LuaTableFieldValue(lua, 4, "texture", "");
			const char* color = LuaTableFieldValue(lua, 4, "color", "#FFC080FF");
			int count = LuaTableFieldValue(lua, 4, "count", 100);
			float size = LuaTableFieldValue(lua, 4, "size", 0.5f);
			bool dark = LuaTableFieldValue(lua, 4, "dark", false);
			float spread = LuaTableFieldValue(lua, 4, "spread", 0.5f);
			float fade = LuaTableFieldValue(lua, 4, "fade", 1.0f);
			speed[0] = LuaTableFieldValue(lua, 4, "dx", 0.0f);
			speed[1] = LuaTableFieldValue(lua, 4, "dy", 0.0f);
			speed[2] = LuaTableFieldValue(lua, 4, "dz", 0.0f);
			pNode = new CSceneNodeParticles(name, texture, size, count, dark, CColor(color), speed, spread, fade, true);
		} else if (!strcmp("planet", type)) {
			const char* texture = LuaTableFieldValue(lua, 4, "texture", "");
			const char* textureNight = LuaTableFieldValue(lua, 4, "textureNight", "");
			float radius = LuaTableFieldValue(lua, 4, "radius", 100.0f);
			int slices = LuaTableFieldValue(lua, 4, "slices", 64);
			pNode = new CSceneNodePlanet(name, texture, textureNight, radius, slices);
		} else if (!strcmp("plant", type)) {
			const char* texture = LuaTableFieldValue(lua, 4, "texture", "");
			float width = LuaTableFieldValue(lua, 4, "width", 1.0f);
			float height = LuaTableFieldValue(lua, 4, "height", 1.0f);
			const char* distributionMap = LuaTableFieldValue(lua, 4, "distributionMap", "");
			int count = LuaTableFieldValue(lua, 4, "count", 100);
			float density = LuaTableFieldValue(lua, 4, "density", 0.3f);
			float range = LuaTableFieldValue(lua, 4, "range", 1000.0f);
			pNode = new CSceneNodePlant(name, texture, width, height, distributionMap, count, density, range);
		} else if (!strcmp("screen", type)) {
			const char* texture = LuaTableFieldValue(lua, 4, "texture", "");
			int width = LuaTableFieldValue(lua, 4, "width", 0);
			int height = LuaTableFieldValue(lua, 4, "height", 0);
			pNode = new CSceneNodeScreen(name, texture, width, height);
		} else if (!strcmp("skybox", type)) {
			string textureArray[6];
			const char* texture = LuaTableFieldValue(lua, 4, "texture", "");
			CStringUtil::Split(textureArray, 6, texture, ",", true);
			pNode = new CSceneNodeSkybox(name, textureArray);
		} else if (!strcmp("sound", type)) {
			const char* soundFile = LuaTableFieldValue(lua, 4, "soundFile", "");
			bool loop = LuaTableFieldValue(lua, 4, "loop", true);
			bool start = LuaTableFieldValue(lua, 4, "start", true);
			pNode = new CSceneNodeSound(name, soundFile, loop, start);
		} else if (!strcmp("static", type)) {
			const char* meshFile = LuaTableFieldValue(lua, 4, "meshFile", "");
			pNode = new CSceneNodeStatic(name, meshFile);
		} else if (!strcmp("terrain", type)) {
			string textureArray[4];
			const char* texture = LuaTableFieldValue(lua, 4, "texture", "");
			const char* heightMap = LuaTableFieldValue(lua, 4, "heightMap", "");
			const char* blendMap = LuaTableFieldValue(lua, 4, "blendMap", "");
			float heightScale = LuaTableFieldValue(lua, 4, "heightScale", 100.0f);
			float mapScale = LuaTableFieldValue(lua, 4, "mapScale", 5000.0f);
			CStringUtil::Split(textureArray, 4, texture, ",", true);
			pNode = new CSceneNodeTerrain(name, heightMap, mapScale, heightScale, textureArray, blendMap);
		} else if (!strcmp("text", type)) {
			wchar_t text[128];
			const char* src = LuaTableFieldValue(lua, 4, "text", "?");
			CStringUtil::Utf8ToWideCharArray(src, text, 128);
			const char* color = LuaTableFieldValue(lua, 4, "color", "#FF0000FF");
			const char* outline = LuaTableFieldValue(lua, 4, "outline", "#FFFF00FF");
			const char* font = LuaTableFieldValue(lua, 4, "font", "default");
			int fontSize = LuaTableFieldValue(lua, 4, "fontSize", 16);
			pNode = new CSceneNodeText(name, text, CColor(color), CColor(outline), font, fontSize);
		} else if (!strcmp("water", type)) {
			const char* normalMap = LuaTableFieldValue(lua, 4, "normalMap", "");
			float depth = LuaTableFieldValue(lua, 4, "depth", 0.0f);
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
* ɾ�������ڵ�
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
			CEngine::GetAnimationManager()->Stop(pNode);
			CEngine::GetSceneManager()->DeleteNode(pNode);
		}
	}
	return 0;
}

/**
* ������г����ڵ�
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
* ��ȡ�����ڵ��б�
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
* ѡ�񳡾��ڵ�
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
		int mesh, face;
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
	lua_pushstring(lua, "");
	return 1;
}

/**
* ��ȡ�����ó����ڵ�����
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
* ��ȡ�����ó����ڵ�Ƕ�
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
* ��ȡ�����ó����ڵ㷽λ
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
* ��ȡ�����ó����ڵ�λ��
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
* ��ȡ�����ó����ڵ�ɼ���
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
* ���³����ڵ�
* ʹ�÷��� scene.update(node, mesh, {{s=0,t=0}, [3]={r=0.5,g=0.5,b=0.5}, [4]={s=1,t=1}})
*/
int CScriptManager::DoSceneUpdate(lua_State* lua) {
	if (lua_isstring(lua, 1) && lua_isinteger(lua, 2) && lua_istable(lua, 3)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		CMeshData* pMeshData = pNode ? pNode->GetMeshData() : 0;
		if (!pMeshData) return 0;
		int meshIndex = (int)lua_tointeger(lua, 2);
		if (meshIndex < 0 || meshIndex >= pMeshData->GetMeshCount()) return 0;
		CMesh* pMesh = pMeshData->GetMesh(meshIndex);
		int vertexCount = pMesh->GetVertexCount();
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
* ��ȡ�����ڵ����񶥵���Ϣ
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
			if (mesh < 0 || mesh >= pMeshData->GetMeshCount()) return 0;
			if (face < 0 || face >= pMeshData->GetMesh(mesh)->GetTriangleCount()) return 0;
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
* ��ȡ�����ó����ڵ��������
*/
int CScriptManager::DoSceneMaterial(lua_State* lua) {
	if (lua_isstring(lua, 1) && lua_isinteger(lua, 2)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		CMeshData* pMeshData = pNode ? pNode->GetMeshData() : 0;
		if (!pMeshData) return 0;
		int meshIndex = (int)lua_tointeger(lua, 2);
		if (meshIndex >= pMeshData->GetMeshCount()) return 0;
		if (lua_istable(lua, 3)) {
			const char* name = LuaTableFieldValue(lua, 3, "name", "");
			float roughness = LuaTableFieldValue(lua, 3, "roughness", -1.0f);
			float metalness = LuaTableFieldValue(lua, 3, "metalness", -1.0f);
			const char* color = LuaTableFieldValue(lua, 3, "color", "");
			const char* shader = LuaTableFieldValue(lua, 3, "shader", "");
			const char* texture = LuaTableFieldValue(lua, 3, "texture", "");
			vector<string> textures;
			if (strlen(texture) > 0) CStringUtil::Split(textures, texture, ",", true);
			if (meshIndex < 0) {
				for (int i = 0; i < pMeshData->GetMeshCount(); i++) {
					CMaterial* pMaterial = pMeshData->GetMesh(i)->GetMaterial();
					if (roughness >= 0.0f) pMaterial->m_fRoughness = roughness;
					if (metalness >= 0.0f) pMaterial->m_fMetalness = metalness;
					if (strlen(color) > 0) CColor(color).GetValue(pMaterial->m_fColor);
					if (strlen(shader) > 0) pMaterial->SetShader(shader);
					for (size_t t = 0; t < textures.size(); t++) pMaterial->SetTexture(textures[t], t);
				}
			} else {
				CMaterial* pMaterial = pMeshData->GetMesh(meshIndex)->GetMaterial();
				if (strlen(name) > 0) pMaterial->GetName() = name;
				if (roughness >= 0.0f) pMaterial->m_fRoughness = roughness;
				if (metalness >= 0.0f) pMaterial->m_fMetalness = metalness;
				if (strlen(color) > 0) CColor(color).GetValue(pMaterial->m_fColor);
				if (strlen(shader) > 0) pMaterial->SetShader(shader);
				for (size_t t = 0; t < textures.size(); t++) pMaterial->SetTexture(textures[t], t);
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
* ���ó����ڵ���Ⱦģʽ
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
			for (int i = 0; i < pMeshData->GetMeshCount(); i++) {
				pMeshData->GetMesh(i)->GetMaterial()->SetRenderMode(cullFace, useDepth, addColor);
			}
		}
	}
	return 0;
}

/**
* ��ȡ�����ڵ�İ�Χ��
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
* ���ó����ڵ㷽��
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
				const char* name = LuaTableFieldValue(lua, 3, "name", "");
				bool loop = LuaTableFieldValue(lua, 3, "loop", true);
				float transition = LuaTableFieldValue(lua, 3, "transition", -1.0f);
				that->StartAnimation(name, loop, transition);
			} else if (!strcmp(function, "BlendAnimation")) {
				if (!lua_istable(lua, 3)) return 0;
				const char* anim1 = LuaTableFieldValue(lua, 3, "anim1", "");
				const char* anim2 = LuaTableFieldValue(lua, 3, "anim2", "");
				float k = LuaTableFieldValue(lua, 3, "k", 0.5f);
				that->BlendAnimation(anim1, anim2, k);
			} else if (!strcmp(function, "StopAnimation")) {
				that->StopAnimation();
			} else if (!strcmp(function, "SetAnimationFrameRate")) {
				if (!lua_istable(lua, 3)) return 0;
				float fps = LuaTableFieldValue(lua, 3, "fps", 30.0f);
				that->SetAnimationFrameRate(fps);
			} else if (!strcmp(function, "ShowSkeleton")) {
				if (!lua_istable(lua, 3)) return 0;
				bool visible = LuaTableFieldValue(lua, 3, "visible", true);
				bool skeletonOnly = LuaTableFieldValue(lua, 3, "skeletonOnly", false);
				that->ShowSkeleton(visible, skeletonOnly);
			} else if (!strcmp(function, "GetAnimationName")) {
				if (!lua_istable(lua, 3)) return 0;
				int index = LuaTableFieldValue(lua, 3, "index", 0);
				lua_pushstring(lua, that->GetAnimationName(index).c_str());
				return 1;
			}
		} else if (type == "blast") {
			CSceneNodeBlast* that = static_cast<CSceneNodeBlast*>(pNode);
			if (!strcmp(function, "InitBlast")) {
				if (!lua_istable(lua, 3)) return 0;
				float duration = LuaTableFieldValue(lua, 3, "duration", 1.0f);
				float size = LuaTableFieldValue(lua, 3, "size", 1.0f);
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
				float x = LuaTableFieldValue(lua, 3, "x", 0.0f);
				float y = LuaTableFieldValue(lua, 3, "y", 0.0f);
				float z = LuaTableFieldValue(lua, 3, "z", 1.0f);
				that->SetAxis(CVector3(x, y, z));
			}
		} else if (type == "line") {
			CSceneNodeLine* that = static_cast<CSceneNodeLine*>(pNode);
			if (!strcmp(function, "AddPoint")) {
				if (!lua_istable(lua, 3)) return 0;
				float x = LuaTableFieldValue(lua, 3, "x", 0.0f);
				float y = LuaTableFieldValue(lua, 3, "y", 0.0f);
				float z = LuaTableFieldValue(lua, 3, "z", 0.0f);
				that->AddPoint(CVector3(x, y, z));
			} else if (!strcmp(function, "ClearPoint")) {
				that->ClearPoint();
			} else if (!strcmp(function, "SmoothLine")) {
				if (!lua_istable(lua, 3)) return 0;
				float ds = LuaTableFieldValue(lua, 3, "ds", 0.1f);
				that->SmoothLine(ds);
			} else if (!strcmp(function, "ShowPoints")) {
				if (!lua_istable(lua, 3)) return 0;
				bool show = LuaTableFieldValue(lua, 3, "show", true);
				float pointSize = LuaTableFieldValue(lua, 3, "pointSize", 1.0f);
				that->ShowPoints(show, pointSize);
			} else if (!strcmp(function, "DisableDepth")) {
				if (!lua_istable(lua, 3)) return 0;
				bool disable = LuaTableFieldValue(lua, 3, "disable", true);
				that->DisableDepth(disable);
			} else if (!strcmp(function, "ScreenSpace")) {
				if (!lua_istable(lua, 3)) return 0;
				bool enable = LuaTableFieldValue(lua, 3, "enable", true);
				that->ScreenSpace(enable);
			} else if (!strcmp(function, "Segment")) {
				if (!lua_istable(lua, 3)) return 0;
				bool enable = LuaTableFieldValue(lua, 3, "enable", true);
				that->Segment(enable);
			} else if (!strcmp(function, "SetShader")) {
				if (!lua_istable(lua, 3)) return 0;
				const char* shader = LuaTableFieldValue(lua, 3, "shader", "");
				that->SetShader(shader);
			} else if (!strcmp(function, "GetPointCount")) {
				lua_pushinteger(lua, that->GetPointCount());
				return 1;
			}
		} else if (type == "particles") {
			CSceneNodeParticles* that = static_cast<CSceneNodeParticles*>(pNode);
			if (!strcmp(function, "InitParticles")) {
				if (!lua_istable(lua, 3)) return 0;
				float initSpeed[3];
				initSpeed[0] = LuaTableFieldValue(lua, 3, "initSpeed_x", 0.0f);
				initSpeed[1] = LuaTableFieldValue(lua, 3, "initSpeed_y", 0.0f);
				initSpeed[2] = LuaTableFieldValue(lua, 3, "initSpeed_z", 0.0f);
				float spreadSpeed = LuaTableFieldValue(lua, 3, "spreadSpeed", 1.0f);
				float fadeSpeed = LuaTableFieldValue(lua, 3, "fadeSpeed", 1.0f);
				that->InitParticles(initSpeed, spreadSpeed, fadeSpeed);
			}
		} else if (type == "sound") {
			CSceneNodeSound* that = static_cast<CSceneNodeSound*>(pNode);
			if (!strcmp(function, "Play")) {
				if (!lua_istable(lua, 3)) return 0;
				bool enable = LuaTableFieldValue(lua, 3, "enable", true);
				that->Play(enable);
			}
		} else if (type == "terrain") {
			CSceneNodeTerrain* that = static_cast<CSceneNodeTerrain*>(pNode);
			if (!strcmp(function, "GetHeight")) {
				if (!lua_istable(lua, 3)) return 0;
				float x = LuaTableFieldValue(lua, 3, "x", 0.0f);
				float y = LuaTableFieldValue(lua, 3, "y", 0.0f);
				lua_pushnumber(lua, that->GetHeight(x, y));
				return 1;
			} else if (!strcmp(function, "GetNormal")) {
				if (!lua_istable(lua, 3)) return 0;
				float x = LuaTableFieldValue(lua, 3, "x", 0.0f);
				float y = LuaTableFieldValue(lua, 3, "y", 0.0f);
				CVector3 normal = that->GetNormal(x, y);
				lua_pushnumber(lua, normal[0]);
				lua_pushnumber(lua, normal[1]);
				lua_pushnumber(lua, normal[2]);
				return 3;
			}
		} else if (type == "text") {
			CSceneNodeText* that = static_cast<CSceneNodeText*>(pNode);
			if (!strcmp(function, "SetText")) {
				if (!lua_istable(lua, 3)) return 0;
				wchar_t buffer[1024];
				const char* text = LuaTableFieldValue(lua, 3, "text", "");
				CStringUtil::Utf8ToWideCharArray(text, buffer, 1024);
				that->SetText(buffer);
			} else if (!strcmp(function, "SetFont")) {
				if (!lua_istable(lua, 3)) return 0;
				const char* font = LuaTableFieldValue(lua, 3, "font", "default");
				int fontSize = LuaTableFieldValue(lua, 3, "fontSize", 16);
				that->SetFont(font, fontSize);
			} else if (!strcmp(function, "SetColor")) {
				if (!lua_istable(lua, 3)) return 0;
				const char* color = LuaTableFieldValue(lua, 3, "color", "red");
				const char* outline = LuaTableFieldValue(lua, 3, "outline", "yellow");
				that->SetColor(CColor(color), CColor(outline));
			}
		}
	}
	return 0;
}

/**
* ��ȡ��ע��ĺ����б�
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
* ��ȡ������ָ������
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
* ע���û��Զ������
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
* ���ú�������
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
* ��Ļ��ͼ
*/
int CScriptManager::DoGraphicsScreenshot(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		string filepath = lua_tostring(lua, 1);
		bool redraw = lua_isboolean(lua, 2) ? lua_toboolean(lua, 2) != 0 : true;
		CEngine::GetGraphicsManager()->Screenshot(filepath, redraw);
	}
	return 0;
}

/**
* ��ȡ������������ʾ
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
* ��ȡ���ڴ�С
*/
int CScriptManager::DoGraphicsWindowSize(lua_State* lua) {
	int width;
	int height;
	CEngine::GetGraphicsManager()->GetWindowSize(&width, &height);
	lua_pushnumber(lua, width);
	lua_pushnumber(lua, height);
	return 2;
}

/**
* ��ȡ��Դλ��
*/
int CScriptManager::DoGraphicsLightPosition(lua_State* lua) {
	CVector3 lightPos = CEngine::GetGraphicsManager()->GetLight();
	lua_pushnumber(lua, lightPos[0]);
	lua_pushnumber(lua, lightPos[1]);
	lua_pushnumber(lua, lightPos[2]);
	lua_pushnumber(lua, lightPos[3]);
	return 4;
}

/**
* ���÷�����Դ
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
* ���õ��Դ
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
* ���þ۹�ƹ�Դ
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
* ��ȡ��������Ӱ�Ƿ���ʾ
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
* ��ȡ���������Ƿ���ʾ
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
		} else {
			CEngine::GetGraphicsManager()->SetFogEnable(false, 0.0f, 0.0f, CColor::White);
		}
		return 0;
	}
	lua_pushboolean(lua, CEngine::GetGraphicsManager()->GetFogEnable());
	return 1;
}

/**
* ��ȡ�����û�����ͼ
*/
int CScriptManager::DoGraphicsEnvironmentMap(lua_State* lua) {
	if (lua_isboolean(lua, 1)) {
		if (lua_toboolean(lua, 1)) {
			if (lua_isstring(lua, 2)) {
				const char* cubemap = lua_tostring(lua, 2);
				CEngine::GetGraphicsManager()->SetEnvironmentMapEnable(true, cubemap);
			}
		} else {
			CEngine::GetGraphicsManager()->SetEnvironmentMapEnable(false, "");
		}
		return 0;
	}
	lua_pushboolean(lua, CEngine::GetGraphicsManager()->GetEnvironmentMapEnable());
	return 1;
}

/**
* ���û�ͼ������ɫ
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
* ����Ļ�ϵĵ��ȡ�ռ�����
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
* ��ȡ�ռ��ͶӰ����Ļ�ϵ�λ��
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
* ����������ȾĿ��
*/
int CScriptManager::DoGraphicsRenderTarget(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		const char* name = lua_tostring(lua, 1);
		bool clear = lua_isboolean(lua, 2) ? lua_toboolean(lua, 2) != 0 : true;
		bool resize = lua_isboolean(lua, 3) ? lua_toboolean(lua, 3) != 0 : false;
		CTexture* pTexture = CEngine::GetTextureManager()->GetTexture(name);
		CGraphicsManager* pGraphicsMgr = CEngine::GetGraphicsManager();
		CTexture* pRenderTarget = pGraphicsMgr->GetRenderTarget();
		pGraphicsMgr->SetRenderTarget(pTexture, 0, true, clear, clear);
		if (resize && pTexture) {
			pGraphicsMgr->SetWindowSize(pTexture->GetWidth(), pTexture->GetHeight());
		}
		lua_pushstring(lua, pRenderTarget ? pRenderTarget->GetFilePath().c_str() : "null");
		return 1;
	}
	return 0;
}

/**
* ��������������
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
* ɾ��������Դ
*/
int CScriptManager::DoTextureDelete(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CTextureManager* pTextureMgr = CEngine::GetTextureManager();
		pTextureMgr->Drop(pTextureMgr->GetTexture(lua_tostring(lua, 1)));
	}
	return 0;
}

/**
* ������������
*/
int CScriptManager::DoTextureUpdate(lua_State* lua) {
	bool success = false;
	if (lua_isstring(lua, 1) && lua_isstring(lua, 2)) {
		CTextureManager* pTextureMgr = CEngine::GetTextureManager();
		CTexture* pTexture = pTextureMgr->GetTexture(lua_tostring(lua, 1));
		success = pTextureMgr->Update(pTexture, lua_tostring(lua, 2));
	}
	lua_pushboolean(lua, success);
	return 1;
}

/**
* ������ɫ��
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
* ɾ����ɫ����Դ
*/
int CScriptManager::DoShaderDelete(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CShaderManager* pShaderMgr = CEngine::GetShaderManager();
		pShaderMgr->Drop(pShaderMgr->GetShader(lua_tostring(lua, 1)));
	}
	return 0;
}

/**
* ������ɫ������
*/
int CScriptManager::DoShaderUpdate(lua_State* lua) {
	bool success = false;
	if (lua_isstring(lua, 1) && lua_isstring(lua, 2) && lua_isstring(lua, 3)) {
		string append = lua_tostring(lua, 2);
		string remove = lua_tostring(lua, 3);
		CShaderManager* pShaderMgr = CEngine::GetShaderManager();
		CShader* pShader = pShaderMgr->GetShader(lua_tostring(lua, 1));
		pShaderMgr->Update(pShader, append, remove);
		success = pShader ? pShader->IsValid() : false;
	}
	lua_pushboolean(lua, success);
	return 1;
}

/**
* ������ɫ������
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
* ��ȡ�Ѽ��ص������б�
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
* ��������
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
* ���ȫ���Ѽ��ص�����
*/
int CScriptManager::DoFontClear(lua_State* lua) {
	CEngine::GetFontManager()->Clear();
	return 0;
}

/**
* ���õ�ǰ����
*/
int CScriptManager::DoFontUse(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		const char* name = lua_tostring(lua, 1);
		CEngine::GetFontManager()->UseFont(name);
	}
	return 0;
}

/**
* ������Դ
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
* ɾ����Դ
*/
int CScriptManager::DoSoundDelete(lua_State* lua) {
	if (lua_isinteger(lua, 1)) {
		CEngine::GetSoundManager()->Drop((int)lua_tointeger(lua, 1));
	}
	return 0;
}

/**
* ��ʼ��������
*/
int CScriptManager::DoSoundPlay(lua_State* lua) {
	if (lua_isinteger(lua, 1)) {
		CEngine::GetSoundManager()->Play((int)lua_tointeger(lua, 1));
	}
	return 0;
}

/**
* ��ͣ��������
*/
int CScriptManager::DoSoundPause(lua_State* lua) {
	CSoundManager* pSoundMgr = CEngine::GetSoundManager();
	if (lua_isinteger(lua, 1)) pSoundMgr->Pause((int)lua_tointeger(lua, 1));
	else if (lua_isboolean(lua, 1)) pSoundMgr->Pause(lua_toboolean(lua, 1) != 0);
	return 0;
}

/**
* ֹͣ��������
*/
int CScriptManager::DoSoundStop(lua_State* lua) {
	if (lua_isinteger(lua, 1)) {
		CEngine::GetSoundManager()->Stop((int)lua_tointeger(lua, 1));
	}
	return 0;
}

/**
* ������Դλ��
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
* ��ȡ����������
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
* ������Ƶ����
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
* �󶨳����ڵ㵽�������
*/
int CScriptManager::DoPhysicsBind(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (!pNode) return 0;
		// �������С��ʼ��Ϊ 1.0
		SGeometry geometry;
		if (pNode->GetType() == "geometry") {
			geometry = static_cast<CSceneNodeGeometry*>(pNode)->GetGeometry(true);
		} else {
			geometry.box.x = 1.0f;
			geometry.box.y = 1.0f;
			geometry.box.z = 1.0f;
		}
		const char* shape = LuaTableFieldValue(lua, 2, "shape", "");
		if (!strcmp(shape, "box")) geometry.shape = SGeometry::BOX;
		else if (!strcmp(shape, "sphere")) geometry.shape = SGeometry::SPHERE;
		else if (!strcmp(shape, "capsule")) geometry.shape = SGeometry::CAPSULE;
		else if (!strcmp(shape, "cylinder")) geometry.shape = SGeometry::CYLINDER;
		else if (!strcmp(shape, "torus")) geometry.shape = SGeometry::TORUS;
		else if (!strcmp(shape, "plane")) geometry.shape = SGeometry::PLANE;
		switch (geometry.shape) {
		case SGeometry::BOX:
			geometry.box.x = LuaTableFieldValue(lua, 2, "x", geometry.box.x);
			geometry.box.y = LuaTableFieldValue(lua, 2, "y", geometry.box.y);
			geometry.box.z = LuaTableFieldValue(lua, 2, "z", geometry.box.z);
			break;
		case SGeometry::SPHERE:
			geometry.sphere.r = LuaTableFieldValue(lua, 2, "r", geometry.sphere.r);
			break;
		case SGeometry::CAPSULE:
			geometry.capsule.r = LuaTableFieldValue(lua, 2, "r", geometry.capsule.r);
			geometry.capsule.h = LuaTableFieldValue(lua, 2, "h", geometry.capsule.h);
			break;
		case SGeometry::CYLINDER:
			geometry.cylinder.r = LuaTableFieldValue(lua, 2, "r", geometry.cylinder.r);
			geometry.cylinder.h = LuaTableFieldValue(lua, 2, "h", geometry.cylinder.h);
			break;
		case SGeometry::TORUS:
			geometry.torus.r = LuaTableFieldValue(lua, 2, "r", geometry.torus.r);
			geometry.torus.c = LuaTableFieldValue(lua, 2, "c", geometry.torus.c);
			break;
		default: break;
		}
		float mass = LuaTableFieldValue(lua, 2, "mass", 1.0f);
		float linearDamping = LuaTableFieldValue(lua, 2, "linearDamping", 0.0f);
		float angularDamping = LuaTableFieldValue(lua, 2, "angularDamping", 0.0f);
		float friction = LuaTableFieldValue(lua, 2, "friction", 0.5f);
		float restitution = LuaTableFieldValue(lua, 2, "restitution", 0.0f);
		bool trigger = LuaTableFieldValue(lua, 2, "trigger", false);
		bool allowSleep = LuaTableFieldValue(lua, 2, "allowSleep", true);
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
* ȡ�������ڵ������ģ��
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
* ���ó����ڵ���ײ�¼��ص�
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
* ���ø���״̬�������������ٶȺͽ��ٶȶ���Ϊ��
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
* ���ø������ٶȺͽ��ٶ�
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
* �����ʩ����
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
* �����ʩ�ӳ���
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
* ����������������
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
* Ϊ�ؽ���������ģ��
*/
int CScriptManager::DoPhysicsJoint(lua_State* lua) {
	if (lua_isstring(lua, 1) && lua_isstring(lua, 2)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (pNode && pNode->GetType() == "animation") {
			CMeshData* meshData = reinterpret_cast<CSceneNodeAnimation*>(pNode)->GetMeshData();
			if (meshData) {
				float mass = 1.0f;
				float hK = 200.0f;
				float vK = 100.0f;
				float damping = -0.1f;
				if (lua_isnumber(lua, 3) && lua_isnumber(lua, 4) && lua_isnumber(lua, 5) && lua_isnumber(lua, 6)) {
					mass = (float)lua_tonumber(lua, 3);
					hK = (float)lua_tonumber(lua, 4);
					vK = (float)lua_tonumber(lua, 5);
					damping = (float)lua_tonumber(lua, 6);
				}
				meshData->AddPhysics(lua_tostring(lua, 2), mass, hK, vK, damping);
			}
		}
	}
	return 0;
}

/**
* ���ö�������
*/
int CScriptManager::DoAnimationParam(lua_State* lua) {
	if (lua_isstring(lua, 1) && lua_isstring(lua, 2) && lua_isnumber(lua, 3)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (pNode) {
			const char* interpolator = lua_tostring(lua, 2);
			float duration = (float)lua_tonumber(lua, 3);
			int repeat = lua_isinteger(lua, 4) ? (int)lua_tointeger(lua, 4) : 1;
			bool swing = lua_isboolean(lua, 5) ? lua_toboolean(lua, 5) != 0 : false;
			CAnimationManager::Interpolator type = CAnimationManager::LINEAR;
			if (!strcmp(interpolator, "linear")) type = CAnimationManager::LINEAR;
			else if (!strcmp(interpolator, "accelerate")) type = CAnimationManager::ACCELERATE;
			else if (!strcmp(interpolator, "decelerate")) type = CAnimationManager::DECELERATE;
			else if (!strcmp(interpolator, "acceleratedecelerate")) type = CAnimationManager::ACCELERATEDECELERATE;
			CEngine::GetAnimationManager()->SetAnimation(pNode, type, duration, repeat, swing);
		}
	}
	return 0;
}

/**
* �������Ŷ���
*/
int CScriptManager::DoAnimationScale(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (pNode) {
			if (lua_isnumber(lua, 2) && lua_isnumber(lua, 3) && lua_isnumber(lua, 4) &&
				lua_isnumber(lua, 5) && lua_isnumber(lua, 6) && lua_isnumber(lua, 7)) {
				float x1 = (float)lua_tonumber(lua, 2);
				float y1 = (float)lua_tonumber(lua, 3);
				float z1 = (float)lua_tonumber(lua, 4);
				float x2 = (float)lua_tonumber(lua, 5);
				float y2 = (float)lua_tonumber(lua, 6);
				float z2 = (float)lua_tonumber(lua, 7);
				CEngine::GetAnimationManager()->AnimateScale(pNode, CVector3(x1, y1, z1), CVector3(x2, y2, z2));
			}
		}
	}
	return 0;
}

/**
* ������ת����
*/
int CScriptManager::DoAnimationRotation(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (pNode) {
			if (lua_isnumber(lua, 2) && lua_isnumber(lua, 3) && lua_isnumber(lua, 4) &&
				lua_isnumber(lua, 5) && lua_isnumber(lua, 6) && lua_isnumber(lua, 7)) {
				CQuaternion a;
				CQuaternion b;
				float x1 = (float)lua_tonumber(lua, 2);
				float y1 = (float)lua_tonumber(lua, 3);
				float z1 = (float)lua_tonumber(lua, 4);
				float x2 = (float)lua_tonumber(lua, 5);
				float y2 = (float)lua_tonumber(lua, 6);
				float z2 = (float)lua_tonumber(lua, 7);
				a.FromEulerAngles(x1, y1, z1);
				b.FromEulerAngles(x2, y2, z2);
				CEngine::GetAnimationManager()->AnimateRotation(pNode, a, b);
			}
		}
	}
	return 0;
}

/**
* ����λ�ƶ���
*/
int CScriptManager::DoAnimationTranslation(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (pNode) {
			if (lua_isnumber(lua, 2) && lua_isnumber(lua, 3) && lua_isnumber(lua, 4) &&
				lua_isnumber(lua, 5) && lua_isnumber(lua, 6) && lua_isnumber(lua, 7)) {
				float x1 = (float)lua_tonumber(lua, 2);
				float y1 = (float)lua_tonumber(lua, 3);
				float z1 = (float)lua_tonumber(lua, 4);
				float x2 = (float)lua_tonumber(lua, 5);
				float y2 = (float)lua_tonumber(lua, 6);
				float z2 = (float)lua_tonumber(lua, 7);
				CEngine::GetAnimationManager()->AnimateTranslation(pNode, CVector3(x1, y1, z1), CVector3(x2, y2, z2));
			}
		}
	}
	return 0;
}

/**
* ��ʼִ�ж���
*/
int CScriptManager::DoAnimationStart(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (pNode) {
			if (!lua_isnumber(lua, 2)) CEngine::GetAnimationManager()->Start(pNode);
			else CEngine::GetAnimationManager()->Start(pNode, (float)lua_tonumber(lua, 2));
		}
	}
	return 0;
}

/**
* ִֹͣ�ж���
*/
int CScriptManager::DoAnimationStop(lua_State* lua) {
	if (lua_isstring(lua, 1)) {
		CSceneNode* pNode = CEngine::GetSceneManager()->GetNodeByName(lua_tostring(lua, 1));
		if (pNode) {
			CEngine::GetAnimationManager()->Stop(pNode);
		}
	}
	return 0;
}