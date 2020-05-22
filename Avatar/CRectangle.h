//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CRECTANGLE_H_
#define _CRECTANGLE_H_
#include "AvatarConfig.h"

/**
* @brief ������
*/
class AVATAR_EXPORT CRectangle {
public:
	//! Ĭ�Ϲ��캯��
	CRectangle();
	//! ���ƹ��캯��
	CRectangle(const CRectangle& rect);
	//! �����鹹�����
	CRectangle(const int rect[4]);
	//! �����Ͻ�xy����Ϳ�߹������
	CRectangle(int x, int y, int w, int h);

	//! ���þ���ֵ
	void SetValue(const CRectangle& rect);
	//! ���þ���ֵ
	void SetValue(const int rect[4]);
	//! ���þ���ֵ
	void SetValue(int x, int y, int w, int h);

	//! ����ƫ��
	CRectangle& Offset(int x, int y);
	//! ����ƫ��
	CRectangle& Offset(int offset[2]);
	//! ����ƫ��
	CRectangle& Offset(const CRectangle& rect);
	//! ��������
	CRectangle& Scale(float scale, bool center);

	//! ��������Ϊ��
	void SetEmpty();
	//! �ж������Ƿ�Ϊ��
	bool IsEmpty() const;
	//! �ж��Ƿ��н���
	bool IsIntersect(const CRectangle& rect) const;
	//! �ж��Ƿ������
	bool IsContain(int x, int y) const;
	//! �ж��Ƿ��������
	bool IsContain(const CRectangle& rect) const;

	//! ��ȡ������߽�
	int GetLeft() const;
	//! ��ȡ�����ϱ߽�
	int GetTop() const;
	//! ��ȡ�����ұ߽�
	int GetRight() const;
	//! ��ȡ�����±߽�
	int GetBottom() const;
	//! ��ȡ���ο��
	int GetWidth() const;
	//! ��ȡ���θ߶�
	int GetHeight() const;
	//! ��ȡ�������
	int GetArea() const;

	//! �� rect �󲢼�
	CRectangle Union(const CRectangle& rect) const;
	//! �� rect �󽻼�
	CRectangle Intersect(const CRectangle& rect) const;

public:
	//! �±������ []
	int& operator [] (int index);
	//! ��������� ==
	bool operator == (const CRectangle& rect) const;
	//! ��������� !=
	bool operator != (const CRectangle& rect) const;
	//! ��������
	CRectangle& operator += (const CRectangle& rect);
	//! ��������
	CRectangle& operator *= (const CRectangle& rect);

public:
	/**
	* ���δ洢��ʽ
	* [x, y, w, h]
	*/
	int m_iValue[4];
	//! �վ��γ���
	const static CRectangle Empty;
};

#endif