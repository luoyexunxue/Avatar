//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEWATER_H_
#define _CSCENENODEWATER_H_
#include "CSceneNode.h"

/**
* @brief ˮ�泡���ڵ�
*/
class AVATAR_EXPORT CSceneNodeWater: public CSceneNode {
public:
	//! ���췽��
	CSceneNodeWater(const string& name, const string& normalMap, float depth);

	//! ��ʼ�������ڵ�
	virtual bool Init();
	//! ���ٳ����ڵ�
	virtual void Destroy();
	//! ��Ⱦ�����ڵ�
	virtual void Render();
	//! ���³����ڵ�
	virtual void Update(float dt);

private:
	//! ����ˮ����Ļ�ɰ�
	void UpdateUnderwaterMask();

private:
	//! ģ��Ĳ�����ʱ��
	float m_fTimeElapse;
	//! ˮ�沨�Ʒ�����ͼ
	string m_strNormalMap;
	//! ˮ���������
	CMesh* m_pMesh;
	//! ��Ӱ RTT ��ͼ
	CTexture* m_pReflectTexture;
	//! ��Ļˮ���ɰ�ڵ�
	CSceneNode* m_pUnderwaterNode;
};

#endif