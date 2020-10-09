//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CZipReader.h"
#include "CLog.h"
#include "AvatarConfig.h"
#include "thirdparty/zlib/zlib.h"
#include "thirdparty/zlib/unzip.h"
#include "thirdparty/zlib/unzip.c"
#include "thirdparty/zlib/ioapi.c"
#include <cstring>

#ifdef AVATAR_WINDOWS
#pragma comment(lib, "thirdparty/zlib/zlib.lib")
#endif

/**
* 构造函数，指定压缩包路径和解压密码
*/
CZipReader::CZipReader(const string& path, const string& password) {
	m_strPassword = password;
	m_pFileInfo = new unz_file_info64();
	m_pZipFile = unzOpen64(path.c_str());
	if (!m_pZipFile) {
		CLog::Error("Open zip file '%s' error", path.c_str());
	}
}

/**
* 析构函数
*/
CZipReader::~CZipReader() {
	delete m_pFileInfo;
	if (m_pZipFile) {
		unzClose(m_pZipFile);
	}
}

/**
* 打开压缩包中的指定文件
*/
bool CZipReader::Open(const string& file) {
	memset(m_pFileInfo, 0, sizeof(unz_file_info64));
	if (!m_pZipFile) return false;
	if (unzLocateFile(m_pZipFile, file.c_str(), 0) != UNZ_OK) return false;
	if (unzGetCurrentFileInfo64(m_pZipFile, m_pFileInfo, 0, 0, 0, 0, 0, 0) == UNZ_OK) {
		const char* password = 0;
		if (!m_strPassword.empty()) {
			password = m_strPassword.c_str();
		}
		if (unzOpenCurrentFilePassword(m_pZipFile, password) == UNZ_OK) {
			return true;
		}
	}
	return false;
}

/**
* 关闭上次打开的文件
*/
void CZipReader::Close() {
	memset(m_pFileInfo, 0, sizeof(unz_file_info64));
	if (m_pZipFile) {
		unzCloseCurrentFile(m_pZipFile);
	}
}

/**
* 获取当前打开的文件大小
*/
size_t CZipReader::Size() {
	if (m_pZipFile) {
		return (size_t)m_pFileInfo->uncompressed_size;
	}
	return 0;
}

/**
* 读取当前打开的文件
*/
size_t CZipReader::Read(unsigned char* buff, size_t size) {
	if (m_pZipFile) {
		return (size_t)unzReadCurrentFile(m_pZipFile, buff, (unsigned int)size);
	}
	return 0;
}