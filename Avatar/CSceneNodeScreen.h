//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODESCREEN_H_
#define _CSCENENODESCREEN_H_
#include "CSceneNode.h"

/**
* @brief ��Ļ��ʾ�����ڵ�
*/
class AVATAR_EXPORT CSceneNodeScreen: public CSceneNode {
public:
	//! ���췽��
	CSceneNodeScreen(const string& name, const string& texture, int width, int height);

	//! ��ʼ�������ڵ�
	virtual bool Init();
	//! ���ٳ����ڵ�
	virtual void Destroy();
	//! ��Ⱦ�����ڵ�
	virtual void Render();
	//! ��ȡ��������
	virtual CMeshData* GetMeshData();

private:
	//! ��Ⱦ��ȣ����أ�
	int m_iWidth;
	//! ��Ⱦ�߶ȣ����أ�
	int m_iHeight;
	//! ͼƬ����
	string m_strTexture;
	//! �������
	CMeshData* m_pMeshData;
};

#endif