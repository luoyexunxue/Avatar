//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CGUIPANEL_H_
#define _CGUIPANEL_H_
#include "CGuiEnvironment.h"

/**
* @brief GUI �������
*/
class CGuiPanel: public CGuiEnvironment::CGuiElement {
public:
	//! ���캯��
	CGuiPanel(const string& name);
	//! �϶�����
	virtual bool Drag(bool release, int dx, int dy, CRectangle& region);
	//! ��������
	virtual bool SetAttribute(const string& name, const string& value);
	//! ��ȡ����
	virtual string GetAttribute(const string& name);
	//! �ؼ�����
	virtual void Draw(const CRectangle& rect, unsigned char* buffer);

private:
	//! ƫ����
	int m_iCurrentOffset[2];
	//! ������ɫ
	unsigned char m_cBackColor[4];
};

#endif