//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeAnimation.h"
#include "CEngine.h"
#include "CMeshLoader.h"
#include <cmath>

/**
* 构造函数
*/
CSceneNodeAnimation::CSceneNodeAnimation(const string& name, const string& meshFile, bool start, bool skeleton)
	: CSceneNode("animation", name) {
	m_strFile = meshFile;
	m_pMeshData = 0;
	m_iSkeletonMode = skeleton ? 0x01 : 0x00;
	m_bAnimation = start;
	m_bLoopAnimation = true;
	m_fAnimationRate = 1.0f;
	m_fBlendFactor = 0.0f;
	m_fTransition = 0.0f;
	m_fTransitionTimeInv = 0.0f;
	m_fSyncFactor[0] = 1.0f;
	m_fSyncFactor[1] = 1.0f;
	m_fAnimationTime[0] = 0.0f;
	m_fAnimationTime[1] = 0.0f;
	m_fAnimationBegin[0] = 0.0f;
	m_fAnimationBegin[1] = 0.0f;
	m_fCurrentTime[0] = 0.0f;
	m_fCurrentTime[1] = 0.0f;
}

/**
* 初始化场景节点
*/
bool CSceneNodeAnimation::Init() {
	m_pMeshData = CMeshLoader::Load(m_strFile, true, false);
	if (!m_pMeshData) return false;
	size_t jointCount = m_pMeshData->GetJointCount();
	size_t meshCount = m_pMeshData->GetMeshCount();
	m_vecMeshVertexPos.resize(meshCount);
	m_vecMeshVertexNor.resize(meshCount);
	for (size_t i = 0; i < jointCount; i++) {
		SJoint* joint = m_pMeshData->GetJoint(i);
		joint->finalMatrix.SetValue(joint->worldMatrix);
		if (joint->physics) {
			CVector3 binding = joint->worldMatrix * CVector3(0.0f, 0.0f, 0.0f, 1.0f);
			joint->physics->position.SetValue(binding);
		}
	}
	m_cLocalBoundingBox.SetInvalid();
	for (size_t i = 0; i < meshCount; i++) {
		CMesh* mesh = m_pMeshData->GetMesh(i);
		size_t vertexCount = mesh->GetVertexCount();
		m_cLocalBoundingBox += mesh->GetBoundingBox();
		m_vecMeshVertexPos[i].resize(vertexCount);
		m_vecMeshVertexNor[i].resize(vertexCount);
		for (size_t v = 0; v < vertexCount; v++) {
			m_vecMeshVertexPos[i][v].SetValue(mesh->GetVertex(v)->m_fPosition);
			m_vecMeshVertexNor[i][v].SetValue(mesh->GetVertex(v)->m_fNormal, 0.0f);
		}
	}
	if (jointCount > 0 && m_bAnimation) {
		m_iSkeletonMode = meshCount > 0 ? m_iSkeletonMode : 0x02;
		StartAnimation("", true, 0.0f);
	} else m_bAnimation = false;
	return true;
}

/**
* 销毁场景节点
*/
void CSceneNodeAnimation::Destroy() {
	if (m_pMeshData) delete m_pMeshData;
}

/**
* 渲染场景节点
*/
void CSceneNodeAnimation::Render() {
	if (CEngine::GetGraphicsManager()->IsDepthRender()) {
		size_t meshCount = m_pMeshData->GetMeshCount();
		for (size_t i = 0; i < meshCount; i++) {
			CMesh* pMesh = m_pMeshData->GetMesh(i);
			pMesh->GetMaterial()->GetTexture()->UseTexture();
			pMesh->Render(false);
		}
	} else {
		if (m_iSkeletonMode < 0x02) {
			CShader* currentShader = CEngine::GetShaderManager()->GetCurrentShader();
			size_t meshCount = m_pMeshData->GetMeshCount();
			for (size_t i = 0; i < meshCount; i++) {
				CMesh* pMesh = m_pMeshData->GetMesh(i);
				if (pMesh->GetMaterial()->GetShader()) {
					CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
					pMesh->GetMaterial()->PassUniform("uProjMatrix", pCamera->GetProjMatrix());
					pMesh->GetMaterial()->PassUniform("uViewMatrix", pCamera->GetViewMatrix());
					pMesh->GetMaterial()->PassUniform("uCameraPos", pCamera->m_cPosition);
					pMesh->GetMaterial()->PassUniform("uModelMatrix", m_cWorldMatrix);
				} else currentShader->UseShader();
				pMesh->Render();
			}
		}
		if (m_iSkeletonMode > 0x00) {
			DrawSkeleton(m_iSkeletonMode == 0x01);
		}
	}
}

/**
* 更新场景节点
*/
void CSceneNodeAnimation::Update(float dt) {
	if (!m_bAnimation) return;
	if (m_fTransition > 0.0f) {
		m_fTransition = m_fTransition > dt? m_fTransition - dt: 0.0f;
		float factor = m_fTransition * m_fTransitionTimeInv;
		float frame = 1.0f / (factor * m_fAnimationTime[0] + (1.0f - factor) * m_fAnimationTime[1]);
		m_fBlendFactor = 1.0f - factor;
		m_fSyncFactor[0] = m_fAnimationTime[0] * frame;
		m_fSyncFactor[1] = m_fAnimationTime[1] * frame;
	}
	dt *= m_fAnimationRate;
	if (m_fBlendFactor > 0.0f) {
		m_fCurrentTime[0] += dt * m_fSyncFactor[0];
		m_fCurrentTime[1] += dt * m_fSyncFactor[1];
		if (m_fCurrentTime[0] > m_fAnimationTime[0] + m_fAnimationBegin[0]
		 || m_fCurrentTime[1] > m_fAnimationTime[1] + m_fAnimationBegin[1]) {
			m_fCurrentTime[0] = m_fAnimationBegin[0];
			m_fCurrentTime[1] = m_fAnimationBegin[1];
			m_bAnimation = m_bLoopAnimation;
		}
	} else {
		m_fCurrentTime[0] += dt;
		if (m_fCurrentTime[0] > m_fAnimationTime[0] + m_fAnimationBegin[0]) {
			m_fCurrentTime[0] = m_fAnimationBegin[0];
			m_bAnimation = m_bLoopAnimation;
		}
	}
	if (m_bAnimation) SetupFrame(dt);
}

/**
* 获取网格数据
*/
CMeshData* CSceneNodeAnimation::GetMeshData() {
	return m_pMeshData;
}

/**
* 动画播放开始
*/
void CSceneNodeAnimation::StartAnimation(const string& name, bool loop, float transition) {
	int animation = m_pMeshData->GetAnimationIndex(name);
	m_bLoopAnimation = loop;
	m_bAnimation = true;
	if (animation >= 0) {
		m_fTransition = transition;
		if (m_fTransition <= 0.0f || m_fAnimationTime[0] == 0.0f) {
			m_fTransition = 0.0f;
			m_fTransitionTimeInv = 0.0f;
			m_fBlendFactor = 0.0f;
			m_fAnimationTime[0] = m_pMeshData->GetAnimationTime(animation);
			m_fAnimationBegin[0] = m_pMeshData->GetAnimationBeginTime(animation);
			m_fCurrentTime[0] = m_fAnimationBegin[0];
		} else {
			m_fTransitionTimeInv = 1.0f / transition;
			if (m_fBlendFactor > 0.5f) {
				m_fAnimationTime[0] = m_fAnimationTime[1];
				m_fAnimationBegin[0] = m_fAnimationBegin[1];
				m_fCurrentTime[0] = m_fCurrentTime[1];
			}
			m_fAnimationTime[1] = m_pMeshData->GetAnimationTime(animation);
			m_fAnimationBegin[1] = m_pMeshData->GetAnimationBeginTime(animation);
			const float timeAnimate = m_fCurrentTime[0] - m_fAnimationBegin[0];
			const float timeFactor = m_fAnimationTime[1] / m_fAnimationTime[0];
			m_fCurrentTime[1] = m_fAnimationBegin[1] + timeFactor * timeAnimate;
		}
		SetupJointKey();
	}
}

/**
* 混合两个动画
* 计算两个动画的 m_fSyncFactor 以保证两个动画周期一致，动画过渡平滑
*/
void CSceneNodeAnimation::BlendAnimation(const string& anim1, const string& anim2, float k) {
	int animation1 = m_pMeshData->GetAnimationIndex(anim1);
	int animation2 = m_pMeshData->GetAnimationIndex(anim2);
	m_bAnimation = animation1 >= 0 && animation2 >= 0;
	if (m_bAnimation) {
		m_fTransition = 0.0f;
		m_fBlendFactor = k;
		m_fAnimationTime[0] = m_pMeshData->GetAnimationTime(animation1);
		m_fAnimationTime[1] = m_pMeshData->GetAnimationTime(animation2);
		m_fAnimationBegin[0] = m_pMeshData->GetAnimationBeginTime(animation1);
		m_fAnimationBegin[1] = m_pMeshData->GetAnimationBeginTime(animation2);
		m_fCurrentTime[0] = m_fAnimationBegin[0];
		m_fCurrentTime[1] = m_fAnimationBegin[1];
		float frame = (1.0f - k) * m_fAnimationTime[0] + k * m_fAnimationTime[1];
		m_fSyncFactor[0] = m_fAnimationTime[0] / frame;
		m_fSyncFactor[1] = m_fAnimationTime[1] / frame;
		SetupJointKey();
	}
}

/**
* 动画播放停止
*/
void CSceneNodeAnimation::StopAnimation() {
	m_bAnimation = false;
}

/**
* 设置动画速率
*/
void CSceneNodeAnimation::SetAnimationFrameRate(float rate) {
	m_fAnimationRate = rate;
}

/**
* 设置骨骼显示
*/
void CSceneNodeAnimation::ShowSkeleton(bool visible, bool skeletonOnly) {
	m_iSkeletonMode = visible ? (skeletonOnly ? 0x02 : 0x01) : 0x00;
}

/**
* 获取动画名称
*/
string CSceneNodeAnimation::GetAnimationName(size_t index) {
	if (index >= m_pMeshData->GetAnimationCount()) return "";
	return m_pMeshData->GetAnimationName(index);
}

/**
* 骨骼注视功能
*/
void CSceneNodeAnimation::PointFacing(const string& joint, const CVector3& front, const CVector3& point, float angle) {
	m_pMeshData->SetFacing(joint, front, point, angle, -0.1f);
}

/**
* 设置骨骼变换
*/
void CSceneNodeAnimation::SetTransform(const string& joint, const CVector3& translation, const CVector3& rotation, const CVector3& scale) {
	m_pMeshData->SetTransform(joint, translation, rotation, scale);
}

/**
* 设置动画帧
*/
void CSceneNodeAnimation::SetupFrame(float dt) {
	CQuaternion rotation;
	CVector3 position;
	CMatrix4 animateMat;
	CVector3 gravity = CEngine::GetPhysicsManager()->GetGravity();
	size_t jointCount = m_pMeshData->GetJointCount();
	for (size_t i = 0; i < jointCount; i++) {
		SJoint* pJoint = m_pMeshData->GetJoint(i);
		if (pJoint->physics) {
			PhysicalSimulation(pJoint, gravity, dt, animateMat);
		} else {
			if (m_fBlendFactor >= 1.0f) JointTransform(pJoint, 1, rotation, position);
			else {
				JointTransform(pJoint, 0, rotation, position);
				if (m_fBlendFactor > 0.0f) {
					CQuaternion rotation_blend;
					CVector3 position_blend;
					JointTransform(pJoint, 1, rotation_blend, position_blend);
					rotation.SetValue(rotation.SlerpShortest(rotation_blend, m_fBlendFactor));
					position.SetValue(position.Lerp(position_blend, m_fBlendFactor));
				}
			}
			animateMat.MakeTransform(CVector3::One, rotation, position);
		}
		if (!pJoint->jointIK) {
			pJoint->worldMatrix.SetValue(pJoint->localMatrix * animateMat);
			if (pJoint->parent) pJoint->worldMatrix = pJoint->parent->worldMatrix * pJoint->worldMatrix;
		}
		// 骨骼变换矩阵为 worldMatrix = parent.worldMatrix * localMatrix * animateMat
		// 顶点变换矩阵为 finalMatrix = worldMatrix * bindMatrixInv
		pJoint->finalMatrix = pJoint->worldMatrix * pJoint->bindMatrixInv;
	}
	UpdateVertex();
}

/**
* 设置关节关键帧
*/
void CSceneNodeAnimation::SetupJointKey() {
	size_t jointCount = m_pMeshData->GetJointCount();
	for (size_t i = 0; i < jointCount; i++) {
		SJoint* pJoint = m_pMeshData->GetJoint(i);
		const size_t numRotKeys = pJoint->keyRot.size();
		const size_t numPosKeys = pJoint->keyPos.size();
		size_t keyIndex1 = 0;
		size_t keyIndex2 = 0;
		size_t keyIndex3 = 0;
		size_t keyIndex4 = 0;
		while (numRotKeys > ++keyIndex1 && m_fAnimationBegin[0] >= pJoint->keyRot[keyIndex1].time);
		while (numRotKeys > ++keyIndex2 && m_fAnimationBegin[1] >= pJoint->keyRot[keyIndex2].time);
		while (numPosKeys > ++keyIndex3 && m_fAnimationBegin[0] >= pJoint->keyPos[keyIndex3].time);
		while (numPosKeys > ++keyIndex4 && m_fAnimationBegin[1] >= pJoint->keyPos[keyIndex4].time);
		pJoint->currentRotKey[2] = keyIndex1 - 1 > 0 ? keyIndex1 - 1 : 0;
		pJoint->currentRotKey[3] = keyIndex2 - 1 > 0 ? keyIndex2 - 1 : 0;
		pJoint->currentPosKey[2] = keyIndex3 - 1 > 0 ? keyIndex3 - 1 : 0;
		pJoint->currentPosKey[3] = keyIndex4 - 1 > 0 ? keyIndex4 - 1 : 0;
	}
}

/**
* 计算关节动画变换
*/
void CSceneNodeAnimation::JointTransform(SJoint* joint, int channel, CQuaternion& rot, CVector3& pos) {
	const size_t numRotKeys = joint->keyRot.size();
	const size_t numPosKeys = joint->keyPos.size();
	if (numRotKeys > 0) {
		size_t keyIndex = joint->currentRotKey[channel];
		float keyTime = joint->keyRot[keyIndex].time;
		if (m_fCurrentTime[channel] < keyTime) keyIndex = joint->currentRotKey[channel + 2];
		while (numRotKeys > ++keyIndex && m_fCurrentTime[channel] >= joint->keyRot[keyIndex].time);
		joint->currentRotKey[channel] = keyIndex - 1;
		const SAnimationRotKey& a = joint->keyRot[keyIndex - 1];
		if (keyIndex == numRotKeys) rot.SetValue(a.param);
		else {
			SAnimationRotKey& b = joint->keyRot[keyIndex];
			rot.SetValue(a.param.SlerpShortest(b.param, (m_fCurrentTime[channel] - a.time) / (b.time - a.time)));
		}
	} else rot.SetValue(0.0f, 0.0f, 0.0f, 1.0f);
	if (numPosKeys > 0) {
		size_t keyIndex = joint->currentPosKey[channel];
		float keyTime = joint->keyPos[keyIndex].time;
		if (m_fCurrentTime[channel] < keyTime) keyIndex = joint->currentPosKey[channel + 2];
		while (numPosKeys > ++keyIndex && m_fCurrentTime[channel] >= joint->keyPos[keyIndex].time);
		joint->currentPosKey[channel] = keyIndex - 1;
		const SAnimationPosKey& a = joint->keyPos[keyIndex - 1];
		if (keyIndex == numPosKeys) pos.SetValue(a.param);
		else {
			SAnimationPosKey& b = joint->keyPos[keyIndex];
			pos.SetValue(a.param.Lerp(b.param, (m_fCurrentTime[channel] - a.time) / (b.time - a.time)));
		}
	} else pos.SetValue(0.0f, 0.0f, 0.0f);
}

/**
* 更新网格顶点
*/
void CSceneNodeAnimation::UpdateVertex() {
	m_cLocalBoundingBox.SetInvalid();
	for (size_t i = 0; i < m_pMeshData->GetMeshCount(); i++) {
		CMesh* mesh = m_pMeshData->GetMesh(i);
		size_t count = mesh->GetVertexCount();
		if (mesh->GetBindCount() != count) continue;
		for (size_t j = 0; j < count; j++) {
			CVertex* vertex = mesh->GetVertex(j);
			CVertexJoint* bind = mesh->GetBind(j);
			if (bind->m_iCount > 0) {
				SJoint* joint = m_pMeshData->GetJoint(bind->m_iJointId[0]);
				CMatrix4 skinMatrix = joint->finalMatrix * bind->m_fWeight[0];
				for (int n = 1; n < bind->m_iCount; n++) {
					joint = m_pMeshData->GetJoint(bind->m_iJointId[n]);
					skinMatrix += joint->finalMatrix * bind->m_fWeight[n];
				}
				vertex->SetPosition(skinMatrix * m_vecMeshVertexPos[i][j]);
				vertex->SetNormal(skinMatrix * m_vecMeshVertexNor[i][j]);
			}
			m_cLocalBoundingBox.Update(vertex->m_fPosition);
		}
		mesh->Update(1);
	}
}

/**
* 绘制骨骼
*/
void CSceneNodeAnimation::DrawSkeleton(bool topMost) {
	size_t jointCount = m_pMeshData->GetJointCount();
	if (jointCount == 0) return;
	if (m_vecJointVertex.size() != (jointCount - 1) << 1) {
		m_vecJointVertex.resize((jointCount - 1) << 1);
		for (size_t i = 0; i < m_vecJointVertex.size(); i += 2) {
			m_vecJointVertex[i + 0].SetColor(CColor::Red);
			m_vecJointVertex[i + 1].SetColor(CColor::Green);
		}
	}
	// 计算骨骼节点模型坐标系位置
	int index = 0;
	for (size_t i = 0; i < jointCount; i++) {
		SJoint* pJoint = m_pMeshData->GetJoint(i);
		pJoint->position[0] = pJoint->worldMatrix[12];
		pJoint->position[1] = pJoint->worldMatrix[13];
		pJoint->position[2] = pJoint->worldMatrix[14];
		if (pJoint->parent) {
			m_vecJointVertex[index++].SetPosition(pJoint->parent->position);
			m_vecJointVertex[index++].SetPosition(pJoint->position);
		}
	}
	CGraphicsManager* pGraphicsMgr = CEngine::GetGraphicsManager();
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	CShader* pShader = pShaderMgr->GetCurrentShader();
	CShader* pSkeletonShader = pShaderMgr->GetShader("default");
	pSkeletonShader->UseShader();
	pSkeletonShader->SetUniform("uProjMatrix", pGraphicsMgr->GetCamera()->GetProjMatrix());
	pSkeletonShader->SetUniform("uViewMatrix", pGraphicsMgr->GetCamera()->GetViewMatrix());
	pSkeletonShader->SetUniform("uModelMatrix", m_cWorldMatrix);
	pSkeletonShader->SetUniform("uPointSize", 5.0f);
	if (topMost) {
		pGraphicsMgr->SetDepthTestEnable(false);
		pGraphicsMgr->DrawLines(&m_vecJointVertex[0], m_vecJointVertex.size(), 2.0f, false);
		pGraphicsMgr->DrawPoints(&m_vecJointVertex[0], m_vecJointVertex.size());
		pGraphicsMgr->SetDepthTestEnable(true);
	} else {
		pGraphicsMgr->DrawLines(&m_vecJointVertex[0], m_vecJointVertex.size(), 2.0f, false);
		pGraphicsMgr->DrawPoints(&m_vecJointVertex[0], m_vecJointVertex.size());
	}
	pShader->UseShader();
}

/**
* 关节物理模拟
*/
void CSceneNodeAnimation::PhysicalSimulation(SJoint* joint, const CVector3& gravity, float dt, CMatrix4& anim) {
	const float maxStep = 1.0f / 60.0f;
	const CVector3 orgin(0.0f, 0.0f, 0.0f, 1.0f);
	SJointDynamic* physics = joint->physics;
	if (!physics->enabled) return;
	if (physics->isManual) {
		anim.SetValue(physics->transform);
		return;
	}
	CMatrix4 mat1 = joint->parent ? m_cWorldMatrix * joint->parent->worldMatrix : m_cWorldMatrix;
	CMatrix4 mat2 = mat1 * joint->localMatrix;
	// 世界坐标系关节目标位置
	const CVector3 targetPosParent = mat1 * orgin;
	const CVector3 targetVector = mat2 * orgin - targetPosParent;
	// 积分
	float stepTime = dt;
	float currTime = 0.0f;
	if (physics->mass <= 0.0f) {
		stepTime = 0.0f;
		physics->position = targetPosParent + targetVector;
	}
	while (stepTime > 0.0f) {
		float time = stepTime > maxStep ? maxStep : stepTime;
		stepTime -= maxStep;
		currTime += time;
		CVector3 parentPos = physics->parentPos.Lerp(targetPosParent, currTime / dt);
		CVector3 jointPos = parentPos + targetVector;
		CVector3 v0 = physics->position - parentPos;
		CVector3 v1 = jointPos - parentPos;
		CVector3 v2 = v0.CrossProduct(v1.CrossProduct(v0));
		float lv0 = v0.Length();
		float lv1 = v1.Length();
		v0.Normalize();
		v1.Normalize();
		v2.Normalize();
		float cosa = v0.DotProduct(v1);
		if (cosa < -1.0f) cosa = -1.0f;
		else if (cosa > 1.0f) cosa = 1.0f;
		if (cosa < 0.0f && v2.DotProduct(v2) < 1E-9) v2.SetValue(v0.Tangent().Normalize());
		// 限制骨骼拉伸极限为2倍
		if (lv0 > lv1 * 2.0f) {
			lv0 = lv1 * 2.0f;
			physics->position = v0 * lv0 + parentPos;
		}
		// 计算受力
		CVector3 vForce = v2 * (physics->bendElasticity * acosf(cosa));
		CVector3 hForce = v0 * (physics->stretchElasticity * (lv1 - lv0));
		CVector3 force = gravity * physics->mass + vForce + hForce;
		// 速度位置 verlet 算法
		CVector3 acceleration = force / physics->mass;
		physics->velocity += (acceleration + physics->acceleration) * (time * 0.5f);
		physics->velocity += physics->velocity * physics->damping;
		physics->acceleration = acceleration;
		physics->position += physics->velocity * time + physics->acceleration * (time * time * 0.5f);
	}
	physics->parentPos.SetValue(targetPosParent);
	// 计算关节变换
	CQuaternion rotation;
	CVector3 localPos = mat2.Invert() * physics->position;
	CVector3 localPosParent = mat2 * physics->parentPos;
	rotation.FromVector(localPosParent, localPosParent - localPos);
	// 点关注模式
	if (physics->isFacing) {
		CQuaternion rot;
		CVector3 src = m_cWorldMatrix * physics->frontDir;
		CVector3 dst = physics->facingPoint - m_cWorldMatrix * CVector3(0.0f, 0.0f, 0.0f, 1.0f);
		CVector3 local_src = mat2 * CVector3(src.m_fValue, 0.0f);
		CVector3 local_dst = mat2 * CVector3(dst.m_fValue, 0.0f);
		physics->direction = physics->direction.Slerp(local_dst, std::min(std::abs(dt / physics->damping), 1.0f));
		rot.FromVector(local_src, physics->direction);
		float angle = std::min(physics->restrictAngle, std::abs(2.0f * acosf(rot[3])));
		rotation *= rot.FromAxisAngle(local_src.CrossProduct(physics->direction), angle);
	}
	anim.MakeTransform(CVector3::One, rotation, localPos);
}