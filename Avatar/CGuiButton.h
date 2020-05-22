//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CGUIBUTTON_H_
#define _CGUIBUTTON_H_
#include "CGuiEnvironment.h"

/**
* @brief GUI ��ť
*/
class CGuiButton: public CGuiEnvironment::CGuiElement {
public:
	//! ���캯��
	CGuiButton(const string& name);
	//! ��������
	virtual ~CGuiButton();
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
	//! �����ı�
	wchar_t m_strText[64];
	//! �������쵽��ť��С
	bool m_bStretchText;
	//! ������ɫ
	unsigned char m_cBackColor[4];
	//! ǰ����ɫ
	unsigned char m_cForeColor[4];
	//! ����ͼƬ����
	unsigned char* m_pImage;
	//! ����ͼƬ���
	int m_iImageWidth;
	//! ����ͼƬ�߶�
	int m_iImageHeight;
	//! ����ͼƬͨ����
	int m_iImageChannels;
};

#endif