//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCRIPTCONTEXT_H_
#define _CSCRIPTCONTEXT_H_
#include "AvatarConfig.h"
#include <string>
using std::string;

/**
* @brief �ű�����ʱ�ӿ�
*/
class AVATAR_EXPORT CScriptContext {
public:
	//! ��ȡ��ǰʵ��
	CScriptContext* GetCurrent(struct lua_State* lua);

	//! �󶨽ű�����
	void BindFunction(const string& name, void* function);
	//! �󶨽ű�����
	void BindFunction(const string& group, const string& name, void* function);

	//! �ж��Ƿ� bool ����
	bool IsBoolean(int index);
	//! �ж��Ƿ� int ����
	bool IsInteger(int index);
	//! �ж��Ƿ� float ����
	bool IsNumber(int index);
	//! �ж��Ƿ� string ����
	bool IsString(int index);
	//! �ж��Ƿ� function ����
	bool IsFunction(int index);

	//! ��ȡ bool ֵ
	bool ToBoolean(int index);
	//! ��ȡ int ֵ
	int ToInteger(int index);
	//! ��ȡ float ֵ
	float ToNumber(int index);
	//! ��ȡ string ֵ
	const char* ToString(int index);

	//! �� bool ֵѹջ
	void PushValue(bool value);
	//! �� int ֵѹջ
	void PushValue(int value);
	//! �� float ֵѹջ
	void PushValue(float value);
	//! �� string ֵѹջ
	void PushValue(const char* value);

	//! ���ûص�����
	int RefCallback();
	//! ȡ���ص�����
	void UnrefCallback(int callback);

	//! ��ʼ���ûص�����
	void InvokeBegin(int callback);
	//! ��ʼ���ûص�����
	void InvokeBegin(const string& function);
	//! �������ûص�����
	void InvokeEnd(int params);

private:
	//! ˽�й��캯��
	CScriptContext();

private:
	//! ��ǰ״̬��
	struct lua_State* m_pLuaState;
	//! ���� CScriptManager ����
	friend class CScriptManager;
};

#endif