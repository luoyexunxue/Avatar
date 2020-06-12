//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODECLOUD_H_
#define _CSCENENODECLOUD_H_
#include "CSceneNode.h"

/**
* @brief 云层场景节点
*/
class AVATAR_EXPORT CSceneNodeCloud: public CSceneNode {
public:
	//! 构造方法
	CSceneNodeCloud(const string& name, const CColor& skyColor, const CColor& cloudColor, float cloudSize);

	//! 初始化场景节点
	virtual bool Init();
	//! 销毁场景节点
	virtual void Destroy();
	//! 渲染场景节点
	virtual void Render();
	//! 更新逻辑
	virtual void Update(float dt);

private:
	//! 云层模拟时间
	float m_fTimeElapse;
	//! 天空颜色
	CColor m_cSkyColor;
	//! 云朵颜色
	CColor m_cCloudColor;
	//! 云团大小
	float m_fCloudSize;
	//! 云层网格对象
	CMesh* m_pMesh;
};

#endif