//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODESCREEN_H_
#define _CSCENENODESCREEN_H_
#include "CSceneNode.h"

/**
* @brief 屏幕显示场景节点
*/
class AVATAR_EXPORT CSceneNodeScreen: public CSceneNode {
public:
	//! 构造方法
	CSceneNodeScreen(const string& name, const string& texture, int width, int height);

	//! 初始化场景节点
	virtual bool Init();
	//! 销毁场景节点
	virtual void Destroy();
	//! 渲染场景节点
	virtual void Render();
	//! 获取网格数据
	virtual CMeshData* GetMeshData();

private:
	//! 渲染宽度（像素）
	int m_iWidth;
	//! 渲染高度（像素）
	int m_iHeight;
	//! 图片纹理
	string m_strTexture;
	//! 网格对象
	CMeshData* m_pMeshData;
};

#endif