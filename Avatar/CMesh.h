//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CMESH_H_
#define _CMESH_H_
#include "AvatarConfig.h"
#include "CMaterial.h"
#include "CVertex.h"
#include "CVertexJoint.h"
#include "CBoundingBox.h"
#include "CRay.h"
#include "CMatrix4.h"
#include <vector>
using std::vector;

/**
* @brief 网格面类
*/
class AVATAR_EXPORT CMesh {
public:
	//! 默认构造函数
	CMesh();
	//! 默认析构函数
	~CMesh();

public:
	//! 获取材质
	inline CMaterial* GetMaterial() const { return m_pMaterial; }

	//! 设置顶点数量
	void SetVertexUsage(unsigned int count);
	//! 添加一个顶点
	void AddVertex(const CVertex& vertex);
	//! 添加一个顶点并指定骨骼绑定信息
	void AddVertex(const CVertex& vertex, const CVertexJoint& bind);
	//! 使用索引方式添加一个三角形
	void AddTriangle(unsigned int a, unsigned int b, unsigned int c);
	//! 使用顶点方式添加一个三角形
	void AddTriangle(const CVertex& a, const CVertex& b, const CVertex& c);

	//! 移除网格顶点
	void RemoveVertex(unsigned int index, int count);
	//! 移除网格三角形
	void RemoveTriangle(unsigned int index, int count);

	//! 附加一个网格
	void Append(const CMesh* mesh);
	//! 对所有顶点进行矩阵变换
	void Transform(const CMatrix4& matrix);
	//! 自动计算顶点法向量
	void SetupNormal();
	//! 反转顶点法向和纹理坐标
	void Reverse(bool normal, bool texCoordU, bool texCoordV);

	//! 获取顶点个数
	int GetVertexCount() const;
	//! 获取三角形个数
	int GetTriangleCount() const;
	//! 获取绑定骨骼顶点数量
	int GetBindCount() const;
	//! 获取网格顶点
	CVertex* GetVertex(unsigned int index);
	//! 获取网格顶点
	CVertex* GetVertex(unsigned int face, unsigned int index);
	//! 获取三角形顶点
	void GetTriangle(unsigned int index, CVertex* vertices[3]);
	//! 获取三角形顶点索引
	void GetTriangle(unsigned int index, unsigned int vertices[3]);
	//! 获取顶点骨骼绑定
	CVertexJoint* GetBind(unsigned int index);

	//! 生成网格
	void Create(bool dynamic);
	//! 更新缓冲
	void Update(int bufferType);
	//! 渲染网格
	void Render(bool material = true);
	//! 网格细分
	void Subdivision();

	//! 获取包围盒
	CBoundingBox GetBoundingBox() const;
	//! 射线拾取，返回距离
	float Intersects(const CRay& ray, int* face, float* bu, float* bv) const;
	//! 计算体积并返回质心坐标
	float Volume(CVector3& centroid) const;
	//! 计算表面积
	float SurfaceArea() const;

	//! 克隆网格对象
	CMesh* Clone() const;

private:
	//! 顶点数组
	vector<CVertex> m_vecVertexArray;
	//! 顶点索引数组
	vector<unsigned int> m_vecIndexArray;
	//! 骨骼绑定数组
	vector<CVertexJoint> m_vecBindArray;
	//! 标记顶点数据大小
	size_t m_iVertexArraySize;
	//! 标记索引数据大小
	size_t m_iIndexArraySize;
	//! 材质信息
	CMaterial* m_pMaterial;

	//! 是否动态网格数据
	bool m_bDynamic;
	//! 标记是否已生成缓冲
	bool m_bCreated;

	//! 顶点缓冲ID
	unsigned int m_iVertexBuffer;
	//! 索引缓冲ID
    unsigned int m_iIndexBuffer;
};

#endif