//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CDeviceAndroid.h"
#include "AvatarConfig.h"

#ifdef AVATAR_ANDROID
#include "CEngine.h"
#include "CLog.h"

/**
* ���캯��
*/
CDeviceAndroid::CDeviceAndroid() {
}

/**
* ��������
*/
CDeviceAndroid::~CDeviceAndroid() {
	m_pInstance = 0;
}

/**
* ����ʵ��
*/
CDeviceAndroid* CDeviceAndroid::m_pInstance = 0;

/**
* ��������
*/
bool CDeviceAndroid::Create(const string& title, int width, int height, bool resizable, bool fullscreen, bool antialias) {
	CEngine::GetGraphicsManager()->SetWindowSize(width, height);
	return true;
}

/**
* ʵ������
*/
void CDeviceAndroid::Destroy() {
	delete this;
}

/**
* ���ƴ���
*/
void CDeviceAndroid::Render() {
}

/**
* ��Ϣ����
*/
void CDeviceAndroid::Handle(float dt) {
}

#endif