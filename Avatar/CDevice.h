//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CDEVICE_H_
#define _CDEVICE_H_
#include <string>
using std::string;

/**
* @brief 设备抽象类
*/
class CDevice {
public:
	//! 创建窗口
	virtual bool Create(const string& title, int width, int height, bool resizable, bool fullscreen, bool antialias) = 0;
	//! 销毁窗口
	virtual void Destroy() = 0;
	//! 绘制输出
	virtual void Render() = 0;
	//! 消息处理
	virtual void Handle(float dt) = 0;

protected:
	//! 虚析构函数
	virtual ~CDevice() {}
};

#endif