//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CLOG_H_
#define _CLOG_H_
#include "AvatarConfig.h"
#include <string>
#include <cstdarg>
#include <fstream>
#include <mutex>
using std::string;

/**
* @brief 日志类
*/
class AVATAR_EXPORT CLog {
public:
	//! 日志输出级别
	enum Level {
		LEVEL_INFO = 1,
		LEVEL_DEBUG = 2,
		LEVEL_WARN = 3,
		LEVEL_ERROR = 4,
		LEVEL_NONE = 5
	};

public:
	//! 创建日志输出
	static void Create(bool console, bool file, bool time = true, const string& title = "");
	//! 销毁日志系统
	static void Destroy();

	//! 设置日志输出级别
	static void SetLevel(CLog::Level level);

	//! 输出一般信息
	static void Info(const char* format, ...);
	//! 输出调试信息
	static void Debug(const char* format, ...);
	//! 输出警告信息
	static void Warn(const char* format, ...);
	//! 输出错误信息
	static void Error(const char* format, ...);

	//! 输出字符串信息
	static void Message(const string& msg);

private:
	//! 默认私有构造函数
	CLog();
	//! 默认私有析构函数
	~CLog();

	//! 日志输出方法
	void Log(CLog::Level level, const char* format, va_list args);
	//! 消息输出方法
	void Log(const string& msg);

	//! 打开控制台
	void ConsoleOpen(const string& name);
	//! 关闭控制台
	void ConsoleClose();
	//! 打开文件输出
	void FileOpen(const string& name);
	//! 关闭文件输出
	void FileClose();

private:
	//! 日志参数
	typedef struct _SLogParam {
		bool console;
		bool file;
		bool printTime;
		CLog::Level logLevel;
		std::mutex lock;
		std::ofstream filestream;
	} SLogParam;

	//! 日志变量
	SLogParam* m_pLogParam;
	//! 实例
	static CLog* m_pInstance;
};

#endif