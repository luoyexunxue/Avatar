//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CGUILISTVIEW_H_
#define _CGUILISTVIEW_H_
#include "CGuiEnvironment.h"
#include "CFontManager.h"
#include <vector>
using std::vector;

/**
* @brief GUI 列表视图
*/
class CGuiListView: public CGuiEnvironment::CGuiElement {
public:
	//! 构造函数
	CGuiListView(const string& name);
	//! 点击事件
	virtual bool OnClick(int x, int y);
	//! 拖动操作
	virtual bool OnDrag(bool release, int dx, int dy, CRectangle& region);
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
	//! 背景颜色
	unsigned char m_cBackColor[4];
	//! 前景颜色
	unsigned char m_cForeColor[4];
	//! 遮挡颜色
	unsigned char m_cFadeColor[4];
	//! 列表项文本
	vector<wchar_t> m_vecItems;
	//! 列表项索引
	vector<size_t> m_vecItemsIndex;
	//! 对齐方式
	CFontManager::Alignment m_iAlignment;
	//! 是否显示分割线
	bool m_bSplitLine;
	//! 背景图片高度
	int m_iItemHeight;
	//! Y方向偏移量
	int m_iCurrentOffset;
	//! 选中的列表项
	int m_iSelectedIndex;
};

#endif