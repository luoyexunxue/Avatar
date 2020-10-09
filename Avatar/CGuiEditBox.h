//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CGUIEDITBOX_H_
#define _CGUIEDITBOX_H_
#include "CGuiEnvironment.h"
#include "CFontManager.h"

/**
* @brief GUI 文本输入框
*/
class CGuiEditBox: public CGuiEnvironment::CGuiElement {
public:
	//! 构造函数
	CGuiEditBox(const string& name);
	//! 是否重绘
	virtual bool Redraw();
	//! 属性设置
	virtual bool SetAttribute(const string& name, const string& value);
	//! 获取属性
	virtual string GetAttribute(const string& name);
	//! 控件绘制
	virtual void Draw(const CRectangle& rect, unsigned char* buffer);

public:
	//! 字体名称
	string m_strFont;
	//! 字体大小
	int m_iFontSize;
	//! 输入文本
	wchar_t m_strText[256];
	//! 对齐方式
	CFontManager::Alignment m_iAlignment;
	//! 背景颜色
	unsigned char m_cBackColor[4];
	//! 文本颜色
	unsigned char m_cForeColor[4];

private:
	//! 闪烁计时
	float m_fTimeElapsed;
	//! 显示光标
	bool m_bShowCursor;
};

#endif