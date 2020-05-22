//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEBOARD_H_
#define _CSCENENODEBOARD_H_
#include "CSceneNode.h"

/**
* @brief ��ͼƽ�泡���ڵ�
* @attention ����ýڵ�Ϊ��������������ӽڵ㣬�����Ⱦ�ӽڵ�
*/
class AVATAR_EXPORT CSceneNodeBoard: public CSceneNode {
public:
	//! ���췽��
	CSceneNodeBoard(const string& name, const string& texture, float width, float height, int billboard);

	//! ��ʼ�������ڵ�
	virtual bool Init();
	//! ���ٳ����ڵ�
	virtual void Destroy();
	//! ��Ⱦ�����ڵ�
	virtual void Render();
	//! ���³����ڵ�
	virtual void Update(float dt);
	//! ��ȡ��������
	virtual CMeshData* GetMeshData();

	//! ý����Ϣ
	void MediaInfo(int* width, int* height, float* length);
	//! ������ת��
	void SetAxis(const CVector3& axis);

private:
	//! ��ͼ���������Ƶ����
	CTexture* CreateTexture(const string& texture);

private:
	//! ��������
	string m_strTexture;
	//! ƽ����
	float m_fWidth;
	//! ƽ��߶�
	float m_fHeight;
	//! ���������
	int m_iBillboardType;
	//! ��ת��
	CVector3 m_cAxis;
	//! �����ID
	int m_iCameraId;
	//! ��Ƶ������
	void* m_pVideoContext;
	//! ��Ƶ��ID
	int m_iSoundId;
	//! ��Ƶ�����С
	int m_iSoundBufferSize;
	//! ��Ƶ������
	unsigned char m_pSoundBuffer[8192];
	//! �ڴ��ļ�
	unsigned char* m_pFileBuffer;
	//! ʱ���¼
	float m_fTimeElapse;
	//! �������
	CMeshData* m_pMeshData;
	//! �������
	CTexture* m_pTexture;
};

#endif