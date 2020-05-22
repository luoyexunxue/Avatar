//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENEMANAGER_H_
#define _CSCENEMANAGER_H_
#include "AvatarConfig.h"
#include "CSceneNode.h"
#include "CVector2.h"
#include "CMatrix4.h"
#include <list>
#include <map>
#include <vector>
using std::list;
using std::map;
using std::vector;

/**
* @brief ����������
*/
class AVATAR_EXPORT CSceneManager {
public:
	//! ��ȡ������ʵ��
	static CSceneManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CSceneManager();
		return m_pInstance;
	}

	//! ʵ�����٣����ģ�������ͷ��ڴ�
	void Destroy();
	//! ��Ⱦ���г����ڵ�
	void RenderScene(const CMatrix4& view, const CMatrix4& proj);
	//! ��Ⱦ��Ļ�����ڵ�
	void RenderScreen(const CMatrix4& proj, const CVector2& offset);
	//! ��Ⱦ��Ӱ��ͼ
	void RenderDepth(const CMatrix4& viewProj, const string& shader);
	//! �������г����ڵ�
	void Update(float dt);

	//! ��ӳ����ڵ�
	bool InsertNode(CSceneNode* node, CSceneNode* parent = 0);
	//! ɾ�������ڵ�
	bool DeleteNode(CSceneNode* node);
	//! ɾ��ָ�����Ƶĳ����ڵ�
	bool DeleteNode(const string& name);
	//! ��������ڵ�
	void ClearNode();

	//! ��ȡ���ڵ�
	CSceneNode* GetRootNode();
	//! ͨ�� ID ���ҽڵ�
	CSceneNode* GetNodeById(int id);
	//! ͨ�����Ʋ��ҽڵ�
	CSceneNode* GetNodeByName(const string& name);
	//! ͨ�����Ͳ��ҽڵ�
	CSceneNode* GetNodeByType(const string& type, int index, CSceneNode* start = 0);
	//! ͨ�����߲��ҽڵ�
	CSceneNode* GetNodeByRay(const CRay& ray, CVector3& hit, int* mesh = 0, int* face = 0);
	//! ��ȡ��������нڵ��б�
	void GetNodeList(vector<CSceneNode*>& nodeList);
	//! ��ȡ�����ָ�����ͽڵ��б�
	void GetNodeList(const string& type, vector<CSceneNode*>& nodeList);

	//! ͳ����������
	int GetMeshCount();
	//! ͳ�������θ���
	int GetTriangleCount();
	//! ͳ�ƶ������
	int GetVertexCount();

private:
	CSceneManager();
	~CSceneManager();

	//! �ݹ���³����ڵ�
	void UpdateAll(CSceneNode* parent, float dt);
	//! �ݹ�ɾ�������ڵ�
	void DeleteAll(CSceneNode* parent);

	//! ���������Ƿ��볡���ڵ��ཻ�������ؽ������
	float Picking(CSceneNode* node, const CRay& ray, CVector3& hit, int* mesh, int* face);
	//! ��͸���ڵ�����
	void SortTranslucentNode();
	//! ��Ļ�ڵ�����
	void SortScreenNode();
	//! ��ʼ����Ⱦ����
	void SetupRenderGroup();

private:
	//! �������ڵ㶨��
	class CRootNode: public CSceneNode {
	public:
		CRootNode(): CSceneNode("root", "__root__") {}
		virtual bool Init() { return true; }
		virtual void Destroy() {}
		virtual void Render() {}
	};

	//! ��Ⱦ�ڵ㶨��
	typedef struct _SRenderNode {
		float distance;
		int shaderIndex;
		CSceneNode* sceneNode;
		_SRenderNode(int shader, CSceneNode* node) {
			shaderIndex = shader;
			sceneNode = node;
		}
	} SRenderNode;
	//! ��Ⱦ���鶨��
	typedef struct _SRenderGroup {
		string groupName;
		list<SRenderNode> nodeList;
		_SRenderGroup(const string& name) {
			groupName = name;
		}
	} SRenderGroup;
	//! ��Ⱦ�ڵ����Ͷ���
	typedef struct _SRenderType {
		int groupIndex;
		int shaderIndex;
		bool renderDepth;
		_SRenderType(int group, int shader, bool depth) {
			groupIndex = group;
			shaderIndex = shader;
			renderDepth = depth;
		}
	} SRenderType;

private:
	//! �������ڵ�
	CSceneNode* m_pRootNode;
	//! ����-�ڵ㼯��
	map<string, CSceneNode*> m_mapSceneNode;
	//! ��Ļ�����ڵ��б�
	list<CSceneNode*> m_lstScreenNode;
	//! �����Ⱦ�����ڵ��б�
	list<CSceneNode*> m_lstDepthNode;
	//! �����ڵ�����б�
	vector<SRenderGroup> m_vecRenderGroup;
	//! ϵͳ������ɫ���б�
	vector<CShader*> m_vecShaderList;
	//! �����ڵ����ͼ���
	map<string, SRenderType> m_mapRenderType;
	//! ʵ��
	static CSceneManager* m_pInstance;
};

#endif