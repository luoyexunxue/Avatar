//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CJSONOBJECT_H_
#define _CJSONOBJECT_H_
#include "AvatarConfig.h"
#include <string>
#include <map>
#include <vector>
using std::string;
using std::map;
using std::vector;

/**
* @brief JSON 解析器
*/
class AVATAR_EXPORT CJsonObject {
public:
	//! 默认构造函数
	CJsonObject();
	//! 移动构造函数
	CJsonObject(CJsonObject&& json);
	//! 指定字符串构造
	CJsonObject(const char* content);
	//! 使用指定长度的字符串构造
	CJsonObject(const char* content, size_t length);

	//! 解析指定字符串
	bool Parse(const char* content, size_t length);
	//! 判断是否包含指定值
	bool IsContain(const char* name) const;
	//! 获取数组节点数组大小
	size_t GetCount() const;

	//! 获取节点内指定名称的值
	CJsonObject& GetValue(const string& name);
	//! 获取数组节点内指定的值
	CJsonObject& GetValue(size_t index);
	//! 同 GetValue(const string& name)
	CJsonObject& operator [] (const string& name);
	//! 同 GetValue(int index)
	CJsonObject& operator [] (size_t index);

public:
	//! 是否为空节点
	bool IsEmpty() const { return m_sValue.type == SValue::NIL; }
	//! 是否为布尔类型节点
	bool IsBool() const { return m_sValue.type == SValue::BOOL; }
	//! 是否为整数类型节点
	bool IsInteger() const { return m_sValue.type == SValue::INTEGER; }
	//! 是否为数字类型节点
	bool IsFloat() const { return m_sValue.type == SValue::FLOAT; }
	//! 是否为字符串类型节点
	bool IsString() const { return m_sValue.type == SValue::STRING; }
	//! 是否为对象类型节点
	bool IsObject() const { return m_sValue.type == SValue::OBJECT; }
	//! 是否为数组类型节点
	bool IsArray() const { return m_sValue.type == SValue::ARRAY; }

	//! 转换为布尔类型
	bool ToBool() const;
	//! 转换为整型类型
	int ToInt() const;
	//! 转换为浮点类型
	float ToFloat() const;
	//! 转换为字符串类型
	string ToString() const;

private:
	//! JSON 值泛定义
	typedef struct _SValue {
		enum { NIL, BOOL, INTEGER, FLOAT, STRING, OBJECT, ARRAY } type;
		size_t length;
		union {
			bool bValue;
			int iValue;
			float fValue;
			const char* pValue;
		};
	} SValue;

	//! 解析对象值
	size_t ParseObject(const char* data, size_t length, CJsonObject& value);
	//! 解析数组值
	size_t ParseArray(const char* data, size_t length, CJsonObject& value);
	//! 解析普通字符串
	size_t ParseString(const char* data, size_t length, CJsonObject& value);
	//! 解析空值
	size_t ParseNull(const char* data, size_t length, CJsonObject& value);
	//! 解析布尔字符串
	size_t ParseTrue(const char* data, size_t length, CJsonObject& value);
	//! 解析布尔字符串
	size_t ParseFalse(const char* data, size_t length, CJsonObject& value);
	//! 解析数字字符串
	size_t ParseNumber(const char* data, size_t length, CJsonObject& value);

private:
	//! JSON 值
	SValue m_sValue;
	//! JSON 内部子元素
	map<string, CJsonObject> m_mapValues;
	//! JSON 内部数组
	vector<CJsonObject> m_vecValues;
};

#endif