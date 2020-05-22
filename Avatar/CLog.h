//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CLOG_H_
#define _CLOG_H_
#include "AvatarConfig.h"
#include <string>
#include <cstdarg>
#include <fstream>
using std::string;
using std::ofstream;

/**
* @brief ��־��
*/
class AVATAR_EXPORT CLog {
public:
	//! ��־�������
	enum Level {
		LEVEL_INFO = 1,
		LEVEL_DEBUG = 2,
		LEVEL_WARN = 3,
		LEVEL_ERROR = 4,
		LEVEL_NONE = 5
	};

public:
	//! ������־���
	static void Create(bool console, bool file, bool time = true, const string& title = "");
	//! ������־ϵͳ
	static void Destroy();

	//! ������־�������
	static void SetLevel(CLog::Level level);

	//! ���һ����Ϣ
	static void Info(const char* msg, ...);
	//! ���������Ϣ
	static void Debug(const char* msg, ...);
	//! ���������Ϣ
	static void Warn(const char* msg, ...);
	//! ���������Ϣ
	static void Error(const char* msg, ...);

	//! ����ַ�����Ϣ
	static void Message(const string& msg);

private:
	//! Ĭ��˽�й��캯��
	CLog();
	//! Ĭ��˽����������
	~CLog();

	//! ��־�������
	void Log(CLog::Level level, const char* msg, va_list args);
	//! ��Ϣ�������
	void Log(const string& msg);

	//! ���������̨
	void ConsoleOpen(const string& name);
	//! �ر��������̨
	void ConsoleClose();

	//! ������ļ�
	void FileOpen(const string& name);
	//! �ر�����ļ�
	void FileClose();

private:
	//! ��־����
	typedef struct _SLogParam {
		bool locked;
		bool console;
		bool file;
		bool printTime;
		CLog::Level logLevel;
		ofstream filestream;
	} SLogParam;

	//! ��־����
	SLogParam* m_pLogParam;
	//! ʵ��
	static CLog* m_pInstance;
};

#endif