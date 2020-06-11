//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CGUIPANEL_H_
#define _CGUIPANEL_H_
#include "CGuiEnvironment.h"

/**
* @brief GUI 面板容器
*/
class CGuiPanel: public CGuiEnvironment::CGuiElement {
public:
	//! 构造函数
	CGuiPanel(const string& name);
	//! 拖动操作
	virtual bool Drag(bool release, int dx, int dy, CRectangle& region);
	//! 属性设置
	virtual bool SetAttribute(const string& name, const string& value);
	//! 获取属性
	virtual string GetAttribute(const string& name);
	//! 控件绘制
	virtual void Draw(const CRectangle& rect, unsigned char* buffer);

private:
	//! 偏移量
	int m_iCurrentOffset[2];
	//! 背景颜色
	unsigned char m_cBackColor[4];
};

#endif