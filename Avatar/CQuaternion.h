//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CQUATERNION_H_
#define _CQUATERNION_H_
#include "AvatarConfig.h"
#include "CMatrix4.h"
#include "CVector3.h"

/**
* @brief ��Ԫ����
*/
class AVATAR_EXPORT CQuaternion {
public:
	//! Ĭ�Ϲ��캯��
	CQuaternion();
	//! ���ƹ��캯��
	CQuaternion(const CQuaternion& quaternion);
	//! ʹ�����鹹����Ԫ��
	CQuaternion(const float quaternion[4]);
	//! ʹ��xyzw������Ԫ��
	CQuaternion(float x, float y, float z, float w);

	//! ������Ԫ��ֵ
	void SetValue(const CQuaternion& quaternion);
	//! ������Ԫ��ֵ
	void SetValue(float x, float y, float z, float w);

	//! ��λ��
	CQuaternion& Normalize();
	//! ����
	CQuaternion& Conjugate();
	//! ȡ��
	CQuaternion& Inverse();

	//! ��ŷ����תΪ��Ԫ��
	CQuaternion& FromEulerAngles(float x, float y, float z);
	//! �����תΪ��Ԫ��
	CQuaternion& FromAxisAngle(const CVector3& axis, float angle);
	//! ������תΪ��Ԫ��
	CQuaternion& FromVector(const CVector3& from, const CVector3& to);
	//! �ɾ���תΪ��Ԫ��
	CQuaternion& FromMatrix(const CMatrix4& matrix);

	//! ת��Ϊŷ���Ǳ�ʾ
	void ToEulerAngles(float* x, float* y, float* z) const;
	//! ת��Ϊ��Ǳ�ʾ
	void ToAxisAngle(CVector3& axis, float* angle) const;
	//! ת��Ϊ�����ʾ
	CMatrix4 ToMatrix() const;

	//! ���β�ֵ
	CQuaternion Slerp(const CQuaternion& quaternion, float t) const;
	//! ���·�����β�ֵ
	CQuaternion SlerpShortest(const CQuaternion& quaternion, float t) const;

public:
	//! �±������ []
	float& operator [] (int index);
	//! ��������� ==
	bool operator == (const CQuaternion& quaternion) const;
	//! ��������� !=
	bool operator != (const CQuaternion& quaternion) const;

	//! �ӷ�����
	CQuaternion operator + (const CQuaternion& quaternion) const;
	//! ��������
	CQuaternion operator - (const CQuaternion& quaternion) const;
	//! �˷�����
	CQuaternion operator * (const CQuaternion& quaternion) const;
	//! ��������
	CQuaternion operator * (float scale) const;
	//! ����������
	CVector3 operator * (const CVector3& vector) const;

	//! ��������� +=
	CQuaternion& operator += (const CQuaternion& quaternion);
	//! ��������� -=
	CQuaternion& operator -= (const CQuaternion& quaternion);
	//! ��������� *=
	CQuaternion& operator *= (const CQuaternion& quaternion);
	//! ��������� *=
	CQuaternion& operator *= (float scale);

public:
	/**
	* ��Ԫ���洢��ʽ
	* [x, y, z, w]
	*/
	float m_fValue[4];
};

#endif