//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* ��ʽ���ַ���
*/
string CStringUtil::Format(const char* format, ...) {
	char msg[1024];
	va_list args;
	va_start(args, format);
	vsnprintf(msg, 1024, format, args);
	va_end(args);
	return msg;
}

/**
* ת��ΪСд
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
* ת��Ϊ��д
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
* ȥ�����ҿհ�
*/
string CStringUtil::Trim(const string& str) {
	const char* p = str.c_str();
	int length = str.length();
	int begIndex = 0;
	int endIndex = str.length() - 1;
	while ((p[begIndex] == ' ' || p[begIndex] == '\t') && begIndex < length) begIndex++;
	while ((p[endIndex] == ' ' || p[endIndex] == '\t') && endIndex >= 0) endIndex--;
	if (endIndex < begIndex) {
		return "";
	} else {
		return str.substr(begIndex, endIndex - begIndex + 1);
	}
}

/**
* ȥ����߿հ�
*/
string CStringUtil::TrimLeft(const string& str) {
	const char* p = str.c_str();
	int length = str.length();
	int index = 0;
	while ((p[index] == ' ' || p[index] == '\t') && index < length) index++;
	return str.substr(index);
}

/**
* ȥ���ұ߿հ�
*/
string CStringUtil::TrimRight(const string& str) {
	const char* p = str.c_str();
	int index = str.length() - 1;
	while ((p[index] == ' ' || p[index] == '\t') && index >= 0) index--;
	return str.substr(0, index + 1);
}

/**
* �滻ָ���ַ���
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
* �Ƴ�ָ���ַ���
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
* ��ָ�����ַ����ָ�
* @param output �������
* @param str ���ָ��ַ���
* @param key �ָ�ؼ���
* @param trim �Ƿ�Էָ����ַ���ȥ�ո�
* @return ���طָ��ĸ���
*/
int CStringUtil::Split(vector<string>& output, const string& str, const char* key, bool trim) {
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
* ��ָ�����ַ����ָ�
* @param output �������
* @param size ��������С
* @param str ���ָ��ַ���
* @param key �ָ�ؼ���
* @param trim �Ƿ�Էָ����ַ���ȥ�ո�
* @return ���طָ��ĸ���
*/
int CStringUtil::Split(string output[], int size, const string& str, const char* key, bool trim) {
	if (str.empty() || size < 1) return 0;
	int count = 0;
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
* ʹ��ָ���ķָ��������ַ�������
* @param input ��������
* @param key �ָ�ؼ���
* @return �������Ӻ���ַ���
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
* ʹ��ָ���ķָ��������ַ�������
* @param input ��������
* @param size ���������С
* @param key �ָ�ؼ���
* @return �������Ӻ���ַ���
*/
string CStringUtil::Join(const string input[], int size, const char* key) {
	if (size <= 0) return "";
	std::ostringstream strbuff;
	strbuff << input[0];
	for (int i = 1; i < size; i++) {
		strbuff << key << input[i];
	}
	return strbuff.str();
}

/**
* UTF8 �ַ���ת��Ϊ wchar_t ����
*/
void CStringUtil::Utf8ToWideCharArray(const char* src, wchar_t* buffer, int size) {
	int count = 0;
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
}

/**
* wchar_t ����ת��Ϊ UTF8 �ַ���
*/
void CStringUtil::WideCharArrayToUtf8(const wchar_t* src, char* buffer, int size) {
	int count = 0;
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
}

/**
* �ֽ�����תΪʮ�������ַ���
* @param split �ָ��
* @param data �ֽ�����
* @param size ���鳤��
*/
string CStringUtil::ByteArrayToHexString(const string& split, const unsigned char* data, int size) {
	if (size <= 0) return "";
	static const char LUT[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	std::ostringstream result;
	if (!split.empty()) {
		result << "0x";
		result.put(LUT[(data[0] & 0xF0) >> 4]);
		result.put(LUT[(data[0] & 0x0F)]);
		for (int i = 1; i < size; i++) {
			result << split << "0x";
			result.put(LUT[(data[i] & 0xF0) >> 4]);
			result.put(LUT[(data[i] & 0x0F)]);
		}
	} else {
		for (int i = 0; i < size; i++) {
			result.put(LUT[(data[i] & 0xF0) >> 4]);
			result.put(LUT[(data[i] & 0x0F)]);
		}
	}
	return result.str();
}

/**
* ����ȫ��ΨһID
* @return 32�ֽڳ���
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
	strftime(buffer, 32, "%Y%m%d%H%M%S", localtime(&_time));
	sprintf(buffer + 14, "%06ld", current.tv_nsec / 1000);
	sprintf(buffer + 20, "%08X", rand());
#endif
	return buffer;
}