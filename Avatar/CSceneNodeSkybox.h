//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODESKYBOX_H_
#define _CSCENENODESKYBOX_H_
#include "CSceneNode.h"

/**
* @brief 天空盒场景节点
*/
class AVATAR_EXPORT CSceneNodeSkybox: public CSceneNode {
public:
	//! 构造方法
	CSceneNodeSkybox(const string& name, const string texture[6]);

	//! 初始化场景节点
	virtual bool Init();
	//! 销毁场景节点
	virtual void Destroy();
	//! 渲染场景节点
	virtual void Render();
	//! 更新逻辑
	virtual void Update(float dt);

private:
	//! 天空盒纹理
	string m_strTexture[6];
	//! 网格对象
	CMesh* m_pMesh;
};

#endif