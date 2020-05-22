//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CDEVICEWINDOWS_H_
#define _CDEVICEWINDOWS_H_
#include "CDevice.h"
#include "AvatarConfig.h"

#ifdef AVATAR_WINDOWS
#include <Windows.h>

/**
* @brief Windows �豸��
*/
class CDeviceWindows: public CDevice {
public:
	//! ��ȡ Windows �豸ʵ��
	static CDeviceWindows* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CDeviceWindows();
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
	CDeviceWindows();
	virtual ~CDeviceWindows();

private:
	typedef struct _SContext {
		bool created;
		bool changed;
		HDC hdc;
		HGLRC hrc;
		HWND hwnd;
		HINSTANCE instance;
		_SContext(): created(true), changed(false), hdc(0), hrc(0), hwnd(0) {}
	} SContext;

	//! �豸������
	SContext* m_pContext;
	//! ʵ��
	static CDeviceWindows* m_pInstance;
};

#endif
#endif