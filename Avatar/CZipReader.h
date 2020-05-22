//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* @brief ZIP �ļ���ȡ��
*/
class AVATAR_EXPORT CZipReader {
public:
	//! ʹ���ļ�·���ͽ�ѹ���빹���ȡ��
	CZipReader(const string& path, const string& password);
	//! Ĭ����������
	~CZipReader();

public:
	//! ��ѹ�����е�ָ���ļ�
	bool Open(const string& file);
	//! �ر��ϴδ򿪵��ļ�
	void Close();
	//! ��ȡ��ǰ�򿪵��ļ���С
	unsigned int Size();
	//! ��ȡ��ǰ�򿪵��ļ�
	unsigned int Read(unsigned char* buff, unsigned int size);

private:
	//! ��ѹ���
	unzFile m_pZipFile;
	//! �򿪵��ļ���Ϣ
	unz_file_info64* m_pFileInfo;
	//! ѹ������ѹ����
	string m_strPassword;
};

#endif