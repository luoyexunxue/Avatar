//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODESKYBOX_H_
#define _CSCENENODESKYBOX_H_
#include "CSceneNode.h"

/**
* @brief ��պг����ڵ�
*/
class AVATAR_EXPORT CSceneNodeSkybox: public CSceneNode {
public:
	//! ���췽��
	CSceneNodeSkybox(const string& name, const string texture[6]);

	//! ��ʼ�������ڵ�
	virtual bool Init();
	//! ���ٳ����ڵ�
	virtual void Destroy();
	//! ��Ⱦ�����ڵ�
	virtual void Render();
	//! �����߼�
	virtual void Update(float dt);

private:
	//! ��պ�����
	string m_strTexture[6];
	//! �������
	CMesh* m_pMesh;
};

#endif