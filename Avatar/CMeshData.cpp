//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CMeshData.h"
#include <cstring>

/**
* ���캯��
*/
CMeshData::CMeshData() {
	m_pMeshData = new SMeshData();
}

/**
* ��������
*/
CMeshData::~CMeshData() {
	size_t numMeshes = m_pMeshData->meshes.size();
	size_t numJoints = m_pMeshData->joints.size();
	for (size_t i = 0; i < numMeshes; i++) delete m_pMeshData->meshes[i];
	for (size_t i = 0; i < numJoints; i++) delete m_pMeshData->joints[i];
	delete m_pMeshData;
}

/**
* ��ȡ��������
* @return ��������
*/
int CMeshData::GetMeshCount() const {
	return m_pMeshData->meshes.size();
}

/**
* ��ȡ�ؽ�����
* @return �ؽ�����
*/
int CMeshData::GetJointCount() const {
	return m_pMeshData->joints.size();
}

/**
* ��ȡ��������
* @return ��������
*/
int CMeshData::GetAnimationCount() const {
	return m_pMeshData->animationNames.size();
}

/**
* ��ȡ���񶥵����
* @return ��������
*/
int CMeshData::GetVertexCount() const {
	int count = 0;
	for (size_t i = 0; i < m_pMeshData->meshes.size(); i++) {
		count += m_pMeshData->meshes[i]->GetVertexCount();
	}
	return count;
}

/**
* ��ȡ���������μ���
* @return ����������
*/
int CMeshData::GetTriangleCount() const {
	int count = 0;
	for (size_t i = 0; i < m_pMeshData->meshes.size(); i++) {
		count += m_pMeshData->meshes[i]->GetTriangleCount();
	}
	return count;
}

/**
* ��ȡ��Χ��
* @return �����AABB��Χ��
*/
CBoundingBox CMeshData::GetBoundingBox() const {
	CBoundingBox boundingBox;
	for (size_t i = 0; i < m_pMeshData->meshes.size(); i++) {
		boundingBox.Update(m_pMeshData->meshes[i]->GetBoundingBox());
	}
	return boundingBox;
}

/**
* ��ȡָ������
* @param index ����
* @return �������ָ��
*/
CMesh* CMeshData::GetMesh(int index) {
	return m_pMeshData->meshes[index];
}

/**
* ��ȡָ���ؽ�
* @param index ����
* @return �ؽڶ���ָ��
*/
SJoint* CMeshData::GetJoint(int index) {
	return m_pMeshData->joints[index];
}

/**
* ��ȡָ���ؽ�
* @param name �ؽ�����
* @return �ؽڶ���ָ�룬���� 0 ��ʾ������
*/
SJoint* CMeshData::GetJoint(const string& name) {
	size_t count = m_pMeshData->joints.size();
	for (size_t i = 0; i < count; i++) {
		if (name == m_pMeshData->joints[i]->name) {
			return m_pMeshData->joints[i];
		}
	}
	return 0;
}

/**
* ��ȡָ�����ƵĶ���
* @param name ��������
* @return �������������� -1 ��ʾ������
*/
int CMeshData::GetAnimationIndex(const string& name) {
	size_t count = m_pMeshData->animationNames.size();
	if (name.empty()) return count > 0? 0: -1;
	for (size_t i = 0; i < count; i++) {
		if (m_pMeshData->animationNames[i] == name) {
			return i;
		}
	}
	return -1;
}

/**
* ��ȡ��������
* @param index ����
* @return ��������
*/
string& CMeshData::GetAnimationName(int index) {
	return m_pMeshData->animationNames[index];
}

/**
* ��ȡ����ʱ��
* @param index ����
* @return ����ʱ��
*/
float CMeshData::GetAnimationTime(int index) {
	float beginTime = m_pMeshData->animationBeginTimes[index];
	float endTime = m_pMeshData->animationEndTimes[index];
	return endTime - beginTime;
}

/**
* ��ȡ������ʼʱ��
* @param index ����
* @return ������ʼʱ��
*/
float CMeshData::GetAnimationBeginTime(int index) {
	return m_pMeshData->animationBeginTimes[index];
}

/**
* ��ȡ��������ʱ��
* @param index ����
* @return ��������ʱ��
*/
float CMeshData::GetAnimationEndTime(int index) {
	return m_pMeshData->animationEndTimes[index];
}

/**
* �������
* @param mesh �������
* @return ��ǰ CMeshData ָ��
*/
CMeshData* CMeshData::AddMesh(CMesh* mesh) {
	m_pMeshData->meshes.push_back(mesh);
	return this;
}

/**
* ��ӹؽڣ���Ҫע������Ӹ��ؽ�
* @param joint ��������
* @return ��ǰ CMeshData ָ��
*/
CMeshData* CMeshData::AddJoint(SJoint* joint) {
	joint->index = m_pMeshData->joints.size();
	for (int i = 0; i < 4; i++) {
		joint->currentRotKey[i] = 0;
		joint->currentPosKey[i] = 0;
	}
	m_pMeshData->joints.push_back(joint);
	return this;
}

/**
* ��Ӷ�����Ϣ
* @param name ��������
* @param beginTime ��ʼʱ��
* @param endTime ����ʱ��
* @return ��ǰ CMeshData ָ��
* @attention �������ݰ����� @ref SJoint ��
*/
CMeshData* CMeshData::AddAnimation(const string& name, float beginTime, float endTime) {
	m_pMeshData->animationNames.push_back(name);
	m_pMeshData->animationBeginTimes.push_back(beginTime);
	m_pMeshData->animationEndTimes.push_back(endTime);
	// ɾ���ظ������任����
	for (size_t i = 0; i < m_pMeshData->joints.size(); i++) {
		SJoint* pJoint = m_pMeshData->joints[i];
		int rot = static_cast<int>(pJoint->keyRot.size()) - 2;
		int pos = static_cast<int>(pJoint->keyPos.size()) - 2;
		for (int n = 0; n < rot; n++) {
			SAnimationRotKey& key = pJoint->keyRot[n];
			if (key.time < beginTime) continue;
			if (pJoint->keyRot[n + 2].time > endTime) break;
			if (pJoint->keyRot[n + 1].param == key.param) {
				pJoint->keyRot.erase(pJoint->keyRot.begin() + (1 + n--));
				rot -= 1;
			}
		}
		for (int n = 0; n < pos; n++) {
			SAnimationPosKey& key = pJoint->keyPos[n];
			if (key.time < beginTime) continue;
			if (pJoint->keyPos[n + 2].time > endTime) break;
			if (pJoint->keyPos[n + 1].param == key.param) {
				pJoint->keyPos.erase(pJoint->keyPos.begin() + (1 + n--));
				pos -= 1;
			}
		}
	}
	return this;
}

/**
* Ϊ�����������ģ��֧��
* @param name �ؽ�����
* @param mass �ؽ�ĩ������
* @param hk �عؽڷ���ĵ���ϵ����������ϵ��
* @param vk ��ֱ�ڹؽڷ���ĵ���ϵ����������ϵ��
* @param damping ����ϵ��
* @return �����Ƿ����
*/
bool CMeshData::SetPhysics(const string& name, float mass, float hk, float vk, float damping) {
	SJoint* pJoint = GetJoint(name);
	if (!pJoint) return false;
	if (!pJoint->physics) {
		pJoint->physics = new SJointDynamic();
		pJoint->physics->enabled = true;
		pJoint->physics->isFacing = false;
	}
	pJoint->physics->mass = mass;
	pJoint->physics->hElasticity = hk;
	pJoint->physics->vElasticity = vk;
	pJoint->physics->damping = damping;
	return true;
}

/**
* ���ù��������
* @param name �ؽ�����
* @param front �ؽڵ�ǰ����(��������ϵ)
* @param point ���õĳ���λ��(��������ϵ)
* @param angle ��ת�Ƕ�����
* @param damping ����ϵ��
* @return �����Ƿ����
*/
bool CMeshData::SetFacing(const string& name, const CVector3& front, const CVector3& point, float angle, float damping) {
	SJoint* pJoint = GetJoint(name);
	if (!pJoint) return false;
	if (!pJoint->physics) {
		pJoint->physics = new SJointDynamic();
		pJoint->physics->enabled = true;
		pJoint->physics->damping = damping;
	}
	pJoint->physics->isFacing = true;
	pJoint->physics->mass = 0.0f;
	pJoint->physics->hElasticity = 0.0f;
	pJoint->physics->vElasticity = 0.0f;
	pJoint->physics->facingPoint = point;
	pJoint->physics->frontDir.SetValue(front.m_fValue, 0.0f);
	pJoint->physics->restrictAngle = angle;
	return true;
}

/**
* �����������
*/
void CMeshData::ClearMesh() {
	size_t count = m_pMeshData->meshes.size();
	for (size_t i = 0; i < count; i++) delete m_pMeshData->meshes[i];
	m_pMeshData->meshes.clear();
}

/**
* ������йؽ�
*/
void CMeshData::ClearJoint() {
	size_t count = m_pMeshData->joints.size();
	for (size_t i = 0; i < count; i++) delete m_pMeshData->joints[i];
	m_pMeshData->joints.clear();
}

/**
* ������ж���
*/
void CMeshData::ClearAnimation() {
	m_pMeshData->animationNames.clear();
	m_pMeshData->animationBeginTimes.clear();
	m_pMeshData->animationEndTimes.clear();
}

/**
* ��¡�������
*/
CMeshData* CMeshData::Clone() const {
	CMeshData* meshData = new CMeshData();
	meshData->m_pMeshData->meshes.resize(m_pMeshData->meshes.size());
	meshData->m_pMeshData->joints.resize(m_pMeshData->joints.size());
	meshData->m_pMeshData->animationNames = m_pMeshData->animationNames;
	meshData->m_pMeshData->animationBeginTimes = m_pMeshData->animationBeginTimes;
	meshData->m_pMeshData->animationEndTimes = m_pMeshData->animationEndTimes;
	for (size_t i = 0; i < m_pMeshData->meshes.size(); i++) {
		meshData->m_pMeshData->meshes[i] = m_pMeshData->meshes[i]->Clone();
	}
	map<SJoint*, SJoint*> jointLookup;
	for (size_t i = 0; i < m_pMeshData->joints.size(); i++) {
		SJoint* src = m_pMeshData->joints[i];
		SJoint* dst = new SJoint();
		jointLookup.insert(std::pair<SJoint*, SJoint*>(src, dst));
		map<SJoint*, SJoint*>::iterator iter = jointLookup.find(src->parent);
		if (iter != jointLookup.end()) dst->parent = iter->second;
		dst->index = src->index;
		dst->name = src->name;
		dst->position = src->position;
		dst->localMatrix = src->localMatrix;
		dst->worldMatrix = src->worldMatrix;
		dst->bindMatrixInv = src->bindMatrixInv;
		dst->finalMatrix = src->finalMatrix;
		if (src->physics) {
			dst->physics = new SJointDynamic();
			dst->physics->enabled = src->physics->enabled;
			dst->physics->mass = src->physics->mass;
			dst->physics->vElasticity = src->physics->vElasticity;
			dst->physics->hElasticity = src->physics->hElasticity;
			dst->physics->damping = src->physics->damping;
			dst->physics->isFacing = src->physics->isFacing;
			dst->physics->restrictAngle = src->physics->restrictAngle;
			dst->physics->facingPoint = src->physics->facingPoint;
			dst->physics->frontDir = src->physics->frontDir;
		}
		memset(dst->currentRotKey, 0, sizeof(int) * 4);
		memset(dst->currentPosKey, 0, sizeof(int) * 4);
		dst->keyRot = src->keyRot;
		dst->keyPos = src->keyPos;
		meshData->m_pMeshData->joints[i] = dst;
	}
	return meshData;
}
