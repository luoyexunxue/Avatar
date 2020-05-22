//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CGUIENVIRONMENT_H_
#define _CGUIENVIRONMENT_H_
#include "CRectangle.h"
#include <string>
#include <list>
#include <vector>
using std::string;
using std::list;
using std::vector;

/**
* @brief ���� GUI ����
*
* ʵ�ֻ���Ϊʹ����Ļ��С���ڴ��������GUIԪ�ض��ǻ��Ƶ��ڴ��У�Ȼ���ٸ����� OpenGL ����
*/
class CGuiEnvironment {
public:
	//! ��ȡ GUI ����ʵ��
	static CGuiEnvironment* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CGuiEnvironment();
		return m_pInstance;
	}
	//! ���� GUI ����
	void Destroy();

	//! ��Ⱦ GUI ����
	void Render();
	//! ����¼�
	bool MouseEvent(int x, int y, int button, int delta);
	//! �����¼�
	bool KeyboardEvent(int key);
	//! ������Ļ��С
	void UpdateSize(int width, int height);

	//! ʹ�ܻ���� GUI
	void SetEnable(bool enable);
	//! ��ȡ GUI ������С
	void GetSize(int* width, int* height);
	//! ���� GUI ����
	void SetScale(float scale);
	//! ���� GUI Ԫ��
	bool GuiCreate(const string& name, const string& type, const string& desc);
	//! �޸� GUI Ԫ��
	bool GuiModify(const string& name, const string& desc);
	//! ɾ�� GUI Ԫ��
	bool GuiDelete(const string& name);

public:
	//! GUIԪ�ػ��ඨ��
	class CGuiElement {
	public:
		//! Ĭ�Ϲ��캯��
		CGuiElement(const string& name) {
			m_pParent = 0;
			m_strName = name;
			m_bVisible = true;
			m_iOffset[0] = 0;
			m_iOffset[1] = 0;
		}
		//! ����������
		virtual ~CGuiElement() {}
		//! �Ƿ���Ҫ�ػ�
		virtual bool Redraw() { return false; }
		//! ��Ӧ�϶�����
		virtual bool Drag(bool release, int dx, int dy, CRectangle& region) {
			return m_pParent ? m_pParent->Drag(release, dx, dy, region) : false;
		}
		//! ��������
		virtual bool SetAttribute(const string& name, const string& value) = 0;
		//! ��ȡ����
		virtual string GetAttribute(const string& name) = 0;
		//! �ؼ�����
		virtual void Draw(const CRectangle& rect, unsigned char* buffer) = 0;

	public:
		//! ����Ԫ��
		CGuiElement* m_pParent;
		//! ��Ԫ��
		list<CGuiElement*> m_lstChildren;
		//! Ԫ������
		string m_strName;
		//! �ɼ���
		bool m_bVisible;
		//! ��Ԫ��ƫ��
		int m_iOffset[2];
		//! Ԫ������
		CRectangle m_cRegion;
		//! ��Ļλ��
		CRectangle m_cRegionScreen;
	};

	//! ��ȡָ�����Ƶ�GUI
	CGuiElement* GetElement(const string& name);
	//! �����ƶ�λ�õ�GUI
	CGuiElement* GetElement(CGuiElement* parent, int x, int y);

private:
	CGuiEnvironment();
	~CGuiEnvironment();

	//! ɾ��ָ����Ԫ�غ�����Ԫ��
	void DeleteElement(CGuiElement* element);
	//! ʹ��ͼ����Ч
	void InvalidateRegion(const CRectangle& region);
	//! ����ָ������Ч����
	void DrawInvalidateRegion();

private:
	//! GUIʹ��״̬
	bool m_bEnabled;
	//! GUI��������ϵ��
	float m_fScaleFactor;
	//! ��ȾGUI������
	unsigned char* m_pGuiBuffer;
	//! ���������С
	CRectangle m_cDrawRegion;
	//! ��Ҫˢ�µ���Ч����
	vector<CRectangle> m_vecInvalidateRect;
	//! GUIԪ���б�
	list<CGuiElement*> m_lstElements;

	//! ��¼���״̬
	int m_iLastMouseButton;
	//! ��¼��갴��λ��
	int m_iLastMouseDownPos[2];

	//! ��ý����Ԫ��
	CGuiElement* m_pFocusElement;
	//! ��ǰ��ק��Ԫ��
	CGuiElement* m_pDragElement;
	//! ������ȾGUI��Mesh
	class CMesh* m_pRenderMesh;

	//! ʵ��
	static CGuiEnvironment* m_pInstance;
};

#endif