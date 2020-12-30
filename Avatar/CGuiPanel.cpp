//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CGuiPanel.h"
#include "CEngine.h"
#include "CStringUtil.h"
#include "CColor.h"
#include <algorithm>

/**
* 构造函数，指定名称
*/
CGuiPanel::CGuiPanel(const string& name) : CGuiElement(name) {
	m_cRegion.SetValue(0, 0, 0, 32);
	m_cRegionScreen.SetValue(m_cRegion);
	m_iCurrentOffset[0] = 0;
	m_iCurrentOffset[1] = 0;
	m_cBackColor[0] = 0xFF;
	m_cBackColor[1] = 0xFF;
	m_cBackColor[2] = 0xFF;
	m_cBackColor[3] = 0xFF;
	m_bOcclusionArea[0] = false;
	m_bOcclusionArea[1] = false;
	m_bOcclusionArea[2] = false;
	m_bOcclusionArea[3] = false;
}

/**
* 拖动操作
*/
bool CGuiPanel::Drag(bool release, int dx, int dy, CRectangle& region) {
	CRectangle rect(0, 0, 0, 0);
	list<CGuiElement*>::iterator iter = m_lstChildren.begin();
	while (iter != m_lstChildren.end()) {
		rect += (*iter)->m_cRegion;
		++iter;
	}
	region.SetValue(m_cRegionScreen);
	if (release) {
		dx += m_iCurrentOffset[0];
		dy += m_iCurrentOffset[1];
		m_iCurrentOffset[0] = std::max(std::min(m_cRegion[2] - rect[2], 0), std::min(-rect[0], dx));
		m_iCurrentOffset[1] = std::max(std::min(m_cRegion[3] - rect[3], 0), std::min(-rect[1], dy));
		return true;
	}
	dx += m_iCurrentOffset[0];
	dy += m_iCurrentOffset[1];
	int limit_x = std::min(m_cRegion[2] - rect[2], 0);
	int limit_y = std::min(m_cRegion[3] - rect[3], 0);
	m_iOffset[0] = std::max(limit_x, std::min(-rect[0], dx));
	m_iOffset[1] = std::max(limit_y, std::min(-rect[1], dy));
	m_bOcclusionArea[0] = m_iOffset[0] != 0;
	m_bOcclusionArea[1] = m_iOffset[1] != 0;
	m_bOcclusionArea[2] = m_iOffset[0] != limit_x;
	m_bOcclusionArea[3] = m_iOffset[1] != limit_y;
	iter = m_lstChildren.begin();
	while (iter != m_lstChildren.end()) {
		CGuiElement* pElement = *iter;
		pElement->m_cRegionScreen.SetValue(pElement->m_cRegion);
		pElement->m_cRegionScreen.Offset(m_cRegionScreen).Offset(m_iOffset);
		++iter;
	}
	return m_lstChildren.size() > 0;
}

/**
* 属性设置
*/
bool CGuiPanel::SetAttribute(const string& name, const string& value) {
	if (name == "visible") m_bVisible = value == "true";
	else if (name == "left") m_cRegion[0] = atoi(value.c_str());
	else if (name == "top") m_cRegion[1] = atoi(value.c_str());
	else if (name == "width") m_cRegion[2] = atoi(value.c_str());
	else if (name == "height") m_cRegion[3] = atoi(value.c_str());
	else if (name == "parent") m_pParent = CGuiEnvironment::GetInstance()->GetElement(value);
	else if (name == "backColor") {
		CColor color(value);
		color.GetValue(m_cBackColor);
	} else return false;
	return true;
}

/**
* 获取属性
*/
string CGuiPanel::GetAttribute(const string& name) {
	if (name == "visible") return m_bVisible? "true": "false";
	else if (name == "left") return CStringUtil::Format("%d", m_cRegion[0]);
	else if (name == "top") return CStringUtil::Format("%d", m_cRegion[1]);
	else if (name == "width") return CStringUtil::Format("%d", m_cRegion[2]);
	else if (name == "height") return CStringUtil::Format("%d", m_cRegion[3]);
	else if (name == "parent") return m_pParent ? m_pParent->m_strName : "";
	else if (name == "backColor") {
		const char* fmt = "#%02X%02X%02X%02X";
		const unsigned char* val = m_cBackColor;
		return CStringUtil::Format(fmt, val[3], val[2], val[1], val[0]);
	}
	return "";
}

/**
* 控件绘制
*/
void CGuiPanel::Draw(const CRectangle& rect, unsigned char* buffer) {
	int drawWidth = rect.GetWidth();
	int drawHeight = rect.GetHeight();
	for (int h = 0; h < drawHeight; h++) {
		for (int w = 0; w < drawWidth; w++) {
			unsigned int index = (h * drawWidth + w) << 2;
			buffer[index + 0] = m_cBackColor[0];
			buffer[index + 1] = m_cBackColor[1];
			buffer[index + 2] = m_cBackColor[2];
			buffer[index + 3] = m_cBackColor[3];
		}
	}
}