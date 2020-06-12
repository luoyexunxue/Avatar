//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CGUILABEL_H_
#define _CGUILABEL_H_
#include "CGuiEnvironment.h"

/**
* @brief GUI 文本标签
*/
class CGuiLabel: public CGuiEnvironment::CGuiElement {
public:
	//! 构造函数
	CGuiLabel(const string& name);
	//! 属性设置
	virtual bool SetAttribute(const string& name, const string& value);
	//! 获取属性
	virtual string GetAttribute(const string& name);
	//! 控件绘制
	virtual void Draw(const CRectangle& rect, unsigned char* buffer);

private:
	//! 字体名称
	string m_strFont;
	//! 字体大小
	int m_iFontSize;
	//! 标签文本
	wchar_t m_strText[256];
	//! 对齐方式
	int m_iAlignment;
	//! 文字拉伸到标签大小
	bool m_bStretchText;
	//! 背景颜色
	unsigned char m_cBackColor[4];
	//! 前景颜色
	unsigned char m_cForeColor[4];
};

#endif