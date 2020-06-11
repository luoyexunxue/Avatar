//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEWATER_H_
#define _CSCENENODEWATER_H_
#include "CSceneNode.h"

/**
* @brief 水面场景节点
*/
class AVATAR_EXPORT CSceneNodeWater: public CSceneNode {
public:
	//! 构造方法
	CSceneNodeWater(const string& name, const string& normalMap, float depth);

	//! 初始化场景节点
	virtual bool Init();
	//! 销毁场景节点
	virtual void Destroy();
	//! 渲染场景节点
	virtual void Render();
	//! 更新场景节点
	virtual void Update(float dt);

private:
	//! 更新水下屏幕蒙版
	void UpdateUnderwaterMask();

private:
	//! 模拟的波动的时间
	float m_fTimeElapse;
	//! 水面波纹法线贴图
	string m_strNormalMap;
	//! 水面网格对象
	CMesh* m_pMesh;
	//! 倒影 RTT 贴图
	CTexture* m_pReflectTexture;
	//! 屏幕水下蒙版节点
	CSceneNode* m_pUnderwaterNode;
};

#endif