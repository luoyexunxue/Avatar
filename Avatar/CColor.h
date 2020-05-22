//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CCOLOR_H_
#define _CCOLOR_H_
#include "AvatarConfig.h"
#include <string>
using std::string;

/**
* @brief ��ɫ��
*/
class AVATAR_EXPORT CColor {
public:
	//! Ĭ�Ϲ��캯��
	CColor();
	//! ���ƹ��캯��
	CColor(const CColor& color);
	//! ��������ʽ��RGBA����
	CColor(const float color[4]);
	//! �����ֽ���������
	CColor(unsigned int color);
	//! ��RGB������ɫ
	CColor(float r, float g, float b);
	//! ��RGBA������ɫ
	CColor(float r, float g, float b, float a);
	//! ��RGB�ֽڹ�����ɫ
	CColor(int r, int g, int b);
	//! ��RGBA�ֽڹ�����ɫ
	CColor(int r, int g, int b, int a);
	//! ���ַ�������
	CColor(const string& color);

	//! ������ɫֵ
	void SetValue(const CColor& color);
	//! ������ɫֵ
	void SetValue(const float color[4]);
	//! ������ɫֵ
	void SetValue(float r, float g, float b);
	//! ������ɫֵ
	void SetValue(float r, float g, float b, float a);

	//! ��ȡ��ɫֵ
	void GetValue(float* color) const;
	//! ��ȡ��ɫֵ
	void GetValue(unsigned char* color) const;
	//! ��仺����
	void FillBuffer(float* buffer, int size, int stride) const;
	//! ��仺����
	void FillBuffer(unsigned char* buffer, int size, int stride) const;

	//! ��ɫ���
	CColor& Add(const CColor& color, float scale);
	//! ��ɫ���
	CColor& Add(const float color[4], float scale);

	//! ��Χ������ 0.0 �� 1.0 ֮��
	CColor& Clamp();
	//! ��ת��ɫ
	CColor& Invert();
	//! �� HSLA ת��
	CColor& FromHsla(float h, float s, float l, float a);
	//! ������������ɫ
	CColor& FromName(const string& name);

	//! ������ɫ����
	string ToString();

public:
	//! �±������ []
	float& operator [] (int index);
	//! ��������� ==
	bool operator == (const CColor& color) const;
	//! ��������� !=
	bool operator != (const CColor& color) const;
	//! �ӷ�����
	CColor operator + (const CColor& color) const;
	//! ��������
	CColor operator - (const CColor& color) const;
	//! �˷�����
	CColor operator * (const CColor& color) const;

	//! ��������� +=
	CColor operator += (const CColor& color);
	//! ��������� -=
	CColor operator -= (const CColor& color);
	//! ��������� *=
	CColor operator *= (const CColor& color);

public:
	/**
	* ��ɫ�洢��ʽ
	* [R, G, B, A]
	*/
	float m_fValue[4];
	//! ͸������
	const static CColor Transparent;
	//! ��ɫ����
	const static CColor White;
	//! ��ɫ����
	const static CColor Gray;
	//! ��ɫ����
	const static CColor Black;
	//! ��ɫ����
	const static CColor Red;
	//! ��ɫ����
	const static CColor Green;
	//! ��ɫ����
	const static CColor Blue;
	//! ��ɫ����
	const static CColor Yellow;
	//! ��ɫ����
	const static CColor Purple;
};

#endif