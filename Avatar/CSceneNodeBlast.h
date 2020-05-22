//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEBLAST_H_
#define _CSCENENODEBLAST_H_
#include "CSceneNode.h"

/**
* @brief ��ըģ�ⳡ���ڵ�
*/
class AVATAR_EXPORT CSceneNodeBlast: public CSceneNode {
public:
	//! ���췽��
	CSceneNodeBlast(const string& name, const string& texture, int row, int column);

	//! ��ʼ�������ڵ�
	virtual bool Init();
	//! ���ٳ����ڵ�
	virtual void Destroy();
	//! ��Ⱦ�����ڵ�
	virtual void Render();
	//! ģ�����
	virtual void Update(float dt);

	//! ��ը��ʼ��
	void InitBlast(float duration, float size);
	//! �ж��Ƿ����
	bool IsFinished() { return !m_bBlast; }

private:
	//! ��ըģ��
	bool m_bBlast;
	//! ��ը����
	string m_strTexture;
	//! ģ��ʱ��
	float m_fTimeElapse;
	//! ����ʱ��
	float m_fDuration;
	//! ����֡
	int m_iFrameRow;
	//! ����֡
	int m_iFrameCol;
	//! ������
	int m_iTotalRow;
	//! ������
	int m_iTotalCol;
	//! �������
	CMesh* m_pMesh;
};

#endif