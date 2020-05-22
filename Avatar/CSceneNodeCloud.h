//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODECLOUD_H_
#define _CSCENENODECLOUD_H_
#include "CSceneNode.h"

/**
* @brief �Ʋ㳡���ڵ�
*/
class AVATAR_EXPORT CSceneNodeCloud: public CSceneNode {
public:
	//! ���췽��
	CSceneNodeCloud(const string& name, const CColor& skyColor, const CColor& cloudColor, float cloudSize);

	//! ��ʼ�������ڵ�
	virtual bool Init();
	//! ���ٳ����ڵ�
	virtual void Destroy();
	//! ��Ⱦ�����ڵ�
	virtual void Render();
	//! �����߼�
	virtual void Update(float dt);

private:
	//! �Ʋ�ģ��ʱ��
	float m_fTimeElapse;
	//! �����ɫ
	CColor m_cSkyColor;
	//! �ƶ���ɫ
	CColor m_cCloudColor;
	//! ���Ŵ�С
	float m_fCloudSize;
	//! �Ʋ��������
	CMesh* m_pMesh;
};

#endif