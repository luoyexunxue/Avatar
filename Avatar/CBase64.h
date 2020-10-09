//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CBASE64_H_
#define _CBASE64_H_
#include "AvatarConfig.h"
#include <string>
#include <vector>
using std::string;
using std::vector;

/**
* @brief Base64 编解码类
*/
class AVATAR_EXPORT CBase64 {
public:
	//! 编码 Base64 字符串
	static size_t Encode(const unsigned char* data, size_t length, char* buffer, size_t size);
	//! 编码 Base64 文本
	static size_t Encode(const unsigned char* data, size_t length, string& buffer);
	//! 编码 Base64 字符串
	static size_t Encode(const vector<unsigned char>& data, char* buffer, size_t size);
	//! 编码 Base64 文本
	static size_t Encode(const vector<unsigned char>& data, string& buffer);

	//! 解码 Base64 字符串
	static size_t Decode(const char* data, unsigned char* buffer, size_t size);
	//! 解码 Base64 文本
	static size_t Decode(const string& data, unsigned char* buffer, size_t size);
	//! 解码 Base64 字符串
	static size_t Decode(const char* data, vector<unsigned char>& buffer);
	//! 解码 Base64 文本
	static size_t Decode(const string& data, vector<unsigned char>& buffer);
};

#endif