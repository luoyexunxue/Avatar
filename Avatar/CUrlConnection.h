//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CURLCONNECTION_H_
#define _CURLCONNECTION_H_
#include "AvatarConfig.h"
#include <string>
#include <vector>
#include <map>
using std::string;
using std::vector;
using std::map;
typedef void CURL;

/**
* @brief URL ������
* @remark ֧�� HTTP/FTP/FTPS �ȶ���Э��
*/
class AVATAR_EXPORT CUrlConnection {
public:
	//! Ĭ�Ϲ��캯��
	CUrlConnection();
	//! ��������ģʽ�� URL ����
	CUrlConnection(bool debug);
	//! Ĭ����������
	~CUrlConnection();

	//! ���ö�ȡ��ʱʱ��
	void SetTimeout(int second);

	//! ��ȡ���ݽӿ�
	int Request(const string& url, string& response);
	//! ��ȡ���ݽӿ�
	int Request(const string& url, vector<unsigned char>& response);
	//! ��ȡ���ݽӿ�
	int Request(const string& url, const string& param, string& response);
	//! ��ȡ���ݽӿ�
	int Request(const string& url, const string& param, vector<unsigned char>& response);

	//! ���� HTTP GET ����
	int HttpGet(const string& url, string& response);
	//! ���� HTTP GET ����
	int HttpGet(const string& url, vector<unsigned char>& response);
	//! ���� HTTP POST ����
	int HttpPost(const string& url, const string& param, string& response);
	//! ���� HTTP POST ����
	int HttpPost(const string& url, const string& param, vector<unsigned char>& response);
	//! ���� HTTP POST ���󣬿��Զ���HTTPͷ��
	int HttpPost(const string& url, const string& param, const map<string, string>& headers, string& response);
	//! ���� HTTP POST ���󣬿��Զ���HTTPͷ��
	int HttpPost(const string& url, const string& param, const map<string, string>& headers, vector<unsigned char>& response);

private:
	//! ���������
	CURL* m_pCachedUrl;
	//! ������Ϣ����
	bool m_bDebugInfo;
	//! ��ʱʱ��
	int m_iTimeout;
	//! ��¼���ô���
	static int m_iRefCount;
};

#endif