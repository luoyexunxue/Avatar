//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CVERTEX_H_
#define _CVERTEX_H_
#include "AvatarConfig.h"
#include "CVector2.h"
#include "CVector3.h"
#include "CColor.h"

/**
* @brief ������
*/
class AVATAR_EXPORT CVertex {
public:
	//! Ĭ�Ϲ��캯��
	CVertex();
	//! ���ƹ��캯��
	CVertex(const CVertex& vertex);
	//! ʹ�ö������깹�춥��
	CVertex(float x, float y, float z);
	//! ʹ�ö���������������깹�춥��
	CVertex(float x, float y, float z, float s, float t);
	//! ʹ�ö�����������������Լ����������춥��
	CVertex(float x, float y, float z, float s, float t, float nx, float ny, float nz);
	//! ʹ�ö�����������������Լ�����������ɫ���춥��
	CVertex(float x, float y, float z, float s, float t, float nx, float ny, float nz, const float color[4]);

	//! ���ö�������
	void SetPosition(const CVector3& pos);
	//! ���ö�������
	void SetPosition(const float pos[3]);
	//! ���ö�������
	void SetPosition(float x, float y, float z);

	//! ������������
	void SetTexCoord(const CVector2& texCoord);
	//! ������������
	void SetTexCoord(const float texCoord[2]);
	//! ������������
	void SetTexCoord(float s, float t);

	//! ���ö��㷨��
	void SetNormal(const CVector3& normal);
	//! ���ö��㷨��
	void SetNormal(const float normal[3]);
	//! ���ö��㷨��
	void SetNormal(float nx, float ny, float nz);

	//! ���ö�����ɫ
	void SetColor(const CColor& color);
	//! ���ö�����ɫ
	void SetColor(const float color[4]);
	//! ���ö�����ɫ
	void SetColor(float r, float g, float b, float a);

public:
	//! ��ת������
	void FlipNormal();
	//! ���Բ�ֵ
	CVertex Interpolate(const CVertex& vertex, float t);

public:
	//! ��ֵ�����
	CVertex& operator = (const CVertex& vertex);
	//! ��������� ==
	bool operator == (const CVertex& vertex) const;
	//! ��������� !=
	bool operator != (const CVertex& vertex) const;

public:
	//! ��������
	float m_fPosition[4];
	//! ��������
	float m_fTexCoord[2];
	//! ���㷨����
	float m_fNormal[4];
	//! ������ɫ
	float m_fColor[4];
};

#endif