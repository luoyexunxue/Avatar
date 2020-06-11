//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeGeometry.h"
#include "CEngine.h"

/**
* 构造函数
*/
CSceneNodeGeometry::CSceneNodeGeometry(const string& name, const string& texture, const SGeometry& geometry)
	: CSceneNode("geometry", name) {
	m_strTexture = texture;
	m_sGeometry = geometry;
	m_pMeshData = 0;
}

/**
* 初始化场景节点
*/
bool CSceneNodeGeometry::Init() {
	CMesh* pMesh = 0;
	SGeometry& g = m_sGeometry;
	switch (m_sGeometry.shape) {
	case SGeometry::BOX: pMesh = CGeometryCreator::CreateBox(g.box.x, g.box.y, g.box.z, g.reversed); break;
	case SGeometry::SPHERE: pMesh = CGeometryCreator::CreateSphere(g.sphere.r, g.slices, g.slices, g.reversed); break;
	case SGeometry::CAPSULE: pMesh = CGeometryCreator::CreateCapsule(g.capsule.r, g.capsule.h, g.slices, g.reversed); break;
	case SGeometry::CYLINDER: pMesh = CGeometryCreator::CreateCylinder(g.cylinder.r, g.cylinder.h, g.slices, g.reversed); break;
	case SGeometry::TORUS: pMesh = CGeometryCreator::CreateTorus(g.torus.r, g.torus.c, g.slices, g.reversed); break;
	case SGeometry::CONE: pMesh = CGeometryCreator::CreateCone(g.cone.r, g.cone.h, g.slices, g.reversed); break;
	case SGeometry::PLANE: pMesh = CGeometryCreator::CreatePlane(g.plane.w, g.plane.h, 3, g.reversed); break;
	case SGeometry::CIRCLE: pMesh = CGeometryCreator::CreateCircle(g.circle.r, 3, g.slices, g.reversed); break;
	default: return false;
	}
	pMesh->GetMaterial()->SetTexture(m_strTexture);
	m_cLocalBoundingBox = pMesh->GetBoundingBox();
	m_pMeshData = new CMeshData();
	m_pMeshData->AddMesh(pMesh);
	return true;
}

/**
* 销毁场景节点
*/
void CSceneNodeGeometry::Destroy() {
	if (m_pMeshData) delete m_pMeshData;
}

/**
* 渲染场景节点
*/
void CSceneNodeGeometry::Render() {
	if (CEngine::GetGraphicsManager()->IsDepthRender()) {
		m_pMeshData->GetMesh(0)->GetMaterial()->GetTexture()->UseTexture();
		m_pMeshData->GetMesh(0)->Render(false);
	} else {
		CMaterial* pMaterial = m_pMeshData->GetMesh(0)->GetMaterial();
		if (pMaterial->GetShader()) {
			CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
			pMaterial->PassUniform("uProjMatrix", pCamera->GetProjMatrix());
			pMaterial->PassUniform("uViewMatrix", pCamera->GetViewMatrix());
			pMaterial->PassUniform("uCameraPos", pCamera->m_cPosition);
			pMaterial->PassUniform("uModelMatrix", m_cWorldMatrix);
		}
		m_pMeshData->GetMesh(0)->Render();
	}
}

/**
* 获取网格数据
*/
CMeshData* CSceneNodeGeometry::GetMeshData() {
	return m_pMeshData;
}

/**
* 获取几何参数
*/
SGeometry CSceneNodeGeometry::GetGeometry(bool worldspace) {
	if (worldspace) {
		float xscale = CVector3(m_cWorldMatrix(0, 0), m_cWorldMatrix(1, 0), m_cWorldMatrix(2, 0)).Length();
		float yscale = CVector3(m_cWorldMatrix(0, 1), m_cWorldMatrix(1, 1), m_cWorldMatrix(2, 1)).Length();
		float zscale = CVector3(m_cWorldMatrix(0, 2), m_cWorldMatrix(1, 2), m_cWorldMatrix(2, 2)).Length();
		SGeometry g = m_sGeometry;
		switch (g.shape) {
		case SGeometry::BOX: g.box.x *= xscale; g.box.y *= yscale; g.box.z *= zscale; break;
		case SGeometry::SPHERE: g.sphere.r *= 0.333f * (xscale + yscale + zscale); break;
		case SGeometry::CAPSULE: g.capsule.r *= 0.5f * (xscale + yscale); g.capsule.h *= zscale; break;
		case SGeometry::CYLINDER: g.cylinder.r *= 0.5f * (xscale + yscale); g.cylinder.h *= zscale; break;
		case SGeometry::TORUS: g.torus.r *= 0.5f * (xscale + yscale); g.torus.c *= zscale; break;
		case SGeometry::CONE: g.cone.r *= 0.5f * (xscale + yscale); g.cone.h *= zscale; break;
		case SGeometry::PLANE: g.plane.w *= xscale; g.plane.h *= yscale; break;
		case SGeometry::CIRCLE: g.circle.r *= 0.5f * (xscale + yscale); break;
		default: break;
		}
		return g;
	}
	return m_sGeometry;
}