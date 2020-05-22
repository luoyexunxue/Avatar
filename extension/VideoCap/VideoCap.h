#ifndef _VIDEOCAP_H_
#define _VIDEOCAP_H_

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

//! 打开摄像头
DLL_EXPORT int VideoCapOpen(int device, int* width, int* height);

//! 获取视频帧
DLL_EXPORT int VideoCapFrame(int device, unsigned char* data);

//! 停止设备
DLL_EXPORT int VideoCapClose(int device);

#endif