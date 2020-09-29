//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CLog.h"
#include "AvatarConfig.h"
#include "CTimer.h"
#include <iostream>
#include <cstring>
#ifdef AVATAR_WINDOWS
#include <windows.h>
#endif
#ifdef AVATAR_ANDROID
#include <android/log.h>
#endif

/**
* 构造函数
*/
CLog::CLog() {
	m_pLogParam = new SLogParam();
	m_pLogParam->console = false;
	m_pLogParam->file = false;
	m_pLogParam->printTime = false;
	m_pLogParam->logLevel = CLog::LEVEL_INFO;
}

/**
* 析构函数
*/
CLog::~CLog() {
	if (m_pLogParam->console) ConsoleClose();
	if (m_pLogParam->file) FileClose();
	delete m_pLogParam;
}

/**
* 单例实例
*/
CLog* CLog::m_pInstance = 0;

/**
* 创建日志输出
*/
void CLog::Create(bool console, bool file, bool time, const string& title) {
	if (m_pInstance) {
		delete m_pInstance;
	}
	m_pInstance = new CLog();
	m_pInstance->m_pLogParam->printTime = time;
	if (console) m_pInstance->ConsoleOpen(title.empty() ? CTimer::GetTimeString("LOG-%Y-%m-%d") : title);
	if (file) m_pInstance->FileOpen(title.empty() ? CTimer::GetTimeString("LOG-%Y-%m-%d.txt") : title);
}

/**
* 销毁日志系统
*/
void CLog::Destroy() {
	if (m_pInstance) {
		delete m_pInstance;
		m_pInstance = 0;
	}
}

/**
* 设置日志输出级别
*/
void CLog::SetLevel(CLog::Level level) {
	if (m_pInstance) {
		m_pInstance->m_pLogParam->logLevel = level;
	}
}

/**
* 输出一般信息
*/
void CLog::Info(const char* format, ...) {
	if (m_pInstance && m_pInstance->m_pLogParam->logLevel <= CLog::LEVEL_INFO) {
		va_list args;
		va_start(args, format);
		m_pInstance->Log(CLog::LEVEL_INFO, format, args);
		va_end(args);
	}
}

/**
* 输出调试信息
*/
void CLog::Debug(const char* format, ...) {
	if (m_pInstance && m_pInstance->m_pLogParam->logLevel <= CLog::LEVEL_DEBUG) {
		va_list args;
		va_start(args, format);
		m_pInstance->Log(CLog::LEVEL_DEBUG, format, args);
		va_end(args);
	}
}

/**
* 输出警告信息
*/
void CLog::Warn(const char* format, ...) {
	if (m_pInstance && m_pInstance->m_pLogParam->logLevel <= CLog::LEVEL_WARN) {
		va_list args;
		va_start(args, format);
		m_pInstance->Log(CLog::LEVEL_WARN, format, args);
		va_end(args);
	}
}

/**
* 输出错误信息
*/
void CLog::Error(const char* format, ...) {
	if (m_pInstance && m_pInstance->m_pLogParam->logLevel <= CLog::LEVEL_ERROR) {
		va_list args;
		va_start(args, format);
		m_pInstance->Log(CLog::LEVEL_ERROR, format, args);
		va_end(args);
	}
}

/**
* 输出字符串信息
*/
void CLog::Message(const string& msg) {
	if (m_pInstance) {
		m_pInstance->Log(msg);
	}
}

/**
* 日志输出方法
*/
void CLog::Log(CLog::Level level, const char* format, va_list args) {
	char header[64] = { 0 };
	if (m_pLogParam->printTime) {
		CTimer::GetTimeString("[%Y-%m-%d %H:%M:%S] ", header, 60);
		switch (level) {
		case CLog::LEVEL_INFO: strncat(header, "I ", 2); break;
		case CLog::LEVEL_DEBUG: strncat(header, "D ", 2); break;
		case CLog::LEVEL_WARN: strncat(header, "W ", 2); break;
		case CLog::LEVEL_ERROR: strncat(header, "E ", 2); break;
		default: strncat(header, "- ", 2); break;
		}
	}
	m_pLogParam->lock.lock();
#ifdef AVATAR_WINDOWS
	WORD color = FOREGROUND_GREEN;
	if (level == CLog::LEVEL_WARN || level == CLog::LEVEL_ERROR) color = FOREGROUND_RED;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
#endif
	if (m_pLogParam->console) {
		printf("%s", header);
		vprintf(format, args);
		printf("\n");
	}
	if (m_pLogParam->file) {
		int length = vsnprintf(0, 0, format, args) + 1;
		char* content = new char[length];
		vsnprintf(content, length, format, args);
		m_pLogParam->filestream << header << content << std::endl;
		delete[] content;
	}
#ifdef AVATAR_ANDROID
	int priority;
	switch (level) {
	case CLog::LEVEL_INFO: priority = ANDROID_LOG_INFO; break;
	case CLog::LEVEL_DEBUG: priority = ANDROID_LOG_DEBUG; break;
	case CLog::LEVEL_WARN: priority = ANDROID_LOG_WARN; break;
	case CLog::LEVEL_ERROR: priority = ANDROID_LOG_ERROR; break;
	default: priority = ANDROID_LOG_SILENT; break;
	}
	__android_log_vprint(priority, "AVATAR", format, args);
#endif
	m_pLogParam->lock.unlock();
}

/**
* 消息输出方法
*/
void CLog::Log(const string& msg) {
	m_pLogParam->lock.lock();
	if (m_pLogParam->console) printf("%s\n", msg.c_str());
	if (m_pLogParam->file) m_pLogParam->filestream << msg << std::endl;
#ifdef AVATAR_ANDROID
	__android_log_write(ANDROID_LOG_INFO, "AVATAR", msg.c_str());
#endif
	m_pLogParam->lock.unlock();
}

/**
* 打开输出控制台
*/
void CLog::ConsoleOpen(const string& name) {
#ifdef AVATAR_WINDOWS
	AllocConsole();
	SetConsoleTitleA(name.c_str());
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
	freopen("CONIN$", "r+t", stdin);
	freopen("CONOUT$", "w+t", stdout);
#endif
	m_pLogParam->console = true;
	printf("  ____ __  __ ____  _____  ____  _____\n");
	printf(" / () \\\\ \\/ // () \\|_   _|/ () \\ | () )\n");
	printf("/__/\\__\\\\__//__/\\__\\ |_| /__/\\__\\|_|\\_\\\n");
}

/**
* 关闭输出控制台
*/
void CLog::ConsoleClose() {
	if (m_pLogParam->console) {
#ifdef AVATAR_WINDOWS
		fclose(stdin);
		fclose(stdout);
		FreeConsole();
#endif
		m_pLogParam->console = false;
	}
}

/**
* 打开输出文件
*/
void CLog::FileOpen(const string& name) {
	m_pLogParam->filestream.open(name.c_str(), std::ios::out | std::ios::app);
	m_pLogParam->file = !m_pLogParam->filestream.fail();
}

/**
* 关闭输出文件
*/
void CLog::FileClose() {
	if (m_pLogParam->file) {
		m_pLogParam->filestream.close();
		m_pLogParam->file = false;
	}
}