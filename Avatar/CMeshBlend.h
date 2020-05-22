//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CMESHBLEND_H_
#define _CMESHBLEND_H_
#include "AvatarConfig.h"
#include "CMesh.h"
#include <vector>
using std::vector;

/**
* @brief ����ģ�ͻ����
*
* ͨ��ָ������������ͨ��Ȩ�ؽ��л�ϣ����ж�����ζ�����
* ע��������֮�䶥����������ͬ�������α���һ�¡�
*/
class AVATAR_EXPORT CMeshBlend {
public:
	//! Ĭ�Ϲ��캯��
	CMeshBlend();
	//! Ĭ����������
	~CMeshBlend();

	//! �������
	void AddMesh(const CMesh* mesh);
	//! �������ָ��Ȩ��
	void AddMesh(const CMesh* mesh, float weight);
	//! ���
	CMesh* Blend();
	//! ��ָ��Ȩ�ػ��
	CMesh* Blend(float* weights, int count);

private:
	//! ��ʼ��Ŀ������
	void SetupMesh();
	//! �Զ����ֵ
	void Interpolate();

private:
	//! �������ɵ�����
	CMesh* m_pTargetMesh;
	//! ����ϵ��������
	vector<CMesh*> m_vecMeshes;
	//! ����ϵ��������Ȩ��
	vector<float> m_vecWeights;
};

#endif