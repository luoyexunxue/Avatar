//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CVECTOR3_H_
#define _CVECTOR3_H_
#include "AvatarConfig.h"

/**
* @brief ��ά������
*/
class AVATAR_EXPORT CVector3 {
public:
	//! Ĭ�Ϲ��캯��
	CVector3();
	//! ���ƹ��캯��
	CVector3(const CVector3& vector);
	//! ʹ�õ�һֵ��ֵ
	CVector3(float val);
	//! ʹ�����鹹������
	CVector3(const float vector[3]);
	//! ʹ��xyz��������
	CVector3(float x, float y, float z);
	//! ʹ�������w��������
	CVector3(const float vector[3], float w);
	//! ʹ��xyzw��������
	CVector3(float x, float y, float z, float w);

	//! ��������ֵ
	void SetValue(const CVector3& vector);
	//! ��������ֵ
	void SetValue(const float vector[3]);
	//! ��������ֵ
	void SetValue(float x, float y, float z);
	//! ��������ֵ
	void SetValue(const float vector[3], float w);
	//! ��������ֵ
	void SetValue(float x, float y, float z, float w);

	//! ת��Ϊ��λ����
	CVector3& Normalize();
	//! ת��Ϊ����
	CVector3& Inverse();

	//! ��ָ��ֵ
	CVector3& Add(const CVector3& vector);
	//! ��ָ��ֵ
	CVector3& Add(const float vector[3]);
	//! ��ָ��ֵ
	CVector3& Add(float x, float y, float z);
	//! ��ָ��ֵ
	CVector3& Sub(const CVector3& vector);
	//! ��ָ��ֵ
	CVector3& Sub(const float vector[3]);
	//! ��ָ��ֵ
	CVector3& Sub(float x, float y, float z);
	//! ����ָ��ֵ
	CVector3& Scale(float scale);
	//! ����ָ��ֵ
	CVector3& Scale(float sx, float sy, float sz);

	//! ȡ�������ĳ���
	float Length() const;
	//! �������
	float DotProduct(const CVector3& vector) const;
	//! �������
	CVector3 CrossProduct(const CVector3& vector) const;
	//! ���Բ�ֵ
	CVector3 Lerp(const CVector3& vector, float t) const;
	//! ���β�ֵ
	CVector3 Slerp(const CVector3& vector, float t) const;
	//! ��������
	CVector3 Reflect(const CVector3& normal) const;
	//! ����������
	CVector3 Tangent() const;

public:
	//! �±������ []
	float& operator [] (int index);
	//! ��������� ==
	bool operator == (const CVector3& vector) const;
	//! ��������� !=
	bool operator != (const CVector3& vector) const;

	//! �ӷ�����
	CVector3 operator + (const CVector3& vector) const;
	//! ��������
	CVector3 operator - (const CVector3& vector) const;
	//! �˷�����
	CVector3 operator * (const CVector3& vector) const;
	//! ����������
	CVector3 operator * (float factor) const;
	//! ����������
	CVector3 operator / (float factor) const;

	//! ��������� +=
	CVector3& operator += (const CVector3& vector);
	//! ��������� -=
	CVector3& operator -= (const CVector3& vector);
	//! ��������� *=
	CVector3& operator *= (float factor);
	//! ��������� /=
	CVector3& operator /= (float factor);

public:
	/**
	* �����洢��ʽ
	* [x, y, z, w]
	*/
	float m_fValue[4];
	//! ����������
	const static CVector3 Zero;
	//! ��λһ����
	const static CVector3 One;
	//! ��λX��������
	const static CVector3 X;
	//! ��λY��������
	const static CVector3 Y;
	//! ��λZ��������
	const static CVector3 Z;
};

#endif