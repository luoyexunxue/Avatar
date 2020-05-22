//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* ���캯����ָ��ѹ����·���ͽ�ѹ����
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
* ��������
*/
CZipReader::~CZipReader() {
	delete m_pFileInfo;
	if (m_pZipFile) {
		unzClose(m_pZipFile);
	}
}

/**
* ��ѹ�����е�ָ���ļ�
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
* �ر��ϴδ򿪵��ļ�
*/
void CZipReader::Close() {
	memset(m_pFileInfo, 0, sizeof(unz_file_info64));
	if (m_pZipFile) {
		unzCloseCurrentFile(m_pZipFile);
	}
}

/**
* ��ȡ��ǰ�򿪵��ļ���С
*/
unsigned int CZipReader::Size() {
	if (m_pZipFile) {
		return (unsigned int)m_pFileInfo->uncompressed_size;
	}
	return 0;
}

/**
* ��ȡ��ǰ�򿪵��ļ�
*/
unsigned int CZipReader::Read(unsigned char* buff, unsigned int size) {
	if (m_pZipFile) {
		return (unsigned int)unzReadCurrentFile(m_pZipFile, buff, size);
	}
	return 0;
}