//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* ���캯��
*/
CLog::CLog() {
	m_pLogParam = new SLogParam();
	m_pLogParam->locked = false;
	m_pLogParam->console = false;
	m_pLogParam->file = false;
}

/**
* ��������
*/
CLog::~CLog() {
	if (m_pLogParam->console) ConsoleClose();
	if (m_pLogParam->file) FileClose();
	delete m_pLogParam;
}

/**
* ����ʵ��
*/
CLog* CLog::m_pInstance = 0;

/**
* ������־���
*/
void CLog::Create(bool console, bool file, bool time, const string& title) {
	if (m_pInstance) {
		delete m_pInstance;
	}
	m_pInstance = new CLog();
	m_pInstance->m_pLogParam->printTime = time;
	m_pInstance->m_pLogParam->logLevel = CLog::LEVEL_INFO;
	if (console) m_pInstance->ConsoleOpen(title.empty() ? CTimer::GetTimeString("LOG-%Y-%m-%d") : title);
	if (file) m_pInstance->FileOpen(title.empty() ? CTimer::GetTimeString("LOG-%Y-%m-%d.txt") : title);
}

/**
* ������־ϵͳ
*/
void CLog::Destroy() {
	if (m_pInstance) {
		delete m_pInstance;
		m_pInstance = 0;
	}
}

/**
* ������־�������
*/
void CLog::SetLevel(CLog::Level level) {
	if (m_pInstance) {
		m_pInstance->m_pLogParam->logLevel = level;
	}
}

/**
* ���һ����Ϣ
*/
void CLog::Info(const char* msg, ...) {
	if (m_pInstance && m_pInstance->m_pLogParam->logLevel <= CLog::LEVEL_INFO) {
		va_list args;
		va_start(args, msg);
		m_pInstance->Log(CLog::LEVEL_INFO, msg, args);
		va_end(args);
	}
}

/**
* ���������Ϣ
*/
void CLog::Debug(const char* msg, ...) {
	if (m_pInstance && m_pInstance->m_pLogParam->logLevel <= CLog::LEVEL_DEBUG) {
		va_list args;
		va_start(args, msg);
		m_pInstance->Log(CLog::LEVEL_DEBUG, msg, args);
		va_end(args);
	}
}

/**
* ���������Ϣ
*/
void CLog::Warn(const char* msg, ...) {
	if (m_pInstance && m_pInstance->m_pLogParam->logLevel <= CLog::LEVEL_WARN) {
		va_list args;
		va_start(args, msg);
		m_pInstance->Log(CLog::LEVEL_WARN, msg, args);
		va_end(args);
	}
}

/**
* ���������Ϣ
*/
void CLog::Error(const char* msg, ...) {
	if (m_pInstance && m_pInstance->m_pLogParam->logLevel <= CLog::LEVEL_ERROR) {
		va_list args;
		va_start(args, msg);
		m_pInstance->Log(CLog::LEVEL_ERROR, msg, args);
		va_end(args);
	}
}

/**
* ����ַ�����Ϣ
*/
void CLog::Message(const string& msg) {
	if (m_pInstance) {
		m_pInstance->Log(msg);
	}
}

/**
* ��־�������
*/
void CLog::Log(CLog::Level level, const char* msg, va_list args) {
	char buff[2048];
	int header = 0;
	if (m_pLogParam->printTime) {
		CTimer::GetTimeString("[%Y-%m-%d %H:%M:%S] ", buff);
		switch (level) {
		case CLog::LEVEL_INFO: strcat(buff, "I "); break;
		case CLog::LEVEL_DEBUG: strcat(buff, "D "); break;
		case CLog::LEVEL_WARN: strcat(buff, "W "); break;
		case CLog::LEVEL_ERROR: strcat(buff, "E "); break;
		default: strcat(buff, "- "); break;
		}
		header = strlen(buff);
	}
	vsnprintf(buff + header, 2047 - header, msg, args);
	strcat(buff, "\n");

	while (m_pLogParam->locked) CTimer::Sleep(1);
	m_pLogParam->locked = true;

#ifdef AVATAR_WINDOWS
	WORD color = FOREGROUND_GREEN;
	if (level == CLog::LEVEL_WARN || level == CLog::LEVEL_ERROR) color = FOREGROUND_RED;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
#endif
	std::cout << buff;
	if (m_pLogParam->file) {
		m_pLogParam->filestream << buff;
		m_pLogParam->filestream.flush();
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
	__android_log_write(priority, "AVATAR", buff);
#endif
	m_pLogParam->locked = false;
}

/**
* ��Ϣ�������
*/
void CLog::Log(const string& msg) {
	while (m_pLogParam->locked) CTimer::Sleep(1);
	m_pLogParam->locked = true;

	std::cout << msg << std::endl;
	if (m_pLogParam->file) {
		m_pLogParam->filestream << msg << std::endl;
		m_pLogParam->filestream.flush();
	}
#ifdef AVATAR_ANDROID
	__android_log_write(ANDROID_LOG_INFO, "AVATAR", msg.c_str());
#endif
	m_pLogParam->locked = false;
}

/**
* ���������̨
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
	std::cout << "  ____ __  __ ____  _____  ____  _____" << std::endl;
	std::cout << " / () \\\\ \\/ // () \\|_   _|/ () \\ | () )" << std::endl;
	std::cout << "/__/\\__\\\\__//__/\\__\\ |_| /__/\\__\\|_|\\_\\" << std::endl;
}

/**
* �ر��������̨
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
* ������ļ�
*/
void CLog::FileOpen(const string& name) {
	m_pLogParam->filestream.open(name.c_str(), std::ios::out | std::ios::app);
	m_pLogParam->file = !m_pLogParam->filestream.fail();
}

/**
* �ر�����ļ�
*/
void CLog::FileClose() {
	if (m_pLogParam->file) {
		m_pLogParam->filestream.close();
		m_pLogParam->file = false;
	}
}