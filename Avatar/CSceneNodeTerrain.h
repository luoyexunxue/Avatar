//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODETERRAIN_H_
#define _CSCENENODETERRAIN_H_
#include "CSceneNode.h"
#include "CCamera.h"
#include "CVector3.h"
#include "CBoundingBox.h"

/**
* @brief 地形场景节点
*/
class AVATAR_EXPORT CSceneNodeTerrain: public CSceneNode {
public:
	//! 构造方法
	CSceneNodeTerrain(const string& name, const string& heightMap, float mapScale,
		float heightScale, const string texture[4], const string& blendMap);

	//! 初始化场景节点
	virtual bool Init();
	//! 销毁场景节点
	virtual void Destroy();
	//! 渲染场景节点
	virtual void Render();
	//! 重载坐标变换
	virtual void Transform();
	//! 获取网格数据
	virtual CMeshData* GetMeshData();

	//! 获取指定地点的高度
	float GetHeight(float x, float y) const;
	//! 获取指定地点的法向
	CVector3 GetNormal(float x, float y) const;
	//! 设置渲染精度容差
	void SetErrorTolerance(float tolerance);

private:
	//! 高度图定义
	typedef struct _SHeightMap {
		int width;
		bool* flag;
		float* data;
		float hScale;
		float vScale;
		_SHeightMap() : width(0), flag(0), data(0) {}
	} SHeightMap;

	//! 四叉树节点
	typedef struct _SQuadTree {
		CMesh* mesh;
		bool visible;
		int level;
		int size;
		int center;
		int corner[4];
		int index[5];
		float geometricError;
		CVector3 position;
		CBoundingBox volume;
		_SQuadTree* parent;
		_SQuadTree* children[4];
		inline bool	IsLeaf() const { return children[0] == 0; }
		inline bool	IsInternal() const { return children[0] != 0; }
	} SQuadTree;

private:
	//! 读取高度图
	bool LoadHeightMap(const string& filename, SHeightMap* heightMap);
	//! 生成地形四叉树
	SQuadTree* BuildQuadTree(SQuadTree* parent, int index, int level, int size);
	//! 生成地形网格
	void BuildTerrainMesh(SQuadTree* node, int index);
	//! 清空四叉树
	void DeleteQuadTree(SQuadTree* node);
	//! 检查四叉树节点可见性
	void CheckVisibility(SQuadTree* node, const CFrustum& frustum, const CVector3& camera, float tolerance);
	//! 递归渲染地形
	void RenderTerrain(SQuadTree* node, bool useMaterial);
	//! 递归平移地形网格
	void TranslateMesh(SQuadTree* node);
	//! 更新地形网格索引缓冲
	bool UpdateIndexBuffer(SQuadTree* node);
	//! 添加三角形并修补裂缝
	void AddTriangle(SQuadTree* node, int index0, int index1, int index2, int flag1, int flag2);
	//! 计算高度图某处的法向量
	inline void GetMapNormal(int x, int y, CVector3& normal) const;

private:
	//! 高度图
	string m_strHeightMap;
	//! 地形 4 个通道纹理
	string m_strTexture[4];
	//! 地形纹理混合通道
	string m_strBlendTexture;
	//! 高度图数据
	SHeightMap* m_pHeightMap;
	//! 四叉树根节点
	SQuadTree* m_pTreeRoot;
	//! 网格数据
	CMeshData* m_pMeshData;
	//! 四叉树最大级别
	int m_iMaxLevel;
	//! 网格所在的四叉树级别
	int m_iMeshLevel;
	//! 几何精度容差系数
	float m_fErrorTolerance;
};

#endif