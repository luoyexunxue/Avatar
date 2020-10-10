//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeTerrain.h"
#include "CEngine.h"
#include "CVector2.h"
#include "CVector3.h"
#include "CStringUtil.h"
#include <cstring>

/**
* 构造函数
*/
CSceneNodeTerrain::CSceneNodeTerrain(const string& name, const string& heightMap, float mapScale,
		float heightScale, const string texture[4], const string& blendMap): CSceneNode("terrain", name) {
	m_strHeightMap = heightMap;
	m_strTexture[0] = texture[0];
	m_strTexture[1] = texture[1];
	m_strTexture[2] = texture[2];
	m_strTexture[3] = texture[3];
	m_strBlendTexture = blendMap;
	m_pTreeRoot = 0;
	m_iMaxLevel = 0;
	m_iMeshLevel = 0;
	m_pHeightMap = new SHeightMap();
	m_pHeightMap->hScale = mapScale;
	m_pHeightMap->vScale = heightScale;
	m_pMeshData = 0;
}

/**
* 初始化场景节点
*/
bool CSceneNodeTerrain::Init() {
	// 读取高度图，宽高必须相等且为2的幂
	if (!LoadHeightMap(m_strHeightMap, m_pHeightMap)) return false;
	// 地形材质
	CShader* pShader = CEngine::GetShaderManager()->GetShader("terrain");
	pShader->UseShader();
	pShader->SetUniform("uTexture[0]", 0);
	pShader->SetUniform("uTexture[1]", 1);
	pShader->SetUniform("uTexture[2]", 2);
	pShader->SetUniform("uTexture[3]", 3);
	pShader->SetUniform("uTexture[4]", 4);
	pShader->SetUniform("uTextureScale[0]", CVector2(m_pHeightMap->hScale, m_pHeightMap->hScale) * 0.1f);
	pShader->SetUniform("uTextureScale[1]", CVector2(1.0f, 1.0f));
	// 地形高度分层
	const float height1 = 0.0f * m_pHeightMap->vScale;
	const float height2 = 0.3f * m_pHeightMap->vScale;
	const float height3 = 0.7f * m_pHeightMap->vScale;
	pShader->SetUniform("uHeightRange", CVector3(height1, height2, height3));
	// 高度图大小的2的幂次就是四叉树最大级别
	int maxLevel = 0;
	int mapSize = m_pHeightMap->size >> 1;
	while (mapSize > 0) {
		mapSize >>= 1;
		maxLevel += 1;
	}
	// 网格所在级别边长最大为 129
	m_iMeshLevel = maxLevel - 7 < 0 ? 0 : maxLevel - 7;
	m_iMaxLevel = maxLevel;
	m_pTreeRoot = BuildQuadTree(0, 0, 0, m_pHeightMap->size);
	m_pMeshData = new CMeshData();
	BuildTerrainMesh(m_pTreeRoot, 0, 0);
	for (size_t i = 0; i < m_pMeshData->GetMeshCount(); i++) {
		CMaterial* material = m_pMeshData->GetMesh(i)->GetMaterial();
		material->SetShader(pShader);
		material->SetTexture(m_strTexture[0], 0);
		material->SetTexture(m_strTexture[1], 1);
		material->SetTexture(m_strTexture[2], 2);
		material->SetTexture(m_strTexture[3], 3);
		material->SetTexture(m_strBlendTexture, 4);
		material->GetTexture(4)->SetWrapModeMirroredRepeat(true, true);
	}
	return true;
}

/**
* 销毁场景节点
*/
void CSceneNodeTerrain::Destroy() {
	if (m_pHeightMap->data) delete[] m_pHeightMap->data;
	if (m_pHeightMap->flag) delete[] m_pHeightMap->flag;
	if (m_pTreeRoot) DeleteQuadTree(m_pTreeRoot);
	if (m_pMeshData) delete m_pMeshData;
	delete m_pHeightMap;
}

/**
* 渲染场景节点
*/
void CSceneNodeTerrain::Render() {
	// 首先更新网格顶点索引缓冲
	CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
	CheckVisibility(m_pTreeRoot, pCamera->GetFrustum(), pCamera->m_cPosition);
	UpdateIndexBuffer(m_pTreeRoot);
	if (CEngine::GetGraphicsManager()->IsDepthRender()) {
		CEngine::GetTextureManager()->GetTexture("white")->UseTexture();
		RenderTerrain(m_pTreeRoot, false);
	} else RenderTerrain(m_pTreeRoot, true);
}

/**
* 重载坐标变换
*/
void CSceneNodeTerrain::Transform() {
	m_cScale.SetValue(CVector3::One);
	m_cOrientation.SetValue(0.0f, 0.0f, 0.0f, 1.0f);
	if (m_pTreeRoot) TranslateMesh(m_pTreeRoot);
	CSceneNode::Transform();
}

/**
* 获取网格数据
*/
CMeshData* CSceneNodeTerrain::GetMeshData() {
	return m_pMeshData;
}

/**
* 获取指定地点的高度
*/
float CSceneNodeTerrain::GetHeight(float x, float y) const {
	const float mapOffset = m_pHeightMap->size * 0.5f;
	const float mapScale = static_cast<float>(m_pHeightMap->size) / m_pHeightMap->hScale;
	int sx = static_cast<int>(x * mapScale + mapOffset);
	int sy = static_cast<int>(y * mapScale + mapOffset);
	if (sx <= m_pHeightMap->size && sy <= m_pHeightMap->size) {
		return m_pHeightMap->data[sx + sy * (m_pHeightMap->size + 1)] + m_cPosition.m_fValue[2];
	}
	return m_cPosition.m_fValue[2];
}

/**
* 获取指定地点的法向
*/
CVector3 CSceneNodeTerrain::GetNormal(float x, float y) const {
	const float mapOffset = m_pHeightMap->size * 0.5f;
	const float mapScale = static_cast<float>(m_pHeightMap->size) / m_pHeightMap->hScale;
	int sx = static_cast<int>(x * mapScale + mapOffset);
	int sy = static_cast<int>(y * mapScale + mapOffset);
	CVector3 normal(0.0f, 0.0f, 1.0f, 0.0f);
	if (sx <= m_pHeightMap->size && sy <= m_pHeightMap->size) {
		GetMapNormal(sx, sy, normal);
	}
	return normal;
}

/**
* 读取高度图
*/
bool CSceneNodeTerrain::LoadHeightMap(const string& filename, SHeightMap* heightMap) {
	CFileManager::FileType fileType;
	string ext = CStringUtil::UpperCase(CFileManager::GetExtension(filename));
	if (ext == "BMP") fileType = CFileManager::BMP;
	else if (ext == "TGA") fileType = CFileManager::TGA;
	else if (ext == "PNG") fileType = CFileManager::PNG;
	else if (ext == "JPG" || ext == "JPEG") fileType = CFileManager::JPG;
	else return false;
	CFileManager::CImageFile file(fileType);
	if (!CEngine::GetFileManager()->ReadFile(filename, &file)) return false;
	// 高度图长宽必须相等，且为2的幂次
	if (file.width != file.height || (file.width & (file.width - 1)) != 0) return false;
	heightMap->size = file.width;
	heightMap->data = new float[(file.width + 1) * (file.height + 1)];
	for (int i = 0; i < file.height; i++) {
		for (int j = 0; j < file.width; j++) {
			int indexSrc = j + i * file.width;
			int indexDes = j + (file.height - i - 1) * (file.width + 1);
			int data = (file.contents[indexSrc * file.channels] & 0x00FF) - 0x80;
			heightMap->data[indexDes] = data * heightMap->vScale * 0.007874f;
		}
	}
	// 扩展高度图大小至 2^n + 1
	int newSize = heightMap->size + 1;
	int index1 = newSize * (newSize - 1);
	int index2 = newSize * (newSize - 2);
	heightMap->data[index1 + newSize - 1] = heightMap->data[index2 + newSize - 2];
	for (int i = 0; i < heightMap->size; i++) {
		heightMap->data[index1 + i] = heightMap->data[index2 + i];
		heightMap->data[newSize * (i + 1) - 1] = heightMap->data[newSize * (i + 1) - 2];
	}
	// 用于修补裂缝的顶点标记
	heightMap->flag = new bool[newSize * newSize];
	memset(heightMap->flag, 0, sizeof(bool) * newSize * newSize);
	return true;
}

/**
* 递归生成地形四叉树
*/
CSceneNodeTerrain::SQuadTree* CSceneNodeTerrain::BuildQuadTree(SQuadTree* parent, int index, int level, int size) {
	if (level >= m_iMaxLevel) return 0;
	const int halfSize = size >> 1;
	const int mapLength = m_pHeightMap->size + 1;
	SQuadTree* node = new SQuadTree();
	node->mesh = 0;
	node->parent = parent;
	node->visible = false;
	node->level = level;
	node->size = size;
	if (parent) {
		if (index == 0) node->center = parent->center + (mapLength + 1) * halfSize;
		else if (index == 1) node->center = parent->center + (mapLength - 1) * halfSize;
		else if (index == 2) node->center = parent->center - (mapLength + 1) * halfSize;
		else if (index == 3) node->center = parent->center - (mapLength - 1) * halfSize;
	} else {
		node->center = (mapLength + 1) * halfSize;
	}
	int cx = node->center / mapLength;
	int cy = node->center % mapLength;
	node->corner[0] = node->center + halfSize * (mapLength + 1);
	node->corner[1] = node->center + halfSize * (mapLength - 1);
	node->corner[2] = node->center - halfSize * (mapLength + 1);
	node->corner[3] = node->center - halfSize * (mapLength - 1);
	node->children[0] = BuildQuadTree(node, 0, level + 1, halfSize);
	node->children[1] = BuildQuadTree(node, 1, level + 1, halfSize);
	node->children[2] = BuildQuadTree(node, 2, level + 1, halfSize);
	node->children[3] = BuildQuadTree(node, 3, level + 1, halfSize);
	// 计算节点包围盒
	const float mapOffset = m_pHeightMap->size * 0.5f;
	const float mapScale = m_pHeightMap->hScale / static_cast<float>(m_pHeightMap->size);
	if (node->IsInternal()) {
		node->volume.SetValue(node->children[0]->volume);
		node->volume.Update(node->children[1]->volume);
		node->volume.Update(node->children[2]->volume);
		node->volume.Update(node->children[3]->volume);
	} else {
		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				float x = (cx + i - mapOffset) * mapScale;
				float y = (cy + j - mapOffset) * mapScale;
				float z = m_pHeightMap->data[cx + i + (cy + j) * mapLength];
				node->volume.Update(CVector3(x, y, z));
			}
		}
	}
	// 节点中心坐标和粗糙度预计算
	float px = (cx - mapOffset) * mapScale;
	float py = (cy - mapOffset) * mapScale;
	node->position.SetValue(px, py, m_pHeightMap->data[cx + cy * mapLength]);
	node->roughness = node->volume.m_cMax[2] - node->volume.m_cMin[2];
	return node;
}

/**
* 递归生成地形网格
*/
void CSceneNodeTerrain::BuildTerrainMesh(SQuadTree* node, int index, int size) {
	if (node->level == m_iMeshLevel) {
		CMesh* mesh = new CMesh();
		mesh->SetVertexUsage((node->size + 1) * (node->size + 1));
		const int halfSize = node->size >> 1;
		const int mapLength = m_pHeightMap->size + 1;
		const float mapOffset = m_pHeightMap->size * 0.5f;
		const float mapScale = m_pHeightMap->hScale / static_cast<float>(m_pHeightMap->size);
		const float texScale = 1.0f / m_pHeightMap->hScale;
		int sx = (node->center / mapLength) - halfSize;
		int sy = (node->center % mapLength) - halfSize;
		CVector3 normal;
		for (int i = 0; i <= node->size; i++) {
			for (int j = 0; j <= node->size; j++) {
				float x = (sx + i - mapOffset) * mapScale;
				float y = (sy + j - mapOffset) * mapScale;
				float z = m_pHeightMap->data[sx + i + (sy + j) * mapLength];
				float s = 0.5f + x * texScale;
				float t = 0.5f - y * texScale;
				GetMapNormal(sx + i, sy + j, normal);
				mesh->AddVertex(CVertex(x, y, z, s, t, normal[0], normal[1], normal[2]));
			}
		}
		for (int i = 0; i < node->size; i++) {
			for (int j = 0; j < node->size; j++) {
				int i1 = i * (node->size + 1) + j;
				int i2 = i1 + node->size + 1;
				mesh->AddTriangle(i1, i2, i1 + 1);
				mesh->AddTriangle(i2, i2 + 1, i1 + 1);
			}
		}
		mesh->Create(true);
		m_pMeshData->AddMesh(mesh);
		node->mesh = mesh;
		// 四个角分别为 右上 右下 左下 左上
		node->index[0] = node->size * (node->size + 2);
		node->index[1] = node->size * (node->size + 1);
		node->index[2] = 0;
		node->index[3] = node->size;
		node->index[4] = halfSize * (node->size + 2);
		size = node->size;
	} else if (node->level > m_iMeshLevel) {
		const int halfSize = node->size >> 1;
		if (index == 0) node->index[0] = node->parent->index[0];
		else if (index == 1) node->index[0] = node->parent->index[0] - node->size;
		else if (index == 2) node->index[0] = node->parent->index[0] - node->size * (size + 2);
		else if (index == 3) node->index[0] = node->parent->index[0] - node->size * (size + 1);
		node->index[1] = node->index[0] - node->size;
		node->index[2] = node->index[0] - node->size * (size + 2);
		node->index[3] = node->index[0] - node->size * (size + 1);
		node->index[4] = node->index[0] - halfSize * (size + 2);
		node->mesh = node->parent->mesh;
	}
	if (node->IsInternal()) {
		BuildTerrainMesh(node->children[0], 0, size);
		BuildTerrainMesh(node->children[1], 1, size);
		BuildTerrainMesh(node->children[2], 2, size);
		BuildTerrainMesh(node->children[3], 3, size);
	}
}

/**
* 递归清空四叉树
*/
void CSceneNodeTerrain::DeleteQuadTree(SQuadTree* node) {
	if (node->IsInternal()) {
		DeleteQuadTree(node->children[0]);
		DeleteQuadTree(node->children[1]);
		DeleteQuadTree(node->children[2]);
		DeleteQuadTree(node->children[3]);
	}
	delete node;
}

/**
* 递归检查四叉树节点可见性
*/
void CSceneNodeTerrain::CheckVisibility(SQuadTree* node, const CFrustum& frustum, const CVector3& eye) {
	// 分辨率调节因子
	const float resolution = 0.05f;
	node->visible = frustum.IsOverlapAABB(node->volume);
	if (node->visible) {
		bool endRetrieval = node->IsLeaf();
		if (!endRetrieval) {
			float error = resolution * (eye - node->position).Length() / node->roughness;
			if (error >= 1.0f && node->level > m_iMeshLevel) {
				node->children[0]->visible = false;
				node->children[1]->visible = false;
				node->children[2]->visible = false;
				node->children[3]->visible = false;
				endRetrieval = true;
			} else {
				CheckVisibility(node->children[0], frustum, eye);
				CheckVisibility(node->children[1], frustum, eye);
				CheckVisibility(node->children[2], frustum, eye);
				CheckVisibility(node->children[3], frustum, eye);
			}
		}
		// 标记每一个可能存在裂缝的角
		if (endRetrieval) {
			m_pHeightMap->flag[node->corner[0]] = true;
			m_pHeightMap->flag[node->corner[1]] = true;
			m_pHeightMap->flag[node->corner[2]] = true;
			m_pHeightMap->flag[node->corner[3]] = true;
		}
	}
}

/**
* 递归渲染地形
*/
void CSceneNodeTerrain::RenderTerrain(SQuadTree* node, bool useMaterial) {
	if (node->visible) {
		if (node->mesh) node->mesh->Render(useMaterial);
		else {
			RenderTerrain(node->children[0], useMaterial);
			RenderTerrain(node->children[1], useMaterial);
			RenderTerrain(node->children[2], useMaterial);
			RenderTerrain(node->children[3], useMaterial);
		}
	}
}

/**
* 递归平移地形网格
*/
void CSceneNodeTerrain::TranslateMesh(SQuadTree* node) {
	const int mapLength = m_pHeightMap->size + 1;
	const float mapOffset = m_pHeightMap->size * 0.5f;
	const float mapScale = m_pHeightMap->hScale / static_cast<float>(m_pHeightMap->size);
	int cx = node->center / mapLength;
	int cy = node->center % mapLength;
	float px = (cx - mapOffset) * mapScale;
	float py = (cy - mapOffset) * mapScale;
	node->position.SetValue(px, py, m_pHeightMap->data[cx + cy * mapLength]);
	node->position.Add(m_cPosition);
	// 重新计算节点包围盒
	if (node->IsInternal()) {
		TranslateMesh(node->children[0]);
		TranslateMesh(node->children[1]);
		TranslateMesh(node->children[2]);
		TranslateMesh(node->children[3]);
		node->volume.SetValue(node->children[0]->volume);
		node->volume.Update(node->children[1]->volume);
		node->volume.Update(node->children[2]->volume);
		node->volume.Update(node->children[3]->volume);
	} else {
		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				float x = (cx + i - mapOffset) * mapScale;
				float y = (cy + j - mapOffset) * mapScale;
				float z = m_pHeightMap->data[cx + i + (cy + j) * mapLength];
				node->volume.Update(CVector3(x, y, z));
			}
		}
		node->volume.Offset(m_cPosition);
	}
}

/**
* 递归更新地形网格索引缓冲
*/
bool CSceneNodeTerrain::UpdateIndexBuffer(SQuadTree* node) {
	if (node->visible) {
		CMesh* mesh = node->mesh;
		if (node->level == m_iMeshLevel) {
			mesh->RemoveTriangle(0, -1);
		}
		bool childInvisible = true;
		if (node->IsInternal()) {
			childInvisible &= UpdateIndexBuffer(node->children[0]);
			childInvisible &= UpdateIndexBuffer(node->children[1]);
			childInvisible &= UpdateIndexBuffer(node->children[2]);
			childInvisible &= UpdateIndexBuffer(node->children[3]);
		}
		// 四个子节点都不可见的情况下绘制当前节点
		if (mesh && childInvisible) {
			AddTriangle(node, node->index[4], node->index[1], node->index[0], node->corner[1], node->corner[0]);
			AddTriangle(node, node->index[4], node->index[2], node->index[1], node->corner[2], node->corner[1]);
			AddTriangle(node, node->index[4], node->index[3], node->index[2], node->corner[3], node->corner[2]);
			AddTriangle(node, node->index[4], node->index[0], node->index[3], node->corner[0], node->corner[3]);
		}
		if (node->level == m_iMeshLevel) {
			mesh->Update(2);
		}
	}
	return !node->visible;
}

/**
* 添加三角形并修补裂缝
*/
void CSceneNodeTerrain::AddTriangle(SQuadTree* node, int center, int index1, int index2, int flag1, int flag2) {
	const int powerOfTwo = m_iMaxLevel - node->level - 1;
	const int stepLocal = (index2 - index1) >> powerOfTwo;
	const int stepWorld = (flag2 - flag1) >> powerOfTwo;
	int indexA = stepLocal + index1;
	int indexB = stepWorld + flag1;
	CMesh* mesh = node->mesh;
	// 对边 index1-index2 进行遍历，添加分割的顶点索引
	while (indexB != flag2) {
		if (m_pHeightMap->flag[indexB]) {
			m_pHeightMap->flag[indexB] = false;
			mesh->AddTriangle(center, index1, indexA);
			index1 = indexA;
		}
		indexA += stepLocal;
		indexB += stepWorld;
	}
	mesh->AddTriangle(center, index1, indexA);
}

/**
* 计算高度图某处的法向量
*/
void CSceneNodeTerrain::GetMapNormal(int x, int y, CVector3& normal) const {
	int x1 = x - 1;
	int y1 = y - 1;
	int x2 = x + 1;
	int y2 = y + 1;
	if (x1 < 0) x1 = x;
	if (y1 < 0) y1 = y;
	if (x2 > m_pHeightMap->size) x2 = x;
	if (y2 > m_pHeightMap->size) y2 = y;
	// 计算周围四个点相对目标点的向量
	const int mapLength = m_pHeightMap->size + 1;
	const float mapScale = m_pHeightMap->hScale / static_cast<float>(m_pHeightMap->size);
	float offset = m_pHeightMap->data[x + y * mapLength];
	CVector3 v1(-mapScale, 0.0f, m_pHeightMap->data[x1 + y * mapLength] - offset);
	CVector3 v2(0.0f, -mapScale, m_pHeightMap->data[x + y1 * mapLength] - offset);
	CVector3 v3(mapScale, 0.0f, m_pHeightMap->data[x2 + y * mapLength] - offset);
	CVector3 v4(0.0f, mapScale, m_pHeightMap->data[x + y2 * mapLength] - offset);
	normal = v1.CrossProduct(v2) + v2.CrossProduct(v3) + v3.CrossProduct(v4) + v4.CrossProduct(v1);
	normal.Normalize();
}