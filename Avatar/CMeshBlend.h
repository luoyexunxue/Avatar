//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CMESHBLEND_H_
#define _CMESHBLEND_H_
#include "AvatarConfig.h"
#include "CMesh.h"
#include <vector>
using std::vector;

/**
* @brief 网格模型混合类
*
* 通过指定多个网格对象，通过权重进行混合，进行顶点变形动画，
* 注意多个网格之间顶点数必须相同，三角形必须一致。
*/
class AVATAR_EXPORT CMeshBlend {
public:
	//! 默认构造函数
	CMeshBlend();
	//! 默认析构函数
	~CMeshBlend();

	//! 添加网格
	void AddMesh(const CMesh* mesh);
	//! 添加网格并指定权重
	void AddMesh(const CMesh* mesh, float weight);
	//! 混合
	CMesh* Blend();
	//! 按指定权重混合
	CMesh* Blend(float* weights, size_t count);

private:
	//! 初始化目标网格
	void SetupMesh();
	//! 对顶点插值
	void Interpolate();

private:
	//! 最终生成的网格
	CMesh* m_pTargetMesh;
	//! 待混合的网格对象
	vector<CMesh*> m_vecMeshes;
	//! 待混合的网格对象权重
	vector<float> m_vecWeights;
};

#endif