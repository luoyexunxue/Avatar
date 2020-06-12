//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEFRESNEL_H_
#define _CSCENENODEFRESNEL_H_
#include "CSceneNode.h"
#include "CMeshData.h"

/**
* @brief 菲涅耳特效场景节点
*/
class AVATAR_EXPORT CSceneNodeFresnel : public CSceneNode {
public:
	//! 构造方法
	CSceneNodeFresnel(const string& name, const string& meshFile);

	//! 初始化场景节点
	virtual bool Init();
	//! 销毁场景节点
	virtual void Destroy();
	//! 渲染场景节点
	virtual void Render();
	//! 获取网格数据
	virtual CMeshData* GetMeshData();

private:
	//! 模型文件
	string m_strFile;
	//! 模型数据
	CMeshData* m_pMeshData;
};

#endif