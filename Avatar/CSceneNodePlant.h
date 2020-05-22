//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEPLANT_H_
#define _CSCENENODEPLANT_H_
#include "CSceneNode.h"

/**
* @brief ֲ��ģ�ͳ����ڵ�
* @attention �˽ڵ������ڵ��νڵ�
*/
class AVATAR_EXPORT CSceneNodePlant: public CSceneNode {
public:
	//! ���췽��
	CSceneNodePlant(const string& name, const string& texture, float width, float height,
		const string& distributionMap, int count, float density, float range);

	//! ��ʼ�������ڵ�
	virtual bool Init();
	//! ���ٳ����ڵ�
	virtual void Destroy();
	//! ��Ⱦ�����ڵ�
	virtual void Render();

private:
	//! ����ֲ����
	CMesh* BuildMesh();
	//! ����ֲ��������
	void AddPlant(CMesh* mesh, float x, float y, float z, float angle);

private:
	//! ֲ������
	string m_strTexture;
	//! ֲ�����
	float m_fWidth;
	//! ֲ���߶�
	float m_fHeight;
	//! �ֲ�ͼ
	string m_strDistributionMap;
	//! ֲ������
	int m_iTotalCount;
	//! �ֲ��ܶ�
	float m_fDensity;
	//! �ֲ���Χ
	float m_fRange;
	//! �������
	CMesh* m_pMesh;
};

#endif