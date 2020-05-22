//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEGEOMETRY_H_
#define _CSCENENODEGEOMETRY_H_
#include "CSceneNode.h"
#include "CGeometryCreator.h"

/**
* @brief �����峡���ڵ�
*/
class AVATAR_EXPORT CSceneNodeGeometry: public CSceneNode {
public:
	//! ���췽��
	CSceneNodeGeometry(const string& name, const string& texture, const SGeometry& geometry);

	//! ��ʼ�������ڵ�
	virtual bool Init();
	//! ���ٳ����ڵ�
	virtual void Destroy();
	//! ��Ⱦ�����ڵ�
	virtual void Render();
	//! ��ȡ��������
	virtual CMeshData* GetMeshData();

	//! ��ȡ���β���
	SGeometry GetGeometry(bool worldspace);

private:
	//! ����������
	string m_strTexture;
	//! ���β���
	SGeometry m_sGeometry;
	//! �������
	CMeshData* m_pMeshData;
};

#endif