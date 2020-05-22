//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODETEXT_H_
#define _CSCENENODETEXT_H_
#include "CSceneNode.h"

/**
* @brief 文字场景节点
*/
class AVATAR_EXPORT CSceneNodeText: public CSceneNode {
public:
	//! 构造方法
	CSceneNodeText(const string& name, const wchar_t* text, const CColor& color,
		const CColor& outline, const string& font, int fontSize);

	//! 初始化场景节点
	virtual bool Init();
	//! 销毁场景节点
	virtual void Destroy();
	//! 渲染场景节点
	virtual void Render();
	//! 更新场景节点
	virtual void Update(float dt);
	//! 重载坐标变换
	virtual void Transform() {}

	//! 设置文本
	void SetText(const wchar_t* text);
	//! 设置字体
	void SetFont(const string& font, int fontSize);
	//! 设置文本颜色
	void SetColor(const CColor& color, const CColor& outline);

private:
	//! 更新文本
	void UpdateText();

private:
	//! 文本颜色
	CColor m_cColor;
	//! 轮廓颜色
	CColor m_cOutline;
	//! 字体名称
	string m_strFont;
	//! 字体大小
	int m_iFontSize;
	//! 渲染的文本
	wchar_t m_strText[128];
	//! 网格对象
	CMesh* m_pMesh;
};

#endif