//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
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
	static int Encode(const unsigned char* data, int length, char* buffer, int size);
	//! 编码 Base64 文本
	static int Encode(const unsigned char* data, int length, string& buffer);
	//! 编码 Base64 字符串
	static int Encode(const vector<unsigned char>& data, char* buffer, int size);
	//! 编码 Base64 文本
	static int Encode(const vector<unsigned char>& data, string& buffer);

	//! 解码 Base64 字符串
	static int Decode(const char* data, unsigned char* buffer, int size);
	//! 解码 Base64 文本
	static int Decode(const string& data, unsigned char* buffer, int size);
	//! 解码 Base64 字符串
	static int Decode(const char* data, vector<unsigned char>& buffer);
	//! 解码 Base64 文本
	static int Decode(const string& data, vector<unsigned char>& buffer);
};

#endif