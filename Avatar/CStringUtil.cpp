//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CStringUtil.h"
#include "AvatarConfig.h"
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <sstream>
#ifdef AVATAR_WINDOWS
#include <windows.h>
#endif

/**
* 格式化字符串
*/
string CStringUtil::Format(const char* format, ...) {
	vector<char> buffer;
	va_list args;
	va_start(args, format);
	buffer.resize(vsnprintf(0, 0, format, args) + 1);
	va_start(args, format);
	vsnprintf(&buffer[0], buffer.size(), format, args);
	va_end(args);
	return string(&buffer[0], buffer.size() - 1);
}

/**
* 转换为小写
*/
string CStringUtil::LowerCase(const string& str) {
	std::ostringstream strbuff;
	for (string::size_type i = 0; i < str.length(); i++) {
		if (str.at(i) >= 'A' && str.at(i) <= 'Z') {
			strbuff.put(str.at(i) + 0x20);
		} else {
			strbuff.put(str.at(i));
		}
	}
	return strbuff.str();
}

/**
* 转换为大写
*/
string CStringUtil::UpperCase(const string& str) {
	std::ostringstream strbuff;
	for (string::size_type i = 0; i < str.length(); i++) {
		if (str.at(i) >= 'a' && str.at(i) <= 'z') {
			strbuff.put(str.at(i) - 0x20);
		} else {
			strbuff.put(str.at(i));
		}
	}
	return strbuff.str();
}

/**
* 去除左右空白
*/
string CStringUtil::Trim(const string& str) {
	const char* p = str.c_str();
	size_t len = str.length();
	size_t beg = 0;
	size_t end = len - 1;
	while ((p[beg] == ' ' || p[beg] == '\t' || p[beg] == '\r' || p[beg] == '\n') && beg < len) beg++;
	while ((p[end] == ' ' || p[end] == '\t' || p[end] == '\r' || p[end] == '\n') && end > 0) end--;
	if (end < beg) {
		return "";
	} else {
		return str.substr(beg, end - beg + 1);
	}
}

/**
* 去除左边空白
*/
string CStringUtil::TrimLeft(const string& str) {
	const char* p = str.c_str();
	size_t len = str.length();
	size_t pos = 0;
	while ((p[pos] == ' ' || p[pos] == '\t' || p[pos] == '\r' || p[pos] == '\n') && pos < len) pos++;
	return str.substr(pos);
}

/**
* 去除右边空白
*/
string CStringUtil::TrimRight(const string& str) {
	const char* p = str.c_str();
	size_t pos = str.length() - 1;
	while ((p[pos] == ' ' || p[pos] == '\t' || p[pos] == '\r' || p[pos] == '\n') && pos >= 0) pos--;
	return str.substr(0, pos + 1);
}

/**
* 替换指定字符串
*/
string CStringUtil::Replace(const string& str, const char* from, const char* to) {
	string temp = str;
	string::size_type pos = 0;
	string::size_type a = strlen(from);
	string::size_type b = strlen(to);
	while ((pos = temp.find(from, pos)) != string::npos) {
		temp.replace(pos, a, to);
		pos += b;
	}
	return temp;
}

/**
* 移除指定字符串
*/
string CStringUtil::Remove(const string& str, const char* key) {
	string temp = str;
	string::size_type size = strlen(key);
	string::size_type pos = temp.find(key);
	while (pos != string::npos) {
		string prev = temp.substr(0, pos);
		string post = temp.substr(pos + size);
		temp = prev + post;
		pos = temp.find(key);
	}
	return temp;
}

/**
* 按指定的字符串分割
* @param output 输出数组
* @param str 待分割字符串
* @param key 分割关键字
* @param trim 是否对分割后的字符串去空格
* @return 返回分割后的个数
*/
size_t CStringUtil::Split(vector<string>& output, const string& str, const char* key, bool trim) {
	output.clear();
	if (str.empty()) return 0;
	string::size_type begIndex = 0;
	string::size_type endIndex = 0;
	while ((endIndex = str.find(key, begIndex)) != string::npos) {
		string item = str.substr(begIndex, endIndex - begIndex);
		output.push_back(trim ? CStringUtil::Trim(item) : item);
		begIndex = endIndex + 1;
	}
	string lastStr = str.substr(begIndex);
	output.push_back(trim ? CStringUtil::Trim(lastStr) : lastStr);
	return output.size();
}

/**
* 按指定的字符串分割
* @param output 输出数组
* @param size 输出数组大小
* @param str 待分割字符串
* @param key 分割关键字
* @param trim 是否对分割后的字符串去空格
* @return 返回分割后的个数
*/
size_t CStringUtil::Split(string output[], size_t size, const string& str, const char* key, bool trim) {
	if (str.empty() || size < 1) return 0;
	size_t count = 0;
	string::size_type begIndex = 0;
	string::size_type endIndex = 0;
	while ((endIndex = str.find(key, begIndex)) != string::npos) {
		string item = str.substr(begIndex, endIndex - begIndex);
		output[count++] = trim ? CStringUtil::Trim(item) : item;
		begIndex = endIndex + 1;
		if (count == size) return count;
	}
	string lastStr = str.substr(begIndex);
	output[count++] = trim ? CStringUtil::Trim(lastStr) : lastStr;
	return count;
}

/**
* 使用指定的分隔符连接字符串数组
* @param input 输入数组
* @param key 分割关键字
* @return 返回连接后的字符串
*/
string CStringUtil::Join(const vector<string>& input, const char* key) {
	if (input.empty()) return "";
	std::ostringstream strbuff;
	strbuff << input[0];
	for (size_t i = 1; i < input.size(); i++) {
		strbuff << key << input[i];
	}
	return strbuff.str();
}

/**
* 使用指定的分隔符连接字符串数组
* @param input 输入数组
* @param size 输入数组大小
* @param key 分割关键字
* @return 返回连接后的字符串
*/
string CStringUtil::Join(const string input[], size_t size, const char* key) {
	if (size == 0) return "";
	std::ostringstream strbuff;
	strbuff << input[0];
	for (size_t i = 1; i < size; i++) {
		strbuff << key << input[i];
	}
	return strbuff.str();
}

/**
* 是否以指定字符串开头
*/
bool CStringUtil::StartWith(const string& str, const char* key, bool ignoreCase) {
	if (ignoreCase) {
		string src_uppercase = UpperCase(str);
		string key_uppercase = UpperCase(key);
		return src_uppercase.find(key_uppercase) == 0;
	}
	return str.find(key) == 0;
}

/**
* 是否以指定字符串结尾
*/
bool CStringUtil::EndWith(const string& str, const char* key, bool ignoreCase) {
	if (ignoreCase) {
		string src_uppercase = UpperCase(str);
		string key_uppercase = UpperCase(key);
		return src_uppercase.rfind(key_uppercase) == str.length() - strlen(key);
	}
	return str.rfind(key) == str.length() - strlen(key);
}

/**
* UTF8 字符串转换为 wchar_t 数组
*/
size_t CStringUtil::Utf8ToWideCharArray(const char* src, wchar_t* buffer, size_t size) {
	size_t count = 0;
	while (*src && count < size - 1) {
		char flag = *src;
		if ((flag & 0x80) == 0x00) {
			buffer[count++] = wchar_t(src[0]);
			src += 1;
		} else if ((flag & 0xE0) == 0xC0) {
			buffer[count++] = ((src[0] & 0x1F) << 6) + (src[1] & 0x3F);
			src += 2;
		} else if ((flag & 0xF0) == 0xE0) {
			buffer[count++] = ((src[0] & 0x0F) << 12) + ((src[1] & 0x3F) << 6) + (src[2] & 0x3F);
			src += 3;
		} else {
			buffer[count++] = wchar_t(-1);
			while (flag & 0x80) {
				flag <<= 1;
				src += 1;
			}
		}
	}
	buffer[count] = 0;
	return count;
}

/**
* wchar_t 数组转换为 UTF8 字符串
*/
size_t CStringUtil::WideCharArrayToUtf8(const wchar_t* src, char* buffer, size_t size) {
	size_t count = 0;
	while (*src && count < size - 1) {
		wchar_t flag = *src++;
		if (flag <= 0x7F) {
			buffer[count++] = char(flag);
		} else if (flag < 0x7FF) {
			if (count + 2 > size) break;
			buffer[count++] = char(((flag >> 6) & 0x1F) | 0xC0);
			buffer[count++] = char((flag & 0x3F) | 0x80);
		} else if (flag <= 0xFFFF) {
			if (count + 3 > size) break;
			buffer[count++] = char(((flag >> 12) & 0x0F) | 0xE0);
			buffer[count++] = char(((flag >> 6) & 0x3F) | 0x80);
			buffer[count++] = char((flag & 0x3F) | 0x80);
		} else {
			if (count + 3 > size) break;
			buffer[count++] = char(0xEF);
			buffer[count++] = char(0xBF);
			buffer[count++] = char(0xBF);
		}
	}
	buffer[count] = 0;
	return count;
}

/**
* 字节数组转为十六进制字符串
* @param split 分割符
* @param data 字节数组
* @param size 数组长度
*/
string CStringUtil::ByteArrayToHexString(const string& split, const unsigned char* data, size_t size) {
	if (size == 0) return "";
	static const char LUT[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	std::ostringstream result;
	if (!split.empty()) {
		result << "0x";
		result.put(LUT[(data[0] & 0xF0) >> 4]);
		result.put(LUT[(data[0] & 0x0F)]);
		for (size_t i = 1; i < size; i++) {
			result << split << "0x";
			result.put(LUT[(data[i] & 0xF0) >> 4]);
			result.put(LUT[(data[i] & 0x0F)]);
		}
	} else {
		for (size_t i = 0; i < size; i++) {
			result.put(LUT[(data[i] & 0xF0) >> 4]);
			result.put(LUT[(data[i] & 0x0F)]);
		}
	}
	return result.str();
}

/**
* 生成全球唯一ID
* @return 32字节长度
*/
string CStringUtil::Guid() {
	char buffer[33] = { 0 };
#ifdef AVATAR_WINDOWS
	GUID guid;
	CoCreateGuid(&guid);
	sprintf_s(buffer, "%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
#else
	time_t _time;
	time(&_time);
	timespec current;
	clock_gettime(CLOCK_MONOTONIC, &current);
	strftime(buffer, 33, "%Y%m%d%H%M%S", localtime(&_time));
	sprintf(buffer + 14, "%06ld%08X%04X", current.tv_nsec / 1000, rand(), rand() % 0x10000);
#endif
	return buffer;
}