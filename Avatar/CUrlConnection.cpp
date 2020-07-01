//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CUrlConnection.h"
#include "AvatarConfig.h"
#include "CLog.h"
#ifndef AVATAR_WINDOWS
#include <sys/select.h>
#endif
#include "thirdparty/libcurl/curl.h"
#include <cstring>

#ifdef AVATAR_WINDOWS
#pragma comment(lib, "thirdparty/libcurl/libcurl.lib")
#endif

/**
* CUrlConnection 库引用次数
*/
int CUrlConnection::m_iRefCount = 0;

/**
* 数据接收回调
*/
static size_t OnWrite(void* data, size_t size, size_t items, vector<unsigned char>* buffer) {
	size_t bytes = size * items;
	size_t index = buffer->size();
	buffer->resize(index + bytes);
	memcpy(&buffer->at(index), data, bytes);
	return bytes;
}

/**
* 调试信息回调
*/
static int OnDebug(CURL* curl, curl_infotype type, char* data, size_t size, void* userData) {
	switch (type) {
	case CURLINFO_TEXT: CLog::Debug("[TEXT] %s", data); break;
	case CURLINFO_HEADER_IN: CLog::Debug("[HEADER IN] %s", data); break;
	case CURLINFO_HEADER_OUT: CLog::Debug("[HEADER OUT] %s", data); break;
	case CURLINFO_DATA_IN: CLog::Debug("[DATA IN] %s", data); break;
	case CURLINFO_DATA_OUT: CLog::Debug("[DATA OUT] %s", data); break;
	default: break;
	}
	return 0;
}

/**
* 构造函数
*/
CUrlConnection::CUrlConnection() {
	m_pCachedUrl = 0;
	m_bDebugInfo = false;
	m_iTimeout = 180;
	if (0 == m_iRefCount++) {
		curl_global_init(CURL_GLOBAL_DEFAULT);
	}
}

/**
* 构造函数，输出调试信息
*/
CUrlConnection::CUrlConnection(bool debug) {
	m_pCachedUrl = 0;
	m_bDebugInfo = debug;
	m_iTimeout = 180;
	if (0 == m_iRefCount++) {
		curl_global_init(CURL_GLOBAL_DEFAULT);
	}
}

/**
* 析构函数
*/
CUrlConnection::~CUrlConnection() {
	if (m_pCachedUrl) {
		curl_easy_cleanup(m_pCachedUrl);
	}
	if (0 == --m_iRefCount) {
		curl_global_cleanup();
	}
}

/**
* 设置读取超时时间
*/
void CUrlConnection::SetTimeout(int second) {
	m_iTimeout = second;
}

/**
* 获取数据接口
*/
int CUrlConnection::Request(const string& url, string& response) {
	vector<unsigned char> buffer;
	int responseCode = Request(url, buffer);
	if (responseCode == 200) {
		response.assign((char*)&buffer[0], buffer.size());
	}
	return responseCode;
}

/**
* 获取数据接口
*/
int CUrlConnection::Request(const string& url, vector<unsigned char>& response) {
	int scheme = 0;
	if (url.find("http://") == 0) scheme = 1;
	else if (url.find("https://") == 0) scheme = 2;
	else if (url.find("ftp://") == 0) scheme = 3;
	else if (url.find("ftps://")) scheme = 4;
	if (scheme > 0) {
		if (!m_pCachedUrl) m_pCachedUrl = curl_easy_init();
		if (!m_pCachedUrl) return -1;
		if (m_bDebugInfo) {
			curl_easy_setopt(m_pCachedUrl, CURLOPT_VERBOSE, 1);
			curl_easy_setopt(m_pCachedUrl, CURLOPT_DEBUGFUNCTION, OnDebug);
		}
		curl_easy_setopt(m_pCachedUrl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(m_pCachedUrl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(m_pCachedUrl, CURLOPT_WRITEFUNCTION, OnWrite);
		curl_easy_setopt(m_pCachedUrl, CURLOPT_WRITEDATA, &response);
		curl_easy_setopt(m_pCachedUrl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(m_pCachedUrl, CURLOPT_COOKIEFILE, "");
		curl_easy_setopt(m_pCachedUrl, CURLOPT_CONNECTTIMEOUT, 30);
		curl_easy_setopt(m_pCachedUrl, CURLOPT_TIMEOUT, m_iTimeout);
		long responseCode = -1;
		if (CURLE_OK == curl_easy_perform(m_pCachedUrl)) {
			responseCode = 200;
			if (scheme == 1 || scheme == 2) {
				curl_easy_getinfo(m_pCachedUrl, CURLINFO_RESPONSE_CODE, &responseCode);
			}
		}
		return (int)responseCode;
	} else {
		CLog::Warn("Url scheme '%s' is not supported", url.c_str());
		return 0;
	}
}

/**
* 获取数据接口
*/
int CUrlConnection::Request(const string& url, const string& param, string& response) {
	vector<unsigned char> buffer;
	int responseCode = Request(url, param, buffer);
	if (responseCode == 200) {
		response.assign((char*)&buffer[0], buffer.size());
	}
	return responseCode;
}

/**
* 获取数据接口
*/
int CUrlConnection::Request(const string& url, const string& param, vector<unsigned char>& response) {
	int scheme = 0;
	if (url.find("http://") == 0) scheme = 1;
	else if (url.find("https://") == 0) scheme = 2;
	if (scheme > 0) {
		if (!m_pCachedUrl) m_pCachedUrl = curl_easy_init();
		if (!m_pCachedUrl) return -1;
		if (m_bDebugInfo) {
			curl_easy_setopt(m_pCachedUrl, CURLOPT_VERBOSE, 1);
			curl_easy_setopt(m_pCachedUrl, CURLOPT_DEBUGFUNCTION, OnDebug);
		}
		curl_easy_setopt(m_pCachedUrl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(m_pCachedUrl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(m_pCachedUrl, CURLOPT_POST, 1);
		curl_easy_setopt(m_pCachedUrl, CURLOPT_POSTFIELDS, param.c_str());
		curl_easy_setopt(m_pCachedUrl, CURLOPT_WRITEFUNCTION, OnWrite);
		curl_easy_setopt(m_pCachedUrl, CURLOPT_WRITEDATA, &response);
		curl_easy_setopt(m_pCachedUrl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(m_pCachedUrl, CURLOPT_COOKIEFILE, "");
		curl_easy_setopt(m_pCachedUrl, CURLOPT_CONNECTTIMEOUT, 30);
		curl_easy_setopt(m_pCachedUrl, CURLOPT_TIMEOUT, m_iTimeout);
		long responseCode = -1;
		if (CURLE_OK == curl_easy_perform(m_pCachedUrl)) {
			curl_easy_getinfo(m_pCachedUrl, CURLINFO_RESPONSE_CODE, &responseCode);
		}
		return (int)responseCode;
	} else {
		CLog::Warn("Url scheme '%s' is not supported", url.c_str());
		return 0;
	}
}

/**
* 单次 HTTP GET 请求
* @note 不会记录 cookie
*/
int CUrlConnection::HttpGet(const string& url, string& response) {
	vector<unsigned char> buffer;
	int responseCode = HttpGet(url, buffer);
	if (responseCode == 200) {
		response.assign((char*)&buffer[0], buffer.size());
	}
	return responseCode;
}

/**
* 单次 HTTP GET 请求
* @note 不会记录 cookie
*/
int CUrlConnection::HttpGet(const string& url, vector<unsigned char>& response) {
	CURL* curl = curl_easy_init();
	if (!curl) return -1;
	if (m_bDebugInfo) {
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWrite);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, m_iTimeout);
	long responseCode = -1;
	if (CURLE_OK == curl_easy_perform(curl)) {
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
	}
	curl_easy_cleanup(curl);
	return (int)responseCode;
}

/**
* 单次 HTTP POST 请求
* @note 不会记录 cookie
*/
int CUrlConnection::HttpPost(const string& url, const string& param, string& response) {
	vector<unsigned char> buffer;
	int responseCode = HttpPost(url, param, buffer);
	if (responseCode == 200) {
		response.assign((char*)&buffer[0], buffer.size());
	}
	return responseCode;
}

/**
* 单次 HTTP POST 请求
* @note 不会记录 cookie
*/
int CUrlConnection::HttpPost(const string& url, const string& param, vector<unsigned char>& response) {
	CURL* curl = curl_easy_init();
	if (!curl) return -1;
	if (m_bDebugInfo) {
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, param.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWrite);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, m_iTimeout);
	long responseCode = -1;
	if (CURLE_OK == curl_easy_perform(curl)) {
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
	}
	curl_easy_cleanup(curl);
	return (int)responseCode;
}

/**
* 单次 HTTP POST 请求，可自定义HTTP头部
* @note 不会记录 cookie
*/
int CUrlConnection::HttpPost(const string& url, const string& param, const map<string, string>& headers, string& response) {
	vector<unsigned char> buffer;
	int responseCode = HttpPost(url, param, headers, buffer);
	if (responseCode == 200) {
		response.assign((char*)&buffer[0], buffer.size());
	}
	return responseCode;
}

/**
* 单次 HTTP POST 请求，可自定义HTTP头部
* @note 不会记录 cookie
*/
int CUrlConnection::HttpPost(const string& url, const string& param, const map<string, string>& headers, vector<unsigned char>& response) {
	CURL* curl = curl_easy_init();
	if (!curl) return -1;
	if (m_bDebugInfo) {
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_slist* header_list = 0;
	map<string, string>::const_iterator iter = headers.begin();
	while (iter != headers.end()) {
		const string line = iter->first + ": " + iter->second;
		header_list = curl_slist_append(header_list, line.c_str());
		++iter;
	}
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, param.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWrite);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, m_iTimeout);
	if (header_list) curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
	long responseCode = -1;
	if (CURLE_OK == curl_easy_perform(curl)) {
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
	}
	curl_slist_free_all(header_list);
	curl_easy_cleanup(curl);
	return (int)responseCode;
}