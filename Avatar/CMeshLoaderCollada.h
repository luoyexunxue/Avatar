//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CMESHLOADERCOLLADA_H_
#define _CMESHLOADERCOLLADA_H_
#include "CMeshLoader.h"
#include "CMatrix4.h"
#include <string>
#include <vector>
#include <map>
#include "thirdparty/rapidxml/rapidxml.hpp"
using std::string;
using std::vector;
using std::map;
using namespace rapidxml;

/**
* @brief Collada(DAE) 模型加载类
*/
class AVATAR_EXPORT CMeshLoaderCollada : public CMeshLoader {
public:
	//! 加载模型
	virtual CMeshData* LoadFile(const string& filename, uint8_t* data, size_t size);

private:
	//! Collada加载器上下文
	typedef struct _SContext {
		string baseDir;
		map<string, string> imageMap;
		map<string, string> materialMap;
		map<string, string> materialMapper;
		map<string, CMatrix4> meshTransformMap;
		map<string, SJoint*> jointMap;
		map<string, string> jointMapper;
		map<string, int> jointNameMap;
		map<string, vector<CVertexJoint>> vertexJointMap;
	} SContext;

private:
	//! 读取图片节点
	void ReadImages(SContext* context, xml_node<>* images);
	//! 读取材质节点
	void ReadMaterials(SContext* context, xml_node<>* materials);
	//! 读取场景节点
	void ReadVisualScenes(SContext* context, xml_node<>* visualScenes, const CMatrix4& matrix, CMeshData* meshData);
	//! 读取控制节点
	void ReadControllers(SContext* context, xml_node<>* controllers, CMeshData* meshData);
	//! 读取几何节点
	void ReadGeometries(SContext* context, xml_node<>* geometries, CMeshData* meshData);
	//! 读取效果节点
	void ReadEffects(SContext* context, xml_node<>* effects, CMeshData* meshData);
	//! 读取动画节点
	void ReadAnimations(SContext* context, xml_node<>* animations, CMeshData* meshData);

	//! 解析节点数据
	void ReadNode(SContext* context, xml_node<>* node, const CMatrix4& matrix, SJoint* joint);
	//! 解析蒙皮数据
	void ReadSkin(SContext* context, xml_node<>* skin, const string& id, CMeshData* meshData);
	//! 解析网格数据
	void ReadMesh(SContext* context, xml_node<>* mesh, const string& id, CMeshData* meshData);
	//! 解析材质数据
	void ReadProfile(SContext* context, xml_node<>* profile, const string& id, CMeshData* meshData);
	//! 读取动画数据
	void ReadAnimationData(SContext* context, xml_node<>* animation);
	//! 读取动画关节
	void ReadAnimationJoint(SContext* context, xml_node<>* animation);

	//! 映射材质名称
	void MapMaterial(SContext* context, xml_node<>* material);
	//! 映射顶点骨骼
	void MapVertexJoint(SContext* context, const string& id, const vector<string>& joint, CMeshData* meshData);
	//! 获取几何数据输入
	int GetElementInput(xml_node<>* element, string source[3], int offset[3]);
};

#endif