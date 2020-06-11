//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CJsonParser.h"
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
CJsonParser::CJsonParser() {
	m_sValue.type = SValue::NIL;
	m_sValue.length = 0;
	m_sValue.pValue = 0;
}

/**
* 构造函数
*/
CJsonParser::CJsonParser(const char* content) {
	m_sValue.type = SValue::NIL;
	m_sValue.length = strlen(content);
	m_sValue.pValue = content;
	Parse(m_sValue.pValue, m_sValue.length);
}

/**
* 构造函数
*/
CJsonParser::CJsonParser(const char* content, int length) {
	m_sValue.type = SValue::NIL;
	m_sValue.length = length;
	m_sValue.pValue = content;
	Parse(m_sValue.pValue, m_sValue.length);
}

/**
* 解析指定字符串
*/
bool CJsonParser::Parse(const char* content, int length) {
	m_sValue.type = SValue::NIL;
	m_sValue.length = length;
	m_sValue.pValue = content;
	m_mapValues.clear();
	m_vecValues.clear();
	int lt = 0;
	int rt = length - 1;
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
bool CJsonParser::IsContain(const char* name) const {
	return m_mapValues.count(name) > 0;
}

/**
* 获取数组节点数组大小
*/
int CJsonParser::GetCount() const {
	return m_vecValues.size();
}

/**
* 获取节点内指定名称的值
*/
CJsonParser& CJsonParser::GetValue(const char* name) {
	return m_mapValues[name];
}

/**
* 获取节点内指定名称的值
*/
CJsonParser& CJsonParser::GetValue(const string& name) {
	return m_mapValues[name];
}

/**
* 获取数组节点内指定的值
*/
CJsonParser& CJsonParser::GetValue(int index) {
	return m_vecValues[index];
}

/**
* 同 GetValue(const char* name)
*/
CJsonParser& CJsonParser::operator [] (const char* name) {
	return m_mapValues[name];
}

/**
* 同 GetValue(const string& name)
*/
CJsonParser& CJsonParser::operator [] (const string& name) {
	return m_mapValues[name];
}

/**
* 同 GetValue(int index)
*/
CJsonParser& CJsonParser::operator [] (int index) {
	return m_vecValues[index];
}

/**
* 转换为布尔类型
*/
bool CJsonParser::ToBool() const {
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
int CJsonParser::ToInt() const {
	if (m_sValue.type == SValue::INTEGER) return m_sValue.iValue;
	if (m_sValue.type == SValue::BOOL) return m_sValue.bValue ? 1 : 0;
	if (m_sValue.type == SValue::FLOAT) return static_cast<int>(m_sValue.fValue);
	if (m_sValue.type == SValue::STRING) return atoi(string(m_sValue.pValue, m_sValue.length).c_str());
	return 0;
}

/**
* 转换为浮点类型
*/
float CJsonParser::ToFloat() const {
	if (m_sValue.type == SValue::FLOAT) return m_sValue.fValue;
	if (m_sValue.type == SValue::BOOL) return m_sValue.bValue ? 1.0f : 0.0f;
	if (m_sValue.type == SValue::INTEGER) return static_cast<float>(m_sValue.iValue);
	if (m_sValue.type == SValue::STRING) return static_cast<float>(atof(string(m_sValue.pValue, m_sValue.length).c_str()));
	return 0.0f;
}

/**
* 转换为字符串类型
*/
string CJsonParser::ToString() const {
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
int CJsonParser::ParseObject(const char* data, int length, CJsonParser& parser) {
	if (length < 2 || data[0] != '{') return 0;
	parser.m_sValue.type = SValue::OBJECT;
	int position = 0;
	int count = 0;
	bool start = true;
	string name = "";
	CJsonParser subject;
	while (++position < length) {
		switch (data[position]) {
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			break;
		case ':': start = false; break;
		case ',': start = true; break;
		case '{':
			count = ParseObject(data + position, length - position, parser.m_mapValues[name]);
			if (count == 0) return 0;
			position += count - 1;
			break;
		case '[':
			count = ParseArray(data + position, length - position, parser.m_mapValues[name]);
			if (count == 0) return 0;
			position += count - 1;
			break;
		case '"':
			count = ParseString(data + position, length - position, subject);
			if (count == 0) return 0;
			if (start) name = subject.ToString();
			else parser.m_mapValues[name] = subject;
			position += count - 1;
			break;
		case '}': return position + 1;
		case ']': return 0;
		default:
			count = ParseNull(data + position, length - position, subject);
			if (count > 0) { parser.m_mapValues[name] = subject; position += count - 1;	break; }
			count = ParseBool(data + position, length - position, subject);
			if (count > 0) { parser.m_mapValues[name] = subject; position += count - 1;	break; }
			count = ParseNumber(data + position, length - position, subject);
			if (count > 0) { parser.m_mapValues[name] = subject; position += count - 1;	break; }
			return 0;
		}
	}
	return 0;
}

/**
* 解析数组值
*/
int CJsonParser::ParseArray(const char* data, int length, CJsonParser& parser) {
	if (length < 2 || data[0] != '[') return 0;
	parser.m_sValue.type = SValue::ARRAY;
	int position = 0;
	int count = 0;
	CJsonParser subject;
	while (++position < length) {
		switch (data[position]) {
		case ' ':
		case '\t':
		case '\r':
		case '\n':
		case ',':
			break;
		case '{':
			parser.m_vecValues.resize(parser.m_vecValues.size() + 1);
			count = ParseObject(data + position, length - position, parser.m_vecValues.back());
			if (count == 0) return 0;
			position += count - 1;
			break;
		case '[':
			parser.m_vecValues.resize(parser.m_vecValues.size() + 1);
			count = ParseArray(data + position, length - position, parser.m_vecValues.back());
			if (count == 0) return 0;
			position += count - 1;
			break;
		case '"':
			parser.m_vecValues.resize(parser.m_vecValues.size() + 1);
			count = ParseString(data + position, length - position, parser.m_vecValues.back());
			if (count == 0) return 0;
			position += count - 1;
			break;
		case '}': return 0;
		case ']': return position + 1;
		default:
			count = ParseNull(data + position, length - position, subject);
			if (count > 0) { parser.m_vecValues.push_back(subject); position += count - 1;	break; }
			count = ParseBool(data + position, length - position, subject);
			if (count > 0) { parser.m_vecValues.push_back(subject); position += count - 1;	break; }
			count = ParseNumber(data + position, length - position, subject);
			if (count > 0) { parser.m_vecValues.push_back(subject); position += count - 1;	break; }
			return 0;
		}
	}
	return 0;
}

/**
* 解析普通字符串
*/
int CJsonParser::ParseString(const char* data, int length, CJsonParser& parser) {
	if (length < 2) return 0;
	if (data[0] == '"') {
		int position = 0;
		while (position + 1 < length && (data[position] == '\\' || data[position + 1] != '"')) position++;
		position += 1;
		if (data[position] == '"') {
			parser.m_sValue.type = SValue::STRING;
			parser.m_sValue.pValue = data + 1;
			parser.m_sValue.length = position - 1;
			return position + 1;
		}
	}
	return 0;
}

/**
* 解析空值
*/
int CJsonParser::ParseNull(const char* data, int length, CJsonParser& parser) {
	if (length < 4) return 0;
	if ((data[0] == 'n' || data[0] == 'N') &&
		(data[1] == 'u' || data[1] == 'U') &&
		(data[2] == 'l' || data[2] == 'L') &&
		(data[3] == 'l' || data[3] == 'L')) {
		parser.m_sValue.type = SValue::NIL;
		parser.m_sValue.pValue = 0;
		parser.m_sValue.length = 4;
		return 4;
	}
	return 0;
}

/**
* 解析布尔字符串
*/
int CJsonParser::ParseBool(const char* data, int length, CJsonParser& parser) {
	if (length < 4) return 0;
	if ((data[0] == 't' || data[0] == 'T') &&
		(data[1] == 'r' || data[1] == 'R') &&
		(data[2] == 'u' || data[2] == 'U') &&
		(data[3] == 'e' || data[3] == 'E')) {
		parser.m_sValue.type = SValue::BOOL;
		parser.m_sValue.bValue = true;
		parser.m_sValue.length = 4;
		return 4;
	}
	if (length < 5) return 0;
	if ((data[0] == 'f' || data[0] == 'F') &&
		(data[1] == 'a' || data[1] == 'A') &&
		(data[2] == 'l' || data[2] == 'L') &&
		(data[3] == 's' || data[3] == 'S') &&
		(data[4] == 'e' || data[4] == 'E')) {
		parser.m_sValue.type = SValue::BOOL;
		parser.m_sValue.bValue = false;
		parser.m_sValue.length = 5;
		return 5;
	}
	return 0;
}

/**
* 解析数字字符串
*/
int CJsonParser::ParseNumber(const char* data, int length, CJsonParser& parser) {
	if (length < 1) return 0;
	if (data[0] == '-' || (data[0] >= '0' && data[0] <= '9')) {
		int position = 0;
		while (position < length && data[position] != ' ' && data[position] != ',' && data[position] != '}' && data[position] != ']') position++;
		bool integer = true;
		for (int i = 0; i < position; i++) {
			if (data[i] == '.' || data[i] == 'e' || data[i] == 'E') {
				integer = false;
				break;
			}
		}
		if (integer) {
			parser.m_sValue.type = SValue::INTEGER;
			parser.m_sValue.iValue = atoi(string(data, position).c_str());
			parser.m_sValue.length = position;
		} else {
			parser.m_sValue.type = SValue::FLOAT;
			parser.m_sValue.fValue = static_cast<float>(atof(string(data, position).c_str()));
			parser.m_sValue.length = position;
		}
		return position;
	}
	return 0;
}