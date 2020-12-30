//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CUdpSocket.h"
#include "AvatarConfig.h"
#include <cstring>

#ifdef AVATAR_WINDOWS
#include <ws2ipdef.h>
#include <WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
typedef int socklen_t;
#else
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#endif

/**
* UDP 库引用次数
*/
int CUdpSocket::m_iRefCount = 0;

/**
* 构造函数
*/
CUdpSocket::CUdpSocket() {
	m_iSocket = -1;
	m_iPort = 0;
	m_bOpened = false;
#ifdef AVATAR_WINDOWS
	if (0 == m_iRefCount++) {
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
	}
#endif
}

/**
* 析构函数
*/
CUdpSocket::~CUdpSocket() {
	Close();
#ifdef AVATAR_WINDOWS
	if (0 == --m_iRefCount) {
		WSACleanup();
	}
#endif
}

/**
* 打开连接
*/
bool CUdpSocket::Open(int port, const char* ip) {
	m_iPort = port;
	m_sLocalAddr.sin_family = AF_INET;
	m_sRemoteAddr.sin_family = AF_INET;
	m_iSocket = (int)socket(AF_INET, SOCK_DGRAM, 0);
	// 若 ip 为空，则为服务器模式
	if (ip) {
		m_sRemoteAddr.sin_port = htons(port);
		inet_pton(AF_INET, ip, &m_sRemoteAddr.sin_addr);
	} else {
		m_sRemoteAddr.sin_port = htons(0);
		m_sLocalAddr.sin_port = htons(port);
		m_sLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		bind(m_iSocket, (sockaddr*)&m_sLocalAddr, sizeof(m_sLocalAddr));
	}
	m_bOpened = true;
	return true;
}

/**
* 关闭连接
*/
void CUdpSocket::Close() {
	if (m_bOpened) {
		m_bOpened = false;
#ifdef AVATAR_WINDOWS
		closesocket(m_iSocket);
#else
		shutdown(m_iSocket, SHUT_RDWR);
		close(m_iSocket);
#endif
	}
}

/**
* 发送字节消息
*/
int CUdpSocket::SendData(const void* data, int size) {
	// 如果无目标地址，则直接返回
	if (ntohs(m_sRemoteAddr.sin_port) == 0) return 0;
	socklen_t destlen = sizeof(m_sRemoteAddr);
	return sendto(m_iSocket, (const char*)data, size, 0, (sockaddr*)&m_sRemoteAddr, destlen);
}

/**
* 向指定地址端口发数据，返回实际发送的字节数
*/
int CUdpSocket::SendData(const void* data, int size, const string& ip, int port) {
	sockaddr_in remoteAddr;
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &remoteAddr.sin_addr);
	return sendto(m_iSocket, (const char*)data, size, 0, (sockaddr*)&remoteAddr, sizeof(remoteAddr));
}

/**
* 接收字节消息
*/
int CUdpSocket::RecvData(void* data, int size) {
	socklen_t destlen = sizeof(m_sRemoteAddr);
	return recvfrom(m_iSocket, (char*)data, size, 0, (sockaddr*)&m_sRemoteAddr, &destlen);
}

/**
* 获取打开的端口
*/
int CUdpSocket::GetPort() {
	return m_iPort;
}

/**
* 获取客户端 IP 地址
*/
string CUdpSocket::GetClientIP() {
	char buffer[32] = { 0 };
	inet_ntop(AF_INET, &m_sRemoteAddr.sin_addr, buffer, 32);
	return string(buffer);
}

/**
* 使能阻塞模式
*/
void CUdpSocket::EnableBlockmode(bool enable) {
	int flag = 0;
#ifdef AVATAR_WINDOWS
	if (!enable) flag = 1;
	ioctlsocket(m_iSocket, FIONBIO, (u_long*)&flag);
#else
	if (!enable) flag = O_NONBLOCK;
	fcntl(m_iSocket, F_SETFL, flag);
#endif
}

/**
* 使能广播发送
*/
void CUdpSocket::EnableBroadcast(bool enable) {
	int optval = 0;
	if (enable) optval = 1;
	setsockopt(m_iSocket, SOL_SOCKET, SO_BROADCAST, (const char*)&optval, sizeof(optval));
}

/**
* 获取广播地址列表
*/
void CUdpSocket::GetBroadcastAddrList(vector<string>& list) {
	list.clear();
#ifdef AVATAR_WINDOWS
	INTERFACE_INFO infos[10];
	DWORD nByteReturned;
	WSAIoctl(m_iSocket, SIO_GET_INTERFACE_LIST, NULL, 0, infos, sizeof(INTERFACE_INFO)* 10, &nByteReturned, NULL, NULL);
	int nInterface = nByteReturned / sizeof(INTERFACE_INFO);
	for (int i = 0; i < nInterface; i++) {
		sockaddr_in ipAddr = infos[i].iiAddress.AddressIn;
		sockaddr_in netMask = infos[i].iiNetmask.AddressIn;
		in_addr broadcastAddr;
		broadcastAddr.s_addr = ipAddr.sin_addr.s_addr & netMask.sin_addr.s_addr;
		broadcastAddr.s_addr |= ~netMask.sin_addr.s_addr;
		list.push_back(inet_ntoa(broadcastAddr));
	}
#else
	// 获取所有网卡
	char buf[2048];
	struct ifconf ifc;
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = buf;
	if (ioctl(m_iSocket, SIOCGIFCONF, &ifc) == -1) {
		return;
	}
	struct ifreq ifr;
	struct ifreq* it = ifc.ifc_req;
	const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));
	for (; it != end; ++it) {
		strcpy(ifr.ifr_name, it->ifr_name);
		if (ioctl(m_iSocket, SIOCGIFFLAGS, &ifr) == 0 && !(ifr.ifr_flags & IFF_LOOPBACK)) {
			sockaddr_in ipAddr;
			sockaddr_in netMask;
			if (ioctl(m_iSocket, SIOCGIFADDR, &ifr) < 0) continue;
			memcpy(&ipAddr, &ifr.ifr_addr, sizeof(sockaddr_in));
			if (ioctl(m_iSocket, SIOCGIFNETMASK, &ifr) < 0) continue;
			memcpy(&netMask, &ifr.ifr_addr, sizeof(sockaddr_in));
			in_addr broadcastAddr;
			broadcastAddr.s_addr = ipAddr.sin_addr.s_addr & netMask.sin_addr.s_addr;
			broadcastAddr.s_addr |= ~netMask.sin_addr.s_addr;
			list.push_back(inet_ntoa(broadcastAddr));
		}
	}
#endif
}