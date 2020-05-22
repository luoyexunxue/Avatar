//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEGEOMETRY_H_
#define _CSCENENODEGEOMETRY_H_
#include "CSceneNode.h"
#include "CGeometryCreator.h"

/**
* @brief 几何体场景节点
*/
class AVATAR_EXPORT CSceneNodeGeometry: public CSceneNode {
public:
	//! 构造方法
	CSceneNodeGeometry(const string& name, const string& texture, const SGeometry& geometry);

	//! 初始化场景节点
	virtual bool Init();
	//! 销毁场景节点
	virtual void Destroy();
	//! 渲染场景节点
	virtual void Render();
	//! 获取网格数据
	virtual CMeshData* GetMeshData();

	//! 获取几何参数
	SGeometry GetGeometry(bool worldspace);

private:
	//! 几何体纹理
	string m_strTexture;
	//! 几何参数
	SGeometry m_sGeometry;
	//! 网格对象
	CMeshData* m_pMeshData;
};

#endif