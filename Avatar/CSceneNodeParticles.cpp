//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeParticles.h"
#include "CEngine.h"
#include "CVector3.h"

/**
* ���캯��
*/
CSceneNodeParticles::CSceneNodeParticles(const string& name, const string& texture,
	float size, int count, bool dark, const CColor& color, const float speed[3], float spread,
	float fade, bool loop): CSceneNode("particles", name) {
	m_vecParticles.resize(count);
	m_pSortIndex = new size_t[count];
	m_fFadeSpeed = fade;
	m_strTexture = texture;
	m_bLoopParticles = loop;
	m_bDarkBlendMode = dark;
	m_fSpreadSpeed = spread;
	m_fInitSpeed[0] = speed[0];
	m_fInitSpeed[1] = speed[1];
	m_fInitSpeed[2] = speed[2];
	m_fParticleSize = size * 0.5f;
	m_fParticleColor[0] = color.m_fValue[0];
	m_fParticleColor[1] = color.m_fValue[1];
	m_fParticleColor[2] = color.m_fValue[2];
	m_fParticleColor[3] = 1.0f;
}

/**
* ��ʼ�������ڵ�
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
		m_pMesh->AddTriangle(i * 4 + 0, i * 4 + 1, i * 4 + 2);
		m_pMesh->AddTriangle(i * 4 + 1, i * 4 + 3, i * 4 + 2);
	}
	m_pMesh->Create(true);
	m_pMesh->GetMaterial()->SetRenderMode(true, true, !m_bDarkBlendMode);
	if (m_bLoopParticles) {
		InitParticles(m_fInitSpeed, m_fSpreadSpeed, m_fFadeSpeed);
	}
	return true;
}

/**
* ���ٳ����ڵ�
*/
void CSceneNodeParticles::Destroy() {
	delete[] m_pSortIndex;
	delete m_pMesh;
}

/**
* ��Ⱦ�����ڵ�
*/
void CSceneNodeParticles::Render() {
	m_pMesh->Render();
}

/**
* ���³����ڵ�
*/
void CSceneNodeParticles::Update(float dt) {
	CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
	// �õ�����ֲ�����ϵ�µķ���
	CMatrix4 worldMatInv(m_cWorldMatrix);
	CVector3 cameraPos = worldMatInv.Invert() * pCamera->m_cPosition;
	// �ֲ�����ϵ�µ��ٶ�����ϵ��
	float sx = CVector3(worldMatInv(0, 0), worldMatInv(1, 0), worldMatInv(2, 0)).Length() * dt;
	float sy = CVector3(worldMatInv(0, 1), worldMatInv(1, 1), worldMatInv(2, 1)).Length() * dt;
	float sz = CVector3(worldMatInv(0, 2), worldMatInv(1, 2), worldMatInv(2, 2)).Length() * dt;

	// ������������
	bool changed = false;
	for (size_t i = 0; i < m_vecParticles.size(); i++) {
		SParticle* particle = &m_vecParticles[i];
		if (!particle->active) continue;
		// ����˥��
		particle->fade -= m_fFadeSpeed * dt;
		if (particle->fade < 0.0f) {
			particle->active = m_bLoopParticles;
			particle->color[3] = 0.0f;
			if (m_bLoopParticles) {
				particle->fade += 1.0f;
				particle->speed[0] = m_fInitSpeed[0] + Random() * m_fSpreadSpeed;
				particle->speed[1] = m_fInitSpeed[1] + Random() * m_fSpreadSpeed;
				particle->speed[2] = m_fInitSpeed[2] + Random() * m_fSpreadSpeed;
				particle->position[0] = 0.0f;
				particle->position[1] = 0.0f;
				particle->position[2] = 0.0f;
			}
			continue;
		}
		// ����λ�ú���ɫ
		particle->position[0] += particle->speed[0] * sx;
		particle->position[1] += particle->speed[1] * sy;
		particle->position[2] += particle->speed[2] * sz;
		CVector3 center_to_camera = cameraPos - CVector3(particle->position);
		particle->distance = center_to_camera.DotProduct(center_to_camera);
		particle->color[3] = m_fParticleColor[3] * particle->fade;
		changed = true;
	}
	if (!changed) return;

	// �����Ӱ���������߷�������ʹ�ò��������㷨
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
	// ��������ƽ��������������
	CMatrix4& viewMat = pCamera->GetViewMatrix();
	CVector3 vec_r = worldMatInv * CVector3(viewMat(0, 0), viewMat(0, 1), viewMat(0, 2), 0);
	CVector3 vec_u = worldMatInv * CVector3(viewMat(1, 0), viewMat(1, 1), viewMat(1, 2), 0);
	vec_r.Scale(m_fParticleSize);
	vec_u.Scale(m_fParticleSize);
	// �����������
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
* ��ʼ������
*/
void CSceneNodeParticles::InitParticles(const float initSpeed[3], float spreadSpeed, float fadeSpeed) {
	m_fInitSpeed[0] = initSpeed[0];
	m_fInitSpeed[1] = initSpeed[1];
	m_fInitSpeed[2] = initSpeed[2];
	m_fSpreadSpeed = spreadSpeed;
	m_fFadeSpeed = fadeSpeed;
	for (size_t i = 0; i < m_vecParticles.size(); i++) {
		SParticle& particle = m_vecParticles[i];
		particle.active = true;
		particle.fade = 0.5f - Random() * 0.5f;
		particle.color[0] = m_fParticleColor[0];
		particle.color[1] = m_fParticleColor[1];
		particle.color[2] = m_fParticleColor[2];
		particle.speed[0] = initSpeed[0] + Random() * spreadSpeed;
		particle.speed[1] = initSpeed[1] + Random() * spreadSpeed;
		particle.speed[2] = initSpeed[2] + Random() * spreadSpeed;
		particle.position[0] = 0.0f;
		particle.position[1] = 0.0f;
		particle.position[2] = 0.0f;
		m_pSortIndex[i] = i;
	}
}

/**
* ��������ɺ���������ֵ -1.0 �� 1.0 ֮��
*/
float CSceneNodeParticles::Random() {
	static unsigned int seed = 20170328;
	seed = 214013 * seed + 2531011;
	return ((int)((seed >> 16) & 0x7FFF) % 201 - 100) * 0.01f;
}