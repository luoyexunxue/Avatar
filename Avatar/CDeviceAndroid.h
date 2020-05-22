//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CDEVICEANDROID_H_
#define _CDEVICEANDROID_H_
#include "CDevice.h"
#include "AvatarConfig.h"

#ifdef AVATAR_ANDROID

/**
* @brief Android �豸��
*/
class CDeviceAndroid: public CDevice {
public:
	//! ��ȡ Android �豸ʵ��
	static CDeviceAndroid* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CDeviceAndroid();
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
	CDeviceAndroid();
	virtual ~CDeviceAndroid();

private:
	//! ʵ��
	static CDeviceAndroid* m_pInstance;
};

#endif
#endif