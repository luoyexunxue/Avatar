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
	//! GLTF加载器上下文
	typedef struct _SContext {
		//! 文件名称
		string filename;
		//! 文件目录
		string baseDir;
		//! 网格对象指针
		CMeshData* meshData;
		//! JSON 对象
		CJsonObject jsonObject;
		//! 骨骼节点集合
		map<int, bool> jointSet;
		//! 骨骼下标映射
		map<int, SJoint*> jointMap;
		//! 骨骼逆局部变换矩阵
		map<int, CMatrix4> localInvMap;
		//! 骨骼绑定逆矩阵
		map<int, CMatrix4> bindInvMap;
		//! 缓存地址
		vector<SBufferBin> buffers;
	} SContext;

private:
	//! 读取二进制缓冲区
	void ReadBuffers(SContext* context);
	//! 读取骨骼
	void ReadSkin(SContext* context);
	//! 读取骨骼节点
	void ReadJointNode(SContext* context, int index, const CMatrix4& matrix, SJoint* parent);
	//! 读取节点
	void ReadMeshNode(SContext* context, int index, const CMatrix4& matrix);
	//! 读取网格
	void ReadMesh(SContext* context, int index, const CMatrix4& matrix, map<size_t, int>& skinMapper);
	//! 读取材质信息
	void ReadMaterial(SContext* context, int index, CMaterial* material, int* texcoord);
	//! 读取纹理数据
	bool ReadTexture(SContext* context, CJsonObject& texture, CMaterial* material);
	//! 读取动画数据
	void ReadAnimation(SContext* context);

	//! 添加三角形
	void AddTriangles(SContext* context, CMesh* mesh, int accessorIndex);
	//! 添加顶点
	void AddVertices(SContext* context, CMesh* mesh, int position, int joints, int weights, map<size_t, int>& skinMapper);
	//! 设置顶点法相
	void SetupVerticesNormal(SContext* context, CMesh* mesh, int accessorIndex);
	//! 设置顶点纹理坐标
	void SetupVerticesTexCoord(SContext* context, CMesh* mesh, int accessorIndex);
	//! 设置顶点颜色
	void SetupVerticesColor(SContext* context, CMesh* mesh, int accessorIndex);
};

#endif