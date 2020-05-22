//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* @brief Collada(DAE) ģ�ͼ�����
*/
class CMeshLoaderCollada: public CMeshLoader {
protected:
	//! ����ģ��
	virtual CMeshData* LoadFile(const string& filename, const string& type);

private:
	//! ��ȡͼƬ�ڵ�
	void ReadImages(xml_node<>* images);
	//! ��ȡ���ʽڵ�
	void ReadMaterials(xml_node<>* materials);
	//! ��ȡ�����ڵ�
	void ReadVisualScenes(xml_node<>* visualScenes, const CMatrix4& matrix, CMeshData* meshData);
	//! ��ȡ���ƽڵ�
	void ReadControllers(xml_node<>* controllers, CMeshData* meshData);
	//! ��ȡ���νڵ�
	void ReadGeometries(xml_node<>* geometries, CMeshData* meshData);
	//! ��ȡЧ���ڵ�
	void ReadEffects(xml_node<>* effects, CMeshData* meshData);
	//! ��ȡ�����ڵ�
	void ReadAnimations(xml_node<>* animations, CMeshData* meshData);

	//! �����ڵ�����
	void ReadNode(xml_node<>* node, const CMatrix4& matrix, SJoint* joint);
	//! ������Ƥ����
	void ReadSkin(xml_node<>* skin, const string& id, CMeshData* meshData);
	//! ������������
	void ReadMesh(xml_node<>* mesh, const string& id, CMeshData* meshData);
	//! ������������
	void ReadProfile(xml_node<>* profile, const string& id, CMeshData* meshData);
	//! ��ȡ��������
	void ReadAnimationData(xml_node<>* animation);
	//! ��ȡ�����ؽ�
	void ReadAnimationJoint(xml_node<>* animation);

private:
	//! ӳ���������
	void MapMaterial(xml_node<>* material);
	//! ӳ�䶥�����
	void MapVertexJoint(const string& id, const vector<string>& joint, CMeshData* meshData);
	//! ��ȡ������������
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