//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CGuiListView.h"
#include "CEngine.h"
#include "CStringUtil.h"
#include "CColor.h"
#include <algorithm>
#include <cstring>

/**
* 构造函数，指定名称
*/
CGuiListView::CGuiListView(const string& name) : CGuiElement(name) {
	m_cRegion.SetValue(0, 0, 64, 32);
	m_cRegionScreen.SetValue(m_cRegion);
	m_strFont = "default";
	m_iFontSize = 16;
	m_iAlignment = CFontManager::MIDDLELEFT;
	m_cBackColor[0] = 0x0D;
	m_cBackColor[1] = 0x74;
	m_cBackColor[2] = 0xD5;
	m_cBackColor[3] = 0xFF;
	m_cForeColor[0] = 0xFF;
	m_cForeColor[1] = 0xFF;
	m_cForeColor[2] = 0xFF;
	m_cForeColor[3] = 0xFF;
	m_cFadeColor[0] = 0x0D;
	m_cFadeColor[1] = 0x50;
	m_cFadeColor[2] = 0xC0;
	m_cFadeColor[3] = 0xFF;
	m_bSplitLine = true;
	m_iItemHeight = 24;
	m_iCurrentOffset = 0;
	m_iSelectedIndex = -1;
}

/**
* 点击事件
*/
bool CGuiListView::OnClick(int x, int y) {
	int selectIndex = (y - m_iOffset[1]) / m_iItemHeight;
	if ((size_t)selectIndex < m_vecItemsIndex.size()) {
		m_iSelectedIndex = selectIndex;
		return true;
	}
	m_iSelectedIndex = -1;
	return false;
}

/**
* 拖动操作
*/
bool CGuiListView::OnDrag(bool release, int dx, int dy, CRectangle& region) {
	region.SetValue(m_cRegionScreen);
	int height = (int)m_vecItemsIndex.size() * m_iItemHeight;
	dy += m_iCurrentOffset;
	m_iOffset[1] = std::max(std::min(m_cRegion[3] - height, 0), std::min(dy, 0));
	if (release) m_iCurrentOffset = m_iOffset[1];
	return CGuiElement::OnDrag(release, dx, dy - m_iOffset[1], region);
}

/**
* 属性设置
*/
bool CGuiListView::SetAttribute(const string& name, const string& value) {
	if (name == "visible") m_bVisible = value == "true";
	else if (name == "left") m_cRegion[0] = atoi(value.c_str());
	else if (name == "top") m_cRegion[1] = atoi(value.c_str());
	else if (name == "width") m_cRegion[2] = atoi(value.c_str());
	else if (name == "height") m_cRegion[3] = atoi(value.c_str());
	else if (name == "parent") m_pParent = CGuiEnvironment::GetInstance()->GetElement(value);
	else if (name == "font") m_strFont = value;
	else if (name == "fontSize") m_iFontSize = atoi(value.c_str());
	else if (name == "splitLine") m_bSplitLine = value == "true";
	else if (name == "itemHeight") m_iItemHeight = atoi(value.c_str());
	else if (name == "items") {
		vector<string> items;
		CStringUtil::Split(items, value, ",", true);
		if (!items.empty() && items.back().empty()) items.pop_back();
		m_vecItems.clear();
		m_vecItemsIndex.clear();
		for (size_t i = 0; i < items.size(); i++) {
			wchar_t buffer[128];
			size_t length = CStringUtil::Utf8ToWideCharArray(items[i].c_str(), buffer, 128) + 1;
			size_t index = m_vecItems.size();
			m_vecItems.resize(index + length);
			m_vecItemsIndex.push_back(index);
			std::memcpy(&m_vecItems[index], buffer, length * sizeof(wchar_t));
		}
	} else if (name == "align") {
		if (value == "left") m_iAlignment = CFontManager::MIDDLELEFT;
		else if (value == "center") m_iAlignment = CFontManager::MIDDLECENTER;
		else if (value == "right") m_iAlignment = CFontManager::MIDDLERIGHT;
	} else if (name == "backColor") {
		CColor color(value);
		color.GetValue(m_cBackColor);
	} else if (name == "foreColor") {
		CColor color(value);
		color.GetValue(m_cForeColor);
	} else if (name == "fadeColor") {
		CColor color(value);
		color.GetValue(m_cFadeColor);
	} else return false;
	return true;
}

/**
* 获取属性
*/
string CGuiListView::GetAttribute(const string& name) {
	if (name == "visible") return m_bVisible? "true": "false";
	else if (name == "left") return CStringUtil::Format("%d", m_cRegion[0]);
	else if (name == "top") return CStringUtil::Format("%d", m_cRegion[1]);
	else if (name == "width") return CStringUtil::Format("%d", m_cRegion[2]);
	else if (name == "height") return CStringUtil::Format("%d", m_cRegion[3]);
	else if (name == "parent") return m_pParent ? m_pParent->m_strName : "";
	else if (name == "font") return m_strFont;
	else if (name == "fontSize") return CStringUtil::Format("%d", m_iFontSize);
	else if (name == "splitLine") return m_bSplitLine ? "true" : "false";
	else if (name == "itemHeight") return CStringUtil::Format("%d", m_iItemHeight);
	else if (name == "selectedIndex") return CStringUtil::Format("%d", m_iSelectedIndex);
	else if (name == "items") {
		string items;
		for (size_t i = 0; i < m_vecItemsIndex.size(); i++) {
			char buffer[512];
			CStringUtil::WideCharArrayToUtf8(&m_vecItems[m_vecItemsIndex[i]], buffer, 512);
			items.append(buffer).append(",");
		}
		if (!items.empty()) items.pop_back();
		return items;
	} else if (name == "align") {
		if (m_iAlignment == CFontManager::MIDDLELEFT) return "left";
		else if (m_iAlignment == CFontManager::MIDDLECENTER) return "center";
		else if (m_iAlignment == CFontManager::MIDDLERIGHT) return "right";
	} else if (name == "backColor") {
		const char* fmt = "#%02X%02X%02X%02X";
		return CStringUtil::Format(fmt, m_cBackColor[3], m_cBackColor[2], m_cBackColor[1], m_cBackColor[0]);
	} else if (name == "foreColor") {
		const char* fmt = "#%02X%02X%02X%02X";
		return CStringUtil::Format(fmt, m_cForeColor[3], m_cForeColor[2], m_cForeColor[1], m_cForeColor[0]);
	} else if (name == "fadeColor") {
		const char* fmt = "#%02X%02X%02X%02X";
		return CStringUtil::Format(fmt, m_cFadeColor[3], m_cFadeColor[2], m_cFadeColor[1], m_cFadeColor[0]);
	}
	return "";
}

/**
* 控件绘制
*/
void CGuiListView::Draw(const CRectangle& rect, unsigned char* buffer) {
	int width = m_cRegion.GetWidth();
	int offsetX = rect.GetLeft() - m_cRegionScreen.GetLeft();
	int offsetY = rect.GetTop() - m_cRegionScreen.GetTop() - m_iOffset[1];
	int startItem = offsetY / m_iItemHeight;
	int height = 0;
	offsetY -= startItem * m_iItemHeight;
	for (size_t i = startItem; i < m_vecItemsIndex.size(); i++) {
		CFontManager::CTextImage image(width, m_iItemHeight);
		CFontManager* pFontMgr = CEngine::GetFontManager();
		pFontMgr->UseFont(m_strFont);
		pFontMgr->SetSize(m_iFontSize);
		pFontMgr->DrawText(&m_vecItems[m_vecItemsIndex[i]], &image, m_iAlignment, false);
		int drawWidth = rect.GetWidth();
		int drawHeight = std::min(std::min(m_iItemHeight - offsetY, m_iItemHeight), rect.GetHeight() - height);
		for (int h = 0; h < drawHeight; h++) {
			for (int w = 0; w < drawWidth; w++) {
				unsigned int index = ((h + height) * drawWidth + w) << 2;
				unsigned char pixel = image.data[(h + offsetY) * width + w + offsetX];
				buffer[index + 0] = (pixel * m_cForeColor[0] + (0xFF - pixel) * m_cBackColor[0]) >> 8;
				buffer[index + 1] = (pixel * m_cForeColor[1] + (0xFF - pixel) * m_cBackColor[1]) >> 8;
				buffer[index + 2] = (pixel * m_cForeColor[2] + (0xFF - pixel) * m_cBackColor[2]) >> 8;
				buffer[index + 3] = (pixel * m_cForeColor[3] + (0xFF - pixel) * m_cBackColor[3]) >> 8;
			}
		}
		offsetY = 0;
		height += drawHeight;
		if (height >= rect.GetHeight()) break;
		// 绘制分割线
		if (m_bSplitLine && height > 0) {
			for (int w = 1; w < drawWidth - 1; w++) {
				unsigned int index = ((height - 1) * drawWidth + w) << 2;
				unsigned char pixel = 0x50;
				buffer[index + 0] = (pixel * m_cForeColor[0] + (0xFF - pixel) * m_cBackColor[0]) >> 8;
				buffer[index + 1] = (pixel * m_cForeColor[1] + (0xFF - pixel) * m_cBackColor[1]) >> 8;
				buffer[index + 2] = (pixel * m_cForeColor[2] + (0xFF - pixel) * m_cBackColor[2]) >> 8;
				buffer[index + 3] = (pixel * m_cForeColor[3] + (0xFF - pixel) * m_cBackColor[3]) >> 8;
			}
		}
	}
	// 绘制剩余背景部分
	int drawWidth = rect.GetWidth();
	int drawHeight = rect.GetHeight();
	for (int h = height; h < drawHeight; h++) {
		for (int w = 0; w < drawWidth; w++) {
			unsigned int index = (h * drawWidth + w) << 2;
			buffer[index + 0] = m_cBackColor[0];
			buffer[index + 1] = m_cBackColor[1];
			buffer[index + 2] = m_cBackColor[2];
			buffer[index + 3] = m_cBackColor[3];
		}
	}
	// 遮挡效果
	const int occlusHeight = 8;
	if (drawHeight < 2 * occlusHeight) return;
	const unsigned char gradient[8] = { 0xFF, 0xDF, 0xBF, 0x9F, 0x7F, 0x5F, 0x3F, 0x1F };
	bool occlusionTop = m_iOffset[1] < 0;
	bool occlusionBottom = m_iOffset[1] > m_cRegion.GetHeight() - (int)m_vecItemsIndex.size() * m_iItemHeight;
	if (occlusionTop) {
		for (int h = 0; h < occlusHeight; h++) {
			unsigned char pixel = gradient[h];
			for (int w = 0; w < drawWidth; w++) {
				unsigned int index = (h * drawWidth + w) << 2;
				buffer[index + 0] = (pixel * m_cFadeColor[0] + (0xFF - pixel) * buffer[index + 0]) >> 8;
				buffer[index + 1] = (pixel * m_cFadeColor[1] + (0xFF - pixel) * buffer[index + 1]) >> 8;
				buffer[index + 2] = (pixel * m_cFadeColor[2] + (0xFF - pixel) * buffer[index + 2]) >> 8;
				buffer[index + 3] = (pixel * m_cFadeColor[3] + (0xFF - pixel) * buffer[index + 3]) >> 8;
			}
		}
	}
	if (occlusionBottom) {
		for (int h = 0; h < occlusHeight; h++) {
			unsigned char pixel = gradient[h];
			for (int w = 0; w < drawWidth; w++) {
				unsigned int index = ((drawHeight - h - 1) * drawWidth + w) << 2;
				buffer[index + 0] = (pixel * m_cFadeColor[0] + (0xFF - pixel) * buffer[index + 0]) >> 8;
				buffer[index + 1] = (pixel * m_cFadeColor[1] + (0xFF - pixel) * buffer[index + 1]) >> 8;
				buffer[index + 2] = (pixel * m_cFadeColor[2] + (0xFF - pixel) * buffer[index + 2]) >> 8;
				buffer[index + 3] = (pixel * m_cFadeColor[3] + (0xFF - pixel) * buffer[index + 3]) >> 8;
			}
		}
	}
}