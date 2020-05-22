//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CGUILABEL_H_
#define _CGUILABEL_H_
#include "CGuiEnvironment.h"

/**
* @brief GUI �ı���ǩ
*/
class CGuiLabel: public CGuiEnvironment::CGuiElement {
public:
	//! ���캯��
	CGuiLabel(const string& name);
	//! ��������
	virtual bool SetAttribute(const string& name, const string& value);
	//! ��ȡ����
	virtual string GetAttribute(const string& name);
	//! �ؼ�����
	virtual void Draw(const CRectangle& rect, unsigned char* buffer);

private:
	//! ��������
	string m_strFont;
	//! �����С
	int m_iFontSize;
	//! ��ǩ�ı�
	wchar_t m_strText[256];
	//! ���뷽ʽ
	int m_iAlignment;
	//! �������쵽��ǩ��С
	bool m_bStretchText;
	//! ������ɫ
	unsigned char m_cBackColor[4];
	//! ǰ����ɫ
	unsigned char m_cForeColor[4];
};

#endif