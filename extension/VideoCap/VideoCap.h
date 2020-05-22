#ifndef _VIDEOCAP_H_
#define _VIDEOCAP_H_

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

//! ������ͷ
DLL_EXPORT int VideoCapOpen(int device, int* width, int* height);

//! ��ȡ��Ƶ֡
DLL_EXPORT int VideoCapFrame(int device, unsigned char* data);

//! ֹͣ�豸
DLL_EXPORT int VideoCapClose(int device);

#endif