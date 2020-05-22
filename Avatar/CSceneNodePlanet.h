//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEPLANET_H_
#define _CSCENENODEPLANET_H_
#include "CSceneNode.h"

/**
* @brief ����ģ�ͳ����ڵ�
*/
class AVATAR_EXPORT CSceneNodePlanet: public CSceneNode {
public:
	//! ���췽��
	CSceneNodePlanet(const string& name, const string& texture, const string& textureNight, float radius, int slices);

	//! ��ʼ�������ڵ�
	virtual bool Init();
	//! ���ٳ����ڵ�
	virtual void Destroy();
	//! ��Ⱦ�����ڵ�
	virtual void Render();
	//! ��ȡ��������
	virtual CMeshData* GetMeshData();

private:
	//! ����뾶
	float m_fRadius;
	//! ϸ�ֶ�
	int m_iTessellation;
	//! ��������
	string m_strTexture;
	//! ҹ���������
	string m_strTextureNight;
	//! �������������
	CMesh* m_pMeshSky;
	//! ��������
	CMeshData* m_pMeshData;
};

#endif