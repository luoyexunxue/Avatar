//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CJsonObject.h"
#include "AvatarConfig.h"
#include <cstdlib>
#include <cstring>
using std::string;
#ifdef AVATAR_WINDOWS
#define snprintf _snprintf_s
#endif

/**
* 构造函数
*/
CJsonObject::CJsonObject() {
	m_sValue.type = SValue::NIL;
	m_sValue.length = 0;
	m_sValue.pValue = 0;
}

/**
* 移动构造函数
*/
CJsonObject::CJsonObject(CJsonObject&& json) : m_mapValues(std::move(json.m_mapValues)), m_vecValues(std::move(json.m_vecValues)) {
	m_sValue = json.m_sValue;
}

/**
* 构造函数
*/
CJsonObject::CJsonObject(const char* content) {
	m_sValue.type = SValue::NIL;
	m_sValue.length = strlen(content);
	m_sValue.pValue = content;
	Parse(m_sValue.pValue, m_sValue.length);
}

/**
* 构造函数
*/
CJsonObject::CJsonObject(const char* content, size_t length) {
	m_sValue.type = SValue::NIL;
	m_sValue.length = length;
	m_sValue.pValue = content;
	Parse(m_sValue.pValue, m_sValue.length);
}

/**
* 解析指定字符串
*/
bool CJsonObject::Parse(const char* content, size_t length) {
	m_sValue.type = SValue::NIL;
	m_sValue.length = length;
	m_sValue.pValue = content;
	m_mapValues.clear();
	m_vecValues.clear();
	size_t lt = 0;
	size_t rt = length - 1;
	while (lt < length && (content[lt] == ' ' || content[lt] == '\t' || content[lt] == '\r' || content[lt] == '\n')) lt++;
	while (rt >= 0 && (content[rt] == ' ' || content[rt] == '\t' || content[rt] == '\r' || content[rt] == '\n')) rt--;
	if (lt >= rt) return false;
	if (ParseObject(content + lt, rt + 1 - lt, *this) > 0) return true;
	if (ParseArray(content + lt, rt + 1 - lt, *this) > 0) return true;
	return false;
}

/**
* 判断是否包含指定值
*/
bool CJsonObject::IsContain(const char* name) const {
	return m_mapValues.count(name) > 0;
}

/**
* 获取数组节点数组大小
*/
size_t CJsonObject::GetCount() const {
	if (SValue::OBJECT == m_sValue.type) {
		return m_mapValues.size();
	}
	return m_vecValues.size();
}

/**
* 获取节点内指定名称的值
*/
CJsonObject& CJsonObject::GetValue(const string& name) {
	return m_mapValues[name];
}

/**
* 获取数组节点内指定的值
*/
CJsonObject& CJsonObject::GetValue(size_t index) {
	if (SValue::OBJECT == m_sValue.type) {
		map<string, CJsonObject>::iterator iter = m_mapValues.begin();
		while (index--) ++iter;
		return iter->second;
	}
	return m_vecValues[index];
}

/**
* 同 GetValue(const string& name)
*/
CJsonObject& CJsonObject::operator [] (const string& name) {
	return m_mapValues[name];
}

/**
* 同 GetValue(int index)
*/
CJsonObject& CJsonObject::operator [] (size_t index) {
	if (SValue::OBJECT == m_sValue.type) {
		map<string, CJsonObject>::iterator iter = m_mapValues.begin();
		while (index--) ++iter;
		return iter->second;
	}
	return m_vecValues[index];
}

/**
* 转换为布尔类型
*/
bool CJsonObject::ToBool() const {
	if (m_sValue.type == SValue::BOOL) return m_sValue.bValue;
	if (m_sValue.type == SValue::INTEGER) return m_sValue.iValue > 0;
	if (m_sValue.type == SValue::FLOAT) return m_sValue.fValue > 0.0f;
	if (m_sValue.type == SValue::STRING && m_sValue.length == 4) {
		if ((m_sValue.pValue[0] == 't' || m_sValue.pValue[0] == 'T') &&
			(m_sValue.pValue[1] == 'r' || m_sValue.pValue[1] == 'R') &&
			(m_sValue.pValue[2] == 'u' || m_sValue.pValue[2] == 'U') &&
			(m_sValue.pValue[3] == 'e' || m_sValue.pValue[3] == 'E')) return true;
	}
	return false;
}

/**
* 转换为整型类型
*/
int CJsonObject::ToInt() const {
	if (m_sValue.type == SValue::INTEGER) return m_sValue.iValue;
	if (m_sValue.type == SValue::BOOL) return m_sValue.bValue ? 1 : 0;
	if (m_sValue.type == SValue::FLOAT) return static_cast<int>(m_sValue.fValue);
	if (m_sValue.type == SValue::STRING) return atoi(string(m_sValue.pValue, m_sValue.length).c_str());
	return 0;
}

/**
* 转换为浮点类型
*/
float CJsonObject::ToFloat() const {
	if (m_sValue.type == SValue::FLOAT) return m_sValue.fValue;
	if (m_sValue.type == SValue::BOOL) return m_sValue.bValue ? 1.0f : 0.0f;
	if (m_sValue.type == SValue::INTEGER) return static_cast<float>(m_sValue.iValue);
	if (m_sValue.type == SValue::STRING) return static_cast<float>(atof(string(m_sValue.pValue, m_sValue.length).c_str()));
	return 0.0f;
}

/**
* 转换为字符串类型
*/
string CJsonObject::ToString() const {
	if (m_sValue.type == SValue::STRING) return string().assign(m_sValue.pValue, m_sValue.length);
	if (m_sValue.type == SValue::BOOL) return m_sValue.bValue ? "true" : "false";
	if (m_sValue.type == SValue::INTEGER) {
		char buffer[16];
		snprintf(buffer, 16, "%d", m_sValue.iValue);
		return buffer;
	}
	if (m_sValue.type == SValue::FLOAT) {
		char buffer[16];
		snprintf(buffer, 16, "%.6f", m_sValue.fValue);
		return buffer;
	}
	return "";
}

/**
* 解析对象值
*/
size_t CJsonObject::ParseObject(const char* data, size_t length, CJsonObject& value) {
	if (length < 2 || data[0] != '{') return 0;
	value.m_sValue.type = SValue::OBJECT;
	size_t position = 0;
	size_t count = 0;
	bool start = true;
	string name = "";
	while (++position < length) {
		CJsonObject subject;
		switch (data[position]) {
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			break;
		case ':': start = false; break;
		case ',': start = true; break;
		case '{':
			count = ParseObject(data + position, length - position, value.m_mapValues[name]);
			if (count == 0) return 0;
			position += count - 1;
			break;
		case '[':
			count = ParseArray(data + position, length - position, value.m_mapValues[name]);
			if (count == 0) return 0;
			position += count - 1;
			break;
		case 'n':
			count = ParseNull(data + position, length - position, value.m_mapValues[name]);
			if (count == 0) return 0;
			position += count - 1;
			break;
		case 't':
			count = ParseTrue(data + position, length - position, value.m_mapValues[name]);
			if (count == 0) return 0;
			position += count - 1;
			break;
		case 'f':
			count = ParseFalse(data + position, length - position, value.m_mapValues[name]);
			if (count == 0) return 0;
			position += count - 1;
			break;
		case '"':
			count = ParseString(data + position, length - position, subject);
			if (count == 0) return 0;
			if (start) name = subject.ToString();
			else value.m_mapValues.insert(std::pair<string, CJsonObject>(name, std::move(subject)));
			position += count - 1;
			break;
		case '}': return position + 1;
		case ']': return 0;
		default:
			count = ParseNumber(data + position, length - position, value.m_mapValues[name]);
			if (count == 0) return 0;
			position += count - 1;
			break;
		}
	}
	return 0;
}

/**
* 解析数组值
*/
size_t CJsonObject::ParseArray(const char* data, size_t length, CJsonObject& value) {
	if (length < 2 || data[0] != '[') return 0;
	value.m_sValue.type = SValue::ARRAY;
	size_t position = 0;
	size_t count = 0;
	while (++position < length) {
		switch (data[position]) {
		case ' ':
		case '\t':
		case '\r':
		case '\n':
		case ',':
			break;
		case '{':
			value.m_vecValues.resize(value.m_vecValues.size() + 1);
			count = ParseObject(data + position, length - position, value.m_vecValues.back());
			if (count == 0) return 0;
			position += count - 1;
			break;
		case '[':
			value.m_vecValues.resize(value.m_vecValues.size() + 1);
			count = ParseArray(data + position, length - position, value.m_vecValues.back());
			if (count == 0) return 0;
			position += count - 1;
			break;
		case 'n':
			value.m_vecValues.resize(value.m_vecValues.size() + 1);
			count = ParseNull(data + position, length - position, value.m_vecValues.back());
			if (count == 0) return 0;
			position += count - 1;
			break;
		case 't':
			value.m_vecValues.resize(value.m_vecValues.size() + 1);
			count = ParseTrue(data + position, length - position, value.m_vecValues.back());
			if (count == 0) return 0;
			position += count - 1;
			break;
		case 'f':
			value.m_vecValues.resize(value.m_vecValues.size() + 1);
			count = ParseFalse(data + position, length - position, value.m_vecValues.back());
			if (count == 0) return 0;
			position += count - 1;
			break;
		case '"':
			value.m_vecValues.resize(value.m_vecValues.size() + 1);
			count = ParseString(data + position, length - position, value.m_vecValues.back());
			if (count == 0) return 0;
			position += count - 1;
			break;
		case '}': return 0;
		case ']': return position + 1;
		default:
			value.m_vecValues.resize(value.m_vecValues.size() + 1);
			count = ParseNumber(data + position, length - position, value.m_vecValues.back());
			if (count == 0) return 0;
			position += count - 1;
			break;
		}
	}
	return 0;
}

/**
* 解析普通字符串
*/
size_t CJsonObject::ParseString(const char* data, size_t length, CJsonObject& value) {
	if (length < 2) return 0;
	size_t position = 0;
	while (position + 1 < length && (data[position] == '\\' || data[position + 1] != '"')) position++;
	position += 1;
	if (data[position] == '"') {
		value.m_sValue.type = SValue::STRING;
		value.m_sValue.pValue = data + 1;
		value.m_sValue.length = position - 1;
		return position + 1;
	}
	return 0;
}

/**
* 解析空值
*/
size_t CJsonObject::ParseNull(const char* data, size_t length, CJsonObject& value) {
	if (length > 3 && data[1] == 'u' && data[2] == 'l' && data[3] == 'l') {
		value.m_sValue.type = SValue::NIL;
		value.m_sValue.pValue = 0;
		value.m_sValue.length = 4;
		return 4;
	}
	return 0;
}

/**
* 解析布尔字符串 True
*/
size_t CJsonObject::ParseTrue(const char* data, size_t length, CJsonObject& value) {
	if (length > 3 && data[1] == 'r' && data[2] == 'u' && data[3] == 'e') {
		value.m_sValue.type = SValue::BOOL;
		value.m_sValue.bValue = true;
		value.m_sValue.length = 4;
		return 4;
	}
	return 0;
}

/**
* 解析布尔字符串 False
*/
size_t CJsonObject::ParseFalse(const char* data, size_t length, CJsonObject& value) {
	if (length > 4 && data[1] == 'a' && data[2] == 'l' && data[3] == 's' && data[4] == 'e') {
		value.m_sValue.type = SValue::BOOL;
		value.m_sValue.bValue = false;
		value.m_sValue.length = 5;
		return 5;
	}
	return 0;
}

/**
* 解析数字字符串
*/
size_t CJsonObject::ParseNumber(const char* data, size_t length, CJsonObject& value) {
	if (data[0] == '-' || (data[0] >= '0' && data[0] <= '9')) {
		size_t position = 0;
		while (position < length && data[position] != ' ' && data[position] != ',' && data[position] != '}' && data[position] != ']') position++;
		bool integer = true;
		for (size_t i = 0; i < position; i++) {
			if (data[i] == '.' || data[i] == 'e' || data[i] == 'E') {
				integer = false;
				break;
			}
		}
		if (integer) {
			value.m_sValue.type = SValue::INTEGER;
			value.m_sValue.iValue = atoi(string(data, position).c_str());
			value.m_sValue.length = position;
		} else {
			value.m_sValue.type = SValue::FLOAT;
			value.m_sValue.fValue = static_cast<float>(atof(string(data, position).c_str()));
			value.m_sValue.length = position;
		}
		return position;
	}
	return 0;
}