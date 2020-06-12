//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CDeviceAndroid.h"
#include "AvatarConfig.h"

#ifdef AVATAR_ANDROID
#include "CEngine.h"
#include "CLog.h"

/**
* 构造函数
*/
CDeviceAndroid::CDeviceAndroid() {
}

/**
* 析构函数
*/
CDeviceAndroid::~CDeviceAndroid() {
	m_pInstance = 0;
}

/**
* 单例实例
*/
CDeviceAndroid* CDeviceAndroid::m_pInstance = 0;

/**
* 创建窗口
*/
bool CDeviceAndroid::Create(const string& title, int width, int height, bool resizable, bool fullscreen, bool antialias) {
	CEngine::GetGraphicsManager()->SetWindowSize(width, height);
	return true;
}

/**
* 实例销毁
*/
void CDeviceAndroid::Destroy() {
	delete this;
}

/**
* 绘制窗口
*/
void CDeviceAndroid::Render() {
}

/**
* 消息处理
*/
void CDeviceAndroid::Handle(float dt) {
}

#endif