//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CGuiEditBox.h"
#include "CEngine.h"
#include "CTimer.h"
#include "CStringUtil.h"
#include "CColor.h"
#include <cstdlib>
#include <cstring>

/**
* 构造函数，指定名称
*/
CGuiEditBox::CGuiEditBox(const string& name) : CGuiElement(name) {
	m_cRegion.SetValue(0, 0, 64, 32);
	m_cRegionScreen.SetValue(m_cRegion);
	m_strFont = "default";
	m_iFontSize = 16;
	m_strText[0] = 0;
	m_iAlignment = CFontManager::MIDDLELEFT;
	m_fTimeElapsed = 0.0f;
	m_bShowCursor = false;
	m_cBackColor[0] = 0xFF;
	m_cBackColor[1] = 0xFF;
	m_cBackColor[2] = 0xFF;
	m_cBackColor[3] = 0xFF;
	m_cForeColor[0] = 0x00;
	m_cForeColor[1] = 0x00;
	m_cForeColor[2] = 0x00;
	m_cForeColor[3] = 0xFF;
}

/**
* 是否重绘
*/
bool CGuiEditBox::Redraw() {
	m_fTimeElapsed += CTimer::Reset("editbox");
	if (m_fTimeElapsed > 0.5f) {
		m_fTimeElapsed = 0.0f;
		m_bShowCursor = !m_bShowCursor;
		return true;
	}
	return false;
}

/**
* 属性设置
*/
bool CGuiEditBox::SetAttribute(const string& name, const string& value) {
	if (name == "visible") m_bVisible = value == "true";
	else if (name == "left") m_cRegion[0] = atoi(value.c_str());
	else if (name == "top") m_cRegion[1] = atoi(value.c_str());
	else if (name == "width") m_cRegion[2] = atoi(value.c_str());
	else if (name == "height") m_cRegion[3] = atoi(value.c_str());
	else if (name == "parent") m_pParent = CGuiEnvironment::GetInstance()->GetElement(value);
	else if (name == "font") m_strFont = value;
	else if (name == "fontSize") m_iFontSize = atoi(value.c_str());
	else if (name == "text") CStringUtil::Utf8ToWideCharArray(value.c_str(), m_strText, 256);
	else if (name == "align") {
		if (value == "left") m_iAlignment = CFontManager::MIDDLELEFT;
		else if (value == "center") m_iAlignment = CFontManager::MIDDLECENTER;
		else if (value == "right") m_iAlignment = CFontManager::MIDDLERIGHT;
	} else if (name == "backColor") {
		CColor color(value);
		color.GetValue(m_cBackColor);
	} else if (name == "foreColor") {
		CColor color(value);
		color.GetValue(m_cForeColor);
	} else if (name == "__key") {
		int key = atoi(value.c_str());
		wchar_t ch = *(wchar_t*)&key;
		int length = wcslen(m_strText);
		if (ch < 0x20) {
			if (ch == 0x08 && length > 0) m_strText[length - 1] = 0;
		} else if (length < 255) {
			m_strText[length] = ch;
			m_strText[length + 1] = 0;
		}
	} else return false;
	return true;
}

/**
* 获取属性
*/
string CGuiEditBox::GetAttribute(const string& name) {
	if (name == "visible") return m_bVisible? "true": "false";
	else if (name == "left") return CStringUtil::Format("%d", m_cRegion[0]);
	else if (name == "top") return CStringUtil::Format("%d", m_cRegion[1]);
	else if (name == "width") return CStringUtil::Format("%d", m_cRegion[2]);
	else if (name == "height") return CStringUtil::Format("%d", m_cRegion[3]);
	else if (name == "parent") return m_pParent ? m_pParent->m_strName : "";
	else if (name == "font") return m_strFont;
	else if (name == "fontSize") return CStringUtil::Format("%d", m_iFontSize);
	else if (name == "text") {
		char text[512] = { 0 };
		CStringUtil::WideCharArrayToUtf8(m_strText, text, 512);
		return string(text);
	} else if (name == "align") {
		if (m_iAlignment == CFontManager::MIDDLELEFT) return "left";
		else if (m_iAlignment == CFontManager::MIDDLECENTER) return "center";
		else if (m_iAlignment == CFontManager::MIDDLERIGHT) return "right";
	} else if (name == "backColor") {
		const char* fmt = "#%02X%02X%02X%02X";
		const unsigned char* val = m_cBackColor;
		return CStringUtil::Format(fmt, val[3], val[2], val[1], val[0]);
	} else if (name == "foreColor") {
		const char* fmt = "#%02X%02X%02X%02X";
		const unsigned char* val = m_cForeColor;
		return CStringUtil::Format(fmt, val[3], val[2], val[1], val[0]);
	}
	return "";
}

/**
* 控件绘制
*/
void CGuiEditBox::Draw(const CRectangle& rect, unsigned char* buffer) {
	// 绘制文本
	int width = m_cRegion.GetWidth();
	int height = m_cRegion.GetHeight();
	CFontManager::CTextImage image(width, height);
	CFontManager* pFontMgr = CEngine::GetFontManager();
	pFontMgr->UseFont(m_strFont);
	pFontMgr->SetSize(m_iFontSize);
	if (!wcslen(m_strText)) {
		const wchar_t empty[2] = { wchar_t(' '), 0 };
		pFontMgr->TextSize(empty, 0, &image.textHeight);
		memset(image.data, 0, image.width * image.height);
	} else {
		pFontMgr->DrawText(m_strText, &image, (CFontManager::Alignment)m_iAlignment, false);
	}
	// 光标位置
	if (m_bShowCursor) {
		int dx = 0;
		int dy = (image.height - image.textHeight) >> 1;
		switch (m_iAlignment) {
		case CFontManager::MIDDLELEFT: dx = image.textWidth + 1; break;
		case CFontManager::MIDDLECENTER: dx = (width + image.textWidth) >> 1; break;
		case CFontManager::MIDDLERIGHT: dx = width - 1; break;
		}
		if (dx < width) {
			for (int h = 0; h < image.textHeight; h++) {
				image.data[(h + dy) * width + dx] = 0xFF;
			}
		}
	}
	// 填充 GUI 缓冲区
	int offsetX = rect.GetLeft() - m_cRegionScreen.GetLeft();
	int offsetY = rect.GetTop() - m_cRegionScreen.GetTop();
	int drawWidth = rect.GetWidth();
	int drawHeight = rect.GetHeight();
	for (int h = 0; h < drawHeight; h++) {
		for (int w = 0; w < drawWidth; w++) {
			unsigned int index = (h * drawWidth + w) << 2;
			unsigned char pixel = image.data[(h + offsetY) * width + w + offsetX];
			buffer[index + 0] = (pixel * m_cForeColor[0] + (0xFF - pixel) * m_cBackColor[0]) >> 8;
			buffer[index + 1] = (pixel * m_cForeColor[1] + (0xFF - pixel) * m_cBackColor[1]) >> 8;
			buffer[index + 2] = (pixel * m_cForeColor[2] + (0xFF - pixel) * m_cBackColor[2]) >> 8;
			buffer[index + 3] = (pixel * m_cForeColor[3] + (0xFF - pixel) * m_cBackColor[3]) >> 8;
		}
	}
}