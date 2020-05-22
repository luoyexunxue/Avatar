//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CGUITRACKBAR_H_
#define _CGUITRACKBAR_H_
#include "CGuiEnvironment.h"

/**
* @brief GUI 跟踪条
*/
class CGuiTrackBar: public CGuiEnvironment::CGuiElement {
public:
	//! 构造函数
	CGuiTrackBar(const string& name);
	//! 属性设置
	virtual bool SetAttribute(const string& name, const string& value);
	//! 获取属性
	virtual string GetAttribute(const string& name);
	//! 控件绘制
	virtual void Draw(const CRectangle& rect, unsigned char* buffer);

private:
	//! 显示百分比
	bool m_bShowPercent;
	//! 字体名称
	string m_strFont;
	//! 字体大小
	int m_iFontSize;
	//! 跟踪条当前值
	float m_fValue;
	//! 跟踪条最小值
	float m_fMinValue;
	//! 跟踪条最大值
	float m_fMaxValue;
	//! 背景颜色
	unsigned char m_cBackColor[4];
	//! 前景颜色
	unsigned char m_cForeColor[4];
	//! 百分值文本
	wchar_t m_strText[8];
};

#endif