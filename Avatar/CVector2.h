//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CVECTOR2_H_
#define _CVECTOR2_H_
#include "AvatarConfig.h"

/**
* @brief ��ά������
*/
class AVATAR_EXPORT CVector2 {
public:
	//! Ĭ�Ϲ��캯��
	CVector2();
	//! ���ƹ��캯��
	CVector2(const CVector2& vector);
	//! ʹ�õ�һֵ��ֵ
	CVector2(float val);
	//! ʹ�����鹹������
	CVector2(const float vector[2]);
	//! ʹ��xy��������
	CVector2(float x, float y);
	//! ʹ��xy��������
	CVector2(int x, int y);

	//! ��������ֵ
	void SetValue(const CVector2& vector);
	//! ��������ֵ
	void SetValue(const float vector[2]);
	//! ��������ֵ
	void SetValue(float x, float y);

	//! ת��Ϊ��λ����
	CVector2& Normalize();

	//! ��ָ��ֵ
	CVector2& Add(const CVector2& vector);
	//! ��ָ��ֵ
	CVector2& Add(const float vector[2]);
	//! ��ָ��ֵ
	CVector2& Add(float x, float y);
	//! ��ָ��ֵ
	CVector2& Sub(const CVector2& vector);
	//! ��ָ��ֵ
	CVector2& Sub(const float vector[2]);
	//! ��ָ��ֵ
	CVector2& Sub(float x, float y);
	//! ����ָ��ֵ
	CVector2& Scale(float scale);
	//! ����ָ��ֵ
	CVector2& Scale(float sx, float sy);

	//! ȡ�������ĳ���
	float Length() const;
	//! �������
	float DotProduct(const CVector2& vector) const;
	//! �������
	float CrossProduct(const CVector2& vector) const;
	//! ���Բ�ֵ
	CVector2 Lerp(const CVector2& vector, float t) const;
	//! ���β�ֵ
	CVector2 Slerp(const CVector2& vector, float t) const;
	//! ��������
	CVector2 Reflect(const CVector2& normal) const;

public:
	//! �±������ []
	float& operator [] (int index);
	//! ��������� ==
	bool operator == (const CVector2& vector) const;
	//! ��������� !=
	bool operator != (const CVector2& vector) const;

	//! �ӷ�����
	CVector2 operator + (const CVector2& vector) const;
	//! ��������
	CVector2 operator - (const CVector2& vector) const;
	//! ����������
	CVector2 operator * (float factor) const;
	//! ����������
	CVector2 operator / (float factor) const;

	//! ��������� +=
	CVector2& operator += (const CVector2& vector);
	//! ��������� -=
	CVector2& operator -= (const CVector2& vector);
	//! ��������� *=
	CVector2& operator *= (float factor);
	//! ��������� /=
	CVector2& operator /= (float factor);

public:
	/**
	* �����洢��ʽ
	* [x, y]
	*/
	float m_fValue[2];
	//! ����������
	const static CVector2 Zero;
	//! ��λX��������
	const static CVector2 X;
	//! ��λY��������
	const static CVector2 Y;
};

#endif