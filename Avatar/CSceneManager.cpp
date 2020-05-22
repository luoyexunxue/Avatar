//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneManager.h"
#include "CEngine.h"
#include "CLog.h"
#include <cstring>

/**
* ���캯��
*/
CSceneManager::CSceneManager() {
	m_pRootNode = new CRootNode();
	m_pRootNode->Init();
	SetupRenderGroup();
}

/**
* ��������
*/
CSceneManager::~CSceneManager() {
	m_pInstance = 0;
}

/**
* ����ʵ��
*/
CSceneManager* CSceneManager::m_pInstance = 0;

/**
* ���ٳ���������
*/
void CSceneManager::Destroy() {
	ClearNode();
	delete m_pRootNode;
	delete this;
}

/**
* ��Ⱦ���е� 3D �����ڵ�
* @param view �����ͼ����
* @param proj ���ͶӰ����
*/
void CSceneManager::RenderScene(const CMatrix4& view, const CMatrix4& proj) {
	// ������ͼͶӰ����
	CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
	for (size_t i = 0; i < m_vecShaderList.size(); i++) {
		CShader* shader = m_vecShaderList[i];
		shader->UseShader();
		shader->SetUniform("uProjMatrix", proj);
		shader->SetUniform("uViewMatrix", view);
		shader->SetUniform("uCameraPos", pCamera->m_cPosition);
	}
	// �Գ����ڵ���з�����Ⱦ
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
* ��Ⱦ��Ļ�����ڵ�
* @param proj ͶӰ����
* @param offset ������Ļ���ĵ�ƫ��
* @note ��Ҫ����Ȳ��Թرգ�����������
*/
void CSceneManager::RenderScreen(const CMatrix4& proj, const CVector2& offset) {
	// ����ͶӰ���󣬺�����ƫ��
	CShader* pShader = CEngine::GetShaderManager()->GetShader("screen");
	pShader->UseShader();
	pShader->SetUniform("uProjMatrix", proj);
	pShader->SetUniform("uOffset", offset);
	// ��Ⱦ������Ļ�����ڵ�
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
* ��Ⱦ�����ͼ
* @param viewProj ��Ⱦ��ȵ���ͼͶӰ����
* @param shader �����Ⱦ����ɫ������
* @note ��Ҫ����ɫ��Ϲرգ�����������
*/
void CSceneManager::RenderDepth(const CMatrix4& viewProj, const string& shader) {
	CShader* pShader = CEngine::GetShaderManager()->GetShader(shader);
	pShader->UseShader();
	pShader->SetUniform("uViewProjMatrix", viewProj);
	// ��Ⱦ����ʵ�峡���ڵ�
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
* �������г����ڵ�
* @param dt ʱ�䲽��
*/
void CSceneManager::Update(float dt) {
	// �ݹ���³����ڵ�
	UpdateAll(m_pRootNode, dt);
	// �ڵ�����
	SortTranslucentNode();
	SortScreenNode();
}

/**
* ��ӳ����ڵ�
* @param node ������ĳ����ڵ�
* @param parent ����ĸ��ڵ㣬��Ϊ���� node ������ڵ�
* @return �Ƿ����ɹ��������� false �� node ���Զ��ͷ�
*/
bool CSceneManager::InsertNode(CSceneNode* node, CSceneNode* parent) {
	node->m_pParent = parent ? parent : m_pRootNode;
	if (m_mapSceneNode.count(node->GetName()) > 0) {
		CLog::Warn("Scene node '%s' already exist", node->GetName().c_str());
		delete node;
		return false;
	}
	// ���г�ʼ��������ʼ�����ɹ��򽫽ڵ�ɾ��
	if (!node->Init()) {
		CLog::Warn("Initialize scene node '%s' failed", node->GetName().c_str());
		delete node;
		return false;
	}
	node->m_pParent->m_lstChildren.push_back(node);
	m_mapSceneNode.insert(std::pair<string, CSceneNode*>(node->GetName(), node));
	// ͨ���жϽڵ����ͣ�ѡ���Լ�����Ӧ�б�
	if (node->GetType() == "screen") {
		m_lstScreenNode.push_back(node);
	} else {
		// ����͸����ϵ���з��飬���Գ����ڵ㰴���ʽ������򣬼��� OpenGL ״̬�л�
		SRenderType& renderType = m_mapRenderType.at(node->GetType());
		SRenderGroup& renderGroup = m_vecRenderGroup[renderType.groupIndex];
		list<SRenderNode>::iterator iter = renderGroup.nodeList.begin();
		while (iter != renderGroup.nodeList.end()) {
			if (renderType.shaderIndex < iter->shaderIndex) break;
			++iter;
		}
		renderGroup.nodeList.insert(iter, SRenderNode(renderType.shaderIndex, node));
		// ���������Ⱦ
		if (renderType.renderDepth) {
			m_lstDepthNode.push_back(node);
		}
	}
	node->Transform();
	return true;
}

/**
* ɾ�������ڵ㣬���ӽڵ㽫���Զ������丸�ڵ�
* @param node ��Ҫɾ���ĳ����ڵ㣬���ڵ����
* @return �Ƿ�ɾ���ɹ��������� true �� node ���Զ��ͷ�
*/
bool CSceneManager::DeleteNode(CSceneNode* node) {
	if (!node) return false;
	CSceneNode* parent = node->m_pParent;
	parent->m_lstChildren.remove(node);
	// ���Ƴ��ڵ���ӽڵ�����丸�ڵ�
	list<CSceneNode*>::iterator iter = node->m_lstChildren.begin();
	while (iter != node->m_lstChildren.end()) {
		parent->m_lstChildren.push_back(*iter);
		++iter;
	}
	node->m_pParent = 0;
	node->m_lstChildren.clear();
	m_mapSceneNode.erase(node->GetName());
	// ɾ�������������б��еĽڵ�����
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
	// ���ٳ����ڵ�
	node->Destroy();
	delete node;
	return true;
}

/**
* ɾ��ָ�����Ƶĳ����ڵ㣬���ӽڵ㽫���Զ������丸�ڵ�
* @param name ��Ҫɾ���ĳ����ڵ�����
* @return �Ƿ�ɾ���ɹ��������� true ��ڵ㱻�Զ��ͷ�
*/
bool CSceneManager::DeleteNode(const string& name) {
	map<string, CSceneNode*>::iterator iter = m_mapSceneNode.find(name);
	if (iter != m_mapSceneNode.end()) {
		return DeleteNode(iter->second);
	}
	return false;
}

/**
* ������г����ڵ�
*/
void CSceneManager::ClearNode() {
	m_mapSceneNode.clear();
	m_lstScreenNode.clear();
	m_lstDepthNode.clear();
	// ��������еĽڵ�
	for (size_t i = 0; i < m_vecRenderGroup.size(); i++) {
		m_vecRenderGroup[i].nodeList.clear();
	}
	// �������нڵ�
	DeleteAll(m_pRootNode);
}

/**
* ��ȡ�������ڵ�
* @return ���ڵ�
*/
CSceneNode* CSceneManager::GetRootNode() {
	return m_pRootNode;
}

/**
* ͨ�� ID ��ȡ�ڵ�
* @param id ��Ҫ��ȡ�Ľڵ� ID
* @return ��Ӧ�ĳ����ڵ㣬���޶�Ӧ�ĳ����ڵ㣬��Ϊ��
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
* ͨ�����ƻ�ȡ�ڵ�
* @param name ��Ҫ��ȡ�Ľڵ�����
* @return ��Ӧ�ĳ����ڵ㣬���޶�Ӧ�ĳ����ڵ㣬��Ϊ��
*/
CSceneNode* CSceneManager::GetNodeByName(const string& name) {
	map<string, CSceneNode*>::iterator iter = m_mapSceneNode.find(name);
	if (iter != m_mapSceneNode.end()) {
		return iter->second;
	}
	return 0;
}

/**
* ͨ�����ͻ�ȡ�ڵ�
* @param type ��Ҫ��ȡ�Ľڵ�����
* @param index ָ��ƥ��� index ���ڵ�
* @param start ָ�����ڵ㣬��Ϊ�մ�����ڵ�
* @return ��Ӧ�ĳ����ڵ㣬���޶�Ӧ�ĳ����ڵ㣬��Ϊ��
*/
CSceneNode* CSceneManager::GetNodeByType(const string& type, int index, CSceneNode* start) {
	if (!start) {
		start = m_pRootNode;
	}
	// �ǵݹ鷽ʽ����������
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
* ͨ��ʰȡ���߻�ȡ�ڵ�
* @param ray ʰȡ����
* @param hit ��ȡ���ĳ����ڵ������ߵĽ���
* @param mesh ��ȡ�����������ཻ�Ľڵ�����
* @param face ��ȡ�����������ཻ��������
* @return ��Ӧ�ĳ����ڵ㣬���޶�Ӧ�ĳ����ڵ㣬��Ϊ��
*/
CSceneNode* CSceneManager::GetNodeByRay(const CRay& ray, CVector3& hit, int* mesh, int* face) {
	int hitMesh;
	int hitFace;
	CVector3 hitPoint;
	float minDistance = -1.0f;
	CSceneNode* pPickNode = 0;
	// �ǵݹ������
	list<CSceneNode*> lstStack;
	lstStack.push_back(m_pRootNode);
	while(!lstStack.empty()) {
		CSceneNode* pNode = lstStack.back();
		lstStack.pop_back();
		if (pNode->m_bVisible) {
			// �����ڵ��������ཻ�������ʹ�ð�Χ�п��ٲ���
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
* ��ȡ��������нڵ��б�
* @param nodeList ���صĽڵ��б�
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
* ��ȡ�����ָ�����ͽڵ��б�
* @param type ��Ҫ��ȡ�Ľڵ�����
* @param nodeList ���صĽڵ��б�
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
* ͳ����������
* @return �������ͳ����Ϣ
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
* ͳ�������θ���
* @return ������ͳ����Ϣ
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
* ͳ�ƶ������
* @return ����ͳ����Ϣ
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
* �ݹ�����ӳ����ڵ�
* @param parent ���ڵ�
* @param dt ʱ�䲽��
*/
void CSceneManager::UpdateAll(CSceneNode* parent, float dt) {
	list<CSceneNode*>::iterator iter = parent->m_lstChildren.begin();
	while (iter != parent->m_lstChildren.end()) {
		CSceneNode* pSceneNode = *iter;
		if (pSceneNode->m_bEnabled) {
			// �ȸ��¸��ڵ�
			pSceneNode->Update(dt);
			UpdateAll(pSceneNode, dt);
		}
		++iter;
	}
}

/**
* �ݹ��Ƴ������ӽڵ�
* @param parent ���ڵ�
*/
void CSceneManager::DeleteAll(CSceneNode* parent) {
	list<CSceneNode*>::iterator iter = parent->m_lstChildren.begin();
	while (iter != parent->m_lstChildren.end()) {
		CSceneNode* pSceneNode = *iter;
		// ��ɾ���ӽڵ�
		DeleteAll(pSceneNode);
		pSceneNode->Destroy();
		delete pSceneNode;
		++iter;
	}
	parent->m_lstChildren.clear();
}

/**
* ʹ�����߶Գ����ڵ������������ѡ��
* @param node ��Ҫ���Եĳ����ڵ�
* @param ray ʰȡ����
* @param hit ������ڵ�Ľ���
* @param mesh �ཻ������
* @param face �ཻ��������
* @return ������룬������ʾ���ཻ
*/
float CSceneManager::Picking(CSceneNode* node, const CRay& ray, CVector3& hit, int* mesh, int* face) {
	CMeshData* pMeshData = node->GetMeshData();
	if (!pMeshData) return -1.0f;
	// ������ת�����ڵ�ֲ�����ϵ��
	CRay localRay = ray;
	CMatrix4 worldMat(node->m_cWorldMatrix);
	localRay.Transform(worldMat.Invert());
	// ��ÿ�������ڵ������ν����ཻ����
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
	// �н���
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
* ��͸������ڵ�����
*/
void CSceneManager::SortTranslucentNode() {
	// �����͸�������ڵ�
	list<SRenderNode>* translucentList = &m_vecRenderGroup[4].nodeList;
	if (translucentList->empty()) return;
	list<SRenderNode>::iterator iter = translucentList->begin();
	list<SRenderNode>::iterator next = translucentList->begin();
	CVector3 cameraPos = CEngine::GetGraphicsManager()->GetCamera()->m_cPosition;
	// ����ڵ���������ľ���ƽ��
	while (iter != translucentList->end()) {
		CVector3 d = iter->sceneNode->GetWorldPosition() - cameraPos;
		iter->distance = d[0] * d[0] + d[1] * d[1] + d[2] * d[2];
		++iter;
	}
	// �Ӵ�С����������Ի��������б������ߵ�����Ч��
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
* ��Ļ�ڵ�����
* @remark �� position.z ��ֵ��С��������
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
* ��ʼ����Ⱦ����
*/
void CSceneManager::SetupRenderGroup() {
	// ��ʼ����Ⱦ���飬����ǰ���������Ⱦ
	m_vecRenderGroup.push_back(SRenderGroup("opaque"));
	m_vecRenderGroup.push_back(SRenderGroup("decal"));
	m_vecRenderGroup.push_back(SRenderGroup("sky"));
	m_vecRenderGroup.push_back(SRenderGroup("water"));
	m_vecRenderGroup.push_back(SRenderGroup("translucent"));
	m_vecRenderGroup.push_back(SRenderGroup("overlay"));
	// ϵͳ������ɫ���б�
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
	// �����ڵ����ͼ��� (group_index, shader_index, render_depth)
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