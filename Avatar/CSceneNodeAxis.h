//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEAXIS_H_
#define _CSCENENODEAXIS_H_
#include "CSceneNode.h"

/**
* @brief ������ָʾ�������ڵ�
*/
class AVATAR_EXPORT CSceneNodeAxis: public CSceneNode {
public:
	//! ���췽��
	CSceneNodeAxis(const string& name);

	//! ��ʼ�������ڵ�
	virtual bool Init();
	//! ���ٳ����ڵ�
	virtual void Destroy();
	//! ��Ⱦ�����ڵ�
	virtual void Render();
	//! ���³����ڵ�
	virtual void Update(float dt);

private:
	//! �������
	CMesh* m_pMesh;
};

#endif