//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEFLAME_H_
#define _CSCENENODEFLAME_H_
#include "CSceneNode.h"

/**
* @brief 火焰模拟场景节点
*/
class AVATAR_EXPORT CSceneNodeFlame: public CSceneNode {
public:
	//! 构造方法
	CSceneNodeFlame(const string& name, const string& texture, const string& distortionMap,
		const string& alphaMask, float width, float height);

	//! 初始化场景节点
	virtual bool Init();
	//! 销毁场景节点
	virtual void Destroy();
	//! 渲染场景节点
	virtual void Render();
	//! 模拟更新
	virtual void Update(float dt);

private:
	//! 模拟时间
	float m_fTimeElapse;
	//! 火焰纹理
	string m_strTexture;
	//! 火焰扰动图
	string m_strDistortionMap;
	//! 透明蒙版
	string m_strAlphaMask;
	//! 宽度
	float m_fWidth;
	//! 高度
	float m_fHeight;
	//! 网格对象
	CMesh* m_pMesh;
};

#endif