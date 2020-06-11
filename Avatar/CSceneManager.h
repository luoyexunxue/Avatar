//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
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
* @brief 场景管理器
*/
class AVATAR_EXPORT CSceneManager {
public:
	//! 获取管理器实例
	static CSceneManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CSceneManager();
		return m_pInstance;
	}

	//! 实例销毁，清空模型树，释放内存
	void Destroy();
	//! 渲染所有场景节点
	void RenderScene(const CMatrix4& view, const CMatrix4& proj);
	//! 渲染屏幕场景节点
	void RenderScreen(const CMatrix4& proj, const CVector2& offset);
	//! 渲染阴影贴图
	void RenderDepth(const CMatrix4& viewProj, const string& shader);
	//! 更新所有场景节点
	void Update(float dt);

	//! 添加场景节点
	bool InsertNode(CSceneNode* node, CSceneNode* parent = 0);
	//! 删除场景节点
	bool DeleteNode(CSceneNode* node);
	//! 删除指定名称的场景节点
	bool DeleteNode(const string& name);
	//! 清除场景节点
	void ClearNode();

	//! 获取根节点
	CSceneNode* GetRootNode();
	//! 通过 ID 查找节点
	CSceneNode* GetNodeById(int id);
	//! 通过名称查找节点
	CSceneNode* GetNodeByName(const string& name);
	//! 通过类型查找节点
	CSceneNode* GetNodeByType(const string& type, int index, CSceneNode* start = 0);
	//! 通过射线查找节点
	CSceneNode* GetNodeByRay(const CRay& ray, CVector3& hit, int* mesh = 0, int* face = 0);
	//! 获取管理的所有节点列表
	void GetNodeList(vector<CSceneNode*>& nodeList);
	//! 获取管理的指定类型节点列表
	void GetNodeList(const string& type, vector<CSceneNode*>& nodeList);

	//! 统计网格数量
	int GetMeshCount();
	//! 统计三角形个数
	int GetTriangleCount();
	//! 统计顶点个数
	int GetVertexCount();

private:
	CSceneManager();
	~CSceneManager();

	//! 递归更新场景节点
	void UpdateAll(CSceneNode* parent, float dt);
	//! 递归删除场景节点
	void DeleteAll(CSceneNode* parent);

	//! 计算射线是否与场景节点相交，并返回交点距离
	float Picking(CSceneNode* node, const CRay& ray, CVector3& hit, int* mesh, int* face);
	//! 半透明节点排序
	void SortTranslucentNode();
	//! 屏幕节点排序
	void SortScreenNode();
	//! 初始化渲染分组
	void SetupRenderGroup();

private:
	//! 场景根节点定义
	class CRootNode: public CSceneNode {
	public:
		CRootNode(): CSceneNode("root", "__root__") {}
		virtual bool Init() { return true; }
		virtual void Destroy() {}
		virtual void Render() {}
	};

	//! 渲染节点定义
	typedef struct _SRenderNode {
		float distance;
		int shaderIndex;
		CSceneNode* sceneNode;
		_SRenderNode(int shader, CSceneNode* node) {
			shaderIndex = shader;
			sceneNode = node;
		}
	} SRenderNode;
	//! 渲染分组定义
	typedef struct _SRenderGroup {
		string groupName;
		list<SRenderNode> nodeList;
		_SRenderGroup(const string& name) {
			groupName = name;
		}
	} SRenderGroup;
	//! 渲染节点类型定义
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
	//! 场景根节点
	CSceneNode* m_pRootNode;
	//! 名称-节点集合
	map<string, CSceneNode*> m_mapSceneNode;
	//! 屏幕场景节点列表
	list<CSceneNode*> m_lstScreenNode;
	//! 深度渲染场景节点列表
	list<CSceneNode*> m_lstDepthNode;
	//! 场景节点分组列表
	vector<SRenderGroup> m_vecRenderGroup;
	//! 系统内置着色器列表
	vector<CShader*> m_vecShaderList;
	//! 场景节点类型集合
	map<string, SRenderType> m_mapRenderType;
	//! 实例
	static CSceneManager* m_pInstance;
};

#endif