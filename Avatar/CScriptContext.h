//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCRIPTCONTEXT_H_
#define _CSCRIPTCONTEXT_H_
#include "AvatarConfig.h"
#include <cstdint>
#include <string>
using std::string;

/**
* @brief 脚本运行时接口
*/
class AVATAR_EXPORT CScriptContext {
public:
	//! 获取当前实例
	CScriptContext* GetCurrent(struct lua_State* lua);

	//! 执行脚本
	bool Execute(const char* script);

	//! 绑定脚本方法
	void BindFunction(const string& name, void* function);
	//! 绑定脚本方法
	void BindFunction(const string& table, const string& name, void* function);

	//! 判断是否 bool 类型
	bool IsBoolean(int index);
	//! 判断是否 int 类型
	bool IsInteger(int index);
	//! 判断是否 float 类型
	bool IsNumber(int index);
	//! 判断是否 string 类型
	bool IsString(int index);
	//! 判断是否 table 类型
	bool IsTable(int index);
	//! 判断是否 function 类型
	bool IsFunction(int index);

	//! 获取 bool 值
	bool ToBoolean(int index);
	//! 获取 int 值
	int ToInteger(int index);
	//! 获取 float 值
	float ToNumber(int index);
	//! 获取 string 值
	const char* ToString(int index);
	//! 获取 string 值
	const char* ToString(int index, size_t* length);

	//! 获取表元素个数
	int TableSize(int index);
	//! 获取表元素到栈顶
	void TableField(int index, int key);
	//! 获取表元素到栈顶
	void TableField(int index, const char* key);
	//! 获取表 bool 字段值
	bool TableValue(int index, const char* name, bool def);
	//! 获取表 int 字段值
	int TableValue(int index, const char* name, int def);
	//! 获取表 float 字段值
	float TableValue(int index, const char* name, float def);
	//! 获取表 string 字段值
	const char* TableValue(int index, const char* name, const char* def);

	//! 将 bool 值压栈
	void PushValue(bool value);
	//! 将 int 值压栈
	void PushValue(int value);
	//! 将 float 值压栈
	void PushValue(float value);
	//! 将 string 值压栈
	void PushValue(const char* value);
	//! 将 string 值压栈
	void PushValue(const char* value, size_t length);
	//! 将空 table 压栈
	void PushTable();
	//! 将栈顶元素作为 table 值压栈
	void PushTable(int key);
	//! 将栈顶元素作为 table 值压栈
	void PushTable(const char* key);
	//! 将栈顶元素出栈
	void PopValue(int count);

	//! 引用回调方法
	int RefCallback();
	//! 取消回调引用
	void UnrefCallback(int callback);

	//! 开始调用回调方法
	void InvokeBegin(int callback);
	//! 开始调用回调方法
	void InvokeBegin(const string& function);
	//! 结束调用回调方法
	int InvokeEnd(int params);

private:
	//! 私有构造函数
	CScriptContext();

private:
	//! 当前状态机
	struct lua_State* m_pLuaState;
	//! 仅从 CScriptManager 创建
	friend class CScriptManager;
};

#endif