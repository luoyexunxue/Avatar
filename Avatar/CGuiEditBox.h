//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CGUIEDITBOX_H_
#define _CGUIEDITBOX_H_
#include "CGuiEnvironment.h"

/**
* @brief GUI �ı������
*/
class CGuiEditBox: public CGuiEnvironment::CGuiElement {
public:
	//! ���캯��
	CGuiEditBox(const string& name);
	//! �Ƿ��ػ�
	virtual bool Redraw();
	//! ��������
	virtual bool SetAttribute(const string& name, const string& value);
	//! ��ȡ����
	virtual string GetAttribute(const string& name);
	//! �ؼ�����
	virtual void Draw(const CRectangle& rect, unsigned char* buffer);

public:
	//! ��������
	string m_strFont;
	//! �����С
	int m_iFontSize;
	//! �����ı�
	wchar_t m_strText[256];
	//! ���뷽ʽ
	int m_iAlignment;
	//! ������ɫ
	unsigned char m_cBackColor[4];
	//! �ı���ɫ
	unsigned char m_cForeColor[4];

private:
	//! ��˸��ʱ
	float m_fTimeElapsed;
	//! ��ʾ���
	bool m_bShowCursor;
};

#endif