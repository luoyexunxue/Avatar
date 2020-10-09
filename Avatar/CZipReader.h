//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CZIPREADER_H_
#define _CZIPREADER_H_
#include "AvatarConfig.h"
#include <string>
using std::string;
typedef void* unzFile;
typedef struct unz_file_info64_s unz_file_info64;

/**
* @brief ZIP 文件读取类
*/
class AVATAR_EXPORT CZipReader {
public:
	//! 使用文件路径和解压密码构造读取器
	CZipReader(const string& path, const string& password);
	//! 默认析构函数
	~CZipReader();

public:
	//! 打开压缩包中的指定文件
	bool Open(const string& file);
	//! 关闭上次打开的文件
	void Close();
	//! 获取当前打开的文件大小
	size_t Size();
	//! 读取当前打开的文件
	size_t Read(unsigned char* buff, size_t size);

private:
	//! 解压句柄
	unzFile m_pZipFile;
	//! 打开的文件信息
	unz_file_info64* m_pFileInfo;
	//! 压缩包解压密码
	string m_strPassword;
};

#endif