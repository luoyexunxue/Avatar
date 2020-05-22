//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEFLAME_H_
#define _CSCENENODEFLAME_H_
#include "CSceneNode.h"

/**
* @brief ����ģ�ⳡ���ڵ�
*/
class AVATAR_EXPORT CSceneNodeFlame: public CSceneNode {
public:
	//! ���췽��
	CSceneNodeFlame(const string& name, const string& texture, const string& distortionMap,
		const string& alphaMask, float width, float height);

	//! ��ʼ�������ڵ�
	virtual bool Init();
	//! ���ٳ����ڵ�
	virtual void Destroy();
	//! ��Ⱦ�����ڵ�
	virtual void Render();
	//! ģ�����
	virtual void Update(float dt);

private:
	//! ģ��ʱ��
	float m_fTimeElapse;
	//! ��������
	string m_strTexture;
	//! �����Ŷ�ͼ
	string m_strDistortionMap;
	//! ͸���ɰ�
	string m_strAlphaMask;
	//! ���
	float m_fWidth;
	//! �߶�
	float m_fHeight;
	//! �������
	CMesh* m_pMesh;
};

#endif