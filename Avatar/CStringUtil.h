//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSTRINGUTIL_H_
#define _CSTRINGUTIL_H_
#include "AvatarConfig.h"
#include <string>
#include <vector>
using std::string;
using std::vector;

/**
* @brief 字符串实用工具类
*/
class AVATAR_EXPORT CStringUtil {
public:
	//! 格式化字符串
	static string Format(const char* format, ...);

	//! 转换为小写
	static string LowerCase(const string& str);
	//! 转换为大写
	static string UpperCase(const string& str);

	//! 去除左右空白
	static string Trim(const string& str);
	//! 去除左边空白
	static string TrimLeft(const string& str);
	//! 去除右边空白
	static string TrimRight(const string& str);

	//! 替换指定字符串
	static string Replace(const string& str, const char* from, const char* to);
	//! 移除指定字符串
	static string Remove(const string& str, const char* key);

	//! 按指定的字符串分割
	static int Split(vector<string>& output, const string& str, const char* key, bool trim);
	//! 按指定的字符串分割
	static int Split(string output[], int size, const string& str, const char* key, bool trim);
	//! 使用指定的分隔符连接字符串数组
	static string Join(const vector<string>& input, const char* key);
	//! 使用指定的分隔符连接字符串数组
	static string Join(const string input[], int size, const char* key);

	//! 是否以指定字符串开头
	static bool StartWith(const string& str, const char* key, bool ignoreCase = false);
	//! 是否以指定字符串结尾
	static bool EndWith(const string& str, const char* key, bool ignoreCase = false);

	//! UTF8 字符串转换为 wchar_t 数组
	static void Utf8ToWideCharArray(const char* src, wchar_t* buffer, int size);
	//! wchar_t 数组转换为 UTF8 字符串
	static void WideCharArrayToUtf8(const wchar_t* src, char* buffer, int size);
	//! 字节数组转为十六进制字符串
	static string ByteArrayToHexString(const string& split, const unsigned char* data, int size);

	//! 生成全球唯一ID
	static string Guid();
};

#endif