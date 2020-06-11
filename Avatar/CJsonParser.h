//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CJSONPARSER_H_
#define _CJSONPARSER_H_
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
class AVATAR_EXPORT CJsonParser {
public:
	//! 默认构造函数
	CJsonParser();
	//! 指定字符串构造
	CJsonParser(const char* content);
	//! 使用指定长度的字符串构造
	CJsonParser(const char* content, int length);

	//! 解析指定字符串
	bool Parse(const char* content, int length);
	//! 判断是否包含指定值
	bool IsContain(const char* name) const;
	//! 获取数组节点数组大小
	int GetCount() const;

	//! 获取节点内指定名称的值
	CJsonParser& GetValue(const char* name);
	//! 获取节点内指定名称的值
	CJsonParser& GetValue(const string& name);
	//! 获取数组节点内指定的值
	CJsonParser& GetValue(int index);
	//! 同 GetValue(const char* name)
	CJsonParser& operator [] (const char* name);
	//! 同 GetValue(const string& name)
	CJsonParser& operator [] (const string& name);
	//! 同 GetValue(int index)
	CJsonParser& operator [] (int index);

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
		int length;
		union {
			bool bValue;
			int iValue;
			float fValue;
			const char* pValue;
		};
	} SValue;

	//! 解析对象值
	int ParseObject(const char* data, int length, CJsonParser& parser);
	//! 解析数组值
	int ParseArray(const char* data, int length, CJsonParser& parser);
	//! 解析普通字符串
	int ParseString(const char* data, int length, CJsonParser& parser);
	//! 解析空值
	int ParseNull(const char* data, int length, CJsonParser& parser);
	//! 解析布尔字符串
	int ParseBool(const char* data, int length, CJsonParser& parser);
	//! 解析数字字符串
	int ParseNumber(const char* data, int length, CJsonParser& parser);

private:
	//! JSON 值
	SValue m_sValue;
	//! JSON 内部子元素
	map<string, CJsonParser> m_mapValues;
	//! JSON 内部数组
	vector<CJsonParser> m_vecValues;
};

#endif