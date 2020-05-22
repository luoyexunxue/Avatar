//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CDEVICEANDROID_H_
#define _CDEVICEANDROID_H_
#include "CDevice.h"
#include "AvatarConfig.h"

#ifdef AVATAR_ANDROID

/**
* @brief Android 设备类
*/
class CDeviceAndroid: public CDevice {
public:
	//! 获取 Android 设备实例
	static CDeviceAndroid* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CDeviceAndroid();
		return m_pInstance;
	}

	//! 创建窗口
	virtual bool Create(const string& title, int width, int height, bool resizable, bool fullscreen, bool antialias);
	//! 销毁窗口
	virtual void Destroy();
	//! 绘制输出
	virtual void Render();
	//! 消息处理
	virtual void Handle(float dt);

private:
	CDeviceAndroid();
	virtual ~CDeviceAndroid();

private:
	//! 实例
	static CDeviceAndroid* m_pInstance;
};

#endif
#endif