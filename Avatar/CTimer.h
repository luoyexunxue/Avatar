//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CTIMER_H_
#define _CTIMER_H_
#include "AvatarConfig.h"
#include <string>
using std::string;

/**
* @brief ��ʱ����
*/
class AVATAR_EXPORT CTimer {
public:
	//! ��ʼ����ʱ��
	static void Init();
	//! ���ü�ʱ
	static float Reset(const string& name, bool reset = true);
	//! ��ͣ��ʱ��
	static void Pause(bool pause);

	//! ������ʱ
	static void Sleep(unsigned long ms);
	//! ��ָ����ʽ��ȡ��ǰʱ���ַ���
	static void GetTimeString(const char* format, char* buffer);
	//! ��ָ����ʽ��ȡ��ǰʱ���ַ���
	static string GetTimeString(const char* format);
};

#endif