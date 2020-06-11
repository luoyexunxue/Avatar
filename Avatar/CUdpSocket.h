//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CUDPSOCKET_H_
#define _CUDPSOCKET_H_
#include "AvatarConfig.h"
#include <vector>
#include <string>
#ifdef AVATAR_WINDOWS
#include <winsock2.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#endif
using std::vector;
using std::string;

/**
* @brief UDP 通信类
*/
class AVATAR_EXPORT CUdpSocket {
public:
	//! 默认构造函数
	CUdpSocket();
	//! 默认析构函数
	~CUdpSocket();

	//! 打开连接，ip 为空为服务器模式
	bool Open(int port, const char* ip = 0);
	//! 关闭连接
	void Close();

public:
	//! 返回实际发送的字节数
	int SendData(const void* data, int size);
	//! 向指定地址端口发数据，返回实际发送的字节数
	int SendData(const void* data, int size, const string& ip, int port);
	//! 返回实际接收的字节数
	int RecvData(void* data, int size);

	//! 获取连接的端口
	int GetPort();
	//! 获取客户端的IP地址
	string GetClientIP();
	//! 使能阻塞模式
	void EnableBlockmode(bool enable);
	//! 使能广播发送
	void EnableBroadcast(bool enable);
	//! 获取广播地址列表
	void GetBroadcastAddrList(vector<string>& list);

private:
	bool m_bOpened;
	int m_iPort;
	int m_iSocket;
	sockaddr_in m_sLocalAddr;
	sockaddr_in m_sRemoteAddr;
	//! 记录引用次数
	static int m_iRefCount;
};

#endif