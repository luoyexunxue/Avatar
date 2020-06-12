//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CDEVICELINUX_H_
#define _CDEVICELINUX_H_
#include "CDevice.h"
#include "AvatarConfig.h"

#ifdef AVATAR_LINUX
#include "thirdparty/glew/glew.h"
#include <GL/glx.h>

/**
* @brief Linux 设备类
*/
class CDeviceLinux: public CDevice {
public:
	//! 获取 Linux 设备实例
	static CDeviceLinux* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CDeviceLinux();
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
	CDeviceLinux();
	virtual ~CDeviceLinux();

private:
	typedef struct _SContext {
		Display* display;
		Window window;
		GLXContext context;
		_SContext(): display(0), window(0), context(0) {}
	} SContext;

	//! 设备上下文
	SContext* m_pContext;
	//! 实例
	static CDeviceLinux* m_pInstance;
};

#endif
#endif