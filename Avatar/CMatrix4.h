//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CMATRIX4_H_
#define _CMATRIX4_H_
#include "AvatarConfig.h"
#include "CVector3.h"
class CQuaternion;
class CPlane;

/**
* @brief 4x4������
*/
class AVATAR_EXPORT CMatrix4 {
public:
	//! Ĭ�Ϲ��캯��
	CMatrix4();
	//! ���ƹ��캯��
	CMatrix4(const CMatrix4& matrix);
	//! ʹ��4x4�����鹹�����
	CMatrix4(const float matrix[16]);

	//! ���þ���ֵ
	void SetValue(const CMatrix4& matrix);
	//! ���þ���ֵ
	void SetValue(const float matrix[16]);

	//! ����Ϊ��λ����
	CMatrix4& SetIdentity();
	//! ���������
	CMatrix4& Invert();
	//! ����ת�þ���
	CMatrix4& Transpose();
	//! ����б�Գƾ���
	CMatrix4& SkewSymmetric(const CVector3& vector);
	//! ���㱻���ŵľ���
	CMatrix4& SetScaled(const CVector3& scale);

	//! ���Ų���
	CMatrix4& Scale(const CVector3& scale);
	//! ���Ų���
	CMatrix4& Scale(float dx, float dy, float dz);

	//! �� X ����ת����
	CMatrix4& RotateX(float angle);
	//! �� Y ����ת����
	CMatrix4& RotateY(float angle);
	//! �� Z ����ת����
	CMatrix4& RotateZ(float angle);

	//! ƽ�Ʋ���
	CMatrix4& Translate(const CVector3& pos);
	//! ƽ�Ʋ���
	CMatrix4& Translate(float dx, float dy, float dz);

	//! ���ɱ任����(˳�� scale, rot, pos)
	void MakeTransform(const CVector3& scale, const CMatrix4& rot, const CVector3& pos);
	//! ���ɱ任����(˳�� scale, rot, pos)
	void MakeTransform(const CVector3& scale, const CQuaternion& rot, const CVector3& pos);

	//! �����бƽ��ü�ͶӰ����
	CMatrix4& ObliqueNearPlaneClipping(const CMatrix4& proj, const CPlane& clip);
	//! ������ͼ����
	CMatrix4& LookAt(const CVector3& pos, const CVector3& look, const CVector3& up);
	//! ��������ͶӰ����
	CMatrix4& Ortho(float width, float height, float zNear, float zFar);
	//! ����͸��ͶӰ����
	CMatrix4& Perspective(float fov, float aspect, float zNear, float zFar);

public:
	//! �±������ []
	float& operator [] (int index);
	//! ��������� ()
	float& operator () (int row, int col);
	//! ��������� ==
	bool operator == (const CMatrix4& matrix) const;
	//! ��������� !=
	bool operator != (const CMatrix4& matrix) const;
	//! �ӷ�����
	CMatrix4 operator + (const CMatrix4& matrix) const;
	//! ��������
	CMatrix4 operator - (const CMatrix4& matrix) const;
	//! �˷�����
	CMatrix4 operator * (const CMatrix4& matrix) const;
	//! ����������
	CVector3 operator * (const CVector3& vector) const;
	//! ����Ԫ������
	CQuaternion operator * (const CQuaternion& rotation) const;
	//! ����������
	CMatrix4 operator * (float factor) const;

	//! ��������� +=
	CMatrix4& operator += (const CMatrix4& matrix);
	//! ��������� -=
	CMatrix4& operator -= (const CMatrix4& matrix);
	//! ��������� *=
	CMatrix4& operator *= (const CMatrix4& matrix);
	//! ��������� *=
	CMatrix4& operator *= (float factor);

public:
	/**
	* ����洢��ʽ���ô洢��ʽ�� OpenGL ����
	* 0 4 8 12
	* 1 5 9 13
	* 2 6 10 14
	* 3 7 11 15
	*/
	float m_fValue[16];
	//! ��λ������
	const static CMatrix4 Identity;
};

#endif