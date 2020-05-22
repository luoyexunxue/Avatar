//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* @brief Linux �豸��
*/
class CDeviceLinux: public CDevice {
public:
	//! ��ȡ Linux �豸ʵ��
	static CDeviceLinux* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CDeviceLinux();
		return m_pInstance;
	}

	//! ��������
	virtual bool Create(const string& title, int width, int height, bool resizable, bool fullscreen, bool antialias);
	//! ���ٴ���
	virtual void Destroy();
	//! �������
	virtual void Render();
	//! ��Ϣ����
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

	//! �豸������
	SContext* m_pContext;
	//! ʵ��
	static CDeviceLinux* m_pInstance;
};

#endif
#endif