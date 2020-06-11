//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CVERTEX_H_
#define _CVERTEX_H_
#include "AvatarConfig.h"
#include "CVector2.h"
#include "CVector3.h"
#include "CColor.h"

/**
* @brief 顶点类
*/
class AVATAR_EXPORT CVertex {
public:
	//! 默认构造函数
	CVertex();
	//! 复制构造函数
	CVertex(const CVertex& vertex);
	//! 使用顶点坐标构造顶点
	CVertex(float x, float y, float z);
	//! 使用顶点坐标和纹理坐标构造顶点
	CVertex(float x, float y, float z, float s, float t);
	//! 使用顶点坐标和纹理坐标以及法向量构造顶点
	CVertex(float x, float y, float z, float s, float t, float nx, float ny, float nz);
	//! 使用顶点坐标和纹理坐标以及法向量和颜色构造顶点
	CVertex(float x, float y, float z, float s, float t, float nx, float ny, float nz, const float color[4]);

	//! 设置顶点坐标
	void SetPosition(const CVector3& pos);
	//! 设置顶点坐标
	void SetPosition(const float pos[3]);
	//! 设置顶点坐标
	void SetPosition(float x, float y, float z);

	//! 设置纹理坐标
	void SetTexCoord(const CVector2& texCoord);
	//! 设置纹理坐标
	void SetTexCoord(const float texCoord[2]);
	//! 设置纹理坐标
	void SetTexCoord(float s, float t);

	//! 设置顶点法向
	void SetNormal(const CVector3& normal);
	//! 设置顶点法向
	void SetNormal(const float normal[3]);
	//! 设置顶点法向
	void SetNormal(float nx, float ny, float nz);

	//! 设置顶点颜色
	void SetColor(const CColor& color);
	//! 设置顶点颜色
	void SetColor(const float color[4]);
	//! 设置顶点颜色
	void SetColor(float r, float g, float b, float a);

public:
	//! 翻转法向量
	void FlipNormal();
	//! 线性插值
	CVertex Interpolate(const CVertex& vertex, float t);

public:
	//! 赋值运算符
	CVertex& operator = (const CVertex& vertex);
	//! 运算符重载 ==
	bool operator == (const CVertex& vertex) const;
	//! 运算符重载 !=
	bool operator != (const CVertex& vertex) const;

public:
	//! 顶点坐标
	float m_fPosition[4];
	//! 纹理坐标
	float m_fTexCoord[2];
	//! 顶点法向量
	float m_fNormal[4];
	//! 顶点颜色
	float m_fColor[4];
};

#endif