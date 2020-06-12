//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEDECAL_H_
#define _CSCENENODEDECAL_H_
#include "CSceneNode.h"
#include "CMatrix4.h"
#include "CFrustum.h"
#include "CPlane.h"

/**
* @brief 贴花场景节点
* @attention 需要指定父节点，贴花将在父节点上进行
*/
class AVATAR_EXPORT CSceneNodeDecal: public CSceneNode {
public:
	//! 构造方法
	CSceneNodeDecal(const string& name, const string& texture, const CMatrix4& proj);

	//! 初始化场景节点
	virtual bool Init();
	//! 销毁场景节点
	virtual void Destroy();
	//! 渲染场景节点
	virtual void Render();
	//! 更新变换矩阵
	virtual void Transform();

private:
	//! 计算贴花投影视景体
	void SetupFrustum();
	//! 生成贴花网格
	void UpdateMesh();
	//! 求三角形在投影框内的交点
	int IntersectWithFrustum(const CVector3& p1, const CVector3& p2, const CVector3& p3, float* result);

private:
	//! 网格纹理
	string m_strTexture;
	//! 贴花投影矩阵
	CMatrix4 m_cDecalMatrix;
	//! 贴花投影构成的视景体(世界坐标系)
	CFrustum m_cDecalFrustum;
	//! 贴花投影视景体6裁剪平面
	CPlane m_cClipPlane[6];
	//! 贴花纹理投影矩阵
	CMatrix4 m_cTexCoordProj;
	//! 网格对象
	CMesh* m_pMesh;
};

#endif