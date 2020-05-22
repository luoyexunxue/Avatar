//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CGUITRACKBAR_H_
#define _CGUITRACKBAR_H_
#include "CGuiEnvironment.h"

/**
* @brief GUI ������
*/
class CGuiTrackBar: public CGuiEnvironment::CGuiElement {
public:
	//! ���캯��
	CGuiTrackBar(const string& name);
	//! ��������
	virtual bool SetAttribute(const string& name, const string& value);
	//! ��ȡ����
	virtual string GetAttribute(const string& name);
	//! �ؼ�����
	virtual void Draw(const CRectangle& rect, unsigned char* buffer);

private:
	//! ��ʾ�ٷֱ�
	bool m_bShowPercent;
	//! ��������
	string m_strFont;
	//! �����С
	int m_iFontSize;
	//! ��������ǰֵ
	float m_fValue;
	//! ��������Сֵ
	float m_fMinValue;
	//! ���������ֵ
	float m_fMaxValue;
	//! ������ɫ
	unsigned char m_cBackColor[4];
	//! ǰ����ɫ
	unsigned char m_cForeColor[4];
	//! �ٷ�ֵ�ı�
	wchar_t m_strText[8];
};

#endif