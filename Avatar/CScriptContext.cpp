//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CScriptContext.h"
#include "thirdparty/lua/lua.hpp"

/**
* Ĭ�Ϲ��캯��
*/
CScriptContext::CScriptContext() {
	m_pLuaState = 0;
}

/**
* ��ȡ��ǰʵ��
*/
CScriptContext* CScriptContext::GetCurrent(struct lua_State* lua) {
	m_pLuaState = lua;
	return this;
}

/**
* �󶨽ű�����
*/
void CScriptContext::BindFunction(const string& name, void* function) {
	lua_pushcfunction(m_pLuaState, (lua_CFunction)function);
	lua_setglobal(m_pLuaState, name.c_str());
}

/**
* �󶨽ű�����
*/
void CScriptContext::BindFunction(const string& group, const string& name, void* function) {
	if (lua_getglobal(m_pLuaState, group.c_str()) != LUA_TTABLE) {
		lua_pop(m_pLuaState, 1);
		lua_newtable(m_pLuaState);
		lua_pushstring(m_pLuaState, name.c_str());
		lua_pushcfunction(m_pLuaState, (lua_CFunction)function);
		lua_rawset(m_pLuaState, -3);
		lua_setglobal(m_pLuaState, group.c_str());
	} else {
		lua_pushstring(m_pLuaState, name.c_str());
		lua_pushcfunction(m_pLuaState, (lua_CFunction)function);
		lua_rawset(m_pLuaState, -3);
		lua_pop(m_pLuaState, 1);
	}
}

/**
* �ж��Ƿ� bool ����
*/
bool CScriptContext::IsBoolean(int index) {
	return lua_isboolean(m_pLuaState, index);
}

/**
* �ж��Ƿ� int ����
*/
bool CScriptContext::IsInteger(int index) {
	return lua_isinteger(m_pLuaState, index) != 0;
}

/**
* �ж��Ƿ� float ����
*/
bool CScriptContext::IsNumber(int index) {
	return lua_isnumber(m_pLuaState, index) != 0;
}

/**
* �ж��Ƿ� string ����
*/
bool CScriptContext::IsString(int index) {
	return lua_isstring(m_pLuaState, index) != 0;
}

/**
* �ж��Ƿ� function ����
*/
bool CScriptContext::IsFunction(int index) {
	return lua_isfunction(m_pLuaState, index);
}

/**
* ��ȡ bool ֵ
*/
bool CScriptContext::ToBoolean(int index) {
	return lua_toboolean(m_pLuaState, index) != 0;
}

/**
* ��ȡ int ֵ
*/
int CScriptContext::ToInteger(int index) {
	return (int)lua_tointeger(m_pLuaState, index);
}

/**
* ��ȡ float ֵ
*/
float CScriptContext::ToNumber(int index) {
	return (float)lua_tonumber(m_pLuaState, index);
}

/**
* ��ȡ string ֵ
*/
const char* CScriptContext::ToString(int index) {
	return lua_tostring(m_pLuaState, index);
}

/**
* �� bool ֵѹջ
*/
void CScriptContext::PushValue(bool value) {
	lua_pushboolean(m_pLuaState, value ? 1 : 0);
}

/**
* �� int ֵѹջ
*/
void CScriptContext::PushValue(int value) {
	lua_pushinteger(m_pLuaState, value);
}

/**
* �� float ֵѹջ
*/
void CScriptContext::PushValue(float value) {
	lua_pushnumber(m_pLuaState, value);
}

/**
* �� string ֵѹջ
*/
void CScriptContext::PushValue(const char* value) {
	lua_pushstring(m_pLuaState, value);
}

/**
* ���ûص�����
*/
int CScriptContext::RefCallback() {
	int paramCount = lua_gettop(m_pLuaState);
	if (lua_isfunction(m_pLuaState, paramCount)) {
		return luaL_ref(m_pLuaState, LUA_REGISTRYINDEX);
	}
	return LUA_NOREF;
}

/**
* ȡ���ص�����
*/
void CScriptContext::UnrefCallback(int callback) {
	luaL_unref(m_pLuaState, LUA_REGISTRYINDEX, callback);
}

/**
* ��ʼ���ûص�����
*/
void CScriptContext::InvokeBegin(int callback) {
	lua_rawgeti(m_pLuaState, LUA_REGISTRYINDEX, callback);
}

/**
* ��ʼ���ûص�����
*/
void CScriptContext::InvokeBegin(const string& function) {
	lua_getglobal(m_pLuaState, function.c_str());
}

/**
* �������ûص�����
*/
void CScriptContext::InvokeEnd(int params) {
	lua_pcall(m_pLuaState, params, 0, 0);
}