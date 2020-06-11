//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CVERTEXJOINT_H_
#define _CVERTEXJOINT_H_
#include "AvatarConfig.h"

/**
* @brief 顶点骨骼绑定类
*/
class AVATAR_EXPORT CVertexJoint {
public:
	//! 默认构造函数
	CVertexJoint();
	//! 复制构造函数
	CVertexJoint(const CVertexJoint& joint);
	//! 构造仅有一个骨骼影响的实例
	CVertexJoint(int jointId);
	//! 构造四个绑定信息实例
	CVertexJoint(const int jointId[4], const float weight[4]);

	//! 添加绑定
	CVertexJoint& AddBind(int jointId, float weight);
	//! 添加绑定
	CVertexJoint& AddBind(const CVertexJoint& bind, float scale);
	//! 单位化权重
	CVertexJoint& Normalize();

public:
	//! 赋值运算符
	CVertexJoint& operator = (const CVertexJoint& joint);

public:
	//! 绑定的数量(最大为4)
	int m_iCount;
	//! 骨骼ID数组
	int m_iJointId[4];
	//! 绑定权重数组
	float m_fWeight[4];
};

#endif