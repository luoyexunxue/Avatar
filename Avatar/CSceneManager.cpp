//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneManager.h"
#include "CEngine.h"
#include "CLog.h"
#include <cstring>

/**
* 构造函数
*/
CSceneManager::CSceneManager() {
	m_pRootNode = new CRootNode();
	m_pRootNode->Init();
	SetupRenderGroup();
}

/**
* 析构函数
*/
CSceneManager::~CSceneManager() {
	m_pInstance = 0;
}

/**
* 单例实例
*/
CSceneManager* CSceneManager::m_pInstance = 0;

/**
* 销毁场景管理器
*/
void CSceneManager::Destroy() {
	ClearNode();
	delete m_pRootNode;
	delete this;
}

/**
* 渲染所有的 3D 场景节点
* @param view 相机视图矩阵
* @param proj 相机投影矩阵
*/
void CSceneManager::RenderScene(const CMatrix4& view, const CMatrix4& proj) {
	// 设置视图投影矩阵
	CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
	for (size_t i = 0; i < m_vecShaderList.size(); i++) {
		CShader* shader = m_vecShaderList[i];
		shader->UseShader();
		shader->SetUniform("uProjMatrix", proj);
		shader->SetUniform("uViewMatrix", view);
		shader->SetUniform("uCameraPos", pCamera->m_cPosition);
	}
	// 对场景节点进行分组渲染
	for (size_t i = 0; i < m_vecRenderGroup.size(); i++) {
		SRenderGroup& renderGroup = m_vecRenderGroup[i];
		list<SRenderNode>::iterator iter = renderGroup.nodeList.begin();
		while (iter != renderGroup.nodeList.end()) {
			CSceneNode* sceneNode = iter->sceneNode;
			if (sceneNode->m_bVisible) {
				int shaderIndex = iter->shaderIndex;
				if (shaderIndex >= 0) {
					m_vecShaderList[shaderIndex]->UseShader();
					m_vecShaderList[shaderIndex]->SetUniform("uModelMatrix", sceneNode->m_cWorldMatrix);
				}
				sceneNode->Render();
			}
			++iter;
		}
	}
}

/**
* 渲染屏幕场景节点
* @param proj 投影矩阵
* @param offset 距离屏幕中心的偏移
* @note 需要将深度测试关闭，避免结果错误
*/
void CSceneManager::RenderScreen(const CMatrix4& proj, const CVector2& offset) {
	// 设置投影矩阵，和中心偏移
	CShader* pShader = CEngine::GetShaderManager()->GetShader("screen");
	pShader->UseShader();
	pShader->SetUniform("uProjMatrix", proj);
	pShader->SetUniform("uOffset", offset);
	// 渲染所有屏幕场景节点
	list<CSceneNode*>::iterator iter = m_lstScreenNode.begin();
	while (iter != m_lstScreenNode.end()) {
		CSceneNode* pSceneNode = *iter;
		if (pSceneNode->m_bVisible) {
			pShader->SetUniform("uModelMatrix", pSceneNode->m_cWorldMatrix);
			pSceneNode->Render();
		}
		++iter;
	}
}

/**
* 渲染深度贴图
* @param viewProj 渲染深度的视图投影矩阵
* @param shader 深度渲染的着色器名称
* @note 需要将颜色混合关闭，避免结果错误
*/
void CSceneManager::RenderDepth(const CMatrix4& viewProj, const string& shader) {
	CShader* pShader = CEngine::GetShaderManager()->GetShader(shader);
	pShader->UseShader();
	pShader->SetUniform("uViewProjMatrix", viewProj);
	// 渲染所有实体场景节点
	list<CSceneNode*>::iterator iter = m_lstDepthNode.begin();
	while (iter != m_lstDepthNode.end()) {
		CSceneNode* pSceneNode = *iter;
		if (pSceneNode->m_bVisible) {
			pShader->SetUniform("uModelMatrix", pSceneNode->m_cWorldMatrix);
			pSceneNode->Render();
		}
		++iter;
	}
}

/**
* 更新所有场景节点
* @param dt 时间步长
*/
void CSceneManager::Update(float dt) {
	// 递归更新场景节点
	UpdateAll(m_pRootNode, dt);
	// 节点排序
	SortTranslucentNode();
	SortScreenNode();
}

/**
* 添加场景节点
* @param node 待插入的场景节点
* @param parent 插入的父节点，若为空则将 node 插入根节点
* @return 是否插入成功，若返回 false 则 node 被自动释放
*/
bool CSceneManager::InsertNode(CSceneNode* node, CSceneNode* parent) {
	node->m_pParent = parent ? parent : m_pRootNode;
	if (m_mapSceneNode.count(node->GetName()) > 0) {
		CLog::Warn("Scene node '%s' already exist", node->GetName().c_str());
		delete node;
		return false;
	}
	// 进行初始化，若初始化不成功则将节点删除
	if (!node->Init()) {
		CLog::Warn("Initialize scene node '%s' failed", node->GetName().c_str());
		delete node;
		return false;
	}
	node->m_pParent->m_lstChildren.push_back(node);
	m_mapSceneNode.insert(std::pair<string, CSceneNode*>(node->GetName(), node));
	// 通过判断节点类型，选择性加入相应列表
	if (node->GetType() == "screen") {
		m_lstScreenNode.push_back(node);
	} else {
		// 按半透明关系进行分组，并对场景节点按材质进行排序，减少 OpenGL 状态切换
		SRenderType& renderType = m_mapRenderType.at(node->GetType());
		SRenderGroup& renderGroup = m_vecRenderGroup[renderType.groupIndex];
		list<SRenderNode>::iterator iter = renderGroup.nodeList.begin();
		while (iter != renderGroup.nodeList.end()) {
			if (renderType.shaderIndex < iter->shaderIndex) break;
			++iter;
		}
		renderGroup.nodeList.insert(iter, SRenderNode(renderType.shaderIndex, node));
		// 参与深度渲染
		if (renderType.renderDepth) {
			m_lstDepthNode.push_back(node);
		}
	}
	node->Transform();
	return true;
}

/**
* 删除场景节点，其子节点将会自动加入其父节点
* @param node 需要删除的场景节点，根节点除外
* @return 是否删除成功，若返回 true 则 node 被自动释放
*/
bool CSceneManager::DeleteNode(CSceneNode* node) {
	if (!node) return false;
	CSceneNode* parent = node->m_pParent;
	parent->m_lstChildren.remove(node);
	// 将移除节点的子节点加入其父节点
	list<CSceneNode*>::iterator iter = node->m_lstChildren.begin();
	while (iter != node->m_lstChildren.end()) {
		parent->m_lstChildren.push_back(*iter);
		++iter;
	}
	node->m_pParent = 0;
	node->m_lstChildren.clear();
	m_mapSceneNode.erase(node->GetName());
	// 删除包含在其他列表中的节点数据
	if (node->GetType() == "screen") {
		m_lstScreenNode.remove(node);
	} else {
		SRenderType& renderType = m_mapRenderType.at(node->GetType());
		SRenderGroup& renderGroup = m_vecRenderGroup[renderType.groupIndex];
		list<SRenderNode>::iterator itera = renderGroup.nodeList.begin();
		while (itera != renderGroup.nodeList.end()) {
			if (itera->sceneNode == node) {
				renderGroup.nodeList.erase(itera);
				break;
			}
			++itera;
		}
		if (renderType.renderDepth) {
			m_lstDepthNode.remove(node);
		}
	}
	// 销毁场景节点
	node->Destroy();
	delete node;
	return true;
}

/**
* 删除指定名称的场景节点，其子节点将会自动加入其父节点
* @param name 需要删除的场景节点名称
* @return 是否删除成功，若返回 true 则节点被自动释放
*/
bool CSceneManager::DeleteNode(const string& name) {
	map<string, CSceneNode*>::iterator iter = m_mapSceneNode.find(name);
	if (iter != m_mapSceneNode.end()) {
		return DeleteNode(iter->second);
	}
	return false;
}

/**
* 清除所有场景节点
*/
void CSceneManager::ClearNode() {
	m_mapSceneNode.clear();
	m_lstScreenNode.clear();
	m_lstDepthNode.clear();
	// 清除分组中的节点
	for (size_t i = 0; i < m_vecRenderGroup.size(); i++) {
		m_vecRenderGroup[i].nodeList.clear();
	}
	// 销毁所有节点
	DeleteAll(m_pRootNode);
}

/**
* 获取场景根节点
* @return 根节点
*/
CSceneNode* CSceneManager::GetRootNode() {
	return m_pRootNode;
}

/**
* 通过 ID 获取节点
* @param id 需要获取的节点 ID
* @return 对应的场景节点，若无对应的场景节点，则为空
*/
CSceneNode* CSceneManager::GetNodeById(int id) {
	map<string, CSceneNode*>::iterator iter = m_mapSceneNode.begin();
	while (iter != m_mapSceneNode.end()) {
		if (iter->second->GetId() == id) {
			return iter->second;
		}
		++iter;
	}
	return 0;
}

/**
* 通过名称获取节点
* @param name 需要获取的节点名称
* @return 对应的场景节点，若无对应的场景节点，则为空
*/
CSceneNode* CSceneManager::GetNodeByName(const string& name) {
	map<string, CSceneNode*>::iterator iter = m_mapSceneNode.find(name);
	if (iter != m_mapSceneNode.end()) {
		return iter->second;
	}
	return 0;
}

/**
* 通过类型获取节点
* @param type 需要获取的节点类型
* @param index 指定匹配第 index 个节点
* @param start 指定父节点，若为空代表根节点
* @return 对应的场景节点，若无对应的场景节点，则为空
*/
CSceneNode* CSceneManager::GetNodeByType(const string& type, int index, CSceneNode* start) {
	if (!start) {
		start = m_pRootNode;
	}
	// 非递归方式遍历场景树
	list<CSceneNode*> lstStack;
	lstStack.push_back(start);
	while(!lstStack.empty()) {
		CSceneNode* pSceneNode = lstStack.back();
		lstStack.pop_back();
		if (pSceneNode->GetType() == type) {
			if (index == 0) return pSceneNode;
			index--;
		}
		list<CSceneNode*>::iterator iter = pSceneNode->m_lstChildren.begin();
		while (iter != pSceneNode->m_lstChildren.end()) {
			lstStack.push_back(*iter);
			++iter;
		}
	}
	return 0;
}

/**
* 通过拾取射线获取节点
* @param ray 拾取射线
* @param hit 获取到的场景节点与射线的交点
* @param mesh 获取到的与射线相交的节点网格
* @param face 获取到的与射线相交的三角形
* @return 对应的场景节点，若无对应的场景节点，则为空
*/
CSceneNode* CSceneManager::GetNodeByRay(const CRay& ray, CVector3& hit, int* mesh, int* face) {
	int hitMesh;
	int hitFace;
	CVector3 hitPoint;
	float minDistance = -1.0f;
	CSceneNode* pPickNode = 0;
	// 非递归遍历树
	list<CSceneNode*> lstStack;
	lstStack.push_back(m_pRootNode);
	while(!lstStack.empty()) {
		CSceneNode* pNode = lstStack.back();
		lstStack.pop_back();
		if (pNode->m_bVisible) {
			// 场景节点与射线相交情况，先使用包围盒快速测试
			if (ray.IntersectAABB(pNode->GetBoundingBox(), hitPoint)) {
				float distance = Picking(pNode, ray, hitPoint, &hitMesh, &hitFace);
				if (distance >= 0 && (distance < minDistance || minDistance < 0)) {
					minDistance = distance;
					pPickNode = pNode;
					hit = hitPoint;
					if (mesh) *mesh = hitMesh;
					if (face) *face = hitFace;
				}
			}
			list<CSceneNode*>::iterator iter = pNode->m_lstChildren.begin();
			while (iter != pNode->m_lstChildren.end()) {
				lstStack.push_back(*iter);
				++iter;
			}
		}
    }
	return pPickNode;
}

/**
* 获取管理的所有节点列表
* @param nodeList 返回的节点列表
*/
void CSceneManager::GetNodeList(vector<CSceneNode*>& nodeList) {
	nodeList.clear();
	list<CSceneNode*> lstStack;
	lstStack.push_back(m_pRootNode);
	while(!lstStack.empty()) {
		CSceneNode* pSceneNode = lstStack.back();
		lstStack.pop_back();
		nodeList.push_back(pSceneNode);
		list<CSceneNode*>::iterator iter = pSceneNode->m_lstChildren.begin();
		while (iter != pSceneNode->m_lstChildren.end()) {
			lstStack.push_back(*iter);
			++iter;
		}
	}
}

/**
* 获取管理的指定类型节点列表
* @param type 需要获取的节点类型
* @param nodeList 返回的节点列表
*/
void CSceneManager::GetNodeList(const string& type, vector<CSceneNode*>& nodeList) {
	nodeList.clear();
	list<CSceneNode*> lstStack;
	lstStack.push_back(m_pRootNode);
	while (!lstStack.empty()) {
		CSceneNode* pSceneNode = lstStack.back();
		lstStack.pop_back();
		if (pSceneNode->GetType() == type) {
			nodeList.push_back(pSceneNode);
		}
		list<CSceneNode*>::iterator iter = pSceneNode->m_lstChildren.begin();
		while (iter != pSceneNode->m_lstChildren.end()) {
			lstStack.push_back(*iter);
			++iter;
		}
	}
}

/**
* 统计网格数量
* @return 网格对象统计信息
*/
int CSceneManager::GetMeshCount() {
	int meshCount = 0;
	map<string, CSceneNode*>::iterator iter = m_mapSceneNode.begin();
	while (iter != m_mapSceneNode.end()) {
		CMeshData* pMeshData = iter->second->GetMeshData();
		if (pMeshData) meshCount += pMeshData->GetMeshCount();
		++iter;
	}
	return meshCount;
}

/**
* 统计三角形个数
* @return 三角形统计信息
*/
int CSceneManager::GetTriangleCount() {
	int triangleCount = 0;
	map<string, CSceneNode*>::iterator iter = m_mapSceneNode.begin();
	while (iter != m_mapSceneNode.end()) {
		CMeshData* pMeshData = iter->second->GetMeshData();
		if (pMeshData) triangleCount += pMeshData->GetTriangleCount();
		++iter;
	}
	return triangleCount;
}

/**
* 统计顶点个数
* @return 顶点统计信息
*/
int CSceneManager::GetVertexCount() {
	int vertexCount = 0;
	map<string, CSceneNode*>::iterator iter = m_mapSceneNode.begin();
	while (iter != m_mapSceneNode.end()) {
		CMeshData* pMeshData = iter->second->GetMeshData();
		if (pMeshData) vertexCount += pMeshData->GetVertexCount();
		++iter;
	}
	return vertexCount;
}

/**
* 递归更新子场景节点
* @param parent 父节点
* @param dt 时间步长
*/
void CSceneManager::UpdateAll(CSceneNode* parent, float dt) {
	list<CSceneNode*>::iterator iter = parent->m_lstChildren.begin();
	while (iter != parent->m_lstChildren.end()) {
		CSceneNode* pSceneNode = *iter;
		if (pSceneNode->m_bEnabled) {
			// 先更新父节点
			pSceneNode->Update(dt);
			UpdateAll(pSceneNode, dt);
		}
		++iter;
	}
}

/**
* 递归移除所有子节点
* @param parent 父节点
*/
void CSceneManager::DeleteAll(CSceneNode* parent) {
	list<CSceneNode*>::iterator iter = parent->m_lstChildren.begin();
	while (iter != parent->m_lstChildren.end()) {
		CSceneNode* pSceneNode = *iter;
		// 先删除子节点
		DeleteAll(pSceneNode);
		pSceneNode->Destroy();
		delete pSceneNode;
		++iter;
	}
	parent->m_lstChildren.clear();
}

/**
* 使用射线对场景节点的网格三角形选择
* @param node 需要测试的场景节点
* @param ray 拾取射线
* @param hit 射线与节点的交点
* @param mesh 相交的网格
* @param face 相交的三角形
* @return 交点距离，负数表示不相交
*/
float CSceneManager::Picking(CSceneNode* node, const CRay& ray, CVector3& hit, int* mesh, int* face) {
	CMeshData* pMeshData = node->GetMeshData();
	if (!pMeshData) return -1.0f;
	// 将射线转换到节点局部坐标系下
	CRay localRay = ray;
	CMatrix4 worldMat(node->m_cWorldMatrix);
	localRay.Transform(worldMat.Invert());
	// 对每个网格内的三角形进行相交测试
	int meshCount = pMeshData->GetMeshCount();
	int meshIndex, faceIndex;
	float bu, bv;
	float distance = -1.0f;
	for (int i = 0; i < meshCount; i++) {
		int f; float u, v;
		float d = pMeshData->GetMesh(i)->Intersects(localRay, &f, &u, &v);
		if (d > 0 && (d < distance || distance < 0)) {
			meshIndex = i;
			faceIndex = f;
			bu = u;
			bv = v;
			distance = d;
		}
	}
	// 有交点
	if (distance >= 0) {
		CMesh* pMesh = pMeshData->GetMesh(meshIndex);
		float* v0 = pMesh->GetVertex(faceIndex, 0)->m_fPosition;
		float* v1 = pMesh->GetVertex(faceIndex, 1)->m_fPosition;
		float* v2 = pMesh->GetVertex(faceIndex, 2)->m_fPosition;
		const float bw = 1.0f - bu - bv;
		hit[0] = bw * v0[0] + bu * v1[0] + bv * v2[0];
		hit[1] = bw * v0[1] + bu * v1[1] + bv * v2[1];
		hit[2] = bw * v0[2] + bu * v1[2] + bv * v2[2];
		hit = node->m_cWorldMatrix * hit;
		*mesh = meshIndex;
		*face = faceIndex;
	}
	return distance;
}

/**
* 半透明分组节点排序
*/
void CSceneManager::SortTranslucentNode() {
	// 排序半透明场景节点
	list<SRenderNode>* translucentList = &m_vecRenderGroup[4].nodeList;
	if (translucentList->empty()) return;
	list<SRenderNode>::iterator iter = translucentList->begin();
	list<SRenderNode>::iterator next = translucentList->begin();
	CVector3 cameraPos = CEngine::GetGraphicsManager()->GetCamera()->m_cPosition;
	// 计算节点与摄像机的距离平方
	while (iter != translucentList->end()) {
		CVector3 d = iter->sceneNode->GetWorldPosition() - cameraPos;
		iter->distance = d[0] * d[0] + d[1] * d[1] + d[2] * d[2];
		++iter;
	}
	// 从大到小插入排序，针对基本有序列表具有最高的排序效率
	iter = translucentList->begin();
	while (++next != translucentList->end()) {
		if (iter->distance < next->distance) {
			while (true) {
				if (iter != translucentList->begin()) {
					if ((--iter)->distance >= next->distance) {
						translucentList->insert(++iter, *next);
						next = translucentList->erase(next);
						iter = --next;
						break;
					}
				} else {
					translucentList->insert(iter, *next);
					next = translucentList->erase(next);
					iter = --next;
					break;
				}
			}
		} else ++iter;
	}
}

/**
* 屏幕节点排序
* @remark 按 position.z 的值从小到大排序
*/
void CSceneManager::SortScreenNode() {
	if (m_lstScreenNode.empty()) return;
	list<CSceneNode*>::iterator iter = m_lstScreenNode.begin();
	list<CSceneNode*>::iterator next = m_lstScreenNode.begin();
	while (++next != m_lstScreenNode.end()) {
		CSceneNode* n1 = *iter;
		CSceneNode* n2 = *next;
		if (n1->m_bVisible && n2->m_bVisible) {
			float z1 = n1->GetWorldPosition()[2];
			float z2 = n2->GetWorldPosition()[2];
			if (z1 > z2) {
				while (true) {
					if (iter != m_lstScreenNode.begin()) {
						n1 = *--iter;
						z1 = n1->GetWorldPosition()[2];
						if (z1 <= z2) {
							m_lstScreenNode.insert(++iter, *next);
							next = m_lstScreenNode.erase(next);
							iter = --next;
							break;
						}
					} else {
						m_lstScreenNode.insert(iter, *next);
						next = m_lstScreenNode.erase(next);
						iter = --next;
						break;
					}
				}
			} else ++iter;
		} else ++iter;
	}
}

/**
* 初始化渲染分组
*/
void CSceneManager::SetupRenderGroup() {
	// 初始化渲染分组，排在前面的优先渲染
	m_vecRenderGroup.push_back(SRenderGroup("opaque"));
	m_vecRenderGroup.push_back(SRenderGroup("decal"));
	m_vecRenderGroup.push_back(SRenderGroup("sky"));
	m_vecRenderGroup.push_back(SRenderGroup("water"));
	m_vecRenderGroup.push_back(SRenderGroup("translucent"));
	m_vecRenderGroup.push_back(SRenderGroup("overlay"));
	// 系统内置着色器列表
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	m_vecShaderList.push_back(pShaderMgr->GetShader("default"));
	m_vecShaderList.push_back(pShaderMgr->GetShader("light"));
	m_vecShaderList.push_back(pShaderMgr->GetShader("texture"));
	m_vecShaderList.push_back(pShaderMgr->GetShader("texturelight"));
	m_vecShaderList.push_back(pShaderMgr->GetShader("skybox"));
	m_vecShaderList.push_back(pShaderMgr->GetShader("terrain"));
	m_vecShaderList.push_back(pShaderMgr->GetShader("cloud"));
	m_vecShaderList.push_back(pShaderMgr->GetShader("fresnel"));
	m_vecShaderList.push_back(pShaderMgr->GetShader("flame"));
	m_vecShaderList.push_back(pShaderMgr->GetShader("blast"));
	m_vecShaderList.push_back(pShaderMgr->GetShader("water"));
	// 场景节点类型集合 (group_index, shader_index, render_depth)
	m_mapRenderType.insert(std::pair<string, SRenderType>("animation", SRenderType(0, 3, true)));
	m_mapRenderType.insert(std::pair<string, SRenderType>("axis", SRenderType(0, 1, false)));
	m_mapRenderType.insert(std::pair<string, SRenderType>("blast", SRenderType(4, 9, false)));
	m_mapRenderType.insert(std::pair<string, SRenderType>("board", SRenderType(4, 2, true)));
	m_mapRenderType.insert(std::pair<string, SRenderType>("fresnel", SRenderType(4, 7, false)));
	m_mapRenderType.insert(std::pair<string, SRenderType>("cloud", SRenderType(2, 6, false)));
	m_mapRenderType.insert(std::pair<string, SRenderType>("decal", SRenderType(1, 3, false)));
	m_mapRenderType.insert(std::pair<string, SRenderType>("flame", SRenderType(4, 8, false)));
	m_mapRenderType.insert(std::pair<string, SRenderType>("geometry", SRenderType(0, 3, true)));
	m_mapRenderType.insert(std::pair<string, SRenderType>("lensflare", SRenderType(5, -1, false)));
	m_mapRenderType.insert(std::pair<string, SRenderType>("line", SRenderType(5, 0, true)));
	m_mapRenderType.insert(std::pair<string, SRenderType>("mesh", SRenderType(0, 3, true)));
	m_mapRenderType.insert(std::pair<string, SRenderType>("particles", SRenderType(4, 2, false)));
	m_mapRenderType.insert(std::pair<string, SRenderType>("planet", SRenderType(4, -1, false)));
	m_mapRenderType.insert(std::pair<string, SRenderType>("plant", SRenderType(4, 2, true)));
	m_mapRenderType.insert(std::pair<string, SRenderType>("skybox", SRenderType(2, 4, false)));
	m_mapRenderType.insert(std::pair<string, SRenderType>("sound", SRenderType(0, -1, false)));
	m_mapRenderType.insert(std::pair<string, SRenderType>("static", SRenderType(0, 3, true)));
	m_mapRenderType.insert(std::pair<string, SRenderType>("terrain", SRenderType(0, 5, true)));
	m_mapRenderType.insert(std::pair<string, SRenderType>("text", SRenderType(4, 2, false)));
	m_mapRenderType.insert(std::pair<string, SRenderType>("water", SRenderType(3, 10, false)));
}