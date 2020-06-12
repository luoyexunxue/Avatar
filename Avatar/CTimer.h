//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CTIMER_H_
#define _CTIMER_H_
#include "AvatarConfig.h"
#include <string>
using std::string;

/**
* @brief 定时器类
*/
class AVATAR_EXPORT CTimer {
public:
	//! 初始化定时器
	static void Init();
	//! 重置计时
	static float Reset(const string& name, bool reset = true);
	//! 暂停计时器
	static void Pause(bool pause);

	//! 毫秒延时
	static void Sleep(unsigned long ms);
	//! 按指定格式获取当前时间字符串
	static void GetTimeString(const char* format, char* buffer);
	//! 按指定格式获取当前时间字符串
	static string GetTimeString(const char* format);
};

#endif