//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* @brief UDP ͨ����
*/
class AVATAR_EXPORT CUdpSocket {
public:
	//! Ĭ�Ϲ��캯��
	CUdpSocket();
	//! Ĭ����������
	~CUdpSocket();

	//! �����ӣ�ip Ϊ��Ϊ������ģʽ
	bool Open(int port, const char* ip = 0);
	//! �ر�����
	void Close();

public:
	//! ����ʵ�ʷ��͵��ֽ���
	int SendData(const void* data, int size);
	//! ��ָ����ַ�˿ڷ����ݣ�����ʵ�ʷ��͵��ֽ���
	int SendData(const void* data, int size, const string& ip, int port);
	//! ����ʵ�ʽ��յ��ֽ���
	int RecvData(void* data, int size);

	//! ��ȡ���ӵĶ˿�
	int GetPort();
	//! ��ȡ�ͻ��˵�IP��ַ
	string GetClientIP();
	//! ʹ������ģʽ
	void EnableBlockmode(bool enable);
	//! ʹ�ܹ㲥����
	void EnableBroadcast(bool enable);
	//! ��ȡ�㲥��ַ�б�
	void GetBroadcastAddrList(vector<string>& list);

private:
	bool m_bOpened;
	int m_iPort;
	int m_iSocket;
	sockaddr_in m_sLocalAddr;
	sockaddr_in m_sRemoteAddr;
	//! ��¼���ô���
	static int m_iRefCount;
};

#endif