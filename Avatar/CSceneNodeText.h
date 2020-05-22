//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODETEXT_H_
#define _CSCENENODETEXT_H_
#include "CSceneNode.h"

/**
* @brief ���ֳ����ڵ�
*/
class AVATAR_EXPORT CSceneNodeText: public CSceneNode {
public:
	//! ���췽��
	CSceneNodeText(const string& name, const wchar_t* text, const CColor& color,
		const CColor& outline, const string& font, int fontSize);

	//! ��ʼ�������ڵ�
	virtual bool Init();
	//! ���ٳ����ڵ�
	virtual void Destroy();
	//! ��Ⱦ�����ڵ�
	virtual void Render();
	//! ���³����ڵ�
	virtual void Update(float dt);
	//! ��������任
	virtual void Transform() {}

	//! �����ı�
	void SetText(const wchar_t* text);
	//! ��������
	void SetFont(const string& font, int fontSize);
	//! �����ı���ɫ
	void SetColor(const CColor& color, const CColor& outline);

private:
	//! �����ı�
	void UpdateText();

private:
	//! �ı���ɫ
	CColor m_cColor;
	//! ������ɫ
	CColor m_cOutline;
	//! ��������
	string m_strFont;
	//! �����С
	int m_iFontSize;
	//! ��Ⱦ���ı�
	wchar_t m_strText[128];
	//! �������
	CMesh* m_pMesh;
};

#endif