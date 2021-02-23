//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeSkybox.h"
#include "CEngine.h"
#include <cmath>

/**
* 构造函数
*/
CSceneNodeSkybox::CSceneNodeSkybox(const string& name, const string texture[6]): CSceneNode("skybox", name) {
	m_pMesh = 0;
	m_pTexture = CEngine::GetTextureManager()->Create(name + "_texture", texture);
}

/**
* 通过渐变色构造
*/
CSceneNodeSkybox::CSceneNodeSkybox(const string& name, const CColor& top, const CColor& middle, const CColor& bottom)
	: CSceneNode("skybox", name) {
	m_pMesh = 0;
	m_pTexture = GenerateGradientMap(256, top, middle, bottom);
}

/**
* 初始化场景节点
*/
bool CSceneNodeSkybox::Init() {
	m_pMesh = new CMesh();
	m_pMesh->SetVertexUsage(8);
	m_pMesh->AddVertex(CVertex(-1000.0f, -1000.0f, -1000.0f));
	m_pMesh->AddVertex(CVertex( 1000.0f, -1000.0f, -1000.0f));
	m_pMesh->AddVertex(CVertex( 1000.0f,  1000.0f, -1000.0f));
	m_pMesh->AddVertex(CVertex(-1000.0f,  1000.0f, -1000.0f));
	m_pMesh->AddVertex(CVertex(-1000.0f, -1000.0f,  1000.0f));
	m_pMesh->AddVertex(CVertex( 1000.0f, -1000.0f,  1000.0f));
	m_pMesh->AddVertex(CVertex( 1000.0f,  1000.0f,  1000.0f));
	m_pMesh->AddVertex(CVertex(-1000.0f,  1000.0f,  1000.0f));
	m_pMesh->AddTriangle(2, 1, 5);
	m_pMesh->AddTriangle(2, 5, 6);
	m_pMesh->AddTriangle(0, 3, 7);
	m_pMesh->AddTriangle(0, 7, 4);
	m_pMesh->AddTriangle(3, 2, 6);
	m_pMesh->AddTriangle(3, 6, 7);
	m_pMesh->AddTriangle(1, 0, 4);
	m_pMesh->AddTriangle(1, 4, 5);
	m_pMesh->AddTriangle(7, 6, 5);
	m_pMesh->AddTriangle(7, 5, 4);
	m_pMesh->AddTriangle(0, 1, 2);
	m_pMesh->AddTriangle(0, 2, 3);
	m_pMesh->GetMaterial()->SetTexture(m_pTexture);
	m_pMesh->Create(false);
	return true;
}

/**
* 销毁场景节点
*/
void CSceneNodeSkybox::Destroy() {
	CEngine::GetTextureManager()->Drop(m_pTexture);
	delete m_pMesh;
}

/**
* 渲染场景节点
*/
void CSceneNodeSkybox::Render() {
	m_pMesh->Render();
}

/**
* 更新逻辑
*/
void CSceneNodeSkybox::Update(float dt) {
	m_cPosition = CEngine::GetGraphicsManager()->GetCamera()->m_cPosition;
	Transform();
}

/**
* 生成渐变立方体图
*/
CTexture* CSceneNodeSkybox::GenerateGradientMap(int size, const CColor& top, const CColor& middle, const CColor& bottom) {
	const float fsize = (float)size;
	const float fscale = 1.0f / 1.570796f;
	unsigned char* data[6];
	for (int i = 0; i < 6; i++) data[i] = new unsigned char[size * size * 4];
	// 侧面颜色插值
	for (int h = 0; h < size; h++) {
		float y = 2.0f * h / fsize - 1.0f;
		for (int w = 0; w < size; w++) {
			float x = 2.0f * w / fsize - 1.0f;
			float theta = atan2f(y, sqrtf(1.0f + x * x));
			if (theta > 0.0f) middle.Lerp(top, theta * fscale).GetValue(&data[2][(h * size + w) << 2]);
			else middle.Lerp(bottom, fabs(theta) * fscale).GetValue(&data[2][(h * size + w) << 2]);
		}
	}
	// 顶面颜色插值
	for (int h = 0; h < size; h++) {
		float y = 2.0f * h / fsize - 1.0f;
		for (int w = 0; w < size; w++) {
			float x = 2.0f * w / fsize - 1.0f;
			float theta = atan2f(1.0f, sqrtf(y * y + x * x));
			middle.Lerp(top, theta * fscale).GetValue(&data[4][(h * size + w) << 2]);
		}
	}
	// 底面颜色插值
	for (int h = 0; h < size; h++) {
		float y = 2.0f * h / fsize - 1.0f;
		for (int w = 0; w < size; w++) {
			float x = 2.0f * w / fsize - 1.0f;
			float theta = atan2f(1.0f, sqrtf(y * y + x * x));
			middle.Lerp(bottom, theta * fscale).GetValue(&data[5][(h * size + w) << 2]);
		}
	}
	// 复制左右侧及后侧纹理
	for (int h = 0; h < size; h++) {
		for (int w = 0; w < size; w++) {
			int index = h * size + w;
			memcpy(data[0] + (((w + 1) * size - 1 - h) << 2), data[2] + index * 4, 4);
			memcpy(data[1] + (((size - 1 - w) * size + h) << 2), data[2] + index * 4, 4);
			memcpy(data[3] + (((size - h) * size - 1 - w) << 2), data[2] + index * 4, 4);
		}
	}
	CTexture* pTexture = CEngine::GetTextureManager()->Create(m_strName + "_texture", size, 4, (const void**)data, true);
	for (int i = 0; i < 6; i++) delete[] data[i];
	return pTexture;
}