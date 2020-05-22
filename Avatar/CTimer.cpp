//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* ��̬��Ա��������
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
* ��ʼ����ʱ��
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
* ���ü�ʱ�������ؾ��ϴε��þ�����ʱ��
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
* ��ͣ��ʱ����ʱ
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
* ������ʱ
*/
void CTimer::Sleep(unsigned long ms) {
#ifdef AVATAR_WINDOWS
	::Sleep(ms);
#else
	usleep(ms * 1000);
#endif
}

/**
* ��ָ����ʽ��ȡ��ǰʱ���ַ���
* �� format Ϊ����Ĭ��Ϊ %Y-%m-%d %H:%M:%S
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
* ��ָ����ʽ��ȡ��ǰʱ���ַ���
* �� format Ϊ����Ĭ��Ϊ %Y-%m-%d %H:%M:%S
*/
string CTimer::GetTimeString(const char* format) {
	char buff[64];
	CTimer::GetTimeString(format, buff);
	return buff;
}