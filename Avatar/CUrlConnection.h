//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
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
* @brief URL 连接类
* @remark 支持 HTTP/FTP/FTPS 等多种协议
*/
class AVATAR_EXPORT CUrlConnection {
public:
	//! 默认构造函数
	CUrlConnection();
	//! 创建调试模式的 URL 连接
	CUrlConnection(bool debug);
	//! 默认析构函数
	~CUrlConnection();

	//! 设置读取超时时间
	void SetTimeout(int second);

	//! 获取数据接口
	int Request(const string& url, string& response);
	//! 获取数据接口
	int Request(const string& url, vector<unsigned char>& response);
	//! 获取数据接口
	int Request(const string& url, const string& param, string& response);
	//! 获取数据接口
	int Request(const string& url, const string& param, vector<unsigned char>& response);

	//! 单次 HTTP GET 请求
	int HttpGet(const string& url, string& response);
	//! 单次 HTTP GET 请求
	int HttpGet(const string& url, vector<unsigned char>& response);
	//! 单次 HTTP POST 请求
	int HttpPost(const string& url, const string& param, string& response);
	//! 单次 HTTP POST 请求
	int HttpPost(const string& url, const string& param, vector<unsigned char>& response);
	//! 单次 HTTP POST 请求，可自定义HTTP头部
	int HttpPost(const string& url, const string& param, const map<string, string>& headers, string& response);
	//! 单次 HTTP POST 请求，可自定义HTTP头部
	int HttpPost(const string& url, const string& param, const map<string, string>& headers, vector<unsigned char>& response);

private:
	//! 缓存的连接
	CURL* m_pCachedUrl;
	//! 调试信息开关
	bool m_bDebugInfo;
	//! 超时时间
	int m_iTimeout;
	//! 记录引用次数
	static int m_iRefCount;
};

#endif