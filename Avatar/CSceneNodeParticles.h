//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEPARTICLES_H_
#define _CSCENENODEPARTICLES_H_
#include "CSceneNode.h"
#include <vector>
using std::vector;

/**
* @brief ����Ⱥ�����ڵ�
*/
class AVATAR_EXPORT CSceneNodeParticles: public CSceneNode {
public:
	//! ���췽��
	CSceneNodeParticles(const string& name, const string& texture, float size, int count, bool dark,
		const CColor& color, const float speed[3], float spread, float fade, bool loop);

	//! ��ʼ�������ڵ�
	virtual bool Init();
	//! ���ٳ����ڵ�
	virtual void Destroy();
	//! ��Ⱦ�����ڵ�
	virtual void Render();
	//! ���³����ڵ�
	virtual void Update(float dt);

	//! ��ʼ������
	void InitParticles(const float initSpeed[3], float spreadSpeed, float fadeSpeed);

private:
	//! ��������ɺ���
	inline float Random();

private:
	//! ���ӽṹ��
	typedef struct _SParticle {
		bool active;
		float fade;
		float color[4];
		float speed[3];
		float position[3];
		float distance;
	} SParticle;

private:
	//! �Ƿ�ѭ��
	bool m_bLoopParticles;
	//! �����ģʽ
	bool m_bDarkBlendMode;
	//! ��ʧ�ٶ�
	float m_fFadeSpeed;
	//! ��������ٶ�
	float m_fSpreadSpeed;
	//! ���ӳ�ʼ�ٶ�
	float m_fInitSpeed[3];
	//! ���Ӵ�С
	float m_fParticleSize;
	//! ���ӳ�ʼ��ɫ
	float m_fParticleColor[4];
	//! ��������
	string m_strTexture;
	//! ����Ⱥ�������
	CMesh* m_pMesh;
	//! ��������
	vector<SParticle> m_vecParticles;
	//! ������������
	size_t* m_pSortIndex;
};

#endif