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
	virtual CMeshData* LoadFile(const string& filename, uint8_t* data, uint32_t size);

private:
	//! 读取图片节点
	void ReadImages(xml_node<>* images);
	//! 读取材质节点
	void ReadMaterials(xml_node<>* materials);
	//! 读取场景节点
	void ReadVisualScenes(xml_node<>* visualScenes, const CMatrix4& matrix, CMeshData* meshData);
	//! 读取控制节点
	void ReadControllers(xml_node<>* controllers, CMeshData* meshData);
	//! 读取几何节点
	void ReadGeometries(xml_node<>* geometries, CMeshData* meshData);
	//! 读取效果节点
	void ReadEffects(xml_node<>* effects, CMeshData* meshData);
	//! 读取动画节点
	void ReadAnimations(xml_node<>* animations, CMeshData* meshData);

	//! 解析节点数据
	void ReadNode(xml_node<>* node, const CMatrix4& matrix, SJoint* joint);
	//! 解析蒙皮数据
	void ReadSkin(xml_node<>* skin, const string& id, CMeshData* meshData);
	//! 解析网格数据
	void ReadMesh(xml_node<>* mesh, const string& id, CMeshData* meshData);
	//! 解析材质数据
	void ReadProfile(xml_node<>* profile, const string& id, CMeshData* meshData);
	//! 读取动画数据
	void ReadAnimationData(xml_node<>* animation);
	//! 读取动画关节
	void ReadAnimationJoint(xml_node<>* animation);

private:
	//! 映射材质名称
	void MapMaterial(xml_node<>* material);
	//! 映射顶点骨骼
	void MapVertexJoint(const string& id, const vector<string>& joint, CMeshData* meshData);
	//! 获取几何数据输入
	int GetElementInput(xml_node<>* element, string source[3], int offset[3]);

private:
	string m_strBaseDir;
	map<string, string> m_mapImage;
	map<string, string> m_mapMaterial;
	map<string, string> m_mapMaterialMapper;
	map<string, CMatrix4> m_mapMeshTransform;
	map<string, SJoint*> m_mapJoints;
	map<string, string> m_mapJointsMapper;
	map<string, int> m_mapJointsName;
	map<string, vector<CVertexJoint>> m_mapVertexJoint;
};

#endif