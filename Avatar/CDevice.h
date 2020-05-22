//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CDEVICE_H_
#define _CDEVICE_H_
#include <string>
using std::string;

/**
* @brief �豸������
*/
class CDevice {
public:
	//! ��������
	virtual bool Create(const string& title, int width, int height, bool resizable, bool fullscreen, bool antialias) = 0;
	//! ���ٴ���
	virtual void Destroy() = 0;
	//! �������
	virtual void Render() = 0;
	//! ��Ϣ����
	virtual void Handle(float dt) = 0;

protected:
	//! ����������
	virtual ~CDevice() {}
};

#endif