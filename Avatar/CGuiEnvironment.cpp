//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CGuiEnvironment.h"
#include "AvatarConfig.h"
#include "CEngine.h"
#include "CLog.h"
#include "CStringUtil.h"
#include "CGuiButton.h"
#include "CGuiLabel.h"
#include "CGuiTrackBar.h"
#include "CGuiEditBox.h"
#include "CGuiPanel.h"
#include "CGuiListView.h"
#include <cstring>
#include <cstdlib>

/**
* GUI事件定义
*/
const int EVENT_MOUSE_CLICK = 0;
const int EVENT_MOUSE_DOWN = 1;
const int EVENT_MOUSE_UP = 2;
const int EVENT_MOUSE_DRAG = 3;
const int EVENT_KEY_PRESS = 4;

/**
* 构造函数
*/
CGuiEnvironment::CGuiEnvironment() {
	m_bEnabled = true;
	m_fScaleFactor = 1.0f;
	m_iScreenSize[0] = 1;
	m_iScreenSize[1] = 1;
	m_iLastMouseButton = 0;
	m_iLastMouseDownPos[0] = -1;
	m_iLastMouseDownPos[1] = -1;
	m_pFocusElement = 0;
	m_pDragElement = 0;
	m_pGuiBuffer = new unsigned char[4];
	m_pRootElement = new CGuiElementRoot();
	m_pRootElement->m_cRegion.Resize(1, 1);
	m_pRootElement->m_cRegionScreen.SetValue(m_pRootElement->m_cRegion);
	// 创建着色器
	const char* vertShader = "\
		in vec4 aPosition;\
		in vec2 aTexCoord;\
		out vec2 vTexCoord;\
		void main()\
		{\
			vTexCoord = aTexCoord;\
			gl_Position = aPosition;\
		}";
	const char* fragShader = "\
		uniform sampler2D uTexture;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		void main() \
		{\
			fragColor = texture(uTexture, vTexCoord);\
		}";
	CShader* pShader = CEngine::GetShaderManager()->Create("gui", vertShader, fragShader);
	pShader->SetUniform("uTexture", 0);
	// 创建纹理
	CTexture* pTexture = CEngine::GetTextureManager()->Create("gui", 1, 1, 4, m_pGuiBuffer, false);
	pTexture->SetWrapModeClampToEdge(true, true);
	// 创建网格对象
	m_pRenderMesh = new CMesh();
	m_pRenderMesh->SetVertexUsage(4);
	m_pRenderMesh->AddVertex(CVertex(-1.0f, 1.0f, 0.0f, 0.0f, 0.0f));
	m_pRenderMesh->AddVertex(CVertex(1.0f, 1.0f, 0.0f, 1.0f, 0.0f));
	m_pRenderMesh->AddVertex(CVertex(1.0f, -1.0f, 0.0f, 1.0f, 1.0f));
	m_pRenderMesh->AddVertex(CVertex(-1.0f, -1.0f, 0.0f, 0.0f, 1.0f));
	m_pRenderMesh->AddTriangle(3, 2, 0);
	m_pRenderMesh->AddTriangle(2, 1, 0);
	m_pRenderMesh->Create(false);
	m_pRenderMesh->GetMaterial()->SetShader(pShader);
	m_pRenderMesh->GetMaterial()->SetTexture(pTexture);
	m_pRenderMesh->GetMaterial()->SetRenderMode(true, false, false);
	CEngine::GetShaderManager()->Drop(pShader);
	CEngine::GetTextureManager()->Drop(pTexture);
}

/**
* 析构函数
*/
CGuiEnvironment::~CGuiEnvironment() {
	m_pInstance = 0;
}

/**
* 单例实例
*/
CGuiEnvironment* CGuiEnvironment::m_pInstance = 0;

/**
* 销毁 GUI 环境
*/
void CGuiEnvironment::Destroy() {
	DeleteElement(m_pRootElement);
	delete m_pRenderMesh;
	delete[] m_pGuiBuffer;
	delete this;
}

/**
* 渲染 GUI 界面
*/
void CGuiEnvironment::Render() {
	if (!m_bEnabled || m_pRootElement->m_lstChildren.empty()) return;
	if (m_pFocusElement && m_pFocusElement->Redraw()) {
		m_vecInvalidateRect.push_back(m_pFocusElement->m_cRegionScreen);
	}
	if (!m_vecInvalidateRect.empty()) {
		DrawInvalidateRegion();
	}
	m_pRenderMesh->Render();
}

/**
* 鼠标事件，若处理事件返回true
*/
bool CGuiEnvironment::MouseEvent(int x, int y, int button, int delta) {
	if (!m_bEnabled) return false;
	x = static_cast<int>(x * m_fScaleFactor);
	y = static_cast<int>(y * m_fScaleFactor);
	CGuiElement* pElement = GetElement(0, x, y);
	// 鼠标在 GUI 组件上面或者有正在拖拽的元素
	bool handleEvent = pElement || m_pDragElement;
	if (handleEvent) {
		if (m_iLastMouseButton == 0 && button == 1) {
			m_iLastMouseDownPos[0] = x;
			m_iLastMouseDownPos[1] = y;
			m_pFocusElement = pElement;
			m_pDragElement = pElement;
			int dx = x - pElement->m_cRegionScreen.GetLeft();
			int dy = y - pElement->m_cRegionScreen.GetTop();
			CEngine::GetScriptManager()->GuiEvent(pElement->m_strName, EVENT_MOUSE_DOWN, dx, dy);
		} else if (m_iLastMouseButton == 1 && button == 0) {
			if (m_pDragElement) {
				CRectangle updateRegion;
				m_pDragElement->OnDrag(true, x - m_iLastMouseDownPos[0], y - m_iLastMouseDownPos[1], updateRegion);
				pElement = m_pDragElement;
				m_pDragElement = 0;
			}
			int dx = x - pElement->m_cRegionScreen.GetLeft();
			int dy = y - pElement->m_cRegionScreen.GetTop();
			CEngine::GetScriptManager()->GuiEvent(pElement->m_strName, EVENT_MOUSE_UP, dx, dy);
			if (m_iLastMouseDownPos[0] == x && m_iLastMouseDownPos[1] == y) {
				pElement->OnClick(dx, dy);
				CEngine::GetScriptManager()->GuiEvent(pElement->m_strName, EVENT_MOUSE_CLICK, dx, dy);
			}
		} else if (m_pDragElement && m_iLastMouseButton == 1 && button == 1) {
			int dx = x - m_pDragElement->m_cRegionScreen.GetLeft();
			int dy = y - m_pDragElement->m_cRegionScreen.GetTop();
			CEngine::GetScriptManager()->GuiEvent(m_pDragElement->m_strName, EVENT_MOUSE_DRAG, dx, dy);
			CRectangle updateRegion(m_pDragElement->m_cRegionScreen);
			if (m_pDragElement->OnDrag(false, x - m_iLastMouseDownPos[0], y - m_iLastMouseDownPos[1], updateRegion)) {
				InvalidateRegion(updateRegion);
			}
		}
	} else if (button > 0) {
		m_pFocusElement = 0;
	}
	m_iLastMouseButton = button;
	return handleEvent;
}

/**
* 键盘事件，若处理事件返回true
*/
bool CGuiEnvironment::KeyboardEvent(int key) {
	if (m_bEnabled && m_pFocusElement) {
		if (m_pFocusElement->SetAttribute("__key", CStringUtil::Format("%d", key))) {
			CEngine::GetScriptManager()->GuiEvent(m_pFocusElement->m_strName, EVENT_KEY_PRESS, key, key);
			InvalidateRegion(m_pFocusElement->m_cRegionScreen);
			return true;
		}
	}
	return false;
}

/**
* 使能或禁用 GUI
*/
void CGuiEnvironment::SetEnable(bool enable) {
	m_bEnabled = enable;
}

/**
* 设置 GUI 分辨率
*/
void CGuiEnvironment::SetScale(float scale) {
	m_fScaleFactor = 1.0f / scale;
	m_pRootElement->m_cRegion.Resize(m_iScreenSize[0], m_iScreenSize[1]);
	m_pRootElement->m_cRegion.Scale(m_fScaleFactor, false);
	m_pRootElement->m_cRegionScreen.SetValue(m_pRootElement->m_cRegion);
	m_vecInvalidateRect.clear();
	m_vecInvalidateRect.push_back(m_pRootElement->m_cRegion);
}

/**
* 设置 GUI 画布大小
*/
void CGuiEnvironment::SetSize(int width, int height) {
	if (m_iScreenSize[0] != width || m_iScreenSize[1] != height) {
		m_iScreenSize[0] = width;
		m_iScreenSize[1] = height;
		m_pRootElement->m_cRegion.Resize(m_iScreenSize[0], m_iScreenSize[1]);
		m_pRootElement->m_cRegion.Scale(m_fScaleFactor, false);
		m_pRootElement->m_cRegionScreen.SetValue(m_pRootElement->m_cRegion);
		m_vecInvalidateRect.clear();
		m_vecInvalidateRect.push_back(m_pRootElement->m_cRegion);
	}
}

/**
* 获取 GUI 画布大小
*/
void CGuiEnvironment::GetSize(int* width, int* height) {
	*width = m_pRootElement->m_cRegion.GetWidth();
	*height = m_pRootElement->m_cRegion.GetHeight();
}

/**
* 创建 GUI 元素
*/
bool CGuiEnvironment::GuiCreate(const string& name, const string& type, const string& desc) {
	// 防止添加重名的元素
	CGuiElement* pElement = GetElement(name);
	if (pElement) return false;
	// 根据类型添加
	if (type == "button") pElement = new CGuiButton(name);
	else if (type == "label") pElement = new CGuiLabel(name);
	else if (type == "trackbar") pElement = new CGuiTrackBar(name);
	else if (type == "editbox") pElement = new CGuiEditBox(name);
	else if (type == "panel") pElement = new CGuiPanel(name);
	else if (type == "listview") pElement = new CGuiListView(name);
	// 解析属性列表
	if (pElement) {
		vector<string> attribs;
		vector<string> keyvalue;
		CStringUtil::Split(attribs, desc, ";", true);
		for (size_t i = 0; i< attribs.size(); i++) {
			if (attribs[i].empty()) continue;
			CStringUtil::Split(keyvalue, attribs[i], "=", true);
			pElement->SetAttribute(keyvalue[0], keyvalue[1]);
		}
		if (!pElement->m_pParent) pElement->m_pParent = m_pRootElement;
		pElement->m_cRegionScreen.SetValue(pElement->m_cRegion);
		pElement->m_cRegionScreen.Offset(pElement->m_pParent->m_cRegionScreen).Offset(pElement->m_pParent->m_iOffset);
		pElement->m_pParent->m_lstChildren.push_back(pElement);
		InvalidateRegion(pElement->m_cRegionScreen);
		return true;
	}
	return false;
}

/**
* 修改 GUI 元素
*/
bool CGuiEnvironment::GuiModify(const string& name, const string& desc) {
	CGuiElement* pElement = GetElement(name);
	if (!pElement) return false;
	CGuiElement* parent_prev = pElement->m_pParent;
	const CRectangle region = pElement->m_cRegionScreen;
	vector<string> attribs;
	vector<string> keyvalue;
	CStringUtil::Split(attribs, desc, ";", true);
	for (size_t i = 0; i < attribs.size(); i++) {
		if (attribs[i].empty()) continue;
		CStringUtil::Split(keyvalue, attribs[i], "=", true);
		pElement->SetAttribute(keyvalue[0], keyvalue[1]);
	}
	if (!pElement->m_pParent) pElement->m_pParent = m_pRootElement;
	if (pElement->m_pParent != parent_prev) {
		pElement->m_pParent->m_lstChildren.push_back(pElement);
		parent_prev->m_lstChildren.remove(pElement);
	}
	UpdateElement(pElement);
	InvalidateRegion(region.Union(pElement->m_cRegionScreen));
	return true;
}

/**
* 删除 GUI 元素
*/
bool CGuiEnvironment::GuiDelete(const string& name) {
	CGuiElement* pElement = GetElement(name);
	if (!pElement) return false;
	pElement->m_pParent->m_lstChildren.remove(pElement);
	InvalidateRegion(pElement->m_cRegionScreen);
	DeleteElement(pElement);
	return true;
}

/**
* 获取指定名称的GUI元素
*/
CGuiEnvironment::CGuiElement* CGuiEnvironment::GetElement(const string& name) {
	list<CGuiElement*> elements;
	elements.push_back(m_pRootElement);
	while (!elements.empty()) {
		CGuiElement* element = elements.back();
		elements.pop_back();
		list<CGuiElement*>::iterator iter = element->m_lstChildren.begin();
		while (iter != element->m_lstChildren.end()) {
			if ((*iter)->m_strName == name) return *iter;
			if (!(*iter)->m_lstChildren.empty()) elements.push_back(*iter);
			++iter;
		}
	}
	return 0;
}

/**
* 根据屏幕坐标获取GUI元素
* @note 只获取可见的 GUI 元素
*/
CGuiEnvironment::CGuiElement* CGuiEnvironment::GetElement(CGuiElement* parent, int x, int y) {
	if (!parent) parent = m_pRootElement;
	list<CGuiElement*>::iterator iter = parent->m_lstChildren.end();
	list<CGuiElement*>::iterator begin = parent->m_lstChildren.begin();
	while (iter != begin) {
		CGuiElement* item = *--iter;
		if (!item->m_pParent->m_bVisible) continue;
		if (item->m_bVisible && item->m_cRegionScreen.IsContain(x, y)) {
			CGuiElement* child = item->m_lstChildren.empty() ? 0 : GetElement(item, x, y);
			return child ? child : item;
		}
	}
	return 0;
}

/**
* 删除指定的元素和其子元素
*/
void CGuiEnvironment::DeleteElement(CGuiElement* element) {
	list<CGuiElement*>::iterator iter = element->m_lstChildren.begin();
	while (iter != element->m_lstChildren.end()) {
		DeleteElement(*iter++);
	}
	if (element->m_pParent) element->m_pParent->m_lstChildren.remove(element);
	if (element == m_pFocusElement) m_pFocusElement = 0;
	if (element == m_pDragElement) m_pDragElement = 0;
	delete element;
}

/**
* 更新指定的元素和其子元素
*/
void CGuiEnvironment::UpdateElement(CGuiElement* element) {
	element->m_cRegionScreen.SetValue(element->m_cRegion);
	element->m_cRegionScreen.Offset(element->m_pParent->m_cRegionScreen).Offset(element->m_pParent->m_iOffset);
	list<CGuiElement*>::iterator iter = element->m_lstChildren.begin();
	while (iter != element->m_lstChildren.end()) {
		UpdateElement(*iter++);
	}
}

/**
* 使指定矩形绘图区无效，导致重绘
*/
void CGuiEnvironment::InvalidateRegion(const CRectangle& rect) {
	// 计算包含在屏幕范围内的区域
	CRectangle r = rect.Intersect(m_pRootElement->m_cRegion);
	for (size_t i = 0; i < m_vecInvalidateRect.size(); i++) {
		if (m_vecInvalidateRect[i].IsContain(r)) return;
	}
	m_vecInvalidateRect.push_back(r);
}

/**
* 绘制指定的无效区域
*/
void CGuiEnvironment::DrawInvalidateRegion() {
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	CTexture* pTexture = m_pRenderMesh->GetMaterial()->GetTexture();
	// 计算是否需要更改绘图区域大小
	if (m_pRootElement->m_cRegion.GetWidth() != pTexture->GetWidth() ||
		m_pRootElement->m_cRegion.GetHeight() != pTexture->GetHeight()) {
		// 清空全部区域
		int bufferSize = m_pRootElement->m_cRegion.GetArea() * 4;
		delete[] m_pGuiBuffer;
		m_pGuiBuffer = new unsigned char[bufferSize];
		memset(m_pGuiBuffer, 0, bufferSize);
		pTextureMgr->Resize(pTexture, m_pRootElement->m_cRegion.GetWidth(), m_pRootElement->m_cRegion.GetHeight());
		pTextureMgr->Update(pTexture, m_pGuiBuffer);
	} else {
		// 清空重绘区域
		int maxArea = 0;
		for (size_t i = 0; i < m_vecInvalidateRect.size(); i++) {
			int area = m_vecInvalidateRect[i].GetArea();
			if (area > maxArea) maxArea = area;
		}
		memset(m_pGuiBuffer, 0, maxArea * 4);
		for (size_t i = 0; i < m_vecInvalidateRect.size(); i++) {
			pTextureMgr->Update(pTexture, m_pGuiBuffer, m_vecInvalidateRect[i]);
		}
	}
	// 绘制需要重绘的元素，深度优先遍历
	list<CGuiElement*> elements;
	elements.push_back(m_pRootElement);
	while (!elements.empty()) {
		CGuiElement* item = elements.back();
		elements.pop_back();
		list<CGuiElement*>::iterator iter = item->m_lstChildren.begin();
		while (iter != item->m_lstChildren.end()) {
			CGuiElement* pElement = *iter++;
			if (!pElement->m_bVisible) continue;
			if (!pElement->m_lstChildren.empty()) elements.push_back(pElement);
			for (size_t i = 0; i < m_vecInvalidateRect.size(); i++) {
				CRectangle drawRegion = m_vecInvalidateRect[i].Intersect(pElement->m_cRegionScreen);
				drawRegion *= pElement->m_pParent->m_cRegionScreen;
				if (!drawRegion.IsEmpty()) {
					// 指定屏幕坐标以及缓冲区绘制元素，缓冲区已被清零
					pElement->Draw(drawRegion, m_pGuiBuffer);
					pTextureMgr->Update(pTexture, m_pGuiBuffer, drawRegion);
				}
			}
		}
	}
	// 重置无效区域
	m_vecInvalidateRect.clear();
}