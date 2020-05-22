//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* @brief �ַ���ʵ�ù�����
*/
class AVATAR_EXPORT CStringUtil {
public:
	//! ��ʽ���ַ���
	static string Format(const char* format, ...);

	//! ת��ΪСд
	static string LowerCase(const string& str);
	//! ת��Ϊ��д
	static string UpperCase(const string& str);

	//! ȥ�����ҿհ�
	static string Trim(const string& str);
	//! ȥ����߿հ�
	static string TrimLeft(const string& str);
	//! ȥ���ұ߿հ�
	static string TrimRight(const string& str);

	//! �滻ָ���ַ���
	static string Replace(const string& str, const char* from, const char* to);
	//! �Ƴ�ָ���ַ���
	static string Remove(const string& str, const char* key);

	//! ��ָ�����ַ����ָ�
	static int Split(vector<string>& output, const string& str, const char* key, bool trim);
	//! ��ָ�����ַ����ָ�
	static int Split(string output[], int size, const string& str, const char* key, bool trim);
	//! ʹ��ָ���ķָ��������ַ�������
	static string Join(const vector<string>& input, const char* key);
	//! ʹ��ָ���ķָ��������ַ�������
	static string Join(const string input[], int size, const char* key);

	//! UTF8 �ַ���ת��Ϊ wchar_t ����
	static void Utf8ToWideCharArray(const char* src, wchar_t* buffer, int size);
	//! wchar_t ����ת��Ϊ UTF8 �ַ���
	static void WideCharArrayToUtf8(const wchar_t* src, char* buffer, int size);
	//! �ֽ�����תΪʮ�������ַ���
	static string ByteArrayToHexString(const string& split, const unsigned char* data, int size);

	//! ����ȫ��ΨһID
	static string Guid();
};

#endif