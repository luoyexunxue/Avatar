/**************************************************************
* Avatar 配置文件
* zhourf 2017-12-25
***************************************************************/

//! AVATAR 版本号
#define AVATAR_VERSION "3.0"

// -------------------------------------------------------------
// 特性支持
// -------------------------------------------------------------

//! 使用 VLD 做类存泄露检测，VS 下可用
#define AVATAR_ENABLE_VLD

//! 使用摄像机捕捉库 VideoCap
#define AVATAR_ENABLE_VIDEOCAP

//! 使用视频播放库 VideoPlay
#define AVATAR_ENABLE_VIDEOPLAY

//! 显示 LOGO
#define AVATAR_ENABLE_LOGO

//! 使用鼠标输入控制
#define AVATAR_ENABLE_MOUSE

//! 使用键盘输入控制
#define AVATAR_ENABLE_KEYBOARD

//! 使用网络游戏手柄控制，UDP协议
#define AVATAR_CONTROL_NETWORK

// -------------------------------------------------------------
// 目标平台
// -------------------------------------------------------------
#ifdef _WIN32
#pragma warning(disable:4251)
#pragma warning(disable:4996)
#define AVATAR_WINDOWS
#elif defined __ANDROID__
#define AVATAR_ANDROID
#elif defined __linux__
#define AVATAR_LINUX
#endif

#ifndef AVATAR_EXPORT
#ifdef AVATAR_WINDOWS
#ifdef AVATAR
#ifdef _WINDLL
#define AVATAR_EXPORT __declspec(dllexport)
#else
#define AVATAR_EXPORT
#endif
#else
#define AVATAR_EXPORT __declspec(dllimport)
#endif
#else
#define AVATAR_EXPORT
#endif
#endif

//! 平台支持
#ifndef AVATAR_WINDOWS
#ifdef AVATAR_ENABLE_VIDEOCAP
#undef AVATAR_ENABLE_VIDEOCAP
#endif
#ifndef AVATAR_ANDROID
#ifdef AVATAR_ENABLE_VIDEOPLAY
#undef AVATAR_ENABLE_VIDEOPLAY
#endif
#endif
#endif