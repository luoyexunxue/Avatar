//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
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
* @brief 引擎 GUI 环境
*
* 实现基础为使用屏幕大小的内存纹理，相关GUI元素都是绘制到内存中，然后再更新至 OpenGL 纹理，
*/
class CGuiEnvironment {
public:
	//! 获取 GUI 环境实例
	static CGuiEnvironment* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CGuiEnvironment();
		return m_pInstance;
	}
	//! 销毁 GUI 环境
	void Destroy();

	//! 渲染 GUI 界面
	void Render(int width, int height);
	//! 鼠标事件
	bool MouseEvent(int x, int y, int button, int delta);
	//! 键盘事件
	bool KeyboardEvent(int key);

	//! 使能或禁用 GUI
	void SetEnable(bool enable);
	//! 获取 GUI 画布大小
	void GetSize(int* width, int* height);
	//! 设置 GUI 缩放
	void SetScale(float scale);
	//! 创建 GUI 元素
	bool GuiCreate(const string& name, const string& type, const string& desc);
	//! 修改 GUI 元素
	bool GuiModify(const string& name, const string& desc);
	//! 删除 GUI 元素
	bool GuiDelete(const string& name);

public:
	//! GUI元素基类定义
	class CGuiElement {
	public:
		//! 默认构造函数
		CGuiElement(const string& name) {
			m_pParent = 0;
			m_strName = name;
			m_bVisible = true;
			m_iOffset[0] = 0;
			m_iOffset[1] = 0;
		}
		//! 虚析构函数
		virtual ~CGuiElement() {}
		//! 是否需要重绘
		virtual bool Redraw() { return false; }
		//! 响应拖动操作
		virtual bool Drag(bool release, int dx, int dy, CRectangle& region) {
			return m_pParent ? m_pParent->Drag(release, dx, dy, region) : false;
		}
		//! 属性设置
		virtual bool SetAttribute(const string& name, const string& value) = 0;
		//! 获取属性
		virtual string GetAttribute(const string& name) = 0;
		//! 控件绘制
		virtual void Draw(const CRectangle& rect, unsigned char* buffer) = 0;

	public:
		//! 父级元素
		CGuiElement* m_pParent;
		//! 子元素
		list<CGuiElement*> m_lstChildren;
		//! 元素名称
		string m_strName;
		//! 可见性
		bool m_bVisible;
		//! 子元素偏移
		int m_iOffset[2];
		//! 元素区域
		CRectangle m_cRegion;
		//! 屏幕位置
		CRectangle m_cRegionScreen;
	};

	//! 获取指定名称的GUI
	CGuiElement* GetElement(const string& name);
	//! 查找制定位置的GUI
	CGuiElement* GetElement(CGuiElement* parent, int x, int y);

private:
	CGuiEnvironment();
	~CGuiEnvironment();

	//! 删除指定的元素和其子元素
	void DeleteElement(CGuiElement* element);
	//! 使绘图区无效
	void InvalidateRegion(const CRectangle& region);
	//! 绘制指定的无效区域
	void DrawInvalidateRegion();

private:
	//! GUI使能状态
	bool m_bEnabled;
	//! GUI画布缩放系数
	float m_fScaleFactor;
	//! 渲染GUI缓冲区
	unsigned char* m_pGuiBuffer;
	//! 记录屏幕大小
	int m_iScreenSize[2];
	//! 画布区域大小
	CRectangle m_cDrawRegion;
	//! 需要刷新的无效区域
	vector<CRectangle> m_vecInvalidateRect;
	//! GUI元素列表
	list<CGuiElement*> m_lstElements;

	//! 记录鼠标状态
	int m_iLastMouseButton;
	//! 记录鼠标按下位置
	int m_iLastMouseDownPos[2];

	//! 获得焦点的元素
	CGuiElement* m_pFocusElement;
	//! 当前拖拽的元素
	CGuiElement* m_pDragElement;
	//! 用于渲染GUI的Mesh
	class CMesh* m_pRenderMesh;

	//! 实例
	static CGuiEnvironment* m_pInstance;
};

#endif