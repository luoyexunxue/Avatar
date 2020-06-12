//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeFresnel.h"
#include "CEngine.h"
#include "CMeshLoader.h"

/**
* 构造函数
*/
CSceneNodeFresnel::CSceneNodeFresnel(const string& name, const string& meshFile): CSceneNode("fresnel", name) {
	m_strFile = meshFile;
	m_pMeshData = 0;
}

/**
* 初始化场景节点
*/
bool CSceneNodeFresnel::Init() {
	m_pMeshData = CMeshLoader::Load(m_strFile, true);
	if (!m_pMeshData) return false;
	m_cLocalBoundingBox = m_pMeshData->GetBoundingBox();
	return true;
}

/**
* 销毁场景节点
*/
void CSceneNodeFresnel::Destroy() {
	CMeshLoader::Remove(m_pMeshData);
}

/**
* 渲染场景节点
*/
void CSceneNodeFresnel::Render() {
	int count = m_pMeshData->GetMeshCount();
	for (int i = 0; i < count; i++) {
		m_pMeshData->GetMesh(i)->Render();
	}
}

/**
* 获取网格数据
*/
CMeshData* CSceneNodeFresnel::GetMeshData() {
	return m_pMeshData;
}