//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CMESHLOADERGLTF_H_
#define _CMESHLOADERGLTF_H_
#include "CMeshLoader.h"
#include "CJsonParser.h"
#include <vector>
#include <map>
using std::vector;
using std::map;

/**
* @brief gltf ģ�ͼ�����
*/
class CMeshLoaderGltf: public CMeshLoader {
protected:
	//! ����ģ��
	virtual CMeshData* LoadFile(const string& filename, const string& type);

private:
	//! ��ȡ�����ƻ�����
	void ReadBuffers();
	//! ��ȡ����
	void ReadSkin();
	//! ��ȡ�����ڵ�
	void ReadJointNode(int index, const CMatrix4& matrix, SJoint* parent);
	//! ��ȡ�ڵ�
	void ReadMeshNode(int index, const CMatrix4& matrix);
	//! ��ȡ����
	void ReadMesh(CJsonParser& mesh, const CMatrix4& matrix, map<int, int>& skinMapper);
	//! ��ȡ������Ϣ
	void ReadMaterial(int index, CMaterial* material, int* texcoord);
	//! ��ȡ��������
	void ReadTexture(CJsonParser& texture, CMaterial* material);
	//! ��ȡ��������
	void ReadAnimation();

	//! ���������
	void AddTriangles(CMesh* mesh, int accessorIndex);
	//! ��Ӷ���
	void AddVertices(CMesh* mesh, int position, int joints, int weights, map<int, int>& skinMapper);
	//! ���ö��㷨��
	void SetupVerticesNormal(CMesh* mesh, int accessorIndex);
	//! ���ö�����������
	void SetupVerticesTexCoord(CMesh* mesh, int accessorIndex);
	//! ���ö�����ɫ
	void SetupVerticesColor(CMesh* mesh, int accessorIndex);

private:
	//! ����Ԫ������
	enum {
		TYPE_BYTE = 5120,
		TYPE_UNSIGNED_BYTE = 5121,
		TYPE_SHORT = 5122,
		TYPE_UNSIGNED_SHORT = 5123,
		TYPE_INT = 5124,
		TYPE_UNSIGNED_INT = 5125,
		TYPE_FLOAT = 5126
	};
	//! ���浥Ԫ
	typedef struct _SBufferBin {
		bool allocated;
		unsigned char* data;
		unsigned int size;
	} SBufferBin;

	//! �ļ�Ŀ¼
	string m_strBaseDir;
	//! �������ָ��
	CMeshData* m_pMeshData;
	//! JSON ����
	CJsonParser m_cJsonParser;
	//! �����ڵ㼯��
	map<int, bool> m_setJoints;
	//! �����±�ӳ��
	map<int, SJoint*> m_mapJoints;
	//! ������ֲ��任����
	map<int, CMatrix4> m_mapLocalInv;
	//! �����������
	map<int, CMatrix4> m_mapBindInv;
	//! �����ַ
	vector<SBufferBin> m_vecBuffer;
};

#endif