//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CTimer.h"
#include "AvatarConfig.h"
#include <ctime>
#include <map>
#ifdef AVATAR_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif
using std::map;

/**
* 静态成员变量申明
*/
static bool TimerEnabled;
static float TimerLastTime;
static float TimerOffsetTime;
static map<string, float> TimerMap;
#ifdef AVATAR_WINDOWS
static float TimerFreqencyInv;
static LARGE_INTEGER TimerStart;
#else
static timespec TimerStart;
#endif

/**
* 初始化定时器
*/
void CTimer::Init() {
	TimerEnabled = true;
	TimerLastTime = 0.0f;
	TimerOffsetTime = 0.0f;
	TimerMap.clear();
#ifdef AVATAR_WINDOWS
	LARGE_INTEGER freqency;
	QueryPerformanceFrequency(&freqency);
	TimerFreqencyInv = 1.0f / freqency.QuadPart;
	QueryPerformanceCounter(&TimerStart);
#else
	clock_gettime(CLOCK_MONOTONIC, &TimerStart);
#endif
}

/**
* 重置计时，并返回距上次调用经过的时间
*/
float CTimer::Reset(const string& name, bool reset) {
#ifdef AVATAR_WINDOWS
	LARGE_INTEGER count;
	QueryPerformanceCounter(&count);
	float current = (count.QuadPart - TimerStart.QuadPart) * TimerFreqencyInv;
#else
	struct timespec count;
	clock_gettime(CLOCK_MONOTONIC, &count);
	float current = (count.tv_sec - TimerStart.tv_sec) + (count.tv_nsec - TimerStart.tv_nsec) * 1E-9;
#endif
	if (!TimerEnabled) current = TimerLastTime;
	current -= TimerOffsetTime;
	map<string, float>::iterator iter = TimerMap.find(name);
	if (iter == TimerMap.end()) {
		TimerMap.insert(std::pair<string, float>(name, current));
		iter = TimerMap.find(name);
	}
	float diff = current - iter->second;
	if (reset) iter->second = current;
	return diff;
}

/**
* 暂停计时器计时
*/
void CTimer::Pause(bool pause) {
	if (TimerEnabled != pause) return;
	TimerEnabled = !pause;
#ifdef AVATAR_WINDOWS
	LARGE_INTEGER count;
	QueryPerformanceCounter(&count);
	float current = (count.QuadPart - TimerStart.QuadPart) * TimerFreqencyInv;
#else
	struct timespec count;
	clock_gettime(CLOCK_MONOTONIC, &count);
	float current = (count.tv_sec - TimerStart.tv_sec) + (count.tv_nsec - TimerStart.tv_nsec) * 1E-9;
#endif
	if (pause) TimerLastTime = current;
	else TimerOffsetTime += current - TimerLastTime;
}

/**
* 毫秒延时
*/
void CTimer::Sleep(unsigned long ms) {
#ifdef AVATAR_WINDOWS
	::Sleep(ms);
#else
	usleep(ms * 1000);
#endif
}

/**
* 按指定格式获取当前时间字符串
* 若 format 为空则默认为 %Y-%m-%d %H:%M:%S
*/
void CTimer::GetTimeString(const char* format, char* buffer) {
	time_t _time;
	time(&_time);

#ifdef AVATAR_WINDOWS
	tm _tm;
	tm* pTm = &_tm;
	localtime_s(pTm, &_time);
#else
	tm* pTm = localtime(&_time);
#endif
	if (!format) format = "%Y-%m-%d %H:%M:%S";
	strftime(buffer, 63, format, pTm);
}

/**
* 按指定格式获取当前时间字符串
* 若 format 为空则默认为 %Y-%m-%d %H:%M:%S
*/
string CTimer::GetTimeString(const char* format) {
	char buff[64];
	CTimer::GetTimeString(format, buff);
	return buff;
}