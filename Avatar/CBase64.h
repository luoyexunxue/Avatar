//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* @brief Base64 �������
*/
class AVATAR_EXPORT CBase64 {
public:
	//! ���� Base64 �ַ���
	static int Encode(const unsigned char* data, int length, char* buffer, int size);
	//! ���� Base64 �ı�
	static int Encode(const unsigned char* data, int length, string& buffer);
	//! ���� Base64 �ַ���
	static int Encode(const vector<unsigned char>& data, char* buffer, int size);
	//! ���� Base64 �ı�
	static int Encode(const vector<unsigned char>& data, string& buffer);

	//! ���� Base64 �ַ���
	static int Decode(const char* data, unsigned char* buffer, int size);
	//! ���� Base64 �ı�
	static int Decode(const string& data, unsigned char* buffer, int size);
	//! ���� Base64 �ַ���
	static int Decode(const char* data, vector<unsigned char>& buffer);
	//! ���� Base64 �ı�
	static int Decode(const string& data, vector<unsigned char>& buffer);
};

#endif