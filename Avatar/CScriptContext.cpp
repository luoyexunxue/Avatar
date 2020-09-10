//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CScriptContext.h"
#include "thirdparty/lua/lua.hpp"

/**
* 默认构造函数
*/
CScriptContext::CScriptContext() {
	m_pLuaState = 0;
}

/**
* 获取当前实例
*/
CScriptContext* CScriptContext::GetCurrent(struct lua_State* lua) {
	m_pLuaState = lua;
	return this;
}

/**
* 执行脚本
*/
bool CScriptContext::Execute(const char* script) {
	return luaL_dostring(m_pLuaState, script) == LUA_OK;
}

/**
* 绑定脚本方法
*/
void CScriptContext::BindFunction(const string& name, void* function) {
	lua_pushcfunction(m_pLuaState, (lua_CFunction)function);
	lua_setglobal(m_pLuaState, name.c_str());
}

/**
* 绑定脚本方法
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
* 判断是否 bool 类型
*/
bool CScriptContext::IsBoolean(int index) {
	return lua_isboolean(m_pLuaState, index);
}

/**
* 判断是否 int 类型
*/
bool CScriptContext::IsInteger(int index) {
	return lua_isinteger(m_pLuaState, index) != 0;
}

/**
* 判断是否 float 类型
*/
bool CScriptContext::IsNumber(int index) {
	return lua_isnumber(m_pLuaState, index) != 0;
}

/**
* 判断是否 string 类型
*/
bool CScriptContext::IsString(int index) {
	return lua_isstring(m_pLuaState, index) != 0;
}

/**
* 判断是否 table 类型
*/
bool CScriptContext::IsTable(int index) {
	return lua_istable(m_pLuaState, index) != 0;
}

/**
* 判断是否 function 类型
*/
bool CScriptContext::IsFunction(int index) {
	return lua_isfunction(m_pLuaState, index);
}

/**
* 获取 bool 值
*/
bool CScriptContext::ToBoolean(int index) {
	return lua_toboolean(m_pLuaState, index) != 0;
}

/**
* 获取 int 值
*/
int CScriptContext::ToInteger(int index) {
	return (int)lua_tointeger(m_pLuaState, index);
}

/**
* 获取 float 值
*/
float CScriptContext::ToNumber(int index) {
	return (float)lua_tonumber(m_pLuaState, index);
}

/**
* 获取 string 值
*/
const char* CScriptContext::ToString(int index) {
	return lua_tostring(m_pLuaState, index);
}

/**
* 获取表 bool 字段值
*/
bool CScriptContext::TableValue(int index, const char* name, bool def) {
	lua_getfield(m_pLuaState, index, name);
	return lua_isboolean(m_pLuaState, -1) ? lua_toboolean(m_pLuaState, -1) != 0 : def;
}

/**
* 获取表 int 字段值
*/
int CScriptContext::TableValue(int index, const char* name, int def) {
	lua_getfield(m_pLuaState, index, name);
	return lua_isinteger(m_pLuaState, -1) ? (int)lua_tointeger(m_pLuaState, -1) : def;
}

/**
* 获取表 float 字段值
*/
float CScriptContext::TableValue(int index, const char* name, float def) {
	lua_getfield(m_pLuaState, index, name);
	return lua_isnumber(m_pLuaState, -1) ? (float)lua_tonumber(m_pLuaState, -1) : def;
}

/**
* 获取表 string 字段值
*/
const char* CScriptContext::TableValue(int index, const char* name, const char* def) {
	lua_getfield(m_pLuaState, index, name);
	return lua_isstring(m_pLuaState, -1) ? (const char*)lua_tostring(m_pLuaState, -1) : def;
}

/**
* 将 bool 值压栈
*/
void CScriptContext::PushValue(bool value) {
	lua_pushboolean(m_pLuaState, value ? 1 : 0);
}

/**
* 将 int 值压栈
*/
void CScriptContext::PushValue(int value) {
	lua_pushinteger(m_pLuaState, value);
}

/**
* 将 float 值压栈
*/
void CScriptContext::PushValue(float value) {
	lua_pushnumber(m_pLuaState, value);
}

/**
* 将 string 值压栈
*/
void CScriptContext::PushValue(const char* value) {
	lua_pushstring(m_pLuaState, value);
}

/**
* 引用回调方法
*/
int CScriptContext::RefCallback() {
	int paramCount = lua_gettop(m_pLuaState);
	if (lua_isfunction(m_pLuaState, paramCount)) {
		return luaL_ref(m_pLuaState, LUA_REGISTRYINDEX);
	}
	return LUA_NOREF;
}

/**
* 取消回调引用
*/
void CScriptContext::UnrefCallback(int callback) {
	luaL_unref(m_pLuaState, LUA_REGISTRYINDEX, callback);
}

/**
* 开始调用回调方法
* @remark 调用 PushValue 提供参数, 再 InvokeEnd 提交
*/
void CScriptContext::InvokeBegin(int callback) {
	lua_rawgeti(m_pLuaState, LUA_REGISTRYINDEX, callback);
}

/**
* 开始调用回调方法
* @remark 调用 PushValue 提供参数, 再 InvokeEnd 提交
*/
void CScriptContext::InvokeBegin(const string& function) {
	lua_getglobal(m_pLuaState, function.c_str());
}

/**
* 结束调用回调方法
* @param params 调用 InvokeBegin 之后 PushValue 的个数
*/
void CScriptContext::InvokeEnd(int params) {
	if (params < 0) {
		lua_insert(m_pLuaState, lua_gettop(m_pLuaState) + params);
		params = -params;
	}
	lua_pcall(m_pLuaState, params, 0, 0);
}