//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeFresnel.h"
#include "CEngine.h"
#include "CMeshLoader.h"

/**
* ���캯��
*/
CSceneNodeFresnel::CSceneNodeFresnel(const string& name, const string& meshFile): CSceneNode("fresnel", name) {
	m_strFile = meshFile;
	m_pMeshData = 0;
}

/**
* ��ʼ�������ڵ�
*/
bool CSceneNodeFresnel::Init() {
	m_pMeshData = CMeshLoader::Load(m_strFile, true);
	if (!m_pMeshData) return false;
	m_cLocalBoundingBox = m_pMeshData->GetBoundingBox();
	return true;
}

/**
* ���ٳ����ڵ�
*/
void CSceneNodeFresnel::Destroy() {
	CMeshLoader::Remove(m_pMeshData);
}

/**
* ��Ⱦ�����ڵ�
*/
void CSceneNodeFresnel::Render() {
	int count = m_pMeshData->GetMeshCount();
	for (int i = 0; i < count; i++) {
		m_pMeshData->GetMesh(i)->Render();
	}
}

/**
* ��ȡ��������
*/
CMeshData* CSceneNodeFresnel::GetMeshData() {
	return m_pMeshData;
}