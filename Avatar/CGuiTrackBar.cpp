//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CGuiTrackBar.h"
#include "CEngine.h"
#include "CStringUtil.h"
#include "CColor.h"
#include <cstdlib>

/**
* 构造函数，指定名称
*/
CGuiTrackBar::CGuiTrackBar(const string& name) : CGuiElement(name) {
	m_cRegion.SetValue(0, 0, 200, 24);
	m_cRegionScreen.SetValue(m_cRegion);
	m_bShowPercent = false;
	m_strFont = "default";
	m_iFontSize = 16;
	m_fValue = 0.0f;
	m_fMinValue = 0.0f;
	m_fMaxValue = 100.0f;
	m_strText[0] = 0;
	m_cBackColor[0] = 0xE2;
	m_cBackColor[1] = 0xE2;
	m_cBackColor[2] = 0xE2;
	m_cBackColor[3] = 0xA0;
	m_cForeColor[0] = 0x0D;
	m_cForeColor[1] = 0x74;
	m_cForeColor[2] = 0xD5;
	m_cForeColor[3] = 0xFF;
}

/**
* 属性设置
*/
bool CGuiTrackBar::SetAttribute(const string& name, const string& value) {
	if (name == "visible") m_bVisible = value == "true";
	else if (name == "left") m_cRegion[0] = atoi(value.c_str());
	else if (name == "top") m_cRegion[1] = atoi(value.c_str());
	else if (name == "width") m_cRegion[2] = atoi(value.c_str());
	else if (name == "height") m_cRegion[3] = atoi(value.c_str());
	else if (name == "parent") m_pParent = CGuiEnvironment::GetInstance()->GetElement(value);
	else if (name == "percent") m_bShowPercent = value == "true";
	else if (name == "font") m_strFont = value;
	else if (name == "fontSize") m_iFontSize = atoi(value.c_str());
	else if (name == "value") m_fValue = static_cast<float>(atof(value.c_str()));
	else if (name == "min") m_fMinValue = static_cast<float>(atof(value.c_str()));
	else if (name == "max") m_fMaxValue = static_cast<float>(atof(value.c_str()));
	else if (name == "backColor") {
		CColor color(value);
		color.GetValue(m_cBackColor);
	} else if (name == "foreColor") {
		CColor color(value);
		color.GetValue(m_cForeColor);
	} else return false;
	return true;
}

/**
* 获取属性
*/
string CGuiTrackBar::GetAttribute(const string& name) {
	if (name == "visible") return m_bVisible? "true": "false";
	else if (name == "left") return CStringUtil::Format("%d", m_cRegion[0]);
	else if (name == "top") return CStringUtil::Format("%d", m_cRegion[1]);
	else if (name == "width") return CStringUtil::Format("%d", m_cRegion[2]);
	else if (name == "height") return CStringUtil::Format("%d", m_cRegion[3]);
	else if (name == "parent") return m_pParent ? m_pParent->m_strName : "";
	else if (name == "percent") return m_bShowPercent ? "true" : "false";
	else if (name == "font") return m_strFont;
	else if (name == "fontSize") return CStringUtil::Format("%d", m_iFontSize);
	else if (name == "value") return CStringUtil::Format("%f", m_fValue);
	else if (name == "min") return CStringUtil::Format("%f", m_fMinValue);
	else if (name == "max") return CStringUtil::Format("%f", m_fMaxValue);
	else if (name == "backColor") {
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
void CGuiTrackBar::Draw(const CRectangle& rect, unsigned char* buffer) {
	float scale = (m_fValue - m_fMinValue) / (m_fMaxValue - m_fMinValue);
	int progress = static_cast<int>(m_cRegion.GetWidth() * scale);
	CFontManager::CTextImage* pImage = 0;
	if (m_bShowPercent) {
		string value = CStringUtil::Format("%d%%", static_cast<int>(scale * 100));
		CStringUtil::Utf8ToWideCharArray(value.c_str(), m_strText, 8);
		pImage = new CFontManager::CTextImage(m_cRegion.GetWidth(), m_cRegion.GetHeight());
		CFontManager* pFontMgr = CEngine::GetFontManager();
		pFontMgr->UseFont(m_strFont);
		pFontMgr->SetSize(m_iFontSize);
		pFontMgr->DrawText(m_strText, pImage, CFontManager::MIDDLECENTER, false);
	}
	// 填充 GUI 缓冲区
	int width = m_cRegion.GetWidth();
	int offsetX = rect.GetLeft() - m_cRegionScreen.GetLeft();
	int offsetY = rect.GetTop() - m_cRegionScreen.GetTop();
	int drawWidth = rect.GetWidth();
	int drawHeight = rect.GetHeight();
	for (int h = 0; h < drawHeight; h++) {
		for (int w = 0; w < drawWidth; w++) {
			unsigned int index = (h * drawWidth + w) << 2;
			unsigned char image = pImage ? pImage->data[(h + offsetY) * width + w + offsetX] : 0x00;
			unsigned char pixel = w + offsetX > progress ? image : 0xFF - image;
			buffer[index + 0] = (pixel * m_cForeColor[0] + (0xFF - pixel) * m_cBackColor[0]) >> 8;
			buffer[index + 1] = (pixel * m_cForeColor[1] + (0xFF - pixel) * m_cBackColor[1]) >> 8;
			buffer[index + 2] = (pixel * m_cForeColor[2] + (0xFF - pixel) * m_cBackColor[2]) >> 8;
			buffer[index + 3] = (pixel * m_cForeColor[3] + (0xFF - pixel) * m_cBackColor[3]) >> 8;
		}
	}
	if (pImage) delete pImage;
}