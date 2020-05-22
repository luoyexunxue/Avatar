#include "VideoCap.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <pthread.h>
#endif
#include <opencv2/opencv.hpp>
#include <ctype.h>
#include <string.h>
#include <map>
using namespace std;
using namespace cv;

#ifdef _WIN32
#pragma comment(lib, "opencv_core2413.lib")
#pragma comment(lib, "opencv_highgui2413.lib")
#pragma comment(lib, "opencv_imgproc2413.lib")
#pragma comment(lib, "opencv_video2413.lib")
#endif

/**
* �ɼ��豸����
*/
struct SCaptureContext {
	VideoCapture device;
	Mat image;
	bool exit;
#ifdef _WIN32
	HANDLE threadHandle;
#else
	pthread_t threadHandle;
#endif
};

// �ɼ��豸�б�
map<int, SCaptureContext*> CaptureContextList;
// ͼ��ɼ��߳�
#ifdef _WIN32
DWORD WINAPI ThreadWorking(LPVOID pParam);
#else
void* ThreadWorking(void* pParam);
#endif

/**
* ������ͷ
*/
DLL_EXPORT int VideoCapOpen(int device, int* width, int* height) {
	SCaptureContext* pContext = new SCaptureContext();
	pContext->device.open(device);
    if (!pContext->device.isOpened()) {
		delete pContext;
		return -1;
	}
	pContext->exit = false;
	pContext->device >> pContext->image;
	*width = pContext->image.cols;
	*height = pContext->image.rows;
#ifdef _WIN32
	pContext->threadHandle = CreateThread(NULL, 0, ThreadWorking, pContext, 0, NULL);
#else
	pthread_create(&pContext->threadHandle, 0, ThreadWorking, pContext);
#endif
	CaptureContextList.insert(pair<int, SCaptureContext*>(device, pContext));
	return 0;
}

/**
* ��ȡ��Ƶ֡
*/
DLL_EXPORT int VideoCapFrame(int device, unsigned char* data) {
	map<int, SCaptureContext*>::iterator iter = CaptureContextList.find(device);
	if (iter != CaptureContextList.end()) {
		const Mat* pImage = &iter->second->image;
		memcpy(data, pImage->data, pImage->rows * pImage->cols * 3);
		return 0;
	}
	return -1;
}

/**
* ֹͣ�豸
*/
DLL_EXPORT int VideoCapClose(int device) {
	map<int, SCaptureContext*>::iterator iter = CaptureContextList.find(device);
	if (iter != CaptureContextList.end()) {
		SCaptureContext* pContext = iter->second;
		pContext->exit = true;
	#ifdef _WIN32
		WaitForSingleObject(pContext->threadHandle, INFINITE);
	#else
		pthread_join(pContext->threadHandle, NULL);
	#endif
		pContext->device.release();
		CaptureContextList.erase(iter);
		return 0;
	}
	return -1;
}

/**
* ͼ��ɼ��߳�
*/
#ifdef _WIN32
DWORD WINAPI ThreadWorking(LPVOID pParam) {
#else
void* ThreadWorking(void* pParam) {
#endif
	Mat frame;
	SCaptureContext* pContext = static_cast<SCaptureContext*>(pParam);
	while (!pContext->exit) {
		pContext->device >> frame;
		if (frame.empty()) break;
		cvtColor(frame, pContext->image, CV_RGB2BGR);
		waitKey(10);
	}
	return 0;
}