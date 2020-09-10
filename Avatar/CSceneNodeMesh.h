//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEMESH_H_
#define _CSCENENODEMESH_H_
#include "CSceneNode.h"

/**
* @brief 网格模型场景节点
*/
class AVATAR_EXPORT CSceneNodeMesh: public CSceneNode {
public:
	//! 构造方法
	CSceneNodeMesh(const string& name, CMesh* mesh);

	//! 初始化场景节点
	virtual bool Init();
	//! 销毁场景节点
	virtual void Destroy();
	//! 渲染场景节点
	virtual void Render();
	//! 获取网格数据
	virtual CMeshData* GetMeshData();

public:
	//! 添加网格对象
	void AddMesh(CMesh* mesh, bool replace);

private:
	//! 网格对象数据
	CMeshData* m_pMeshData;
};

#endif