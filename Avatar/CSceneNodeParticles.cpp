﻿//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeParticles.h"
#include "CEngine.h"
#include "CVector3.h"

/**
* 构造函数
*/
CSceneNodeParticles::CSceneNodeParticles(const string& name, const string& texture, const CColor& color,
	float size, bool dark, int count, bool loop) : CSceneNode("particles", name) {
	m_vecParticles.resize(count);
	m_pSortIndex = new size_t[count];
	m_fFadeSpeed = 1.0f;
	m_strTexture = texture;
	m_bLoop = loop;
	m_bDarkBlend = dark;
	m_bGravityEffect = false;
	m_fSpreadSpeed = 1.0f;
	m_fEmitSpeed[0] = 0.0f;
	m_fEmitSpeed[1] = 0.0f;
	m_fEmitSpeed[2] = 0.0f;
	m_fParticleSize = size * 0.5f;
	m_fParticleColor[0] = color.m_fValue[0];
	m_fParticleColor[1] = color.m_fValue[1];
	m_fParticleColor[2] = color.m_fValue[2];
	m_fParticleColor[3] = 1.0f;
}

/**
* 初始化场景节点
*/
bool CSceneNodeParticles::Init() {
	m_pMesh = new CMesh();
	m_pMesh->GetMaterial()->SetTexture(m_strTexture);
	m_pMesh->SetVertexUsage(m_vecParticles.size() * 4);
	for (size_t i = 0; i < m_vecParticles.size(); i++) {
		m_pMesh->AddVertex(CVertex(-m_fParticleSize, 0, -m_fParticleSize, 1, 0));
		m_pMesh->AddVertex(CVertex( m_fParticleSize, 0, -m_fParticleSize, 1, 1));
		m_pMesh->AddVertex(CVertex(-m_fParticleSize, 0,  m_fParticleSize, 0, 0));
		m_pMesh->AddVertex(CVertex( m_fParticleSize, 0,  m_fParticleSize, 0, 1));
		unsigned int idx = (unsigned int)(i * 4);
		m_pMesh->AddTriangle(idx + 0, idx + 1, idx + 2);
		m_pMesh->AddTriangle(idx + 1, idx + 3, idx + 2);
	}
	m_pMesh->Create(true);
	m_pMesh->GetMaterial()->SetRenderMode(true, true, !m_bDarkBlend);
	if (m_bLoop) {
		SetupSpeed(m_fEmitSpeed, m_fSpreadSpeed, m_fFadeSpeed);
	}
	return true;
}

/**
* 销毁场景节点
*/
void CSceneNodeParticles::Destroy() {
	delete[] m_pSortIndex;
	delete m_pMesh;
}

/**
* 渲染场景节点
*/
void CSceneNodeParticles::Render() {
	m_pMesh->Render();
}

/**
* 更新场景节点
*/
void CSceneNodeParticles::Update(float dt) {
	CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
	CVector3 gravity = CEngine::GetPhysicsManager()->GetGravity() * dt;
	// 得到相机局部坐标系下的方向
	CMatrix4 worldMatInv(m_cWorldMatrix);
	CVector3 cameraPos = worldMatInv.Invert() * pCamera->m_cPosition;
	// 局部坐标系下的速度缩放系数
	float sx = CVector3(worldMatInv(0, 0), worldMatInv(1, 0), worldMatInv(2, 0)).Length() * dt;
	float sy = CVector3(worldMatInv(0, 1), worldMatInv(1, 1), worldMatInv(2, 1)).Length() * dt;
	float sz = CVector3(worldMatInv(0, 2), worldMatInv(1, 2), worldMatInv(2, 2)).Length() * dt;

	// 更新粒子属性
	bool changed = false;
	for (size_t i = 0; i < m_vecParticles.size(); i++) {
		SParticle* particle = &m_vecParticles[i];
		if (!particle->active) continue;
		// 粒子衰减
		particle->fade -= m_fFadeSpeed * dt;
		if (particle->fade < 0.0f) {
			particle->active = m_bLoop;
			particle->color[3] = 0.0f;
			if (m_bLoop) {
				particle->fade += 1.0f;
				particle->speed[0] = m_fEmitSpeed[0] + Random() * m_fSpreadSpeed;
				particle->speed[1] = m_fEmitSpeed[1] + Random() * m_fSpreadSpeed;
				particle->speed[2] = m_fEmitSpeed[2] + Random() * m_fSpreadSpeed;
				particle->position[0] = 0.0f;
				particle->position[1] = 0.0f;
				particle->position[2] = 0.0f;
			}
			continue;
		}
		if (m_bGravityEffect) {
			particle->speed[0] += gravity[0];
			particle->speed[1] += gravity[1];
			particle->speed[2] += gravity[2];
		}
		// 更新位置和颜色
		particle->position[0] += particle->speed[0] * sx;
		particle->position[1] += particle->speed[1] * sy;
		particle->position[2] += particle->speed[2] * sz;
		CVector3 center_to_camera = cameraPos - CVector3(particle->position);
		particle->distance = center_to_camera.DotProduct(center_to_camera);
		particle->color[3] = m_fParticleColor[3] * particle->fade;
		changed = true;
	}
	if (!changed) return;

	// 对粒子按摄像机视线方向排序，使用插入排序算法
	for (size_t i = 1; i < m_vecParticles.size(); i++) {
		if (m_vecParticles[m_pSortIndex[i - 1]].distance < m_vecParticles[m_pSortIndex[i]].distance) {
			size_t j = i - 1;
			while (j != 0) {
				j -= 1;
				if (m_vecParticles[m_pSortIndex[j]].distance > m_vecParticles[m_pSortIndex[i]].distance) {
					j += 1;
					break;
				}
			}
			size_t temp = m_pSortIndex[i];
			for (size_t k = i; k > j; k--) {
				m_pSortIndex[k] = m_pSortIndex[k - 1];
			}
			m_pSortIndex[j] = temp;
		}
	}
	// 计算粒子平面右上两个向量
	CMatrix4& viewMat = pCamera->GetViewMatrix();
	CVector3 vec_r = worldMatInv * CVector3(viewMat(0, 0), viewMat(0, 1), viewMat(0, 2), 0);
	CVector3 vec_u = worldMatInv * CVector3(viewMat(1, 0), viewMat(1, 1), viewMat(1, 2), 0);
	vec_r.Scale(m_fParticleSize);
	vec_u.Scale(m_fParticleSize);
	// 更新网格对象
	for (size_t i = 0; i < m_vecParticles.size(); i++) {
		SParticle* particle = &m_vecParticles[m_pSortIndex[i]];
		size_t index = i << 2;
		m_pMesh->GetVertex(index + 0)->SetColor(particle->color);
		m_pMesh->GetVertex(index + 0)->SetPosition(CVector3(particle->position) - vec_r);
		m_pMesh->GetVertex(index + 1)->SetColor(particle->color);
		m_pMesh->GetVertex(index + 1)->SetPosition(CVector3(particle->position) - vec_u);
		m_pMesh->GetVertex(index + 2)->SetColor(particle->color);
		m_pMesh->GetVertex(index + 2)->SetPosition(CVector3(particle->position) + vec_u);
		m_pMesh->GetVertex(index + 3)->SetColor(particle->color);
		m_pMesh->GetVertex(index + 3)->SetPosition(CVector3(particle->position) + vec_r);
	}
	m_pMesh->Update(1);
}

/**
* 初始化粒子
*/
void CSceneNodeParticles::SetupSpeed(const CVector3& emit, float spread, float fade) {
	m_fEmitSpeed[0] = emit.m_fValue[0];
	m_fEmitSpeed[1] = emit.m_fValue[1];
	m_fEmitSpeed[2] = emit.m_fValue[2];
	m_fSpreadSpeed = spread;
	m_fFadeSpeed = fade;
	for (size_t i = 0; i < m_vecParticles.size(); i++) {
		SParticle& particle = m_vecParticles[i];
		particle.active = true;
		particle.fade = 0.5f - Random() * 0.5f;
		particle.color[0] = m_fParticleColor[0];
		particle.color[1] = m_fParticleColor[1];
		particle.color[2] = m_fParticleColor[2];
		particle.speed[0] = m_fEmitSpeed[0] + Random() * spread;
		particle.speed[1] = m_fEmitSpeed[1] + Random() * spread;
		particle.speed[2] = m_fEmitSpeed[2] + Random() * spread;
		particle.position[0] = 0.0f;
		particle.position[1] = 0.0f;
		particle.position[2] = 0.0f;
		m_pSortIndex[i] = i;
	}
}

/**
* 设置是否受重力加速度影响
*/
void CSceneNodeParticles::SetGravityEffect(bool enable) {
	m_bGravityEffect = enable;
}

/**
* 随机数生成函数，返回值 -1.0 至 1.0 之间
*/
float CSceneNodeParticles::Random() {
	static unsigned int seed = 20170328;
	seed = 214013 * seed + 2531011;
	return ((int)((seed >> 16) & 0x7FFF) % 201 - 100) * 0.01f;
}