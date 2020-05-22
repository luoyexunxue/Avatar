//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODETERRAIN_H_
#define _CSCENENODETERRAIN_H_
#include "CSceneNode.h"
#include "CCamera.h"
#include "CVector3.h"
#include "CBoundingBox.h"
#include <vector>
using std::vector;

/**
* @brief ���γ����ڵ�
*/
class AVATAR_EXPORT CSceneNodeTerrain: public CSceneNode {
public:
	//! ���췽��
	CSceneNodeTerrain(const string& name, const string& heightMap, float mapScale,
		float heightScale, const string texture[4], const string& blendMap);

	//! ��ʼ�������ڵ�
	virtual bool Init();
	//! ���ٳ����ڵ�
	virtual void Destroy();
	//! ��Ⱦ�����ڵ�
	virtual void Render();
	//! ��������任
	virtual void Transform();
	//! ��ȡ��������
	virtual CMeshData* GetMeshData();

	//! ��ȡָ���ص�ĸ߶�
	float GetHeight(float x, float y) const;
	//! ��ȡָ���ص�ķ���
	CVector3 GetNormal(float x, float y) const;

private:
	//! �߶�ͼ����
	typedef struct _SHeightMap {
		int size;
		bool* flag;
		float* data;
		float hScale;
		float vScale;
		_SHeightMap(): size(0), flag(0), data(0) {}
	} SHeightMap;

	//! �Ĳ����ڵ�
	typedef struct _SQuadTree {
		bool visible;
		int level;
		int size;
		int center;
		int corner[4];
		int index[5];
		float roughness;
		CMesh* mesh;
		CVector3 position;
		CBoundingBox volume;
		_SQuadTree* parent;
		_SQuadTree* children[4];
		inline bool	IsLeaf() const { return children[0] == 0; }
		inline bool	IsInternal() const { return children[0] != 0; }
	} SQuadTree;

private:
	//! ��ȡ�߶�ͼ
	bool LoadHeightMap(const string& filename, SHeightMap* heightMap);
	//! ���ɵ����Ĳ���
	SQuadTree* BuildQuadTree(SQuadTree* parent, int index, int level, int size);
	//! ���ɵ�������
	void BuildTerrainMesh(SQuadTree* node, int index, int size);
	//! ����Ĳ���
	void DeleteQuadTree(SQuadTree* node);
	//! ����Ĳ����ڵ�ɼ���
	void CheckVisibility(SQuadTree* node, const CFrustum& frustum, const CVector3& eye);
	//! �ݹ���Ⱦ����
	void RenderTerrain(SQuadTree* node, bool useMaterial);
	//! �ݹ�ƽ�Ƶ�������
	void TranslateMesh(SQuadTree* node);
	//! ���µ���������������
	bool UpdateIndexBuffer(SQuadTree* node);
	//! ��������β��޲��ѷ�
	void AddTriangle(SQuadTree* node, int center, int index1, int index2, int flag1, int flag2);
	//! ����߶�ͼĳ���ķ�����
	inline void GetMapNormal(int x, int y, CVector3& normal) const;

private:
	//! �߶�ͼ
	string m_strHeightMap;
	//! ���� 4 ��ͨ������
	string m_strTexture[4];
	//! ����������ͨ��
	string m_strBlendTexture;
	//! �߶�ͼ����
	SHeightMap* m_pHeightMap;
	//! �Ĳ������ڵ�
	SQuadTree* m_pTreeRoot;
	//! ��������
	CMeshData* m_pMeshData;
	//! �Ĳ�����󼶱�
	int m_iMaxLevel;
	//! �������ڵ��Ĳ�������
	int m_iMeshLevel;
};

#endif