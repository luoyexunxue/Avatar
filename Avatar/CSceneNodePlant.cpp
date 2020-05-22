//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodePlant.h"
#include "CEngine.h"
#include "CStringUtil.h"
#include "CSceneNodeTerrain.h"
#include <cmath>

/**
* 构造函数
* @note range 以分布图的宽度为基准
*/
CSceneNodePlant::CSceneNodePlant(const string& name, const string& texture, float width, float height,
	const string& distributionMap, int count, float density, float range)
	: CSceneNode("plant", name) {
	m_strTexture = texture;
	m_fWidth = width;
	m_fHeight = height;
	m_strDistributionMap = distributionMap;
	m_iTotalCount = count;
	m_fDensity = density;
	m_fRange = range;
	m_pMesh = 0;
	if (m_fDensity <= 0.01f) {
		m_fDensity = 0.01f;
	}
}

/**
* 初始化场景节点
*/
bool CSceneNodePlant::Init() {
	m_pMesh = BuildMesh();
	if (!m_pMesh) return false;
	m_pMesh->GetMaterial()->SetTexture(m_strTexture);
	m_pMesh->GetMaterial()->GetTexture()->SetWrapModeClampToEdge(true, true);
	m_pMesh->GetMaterial()->SetRenderMode(false, true, false);
	m_cLocalBoundingBox = m_pMesh->GetBoundingBox();
	return true;
}

/**
* 销毁场景节点
*/
void CSceneNodePlant::Destroy() {
	if (m_pMesh) delete m_pMesh;
}

/**
* 渲染场景节点
*/
void CSceneNodePlant::Render() {
	if (CEngine::GetGraphicsManager()->IsDepthRender()) {
		m_pMesh->GetMaterial()->GetTexture()->UseTexture();
		m_pMesh->Render(false);
	} else {
		m_pMesh->Render();
	}
}

/**
* 生成植被层
*/
CMesh* CSceneNodePlant::BuildMesh() {
	// 需要地形节点获取植被分布高度
	CSceneNode* node = CEngine::GetSceneManager()->GetNodeByType("terrain", 0);
	if (!node) return 0;
	CFileManager::FileType fileType;
	string ext = CStringUtil::UpperCase(CFileManager::GetExtension(m_strDistributionMap));
	if (ext == "BMP") fileType = CFileManager::BMP;
	else if (ext == "TGA") fileType = CFileManager::TGA;
	else if (ext == "PNG") fileType = CFileManager::PNG;
	else if (ext == "JPG" || ext == "JPEG") fileType = CFileManager::JPG;
	else return 0;
	CFileManager::CImageFile file(fileType);
	if (!CEngine::GetFileManager()->ReadFile(m_strDistributionMap, &file)) return 0;
	// 按照植被分布图进行随机分布
	const float scaleX = m_fRange;
	const float scaleY = m_fRange * file.height / file.width;
	const float rescale = 1.0f / RAND_MAX;
	const float minimum = 1.0f / (1.0f + expf(m_fDensity));
	CSceneNodeTerrain* terrain = reinterpret_cast<CSceneNodeTerrain*>(node);
	CMesh* mesh = new CMesh();
	mesh->SetVertexUsage(m_iTotalCount * 8);
	for (int i = 0; i < m_iTotalCount; i++) {
		int loopCount = 0;
		float centerx = 0.0f;
		float centery = 0.0f;
		do {
			centerx = scaleX * (rand() * rescale - 0.5f);
			centery = scaleY * (rand() * rescale - 0.5f);
			int ix = static_cast<int>((centerx / scaleX + 0.5f) * file.width) % file.width;
			int iy = static_cast<int>((centery / scaleY + 0.5f) * file.height) % file.height;
			float t = file.contents[(ix + (file.height - iy - 1) * file.width) * file.channels] * 0.003922f;
			// 使用了 logistic sigmoid 函数
			float sigmoid = 1.0f / (1.0f + expf(-m_fDensity * (t - 0.5f) * 2.0f));
			float density = (sigmoid - minimum) / (1.0f - 2.0f * minimum);
			if (rand() * rescale <= density) break;
		} while (loopCount++ < 1000);
		float centerz = terrain->GetHeight(centerx, centery);
		float angle = 3.141593f * 2.0f * (rand() * rescale);
		AddPlant(mesh, centerx, centery, centerz, angle);
	}
	mesh->Create(false);
	return mesh;
}

/**
* 加入植被到网格
*/
void CSceneNodePlant::AddPlant(CMesh* mesh, float x, float y, float z, float angle) {
	const float sina = sinf(angle) * m_fWidth * 0.5f;
	const float cosa = cosf(angle) * m_fWidth * 0.5f;
	const float baseColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	const float topColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const int baseIndex = mesh->GetVertexCount();
	mesh->AddVertex(CVertex(x - cosa, y - sina, z, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, baseColor));
	mesh->AddVertex(CVertex(x + cosa, y + sina, z, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, baseColor));
	mesh->AddVertex(CVertex(x - cosa, y - sina, z + m_fHeight, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, topColor));
	mesh->AddVertex(CVertex(x + cosa, y + sina, z + m_fHeight, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, topColor));
	mesh->AddVertex(CVertex(x + sina, y - cosa, z, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, baseColor));
	mesh->AddVertex(CVertex(x - sina, y + cosa, z, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, baseColor));
	mesh->AddVertex(CVertex(x + sina, y - cosa, z + m_fHeight, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, topColor));
	mesh->AddVertex(CVertex(x - sina, y + cosa, z + m_fHeight, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, topColor));
	mesh->AddTriangle(baseIndex + 0, baseIndex + 1, baseIndex + 2);
	mesh->AddTriangle(baseIndex + 1, baseIndex + 3, baseIndex + 2);
	mesh->AddTriangle(baseIndex + 4, baseIndex + 5, baseIndex + 6);
	mesh->AddTriangle(baseIndex + 5, baseIndex + 7, baseIndex + 6);
}
