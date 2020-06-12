//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEAXIS_H_
#define _CSCENENODEAXIS_H_
#include "CSceneNode.h"

/**
* @brief 坐标轴指示器场景节点
*/
class AVATAR_EXPORT CSceneNodeAxis: public CSceneNode {
public:
	//! 构造方法
	CSceneNodeAxis(const string& name);

	//! 初始化场景节点
	virtual bool Init();
	//! 销毁场景节点
	virtual void Destroy();
	//! 渲染场景节点
	virtual void Render();
	//! 更新场景节点
	virtual void Update(float dt);

private:
	//! 网格对象
	CMesh* m_pMesh;
};

#endif