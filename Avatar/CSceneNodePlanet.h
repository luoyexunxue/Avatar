//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEPLANET_H_
#define _CSCENENODEPLANET_H_
#include "CSceneNode.h"

/**
* @brief 星球模型场景节点
*/
class AVATAR_EXPORT CSceneNodePlanet: public CSceneNode {
public:
	//! 构造方法
	CSceneNodePlanet(const string& name, const string& texture, const string& textureNight, float radius, int slices);

	//! 初始化场景节点
	virtual bool Init();
	//! 销毁场景节点
	virtual void Destroy();
	//! 渲染场景节点
	virtual void Render();
	//! 获取网格数据
	virtual CMeshData* GetMeshData();

private:
	//! 星球半径
	float m_fRadius;
	//! 细分度
	int m_iTessellation;
	//! 表面纹理
	string m_strTexture;
	//! 夜间表面纹理
	string m_strTextureNight;
	//! 大气层网格对象
	CMesh* m_pMeshSky;
	//! 网格数据
	CMeshData* m_pMeshData;
};

#endif