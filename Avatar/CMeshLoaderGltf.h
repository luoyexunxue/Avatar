//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CMESHLOADERGLTF_H_
#define _CMESHLOADERGLTF_H_
#include "CMeshLoader.h"
#include "CJsonObject.h"
#include <vector>
#include <map>
using std::vector;
using std::map;

/**
* @brief gltf 模型加载类
*/
class AVATAR_EXPORT CMeshLoaderGltf : public CMeshLoader {
public:
	//! 加载模型
	virtual CMeshData* LoadFile(const string& filename, uint8_t* data, size_t size);

private:
	//! 读取二进制缓冲区
	void ReadBuffers();
	//! 读取骨骼
	void ReadSkin();
	//! 读取骨骼节点
	void ReadJointNode(int index, const CMatrix4& matrix, SJoint* parent);
	//! 读取节点
	void ReadMeshNode(int index, const CMatrix4& matrix);
	//! 读取网格
	void ReadMesh(CJsonObject& mesh, const CMatrix4& matrix, map<int, int>& skinMapper);
	//! 读取材质信息
	void ReadMaterial(int index, CMaterial* material, int* texcoord);
	//! 读取纹理数据
	void ReadTexture(CJsonObject& texture, CMaterial* material);
	//! 读取动画数据
	void ReadAnimation();

	//! 添加三角形
	void AddTriangles(CMesh* mesh, int accessorIndex);
	//! 添加顶点
	void AddVertices(CMesh* mesh, int position, int joints, int weights, map<int, int>& skinMapper);
	//! 设置顶点法相
	void SetupVerticesNormal(CMesh* mesh, int accessorIndex);
	//! 设置顶点纹理坐标
	void SetupVerticesTexCoord(CMesh* mesh, int accessorIndex);
	//! 设置顶点颜色
	void SetupVerticesColor(CMesh* mesh, int accessorIndex);

private:
	//! 缓存元素类型
	enum {
		TYPE_BYTE = 5120,
		TYPE_UNSIGNED_BYTE = 5121,
		TYPE_SHORT = 5122,
		TYPE_UNSIGNED_SHORT = 5123,
		TYPE_INT = 5124,
		TYPE_UNSIGNED_INT = 5125,
		TYPE_FLOAT = 5126
	};
	//! 缓存单元
	typedef struct _SBufferBin {
		bool allocated;
		unsigned char* data;
		size_t size;
	} SBufferBin;

	//! 文件名称
	string m_strFilename;
	//! 文件目录
	string m_strBaseDir;
	//! 网格对象指针
	CMeshData* m_pMeshData;
	//! JSON 对象
	CJsonObject m_cJsonObject;
	//! 骨骼节点集合
	map<int, bool> m_setJoints;
	//! 骨骼下标映射
	map<int, SJoint*> m_mapJoints;
	//! 骨骼逆局部变换矩阵
	map<int, CMatrix4> m_mapLocalInv;
	//! 骨骼绑定逆矩阵
	map<int, CMatrix4> m_mapBindInv;
	//! 缓存地址
	vector<SBufferBin> m_vecBuffer;
};

#endif