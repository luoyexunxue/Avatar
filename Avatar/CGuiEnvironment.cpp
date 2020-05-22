//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
#include <cstring>
#include <cstdlib>

/**
* GUI�¼�����
*/
const int EVENT_MOUSE_CLICK = 0;
const int EVENT_MOUSE_DOWN = 1;
const int EVENT_MOUSE_UP = 2;
const int EVENT_MOUSE_DRAG = 3;
const int EVENT_KEY_PRESS = 4;

/**
* ���캯��
*/
CGuiEnvironment::CGuiEnvironment() {
	m_bEnabled = true;
	m_fScaleFactor = 1.0f;
	m_cDrawRegion.SetValue(0, 0, 1, 1);
	m_iLastMouseButton = 0;
	m_iLastMouseDownPos[0] = -1;
	m_iLastMouseDownPos[1] = -1;
	m_pFocusElement = 0;
	m_pDragElement = 0;
	m_pGuiBuffer = new unsigned char[4];
	// ������ɫ��
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
	// ��������
	CTexture* pTexture = CEngine::GetTextureManager()->Create("gui", 1, 1, 4, m_pGuiBuffer, false);
	pTexture->SetWrapModeClampToEdge(true, true);
	// �����������
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
* ��������
*/
CGuiEnvironment::~CGuiEnvironment() {
	m_pInstance = 0;
}

/**
* ����ʵ��
*/
CGuiEnvironment* CGuiEnvironment::m_pInstance = 0;

/**
* ���� GUI ����
*/
void CGuiEnvironment::Destroy() {
	list<CGuiElement*>::iterator iter = m_lstElements.begin();
	while (iter != m_lstElements.end()) {
		delete (*iter);
		++iter;
	}
	m_lstElements.clear();
	delete m_pRenderMesh;
	delete[] m_pGuiBuffer;
	delete this;
}

/**
* ��Ⱦ GUI ����
*/
void CGuiEnvironment::Render() {
	if (!m_bEnabled || m_lstElements.empty()) return;
	if (m_pFocusElement && m_pFocusElement->Redraw()) {
		m_vecInvalidateRect.push_back(m_pFocusElement->m_cRegionScreen);
	}
	if (!m_vecInvalidateRect.empty()) {
		DrawInvalidateRegion();
	}
	m_pRenderMesh->Render();
}

/**
* ����¼����������¼�����true
*/
bool CGuiEnvironment::MouseEvent(int x, int y, int button, int delta) {
	if (!m_bEnabled) return false;
	x = static_cast<int>(x * m_fScaleFactor);
	y = static_cast<int>(y * m_fScaleFactor);
	CGuiElement* pElement = GetElement(0, x, y);
	// ����� GUI ������������������ק��Ԫ��
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
				m_pDragElement->Drag(true, x - m_iLastMouseDownPos[0], y - m_iLastMouseDownPos[1], updateRegion);
				pElement = m_pDragElement;
				m_pDragElement = 0;
			}
			int dx = x - pElement->m_cRegionScreen.GetLeft();
			int dy = y - pElement->m_cRegionScreen.GetTop();
			CEngine::GetScriptManager()->GuiEvent(pElement->m_strName, EVENT_MOUSE_UP, dx, dy);
			if (m_iLastMouseDownPos[0] == x && m_iLastMouseDownPos[1] == y) {
				CEngine::GetScriptManager()->GuiEvent(pElement->m_strName, EVENT_MOUSE_CLICK, dx, dy);
			}
		} else if (m_pDragElement && m_iLastMouseButton == 1 && button == 1) {
			int dx = x - m_pDragElement->m_cRegionScreen.GetLeft();
			int dy = y - m_pDragElement->m_cRegionScreen.GetTop();
			CEngine::GetScriptManager()->GuiEvent(m_pDragElement->m_strName, EVENT_MOUSE_DRAG, dx, dy);
			CRectangle updateRegion(m_pDragElement->m_cRegionScreen);
			if (m_pDragElement->Drag(false, x - m_iLastMouseDownPos[0], y - m_iLastMouseDownPos[1], updateRegion)) {
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
* �����¼����������¼�����true
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
* ������Ļ��С
*/
void CGuiEnvironment::UpdateSize(int width, int height) {
	m_cDrawRegion.SetValue(0, 0, width, height);
	m_cDrawRegion.Scale(m_fScaleFactor, false);
	m_vecInvalidateRect.clear();
	m_vecInvalidateRect.push_back(m_cDrawRegion);
}

/**
* ʹ�ܻ���� GUI
*/
void CGuiEnvironment::SetEnable(bool enable) {
	m_bEnabled = enable;
}

/**
* ��ȡ GUI ������С
*/
void CGuiEnvironment::GetSize(int* width, int* height) {
	*width = m_cDrawRegion.GetWidth();
	*height = m_cDrawRegion.GetHeight();
}

/**
* ���� GUI �ֱ���
*/
void CGuiEnvironment::SetScale(float scale) {
	m_fScaleFactor = 1.0f / scale;
	m_cDrawRegion.Scale(m_fScaleFactor, false);
	m_vecInvalidateRect.clear();
	m_vecInvalidateRect.push_back(m_cDrawRegion);
}

/**
* ���� GUI Ԫ��
*/
bool CGuiEnvironment::GuiCreate(const string& name, const string& type, const string& desc) {
	// ��ֹ���������Ԫ��
	CGuiElement* pElement = GetElement(name);
	if (pElement) return false;
	// �����������
	if (type == "button") pElement = new CGuiButton(name);
	else if (type == "label") pElement = new CGuiLabel(name);
	else if (type == "trackbar") pElement = new CGuiTrackBar(name);
	else if (type == "editbox") pElement = new CGuiEditBox(name);
	else if (type == "panel") pElement = new CGuiPanel(name);
	// ���������б�
	if (pElement) {
		vector<string> attribs;
		vector<string> keyvalue;
		CStringUtil::Split(attribs, desc, ";", true);
		for (size_t i = 0; i< attribs.size(); i++) {
			if (attribs[i].empty()) continue;
			CStringUtil::Split(keyvalue, attribs[i], "=", true);
			pElement->SetAttribute(keyvalue[0], keyvalue[1]);
		}
		m_lstElements.push_back(pElement);
		CGuiElement* parent = pElement->m_pParent;
		pElement->m_cRegionScreen.SetValue(pElement->m_cRegion);
		if (parent) {
			pElement->m_cRegionScreen.Offset(parent->m_cRegionScreen).Offset(parent->m_iOffset);
			parent->m_lstChildren.push_back(pElement);
		}
		InvalidateRegion(pElement->m_cRegionScreen);
		return true;
	}
	return false;
}

/**
* �޸� GUI Ԫ��
*/
bool CGuiEnvironment::GuiModify(const string& name, const string& desc) {
	CGuiElement* pElement = GetElement(name);
	if (pElement) {
		CGuiElement* parent = pElement->m_pParent;
		const CRectangle region = pElement->m_cRegionScreen;
		vector<string> attribs;
		vector<string> keyvalue;
		CStringUtil::Split(attribs, desc, ";", true);
		for (size_t i = 0; i < attribs.size(); i++) {
			if (attribs[i].empty()) continue;
			CStringUtil::Split(keyvalue, attribs[i], "=", true);
			pElement->SetAttribute(keyvalue[0], keyvalue[1]);
		}
		CGuiElement* newParent = pElement->m_pParent;
		pElement->m_cRegionScreen.SetValue(pElement->m_cRegion);
		if (newParent) {
			pElement->m_cRegionScreen.Offset(newParent->m_cRegionScreen).Offset(newParent->m_iOffset);
			if (newParent != parent) newParent->m_lstChildren.push_back(pElement);
		}
		if (newParent != parent && parent) {
			InvalidateRegion(region);
			parent->m_lstChildren.remove(pElement);
		}
		InvalidateRegion(region.Union(pElement->m_cRegionScreen));
		return true;
	}
	return false;
}

/**
* ɾ�� GUI Ԫ��
*/
bool CGuiEnvironment::GuiDelete(const string& name) {
	list<CGuiElement*>::iterator iter = m_lstElements.begin();
	while (iter != m_lstElements.end()) {
		CGuiElement* pElement = *iter;
		if (pElement->m_strName == name) {
			m_lstElements.erase(iter);
			InvalidateRegion(pElement->m_cRegionScreen);
			DeleteElement(pElement);
			return true;
		}
		++iter;
	}
	return false;
}

/**
* ��ȡָ�����Ƶ�GUIԪ��
*/
CGuiEnvironment::CGuiElement* CGuiEnvironment::GetElement(const string& name) {
	list<CGuiElement*>::iterator iter = m_lstElements.begin();
	while (iter != m_lstElements.end()) {
		if ((*iter)->m_strName == name) return *iter;
		++iter;
	}
	return 0;
}

/**
* ������Ļ�����ȡGUIԪ��
* @note ֻ��ȡ�ɼ��� GUI Ԫ��
*/
CGuiEnvironment::CGuiElement* CGuiEnvironment::GetElement(CGuiElement* parent, int x, int y) {
	list<CGuiElement*>::iterator iter = parent ? parent->m_lstChildren.end() : m_lstElements.end();
	list<CGuiElement*>::iterator begin = parent ? parent->m_lstChildren.begin() : m_lstElements.begin();
	while (iter != begin) {
		CGuiElement* item = *--iter;
		if (item->m_pParent && !item->m_pParent->m_bVisible) continue;
		if (item->m_bVisible && item->m_cRegionScreen.IsContain(x, y)) {
			CGuiElement* child = item->m_lstChildren.empty() ? 0 : GetElement(item, x, y);
			return child ? child : item;
		}
	}
	return 0;
}

/**
* ɾ��ָ����Ԫ�غ�����Ԫ��
*/
void CGuiEnvironment::DeleteElement(CGuiElement* element) {
	list<CGuiElement*>::iterator iter = m_lstElements.begin();
	while (iter != m_lstElements.end()) {
		CGuiElement* child = *iter;
		if (element == child->m_pParent) {
			DeleteElement(child);
		}
		++iter;
	}
	if (element->m_pParent) element->m_pParent->m_lstChildren.remove(element);
	if (element == m_pFocusElement) m_pFocusElement = 0;
	if (element == m_pDragElement) m_pDragElement = 0;
	delete element;
}

/**
* ʹָ�����λ�ͼ����Ч�������ػ�
*/
void CGuiEnvironment::InvalidateRegion(const CRectangle& rect) {
	// �����������Ļ��Χ�ڵ�����
	CRectangle r = rect.Intersect(m_cDrawRegion);
	for (size_t i = 0; i < m_vecInvalidateRect.size(); i++) {
		if (m_vecInvalidateRect[i].IsContain(r)) return;
	}
	m_vecInvalidateRect.push_back(r);
}

/**
* ����ָ������Ч����
*/
void CGuiEnvironment::DrawInvalidateRegion() {
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	CTexture* pTexture = m_pRenderMesh->GetMaterial()->GetTexture();
	// �����Ƿ���Ҫ���Ļ�ͼ�����С
	if (m_cDrawRegion.GetWidth() != pTexture->GetWidth() ||
		m_cDrawRegion.GetHeight() != pTexture->GetHeight()) {
		// ���ȫ������
		int bufferSize = m_cDrawRegion.GetArea() * 4;
		delete[] m_pGuiBuffer;
		m_pGuiBuffer = new unsigned char[bufferSize];
		memset(m_pGuiBuffer, 0, bufferSize);
		pTextureMgr->Resize(pTexture, m_cDrawRegion.GetWidth(), m_cDrawRegion.GetHeight());
		pTextureMgr->Update(pTexture, m_pGuiBuffer);
	} else {
		// ����ػ�����
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
	// ������Ҫ�ػ��Ԫ��
	list<CGuiElement*>::iterator iter = m_lstElements.begin();
	while (iter != m_lstElements.end()) {
		CGuiElement* pElement = *iter;
		for (size_t i = 0; i < m_vecInvalidateRect.size(); i++) {
			CRectangle drawRegion = m_vecInvalidateRect[i].Intersect(pElement->m_cRegionScreen);
			if (pElement->m_pParent) drawRegion *= pElement->m_pParent->m_cRegionScreen;
			if (!drawRegion.IsEmpty() && pElement->m_bVisible) {
				// ��Ԫ�ؿɼ�����Ⱦ�����һ������򾭹���Ԫ������ü�
				bool parentVisible = true;
				CGuiElement* parent = pElement->m_pParent;
				while (parent && parentVisible) {
					parentVisible = parentVisible && parent->m_bVisible;
					parent = parent->m_pParent;
				}
				if (parentVisible) {
					pElement->Draw(drawRegion, m_pGuiBuffer);
					pTextureMgr->Update(pTexture, m_pGuiBuffer, drawRegion);
				}
			}
		}
		++iter;
	}
	// ������Ч����
	m_vecInvalidateRect.clear();
}