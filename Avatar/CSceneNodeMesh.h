//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEMESH_H_
#define _CSCENENODEMESH_H_
#include "CSceneNode.h"

/**
* @brief ����ģ�ͳ����ڵ�
*/
class AVATAR_EXPORT CSceneNodeMesh: public CSceneNode {
public:
	//! ���췽��
	CSceneNodeMesh(const string& name, CMesh* mesh);

	//! ��ʼ�������ڵ�
	virtual bool Init();
	//! ���ٳ����ڵ�
	virtual void Destroy();
	//! ��Ⱦ�����ڵ�
	virtual void Render();
	//! ��ȡ��������
	virtual CMeshData* GetMeshData();

private:
	//! �������
	CMesh* m_pMesh;
	//! �����������
	CMeshData* m_pMeshData;
};

#endif