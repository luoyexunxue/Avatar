//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEDECAL_H_
#define _CSCENENODEDECAL_H_
#include "CSceneNode.h"
#include "CMatrix4.h"
#include "CFrustum.h"
#include "CPlane.h"

/**
* @brief ���������ڵ�
* @attention ��Ҫָ�����ڵ㣬�������ڸ��ڵ��Ͻ���
*/
class AVATAR_EXPORT CSceneNodeDecal: public CSceneNode {
public:
	//! ���췽��
	CSceneNodeDecal(const string& name, const string& texture, const CMatrix4& proj);

	//! ��ʼ�������ڵ�
	virtual bool Init();
	//! ���ٳ����ڵ�
	virtual void Destroy();
	//! ��Ⱦ�����ڵ�
	virtual void Render();
	//! ���±任����
	virtual void Transform();

private:
	//! ��������ͶӰ�Ӿ���
	void SetupFrustum();
	//! ������������
	void UpdateMesh();
	//! ����������ͶӰ���ڵĽ���
	int IntersectWithFrustum(const CVector3& p1, const CVector3& p2, const CVector3& p3, float* result);

private:
	//! ��������
	string m_strTexture;
	//! ����ͶӰ����
	CMatrix4 m_cDecalMatrix;
	//! ����ͶӰ���ɵ��Ӿ���(��������ϵ)
	CFrustum m_cDecalFrustum;
	//! ����ͶӰ�Ӿ���6�ü�ƽ��
	CPlane m_cClipPlane[6];
	//! ��������ͶӰ����
	CMatrix4 m_cTexCoordProj;
	//! �������
	CMesh* m_pMesh;
};

#endif