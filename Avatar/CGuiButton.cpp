//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CGuiButton.h"
#include "CEngine.h"
#include "CStringUtil.h"
#include "CColor.h"
#include <cstring>
#include <cstdlib>

/**
* 构造函数，指定名称
*/
CGuiButton::CGuiButton(const string& name) : CGuiElement(name) {
	m_cRegion.SetValue(0, 0, 64, 32);
	m_cRegionScreen.SetValue(m_cRegion);
	m_strFont = "default";
	m_iFontSize = 16;
	m_strText[0] = 0;
	m_bStretchText = false;
	m_cBackColor[0] = 0x0D;
	m_cBackColor[1] = 0x74;
	m_cBackColor[2] = 0xD5;
	m_cBackColor[3] = 0xFF;
	m_cForeColor[0] = 0xFF;
	m_cForeColor[1] = 0xFF;
	m_cForeColor[2] = 0xFF;
	m_cForeColor[3] = 0xFF;
	// 背景图片
	m_pImage = 0;
	m_iImageWidth = 0;
	m_iImageHeight = 0;
	m_iImageChannels = 0;
}

/**
* 析构函数
*/
CGuiButton::~CGuiButton() {
	if (m_pImage) delete[] m_pImage;
}

/**
* 属性设置
*/
bool CGuiButton::SetAttribute(const string& name, const string& value) {
	if (name == "visible") m_bVisible = value == "true";
	else if (name == "left") m_cRegion[0] = atoi(value.c_str());
	else if (name == "top") m_cRegion[1] = atoi(value.c_str());
	else if (name == "width") m_cRegion[2] = atoi(value.c_str());
	else if (name == "height") m_cRegion[3] = atoi(value.c_str());
	else if (name == "parent") m_pParent = CGuiEnvironment::GetInstance()->GetElement(value);
	else if (name == "font") m_strFont = value;
	else if (name == "fontSize") m_iFontSize = atoi(value.c_str());
	else if (name == "text") CStringUtil::Utf8ToWideCharArray(value.c_str(), m_strText, 64);
	else if (name == "stretch") m_bStretchText = value == "true";
	else if (name == "image") {
		CFileManager::CImageFile* pFile = CTextureManager::ReadImage(value, false);
		if (pFile) {
			if (m_pImage) delete[] m_pImage;
			m_pImage = new unsigned char[pFile->size];
			m_iImageWidth = pFile->width;
			m_iImageHeight = pFile->height;
			m_iImageChannels = pFile->channels;
			memcpy(m_pImage, pFile->contents, pFile->size);
			delete pFile;
		}
	} else if (name == "backColor") {
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
string CGuiButton::GetAttribute(const string& name) {
	if (name == "visible") return m_bVisible? "true": "false";
	else if (name == "left") return CStringUtil::Format("%d", m_cRegion[0]);
	else if (name == "top") return CStringUtil::Format("%d", m_cRegion[1]);
	else if (name == "width") return CStringUtil::Format("%d", m_cRegion[2]);
	else if (name == "height") return CStringUtil::Format("%d", m_cRegion[3]);
	else if (name == "parent") return m_pParent ? m_pParent->m_strName : "";
	else if (name == "font") return m_strFont;
	else if (name == "fontSize") return CStringUtil::Format("%d", m_iFontSize);
	else if (name == "stretch") return m_bStretchText ? "true" : "false";
	else if (name == "text") {
		char text[128] = { 0 };
		CStringUtil::WideCharArrayToUtf8(m_strText, text, 128);
		return string(text);
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
void CGuiButton::Draw(const CRectangle& rect, unsigned char* buffer) {
	int width = m_cRegion.GetWidth();
	int height = m_cRegion.GetHeight();
	// 绘制标题
	CFontManager::CTextImage* pImage = 0;
	if (m_strText[0]) {
		pImage = new CFontManager::CTextImage(width, height);
		CFontManager* pFontMgr = CEngine::GetFontManager();
		pFontMgr->UseFont(m_strFont);
		pFontMgr->SetSize(m_iFontSize);
		pFontMgr->DrawText(m_strText, pImage, CFontManager::MIDDLECENTER, m_bStretchText);
	}
	// 填充 GUI 缓冲区
	int offsetX = rect.GetLeft() - m_cRegionScreen.GetLeft();
	int offsetY = rect.GetTop() - m_cRegionScreen.GetTop();
	int drawWidth = rect.GetWidth();
	int drawHeight = rect.GetHeight();
	for (int h = 0; h < drawHeight; h++) {
		for (int w = 0; w < drawWidth; w++) {
			unsigned int index = (h * drawWidth + w) << 2;
			unsigned char pixel = pImage? pImage->data[(h + offsetY) * width + w + offsetX]: 0;
			// 背景图片，与背景颜色混合并拉伸平铺
			if (m_pImage) {
				int ix = (w + offsetX) * m_iImageWidth / width;
				int iy = (h + offsetY) * m_iImageHeight / height;
				const unsigned char* rgb = &m_pImage[(iy * m_iImageWidth + ix) * m_iImageChannels];
				unsigned char backColor[4] = {
					(unsigned char)((m_cBackColor[0] * rgb[0]) >> 8),
					(unsigned char)((m_cBackColor[1] * (m_iImageChannels >= 3? rgb[1]: rgb[0])) >> 8),
					(unsigned char)((m_cBackColor[2] * (m_iImageChannels >= 3? rgb[2]: rgb[0])) >> 8),
					(unsigned char)((m_cBackColor[3] * (m_iImageChannels >= 4? rgb[3]: 0xFF)) >> 8)
				};
				buffer[index + 0] = (pixel * m_cForeColor[0] + (0xFF - pixel) * backColor[0]) >> 8;
				buffer[index + 1] = (pixel * m_cForeColor[1] + (0xFF - pixel) * backColor[1]) >> 8;
				buffer[index + 2] = (pixel * m_cForeColor[2] + (0xFF - pixel) * backColor[2]) >> 8;
				buffer[index + 3] = (pixel * m_cForeColor[3] + (0xFF - pixel) * backColor[3]) >> 8;
			} else {
				buffer[index + 0] = (pixel * m_cForeColor[0] + (0xFF - pixel) * m_cBackColor[0]) >> 8;
				buffer[index + 1] = (pixel * m_cForeColor[1] + (0xFF - pixel) * m_cBackColor[1]) >> 8;
				buffer[index + 2] = (pixel * m_cForeColor[2] + (0xFF - pixel) * m_cBackColor[2]) >> 8;
				buffer[index + 3] = (pixel * m_cForeColor[3] + (0xFF - pixel) * m_cBackColor[3]) >> 8;
			}
		}
	}
	if (pImage) delete pImage;
}