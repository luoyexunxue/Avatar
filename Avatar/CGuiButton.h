//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CGUIBUTTON_H_
#define _CGUIBUTTON_H_
#include "CGuiEnvironment.h"

/**
* @brief GUI 按钮
*/
class CGuiButton: public CGuiEnvironment::CGuiElement {
public:
	//! 构造函数
	CGuiButton(const string& name);
	//! 析构函数
	virtual ~CGuiButton();
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
	//! 标题文本
	wchar_t m_strText[64];
	//! 文字拉伸到按钮大小
	bool m_bStretchText;
	//! 背景颜色
	unsigned char m_cBackColor[4];
	//! 前景颜色
	unsigned char m_cForeColor[4];
	//! 背景图片数据
	unsigned char* m_pImage;
	//! 背景图片宽度
	int m_iImageWidth;
	//! 背景图片高度
	int m_iImageHeight;
	//! 背景图片通道数
	int m_iImageChannels;
};

#endif