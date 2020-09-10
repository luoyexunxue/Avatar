#ifndef _VIDEOPLAY_H_
#define _VIDEOPLAY_H_

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

//! ����Ƶ�ļ�
DLL_EXPORT int VideoPlayOpen(const char* file, int size, void** context);

//! ��ȡ����Ƶ��Ϣ
DLL_EXPORT int VideoPlayInfo(void* context, int* width, int* height, float* length, int* channel, int* sampleRate, int* sampleBits);

//! ���ò���ʱ��
DLL_EXPORT int VideoPlayTime(void* context, float time);

//! ����ָ������ʱ��
DLL_EXPORT int VideoPlaySeek(void* context, float time);

//! ��ȡ��Ƶ֡
DLL_EXPORT int VideoPlayVideo(void* context, unsigned char* data);

//! ��ȡ��Ƶ֡
DLL_EXPORT int VideoPlayAudio(void* context, unsigned char* data, int size);

//! �ر���Ƶ�ļ�
DLL_EXPORT int VideoPlayClose(void* context);

#endif