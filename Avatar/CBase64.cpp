//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CBase64.h"
#include <cstring>

/**
* Base64 编码查找表
*/
static const char BASE64_ENCODE_LUT[64] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

/**
* Base64 解码查找表
*/
static const unsigned char BASE64_DECODE_LUT[128] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x3F,
	0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
	0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
	0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00
};

/**
* 编码 Base64 字符串
* @param data 待编码的二进制数据
* @param length 待编码的数据长度
* @param buffer 存放编码完成的字符串缓冲区
* @param size 缓冲区 buffer 的大小
* @return 编码后的字符串长度
* @note buffer 的大小一般为 (length * 4 / 3 + 4) + 1，最后+1为字符串终止符
*/
size_t CBase64::Encode(const unsigned char* data, size_t length, char* buffer, size_t size) {
	size_t blocks = length / 3;
	size_t padding = length % 3;
	size_t codingLength = padding == 0 ? blocks * 4 : blocks * 4 + 4;
	if (codingLength + 1 > size) {
		blocks = (size - 1) >> 2;
		padding = 0;
		codingLength = blocks * 4;
	}
	for (size_t i = 0; i < blocks; i++) {
		unsigned char a = data[i * 3 + 0];
		unsigned char b = data[i * 3 + 1];
		unsigned char c = data[i * 3 + 2];
		buffer[i * 4 + 0] = BASE64_ENCODE_LUT[a >> 2];
		buffer[i * 4 + 1] = BASE64_ENCODE_LUT[((a << 4) & 0x3F) | (b >> 4)];
		buffer[i * 4 + 2] = BASE64_ENCODE_LUT[((b << 2) & 0x3F) | (c >> 6)];
		buffer[i * 4 + 3] = BASE64_ENCODE_LUT[c & 0x3F];
	}
	if (padding == 1) {
		unsigned char a = data[blocks * 3 + 0];
		buffer[blocks * 4 + 0] = BASE64_ENCODE_LUT[a >> 2];
		buffer[blocks * 4 + 1] = BASE64_ENCODE_LUT[(a << 4) & 0x3F];
		buffer[blocks * 4 + 2] = '=';
		buffer[blocks * 4 + 3] = '=';
	} else if (padding == 2) {
		unsigned char a = data[blocks * 3 + 0];
		unsigned char b = data[blocks * 3 + 1];
		buffer[blocks * 4 + 0] = BASE64_ENCODE_LUT[a >> 2];
		buffer[blocks * 4 + 1] = BASE64_ENCODE_LUT[((a << 4) & 0x3F) | (b >> 4)];
		buffer[blocks * 4 + 2] = BASE64_ENCODE_LUT[(b << 2) & 0x3F];
		buffer[blocks * 4 + 3] = '=';
	}
	buffer[codingLength] = 0;
	return codingLength;
}

/**
* 编码 Base64 文本
* @param data 待编码的二进制数据
* @param length 待编码的数据长度
* @param buffer 存放编码完成的字符串对象
* @return 编码后的字符串长度
*/
size_t CBase64::Encode(const unsigned char* data, size_t length, string& buffer) {
	size_t blocks = length / 3;
	size_t padding = length % 3;
	size_t codingLength = padding == 0 ? blocks * 4 : blocks * 4 + 4;
	char temp[5] = { 0, 0, 0, 0, 0 };
	buffer.clear();
	for (size_t i = 0; i < blocks; i++) {
		unsigned char a = data[i * 3 + 0];
		unsigned char b = data[i * 3 + 1];
		unsigned char c = data[i * 3 + 2];
		temp[0] = BASE64_ENCODE_LUT[a >> 2];
		temp[1] = BASE64_ENCODE_LUT[((a << 4) & 0x3F) | (b >> 4)];
		temp[2] = BASE64_ENCODE_LUT[((b << 2) & 0x3F) | (c >> 6)];
		temp[3] = BASE64_ENCODE_LUT[c & 0x3F];
		buffer.append(temp);
	}
	if (padding == 1) {
		unsigned char a = data[blocks * 3 + 0];
		temp[0] = BASE64_ENCODE_LUT[a >> 2];
		temp[1] = BASE64_ENCODE_LUT[(a << 4) & 0x3F];
		temp[2] = '=';
		temp[3] = '=';
		buffer.append(temp);
	} else if (padding == 2) {
		unsigned char a = data[blocks * 3 + 0];
		unsigned char b = data[blocks * 3 + 1];
		temp[0] = BASE64_ENCODE_LUT[a >> 2];
		temp[1] = BASE64_ENCODE_LUT[((a << 4) & 0x3F) | (b >> 4)];
		temp[2] = BASE64_ENCODE_LUT[(b << 2) & 0x3F];
		temp[3] = '=';
		buffer.append(temp);
	}
	return codingLength;
}

/**
* 编码 Base64 字符串
* @param data 待编码的二进制数据
* @param buffer 存放编码完成的字符串缓冲区
* @param size 缓冲区 buffer 的大小
* @return 编码后的字符串长度
* @note buffer 的大小一般为 (length * 4 / 3 + 4) + 1，最后+1为字符串终止符
*/
size_t CBase64::Encode(const vector<unsigned char>& data, char* buffer, size_t size) {
	return Encode(&data[0], data.size(), buffer, size);
}

/**
* 编码 Base64 文本
* @param data 待编码的二进制数据
* @param buffer 存放编码完成的字符串对象
* @return 编码后的字符串长度
*/
size_t CBase64::Encode(const vector<unsigned char>& data, string& buffer) {
	return Encode(&data[0], data.size(), buffer);
}

/**
* 解码 Base64 字符串
* @param data Base64 编码的字符串
* @param buffer 存放解码结果的缓冲区
* @param size 缓冲区 buffer 的大小
* @return 解码后的二进制数据长度
* @note buffer 的大小一般为 strlen(data) * 3 / 4
*/
size_t CBase64::Decode(const char* data, unsigned char* buffer, size_t size) {
	size_t length = strlen(data);
	size_t blocks = length / 4;
	size_t padding = 0;
	while (data[length - padding - 1] == '=' && padding < 3) padding++;
	if (padding != 0) {
		blocks -= 1;
		padding = 3 - padding;
	}
	size_t codingLength = blocks * 3 + padding;
	if (codingLength > size) {
		blocks = size / 3;
		padding = size % 3;
		codingLength = blocks * 3 + padding;
	}
	for (size_t i = 0; i < blocks; i++) {
		unsigned char a = BASE64_DECODE_LUT[(int)data[i * 4 + 0]];
		unsigned char b = BASE64_DECODE_LUT[(int)data[i * 4 + 1]];
		unsigned char c = BASE64_DECODE_LUT[(int)data[i * 4 + 2]];
		unsigned char d = BASE64_DECODE_LUT[(int)data[i * 4 + 3]];
		buffer[i * 3 + 0] = (unsigned char)((a << 2) | (b >> 4));
		buffer[i * 3 + 1] = (unsigned char)((b << 4) | (c >> 2));
		buffer[i * 3 + 2] = (unsigned char)((c << 6) | d);
	}
	if (padding == 1) {
		unsigned char a = BASE64_DECODE_LUT[(int)data[blocks * 4 + 0]];
		unsigned char b = BASE64_DECODE_LUT[(int)data[blocks * 4 + 1]];
		buffer[blocks * 3 + 0] = (unsigned char)((a << 2) | (b >> 4));
	} else if (padding == 2) {
		unsigned char a = BASE64_DECODE_LUT[(int)data[blocks * 4 + 0]];
		unsigned char b = BASE64_DECODE_LUT[(int)data[blocks * 4 + 1]];
		unsigned char c = BASE64_DECODE_LUT[(int)data[blocks * 4 + 2]];
		buffer[blocks * 3 + 0] = (unsigned char)((a << 2) | (b >> 4));
		buffer[blocks * 3 + 1] = (unsigned char)((b << 4) | (c >> 2));
	}
	return codingLength;
}

/**
* 解码 Base64 文本
* @param data Base64 编码的字符串
* @param buffer 存放解码结果的缓冲区
* @param size 缓冲区 buffer 的大小
* @return 解码后的二进制数据长度
* @note buffer 的大小一般为 data.length() * 3 / 4
*/
size_t CBase64::Decode(const string& data, unsigned char* buffer, size_t size) {
	return Decode(data.c_str(), buffer, size);
}

/**
* 解码 Base64 字符串
* @param data Base64 编码的字符串
* @param buffer 存放解码结果的缓冲区
* @return 解码后的二进制数据长度
*/
size_t CBase64::Decode(const char* data, vector<unsigned char>& buffer) {
	size_t length = strlen(data);
	size_t blocks = length / 4;
	size_t padding = 0;
	while (data[length - padding - 1] == '=' && padding < 3) padding++;
	if (padding != 0) {
		blocks -= 1;
		padding = 3 - padding;
	}
	size_t codingLength = blocks * 3 + padding;
	buffer.resize(codingLength);
	for (size_t i = 0; i < blocks; i++) {
		unsigned char a = BASE64_DECODE_LUT[(int)data[i * 4 + 0]];
		unsigned char b = BASE64_DECODE_LUT[(int)data[i * 4 + 1]];
		unsigned char c = BASE64_DECODE_LUT[(int)data[i * 4 + 2]];
		unsigned char d = BASE64_DECODE_LUT[(int)data[i * 4 + 3]];
		buffer[i * 3 + 0] = (unsigned char)((a << 2) | (b >> 4));
		buffer[i * 3 + 1] = (unsigned char)((b << 4) | (c >> 2));
		buffer[i * 3 + 2] = (unsigned char)((c << 6) | d);
	}
	if (padding == 1) {
		unsigned char a = BASE64_DECODE_LUT[(int)data[blocks * 4 + 0]];
		unsigned char b = BASE64_DECODE_LUT[(int)data[blocks * 4 + 1]];
		buffer[blocks * 3 + 0] = (unsigned char)((a << 2) | (b >> 4));
	} else if (padding == 2) {
		unsigned char a = BASE64_DECODE_LUT[(int)data[blocks * 4 + 0]];
		unsigned char b = BASE64_DECODE_LUT[(int)data[blocks * 4 + 1]];
		unsigned char c = BASE64_DECODE_LUT[(int)data[blocks * 4 + 2]];
		buffer[blocks * 3 + 0] = (unsigned char)((a << 2) | (b >> 4));
		buffer[blocks * 3 + 1] = (unsigned char)((b << 4) | (c >> 2));
	}
	return codingLength;
}

/**
* 解码 Base64 文本
* @param data Base64 编码的字符串
* @param buffer 存放解码结果的缓冲区
* @return 解码后的二进制数据长度
*/
size_t CBase64::Decode(const string& data, vector<unsigned char>& buffer) {
	return Decode(data.c_str(), buffer);
}