//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODESTATIC_H_
#define _CSCENENODESTATIC_H_
#include "CSceneNode.h"
#include "CMeshData.h"

/**
* @brief ��̬ģ�ͳ����ڵ�
*/
class AVATAR_EXPORT CSceneNodeStatic: public CSceneNode {
public:
	//! ���췽��
	CSceneNodeStatic(const string& name, const string& meshFile);

	//! ��ʼ�������ڵ�
	virtual bool Init();
	//! ���ٳ����ڵ�
	virtual void Destroy();
	//! ��Ⱦ�����ڵ�
	virtual void Render();
	//! ��ȡ��������
	virtual CMeshData* GetMeshData();

private:
	//! ģ���ļ�
	string m_strFile;
	//! ģ������
	CMeshData* m_pMeshData;
};

#endif