/**************************************************************
* Avatar �����ļ�
* zhourf 2017-12-25
***************************************************************/

//! AVATAR �汾��
#define AVATAR_VERSION "3.0"

// -------------------------------------------------------------
// ����֧��
// -------------------------------------------------------------

//! ʹ�� VLD �����й¶��⣬VS �¿���
#define AVATAR_ENABLE_VLD

//! ʹ���������׽�� VideoCap
#define AVATAR_ENABLE_VIDEOCAP

//! ʹ����Ƶ���ſ� VideoPlay
#define AVATAR_ENABLE_VIDEOPLAY

//! ��ʾ LOGO
#define AVATAR_ENABLE_LOGO

//! ʹ������������
#define AVATAR_ENABLE_MOUSE

//! ʹ�ü����������
#define AVATAR_ENABLE_KEYBOARD

//! ʹ��������Ϸ�ֱ����ƣ�UDPЭ��
#define AVATAR_CONTROL_NETWORK

// -------------------------------------------------------------
// Ŀ��ƽ̨
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

//! ƽ̨֧��
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