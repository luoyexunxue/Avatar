//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* @brief �ű�������
*/
class AVATAR_EXPORT CScriptManager {
public:
	//! ��ȡ������ʵ��
	static CScriptManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CScriptManager();
		return m_pInstance;
	}
	//! ʵ������
	void Destroy();
	//! ����ڽű�
	void OpenScript(const string& script, void* engine);
	//! ��ȡ�ӿ�������
	CScriptContext* GetContext(lua_State* lua);

	//! ��ʼ������¼�
	void OnReady();
	//! �߼������¼�
	void OnUpdate(float dt);
	//! �����˳��¼�
	void OnExit();
	//! �����С�ı��¼�
	void OnSize(int width, int height);
	//! �����¼�
	void OnInput(const string& name, int value, int arg1, int arg2, int arg3);
	//! �����¼�
	void OnInput(const string& name, int value, float arg1, float arg2, float arg3);

	//! �ű�����
	void Script(const char* script);
	//! ע�᷽���ص�
	void Register(const string& function, void* callback);
	//! GUI�����¼�
	void GuiEvent(const string& name, int evt, int arg1, int arg2);
	//! ������ײ�¼�
	void CollideEnter(int callback, const string& name, const CVector3& pos);
	//! ������ײȡ���¼�
	void CollideLeave(int callback, const string& name);
	//! �����¼�
	void HandleEvent();

private:
	CScriptManager();
	~CScriptManager();

	//! ע��ű����ýӿ�
	void RegisterInterface(lua_State* lua);

	//! ϵͳ�ӿ�
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
	//! GUI �ӿ�
	static int DoGuiEnable(lua_State* lua);
	static int DoGuiSize(lua_State* lua);
	static int DoGuiScale(lua_State* lua);
	static int DoGuiCreate(lua_State* lua);
	static int DoGuiModify(lua_State* lua);
	static int DoGuiDelete(lua_State* lua);
	static int DoGuiAttrib(lua_State* lua);
	//! ����ӿ�
	static int DoCameraType(lua_State* lua);
	static int DoCameraControl(lua_State* lua);
	static int DoCameraFov(lua_State* lua);
	static int DoCameraClip(lua_State* lua);
	static int DoCameraPosition(lua_State* lua);
	static int DoCameraTarget(lua_State* lua);
	static int DoCameraAngle(lua_State* lua);
	static int DoCameraDirection(lua_State* lua);
	static int DoCameraBind(lua_State* lua);
	//! �����ڵ�ӿ�
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
	//! ����ӿ�
	static int DoPostList(lua_State* lua);
	static int DoPostEnable(lua_State* lua);
	static int DoPostRegister(lua_State* lua);
	static int DoPostParam(lua_State* lua);
	//! ͼ�νӿ�
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
	//! ����ӿ�
	static int DoTextureCreate(lua_State* lua);
	static int DoTextureDelete(lua_State* lua);
	static int DoTextureUpdate(lua_State* lua);
	//! ��ɫ���ӿ�
	static int DoShaderCreate(lua_State* lua);
	static int DoShaderDelete(lua_State* lua);
	static int DoShaderUpdate(lua_State* lua);
	static int DoShaderParam(lua_State* lua);
	//! ����ӿ�
	static int DoFontList(lua_State* lua);
	static int DoFontLoad(lua_State* lua);
	static int DoFontClear(lua_State* lua);
	static int DoFontUse(lua_State* lua);
	//! �����ӿ�
	static int DoSoundCreate(lua_State* lua);
	static int DoSoundDelete(lua_State* lua);
	static int DoSoundPlay(lua_State* lua);
	static int DoSoundPause(lua_State* lua);
	static int DoSoundStop(lua_State* lua);
	static int DoSoundPosition(lua_State* lua);
	static int DoSoundVolume(lua_State* lua);
	static int DoSoundUpdate(lua_State* lua);
	//! ��������ӿ�
	static int DoPhysicsBind(lua_State* lua);
	static int DoPhysicsUnbind(lua_State* lua);
	static int DoPhysicsCollide(lua_State* lua);
	static int DoPhysicsReset(lua_State* lua);
	static int DoPhysicsVelocity(lua_State* lua);
	static int DoPhysicsApplyForce(lua_State* lua);
	static int DoPhysicsApplyImpulse(lua_State* lua);
	static int DoPhysicsGravity(lua_State* lua);
	static int DoPhysicsJoint(lua_State* lua);
	//! �����ӿ�
	static int DoAnimationParam(lua_State* lua);
	static int DoAnimationScale(lua_State* lua);
	static int DoAnimationRotation(lua_State* lua);
	static int DoAnimationTranslation(lua_State* lua);
	static int DoAnimationStart(lua_State* lua);
	static int DoAnimationStop(lua_State* lua);

private:
	//! GUI�¼�����
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
	//! Lua �����
	lua_State* m_pLuaState;
	//! �ű��ӿ�������
	CScriptContext* m_pContext;
	//! �Զ��巽���ص�
	map<string, void*> m_mapFunction;
	//! GUI �����¼���
	map<string, int> m_mapGuiEvent;
	//! GUI �¼�����
	vector<SGuiEvent> m_vecEventQueue;
	//! �ű��������
	queue<string> m_queScriptQueue;
	//! �ű�������ʵ��
	static CScriptManager* m_pInstance;
};

#endif