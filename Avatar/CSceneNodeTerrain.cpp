//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeTerrain.h"
#include "CEngine.h"
#include "CVector2.h"
#include "CVector3.h"
#include "CStringUtil.h"
#include "CLog.h"
#include <cstring>
#include <cmath>

/**
* 构造函数
* @note 纹理图片有4个通道，其中第4个通道为混合纹理通道，其余3个为分层纹理通道
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
	m_fErrorTolerance = 20.0f;
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
	m_iMaxLevel = 0;
	while ((m_pHeightMap->width & (1 << m_iMaxLevel)) == 0) m_iMaxLevel += 1;
	// 网格所在级别边长最大为 129
	m_iMeshLevel = m_iMaxLevel > 7 ? m_iMaxLevel - 7 : 0;
	m_pTreeRoot = BuildQuadTree(0, 0, 0, m_pHeightMap->width);
	m_pMeshData = new CMeshData();
	BuildTerrainMesh(m_pTreeRoot, 0);
	for (size_t i = 0; i < m_pMeshData->GetMeshCount(); i++) {
		CMaterial* material = m_pMeshData->GetMesh(i)->GetMaterial();
		material->SetTexture(m_strTexture[0], 0);
		material->SetTexture(m_strTexture[1], 1);
		material->SetTexture(m_strTexture[2], 2);
		material->SetTexture(m_strTexture[3], 3);
		material->SetTexture(m_strBlendTexture, 4);
		material->GetTexture(4)->SetWrapModeMirroredRepeat(true, true);
	}
	const float width = m_pHeightMap->hScale * 0.5f;
	m_cLocalBoundingBox.SetValue(-width, -width, -m_pHeightMap->vScale, width, width, m_pHeightMap->vScale);
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
	CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
	float fov = pCamera->GetFieldOfView() * 0.0174533f;
	float tolerance = m_fErrorTolerance * 2.0f * (1.0f - cosf(fov)) / (pCamera->GetViewWidth() * sinf(fov));
	CheckVisibility(m_pTreeRoot, pCamera->GetFrustum(), pCamera->m_cPosition, tolerance);
	// 更新网格顶点索引缓冲
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
	const float mapOffset = m_pHeightMap->width * 0.5f;
	const float mapScale = static_cast<float>(m_pHeightMap->width) / m_pHeightMap->hScale;
	x = x * mapScale + mapOffset;
	y = y * mapScale + mapOffset;
	int sx1 = static_cast<int>(x);
	int sy1 = static_cast<int>(y);
	int sx2 = sx1 + 1;
	int sy2 = sy1 + 1;
	if (sx2 < 0 || sy2 < 0 || sx1 > m_pHeightMap->width || sy1 > m_pHeightMap->width) return m_cPosition.m_fValue[2];
	if (sx1 < 0) sx1 = 0;
	if (sy1 < 0) sy1 = 0;
	if (sx2 > m_pHeightMap->width) sx2 = m_pHeightMap->width;
	if (sy2 > m_pHeightMap->width) sy2 = m_pHeightMap->width;
	// 双线性插值
	float p1 = m_pHeightMap->data[sx1 + sy1 * (m_pHeightMap->width + 1)];
	float p2 = m_pHeightMap->data[sx1 + sy2 * (m_pHeightMap->width + 1)];
	float p3 = m_pHeightMap->data[sx2 + sy1 * (m_pHeightMap->width + 1)];
	float p4 = m_pHeightMap->data[sx2 + sy2 * (m_pHeightMap->width + 1)];
	float kx = x - sx1;
	float ky = y - sy1;
	float height = p1 * (1.0f - kx) * (1.0f - ky);
	height += p2 * (1.0f - kx) * ky;
	height += p3 * kx * (1.0f - ky);
	height += p4 * kx * ky;
	return height + m_cPosition.m_fValue[2];
}

/**
* 获取指定地点的法向
*/
CVector3 CSceneNodeTerrain::GetNormal(float x, float y) const {
	const float mapOffset = m_pHeightMap->width * 0.5f;
	const float mapScale = static_cast<float>(m_pHeightMap->width) / m_pHeightMap->hScale;
	int sx = static_cast<int>(x * mapScale + mapOffset);
	int sy = static_cast<int>(y * mapScale + mapOffset);
	CVector3 normal(0.0f, 0.0f, 1.0f, 0.0f);
	if (sx >= 0 && y >= 0 && sx <= m_pHeightMap->width && sy <= m_pHeightMap->width) {
		GetMapNormal(sx, sy, normal);
	}
	return normal;
}

/**
* 设置渲染精度容差
*/
void CSceneNodeTerrain::SetErrorTolerance(float tolerance) {
	if (tolerance < 10.0f) tolerance = 10.0f;
	m_fErrorTolerance = tolerance;
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
	if (file.width != file.height || (file.width & (file.width - 1)) != 0) {
		CLog::Warn("Height map width not equal with height or not power of 2");
		return false;
	}
	// 高度图宽度 2^n，顶点个数 2^n + 1，方便四叉树划分
	const int size = file.width + 1;
	heightMap->width = file.width;
	heightMap->data = new float[size * size];
	for (int i = 0; i < file.height; i++) {
		for (int j = 0; j < file.width; j++) {
			int indexSrc = j + i * file.width;
			int indexDes = j + (size - i - 2) * size;
			int data = (file.contents[indexSrc * file.channels] & 0x00FF) - 0x80;
			heightMap->data[indexDes] = data * heightMap->vScale * 0.007874f;
		}
	}
	int isrc = size * (size - 2);
	int idst = size * (size - 1);
	for (int i = 0; i < heightMap->width; i++) heightMap->data[idst + i] = heightMap->data[isrc + i];
	for (int i = 1; i <= size; i++) heightMap->data[size * i - 1] = heightMap->data[size * i - 2];
	// 用于修补裂缝的顶点标记
	heightMap->flag = new bool[size * size];
	memset(heightMap->flag, 0, sizeof(bool) * size * size);
	return true;
}

/**
* 递归生成地形四叉树
* @param parent 父节点
* @param index 节点索引[0-RD 1-LD 2-LU 3-RU]
* @param level 节点层级，四叉树根节点层级为0
* @param size 节点宽度，对应高度图
* @return 当前创建好的四叉树节点
*/
CSceneNodeTerrain::SQuadTree* CSceneNodeTerrain::BuildQuadTree(SQuadTree* parent, int index, int level, int size) {
	if (level >= m_iMaxLevel) return 0;
	const int halfSize = size >> 1;
	const int mapLength = m_pHeightMap->width + 1;
	SQuadTree* node = new SQuadTree();
	node->mesh = 0;
	node->parent = parent;
	node->visible = false;
	node->level = level;
	node->size = size;
	if (parent) {
		switch (index) {
		case 0: node->center = parent->center + halfSize * (mapLength + 1); break;
		case 1: node->center = parent->center + halfSize * (mapLength - 1); break;
		case 2: node->center = parent->center - halfSize * (mapLength + 1); break;
		case 3: node->center = parent->center - halfSize * (mapLength - 1); break;
		default: break;
		}
	} else node->center = halfSize * (mapLength + 1);
	node->corner[0] = node->center + halfSize * (mapLength + 1);
	node->corner[1] = node->center + halfSize * (mapLength - 1);
	node->corner[2] = node->center - halfSize * (mapLength + 1);
	node->corner[3] = node->center - halfSize * (mapLength - 1);
	node->children[0] = BuildQuadTree(node, 0, level + 1, halfSize);
	node->children[1] = BuildQuadTree(node, 1, level + 1, halfSize);
	node->children[2] = BuildQuadTree(node, 2, level + 1, halfSize);
	node->children[3] = BuildQuadTree(node, 3, level + 1, halfSize);
	// 计算节点包围盒
	const int cx = node->center / mapLength;
	const int cy = node->center % mapLength;
	const float mapOffset = m_pHeightMap->width * 0.5f;
	const float mapScale = m_pHeightMap->hScale / static_cast<float>(m_pHeightMap->width);
	if (node->IsInternal()) {
		node->volume.SetValue(node->children[0]->volume);
		node->volume.Update(node->children[1]->volume);
		node->volume.Update(node->children[2]->volume);
		node->volume.Update(node->children[3]->volume);
	} else {
		// 叶子节点宽度为2，包含3x3个顶点
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
	node->geometricError = node->volume.m_cMax[2] - node->volume.m_cMin[2];
	return node;
}

/**
* 递归生成地形网格
* @param node 当前四叉树节点
* @param index 在父节点中子节点的索引
*/
void CSceneNodeTerrain::BuildTerrainMesh(SQuadTree* node, int index) {
	if (node->level == m_iMeshLevel) {
		CMesh* mesh = new CMesh();
		mesh->SetVertexUsage((node->size + 1) * (node->size + 1));
		const int halfSize = node->size >> 1;
		const int mapLength = m_pHeightMap->width + 1;
		const float mapOffset = m_pHeightMap->width * 0.5f;
		const float mapScale = m_pHeightMap->hScale / static_cast<float>(m_pHeightMap->width);
		const float texScale = 1.0f / m_pHeightMap->hScale;
		const int sx = (node->center / mapLength) - halfSize;
		const int sy = (node->center % mapLength) - halfSize;
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
		// Mesh的四个角及中心顶点索引
		node->index[0] = node->size * (node->size + 2);
		node->index[1] = node->size * (node->size + 1);
		node->index[2] = 0;
		node->index[3] = node->size;
		node->index[4] = halfSize * (node->size + 2);
	} else if (node->level > m_iMeshLevel) {
		const int meshSize = m_pHeightMap->width >> m_iMeshLevel;
		const int halfSize = node->size >> 1;
		// 计算网格子节点四个角及中心顶点索引
		int offset[4] = { 0, node->size, node->size * (meshSize + 2), node->size * (meshSize + 1) };
		node->index[0] = node->parent->index[0] - offset[index];
		node->index[1] = node->index[0] - offset[1];
		node->index[2] = node->index[0] - offset[2];
		node->index[3] = node->index[0] - offset[3];
		node->index[4] = node->index[0] - halfSize * (meshSize + 2);
		node->mesh = node->parent->mesh;
	}
	if (node->IsInternal()) {
		BuildTerrainMesh(node->children[0], 0);
		BuildTerrainMesh(node->children[1], 1);
		BuildTerrainMesh(node->children[2], 2);
		BuildTerrainMesh(node->children[3], 3);
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
void CSceneNodeTerrain::CheckVisibility(SQuadTree* node, const CFrustum& frustum, const CVector3& camera, float tolerance) {
	node->visible = frustum.IsOverlapAABB(node->volume);
	if (node->visible) {
		bool endRetrieval = node->IsLeaf();
		if (!endRetrieval) {
			float toleranceError = tolerance * (camera - node->position).Length();
			if (toleranceError > node->geometricError && node->level > m_iMeshLevel) {
				node->children[0]->visible = false;
				node->children[1]->visible = false;
				node->children[2]->visible = false;
				node->children[3]->visible = false;
				endRetrieval = true;
			} else {
				CheckVisibility(node->children[0], frustum, camera, tolerance);
				CheckVisibility(node->children[1], frustum, camera, tolerance);
				CheckVisibility(node->children[2], frustum, camera, tolerance);
				CheckVisibility(node->children[3], frustum, camera, tolerance);
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
	const int mapLength = m_pHeightMap->width + 1;
	const float mapOffset = m_pHeightMap->width * 0.5f;
	const float mapScale = m_pHeightMap->hScale / static_cast<float>(m_pHeightMap->width);
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
		bool childVisible = false;
		if (node->IsInternal()) {
			childVisible |= UpdateIndexBuffer(node->children[0]);
			childVisible |= UpdateIndexBuffer(node->children[1]);
			childVisible |= UpdateIndexBuffer(node->children[2]);
			childVisible |= UpdateIndexBuffer(node->children[3]);
		}
		// 四个子节点都不可见的情况下绘制当前节点
		if (mesh && !childVisible) {
			AddTriangle(node, node->index[4], node->index[1], node->index[0], node->corner[1], node->corner[0]);
			AddTriangle(node, node->index[4], node->index[2], node->index[1], node->corner[2], node->corner[1]);
			AddTriangle(node, node->index[4], node->index[3], node->index[2], node->corner[3], node->corner[2]);
			AddTriangle(node, node->index[4], node->index[0], node->index[3], node->corner[0], node->corner[3]);
		}
		if (node->level == m_iMeshLevel) {
			mesh->Update(2);
		}
	}
	return node->visible;
}

/**
* 添加三角形并修补裂缝
* @param node 四叉树节点
* @param index0 节点中心网格索引
* @param index1 节点网格边索引1
* @param index2 节点网格边索引2
* @param flag1 节点高度图边1索引
* @param flag2 节点高度图边2索引
*/
void CSceneNodeTerrain::AddTriangle(SQuadTree* node, int index0, int index1, int index2, int flag1, int flag2) {
	// 相当于对网格坐标系local和高度图坐标系world分别除以(node->size/2)，也就是步进长度为2
	const int shift = m_iMaxLevel - node->level - 1;
	const int stepLocal = (index2 - index1) >> shift;
	const int stepWorld = (flag2 - flag1) >> shift;
	int index_local = stepLocal + index1;
	int index_world = stepWorld + flag1;
	// 对边 index1-index2 进行遍历，添加分割的顶点索引
	while (index_world != flag2) {
		if (m_pHeightMap->flag[index_world]) {
			m_pHeightMap->flag[index_world] = false;
			node->mesh->AddTriangle(index0, index1, index_local);
			index1 = index_local;
		}
		index_local += stepLocal;
		index_world += stepWorld;
	}
	node->mesh->AddTriangle(index0, index1, index_local);
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
	if (x2 > m_pHeightMap->width) x2 = x;
	if (y2 > m_pHeightMap->width) y2 = y;
	// 计算周围四个点相对目标点的向量
	const int mapLength = m_pHeightMap->width + 1;
	const float mapScale = m_pHeightMap->hScale / static_cast<float>(m_pHeightMap->width);
	float offset = m_pHeightMap->data[x + y * mapLength];
	CVector3 v1(-mapScale, 0.0f, m_pHeightMap->data[x1 + y * mapLength] - offset);
	CVector3 v2(0.0f, -mapScale, m_pHeightMap->data[x + y1 * mapLength] - offset);
	CVector3 v3(mapScale, 0.0f, m_pHeightMap->data[x2 + y * mapLength] - offset);
	CVector3 v4(0.0f, mapScale, m_pHeightMap->data[x + y2 * mapLength] - offset);
	normal = v1.CrossProduct(v2) + v2.CrossProduct(v3) + v3.CrossProduct(v4) + v4.CrossProduct(v1);
	normal.Normalize();
}