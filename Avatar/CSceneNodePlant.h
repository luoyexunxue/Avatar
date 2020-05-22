//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEPLANT_H_
#define _CSCENENODEPLANT_H_
#include "CSceneNode.h"

/**
* @brief 植被模型场景节点
* @attention 此节点依赖于地形节点
*/
class AVATAR_EXPORT CSceneNodePlant: public CSceneNode {
public:
	//! 构造方法
	CSceneNodePlant(const string& name, const string& texture, float width, float height,
		const string& distributionMap, int count, float density, float range);

	//! 初始化场景节点
	virtual bool Init();
	//! 销毁场景节点
	virtual void Destroy();
	//! 渲染场景节点
	virtual void Render();

private:
	//! 生成植被层
	CMesh* BuildMesh();
	//! 加入植被到网格
	void AddPlant(CMesh* mesh, float x, float y, float z, float angle);

private:
	//! 植被纹理
	string m_strTexture;
	//! 植被宽度
	float m_fWidth;
	//! 植被高度
	float m_fHeight;
	//! 分布图
	string m_strDistributionMap;
	//! 植被总数
	int m_iTotalCount;
	//! 分布密度
	float m_fDensity;
	//! 分布范围
	float m_fRange;
	//! 网格对象
	CMesh* m_pMesh;
};

#endif