//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CVERTEXJOINT_H_
#define _CVERTEXJOINT_H_
#include "AvatarConfig.h"

/**
* @brief �����������
*/
class AVATAR_EXPORT CVertexJoint {
public:
	//! Ĭ�Ϲ��캯��
	CVertexJoint();
	//! ���ƹ��캯��
	CVertexJoint(const CVertexJoint& joint);
	//! �������һ������Ӱ���ʵ��
	CVertexJoint(int jointId);
	//! �����ĸ�����Ϣʵ��
	CVertexJoint(const int jointId[4], const float weight[4]);

	//! ��Ӱ�
	CVertexJoint& AddBind(int jointId, float weight);
	//! ��Ӱ�
	CVertexJoint& AddBind(const CVertexJoint& bind, float scale);
	//! ��λ��Ȩ��
	CVertexJoint& Normalize();

public:
	//! ��ֵ�����
	CVertexJoint& operator = (const CVertexJoint& joint);

public:
	//! �󶨵�����(���Ϊ4)
	int m_iCount;
	//! ����ID����
	int m_iJointId[4];
	//! ��Ȩ������
	float m_fWeight[4];
};

#endif