//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODELENSFLARE_H_
#define _CSCENENODELENSFLARE_H_
#include "CSceneNode.h"

/**
* @brief ��ͷ���γ����ڵ�
*/
class AVATAR_EXPORT CSceneNodeLensflare: public CSceneNode {
public:
	//! ���췽��
	CSceneNodeLensflare(const string& name);

	//! ��ʼ�������ڵ�
	virtual bool Init();
	//! ���ٳ����ڵ�
	virtual void Destroy();
	//! ��Ⱦ�����ڵ�
	virtual void Render();
	//! ���³����ڵ�
	virtual void Update(float dt);
	//! ��������任
	virtual void Transform() {}

private:
	//! ���νṹ��
	typedef struct _SLensFlare {
		float offset;
		float size;
		float color[4];
		CTexture* texture;
		/**
		* ���ù��β���
		*/
		void SetParameter(float offset, float size, float r, float g, float b, float a, CTexture* texture) {
			this->offset = offset;
			this->size = size;
			this->color[0] = r;
			this->color[1] = g;
			this->color[2] = b;
			this->color[3] = a;
			this->texture = texture;
		}
	} SLensFlare;

private:
	//! �����б�
	SLensFlare m_sLensFlares[6];
	//! ̫������
	CTexture* m_pTextureFlare;
	//! ��������1
	CTexture* m_pTextureGlow;
	//! ��������2
	CTexture* m_pTextureHalo;
	//! ������Ⱦ����
	CMesh* m_pMesh;
	//! ̫����λ��
	CVector3 m_cSunPosition;
	//! ��Ļ����λ��
	CVector2 m_cCenterPos;
	//! ̫������Ļ��λ��
	CVector3 m_cScreenPos;
};

#endif