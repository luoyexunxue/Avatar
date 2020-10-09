//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CMeshData.h"
#include <cstring>

/**
* 构造函数
*/
CMeshData::CMeshData() {
	m_pMeshData = new SMeshData();
}

/**
* 析构函数
*/
CMeshData::~CMeshData() {
	size_t numMeshes = m_pMeshData->meshes.size();
	size_t numJoints = m_pMeshData->joints.size();
	for (size_t i = 0; i < numMeshes; i++) delete m_pMeshData->meshes[i];
	for (size_t i = 0; i < numJoints; i++) delete m_pMeshData->joints[i];
	delete m_pMeshData;
}

/**
* 获取网格数量
* @return 网格数量
*/
size_t CMeshData::GetMeshCount() const {
	return m_pMeshData->meshes.size();
}

/**
* 获取关节数量
* @return 关节数量
*/
size_t CMeshData::GetJointCount() const {
	return m_pMeshData->joints.size();
}

/**
* 获取动画数量
* @return 动画数量
*/
size_t CMeshData::GetAnimationCount() const {
	return m_pMeshData->animationNames.size();
}

/**
* 获取网格顶点计数
* @return 顶点总数
*/
size_t CMeshData::GetVertexCount() const {
	size_t count = 0;
	for (size_t i = 0; i < m_pMeshData->meshes.size(); i++) {
		count += m_pMeshData->meshes[i]->GetVertexCount();
	}
	return count;
}

/**
* 获取网格三角形计数
* @return 三角形总数
*/
size_t CMeshData::GetTriangleCount() const {
	size_t count = 0;
	for (size_t i = 0; i < m_pMeshData->meshes.size(); i++) {
		count += m_pMeshData->meshes[i]->GetTriangleCount();
	}
	return count;
}

/**
* 获取包围盒
* @return 网格的AABB包围盒
*/
CBoundingBox CMeshData::GetBoundingBox() const {
	CBoundingBox boundingBox;
	for (size_t i = 0; i < m_pMeshData->meshes.size(); i++) {
		boundingBox.Update(m_pMeshData->meshes[i]->GetBoundingBox());
	}
	return boundingBox;
}

/**
* 获取指定网格
* @param index 索引
* @return 网格对象指针
*/
CMesh* CMeshData::GetMesh(size_t index) {
	return m_pMeshData->meshes[index];
}

/**
* 获取指定关节
* @param index 索引
* @return 关节对象指针
*/
SJoint* CMeshData::GetJoint(size_t index) {
	return m_pMeshData->joints[index];
}

/**
* 获取指定关节
* @param name 关节名称
* @return 关节对象指针，返回 0 表示不存在
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
* 获取指定名称的动画
* @param name 动画名称
* @return 动画索引，返回 -1 表示不存在
*/
int CMeshData::GetAnimationIndex(const string& name) {
	int count = static_cast<int>(m_pMeshData->animationNames.size());
	if (name.empty()) return count > 0 ? 0 : -1;
	for (int i = 0; i < count; i++) {
		if (m_pMeshData->animationNames[i] == name) {
			return i;
		}
	}
	return -1;
}

/**
* 获取动画名称
* @param index 索引
* @return 动画名称
*/
string& CMeshData::GetAnimationName(size_t index) {
	return m_pMeshData->animationNames[index];
}

/**
* 获取动画时长
* @param index 索引
* @return 动画时长
*/
float CMeshData::GetAnimationTime(size_t index) {
	float beginTime = m_pMeshData->animationBeginTimes[index];
	float endTime = m_pMeshData->animationEndTimes[index];
	return endTime - beginTime;
}

/**
* 获取动画开始时间
* @param index 索引
* @return 动画开始时间
*/
float CMeshData::GetAnimationBeginTime(size_t index) {
	return m_pMeshData->animationBeginTimes[index];
}

/**
* 获取动画结束时间
* @param index 索引
* @return 动画结束时间
*/
float CMeshData::GetAnimationEndTime(size_t index) {
	return m_pMeshData->animationEndTimes[index];
}

/**
* 添加网格
* @param mesh 网格对象
* @return 当前 CMeshData 指针
*/
CMeshData* CMeshData::AddMesh(CMesh* mesh) {
	m_pMeshData->meshes.push_back(mesh);
	return this;
}

/**
* 添加关节，需要注意先添加父关节
* @param joint 骨骼对象
* @return 当前 CMeshData 指针
*/
CMeshData* CMeshData::AddJoint(SJoint* joint) {
	joint->index = (int)m_pMeshData->joints.size();
	for (int i = 0; i < 4; i++) {
		joint->currentRotKey[i] = 0;
		joint->currentPosKey[i] = 0;
	}
	m_pMeshData->joints.push_back(joint);
	return this;
}

/**
* 添加动画信息
* @param name 动画名称
* @param beginTime 开始时间
* @param endTime 结束时间
* @return 当前 CMeshData 指针
* @attention 动画数据包含在 @ref SJoint 中
*/
CMeshData* CMeshData::AddAnimation(const string& name, float beginTime, float endTime) {
	m_pMeshData->animationNames.push_back(name);
	m_pMeshData->animationBeginTimes.push_back(beginTime);
	m_pMeshData->animationEndTimes.push_back(endTime);
	// 删除重复动画变换数据
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
* 为骨骼添加物理模拟支持
* @param name 关节名称
* @param mass 关节末端质量
* @param hk 沿关节方向的弹性系数，即拉伸系数
* @param vk 垂直于关节方向的弹性系数，即折弯系数
* @param damping 阻尼系数
* @return 骨骼是否存在
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
* 设置骨骼朝向点
* @param name 关节名称
* @param front 关节的前朝向(物体坐标系)
* @param point 设置的朝向位置(世界坐标系)
* @param angle 旋转角度限制
* @param damping 阻尼系数
* @return 骨骼是否存在
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
* 清空所有网格
*/
void CMeshData::ClearMesh(bool release) {
	if (release) {
		size_t count = m_pMeshData->meshes.size();
		for (size_t i = 0; i < count; i++) {
			delete m_pMeshData->meshes[i];
		}
	}
	m_pMeshData->meshes.clear();
}

/**
* 清空所有关节
*/
void CMeshData::ClearJoint(bool release) {
	if (release) {
		size_t count = m_pMeshData->joints.size();
		for (size_t i = 0; i < count; i++) {
			delete m_pMeshData->joints[i];
		}
	}
	m_pMeshData->joints.clear();
}

/**
* 清空所有动画
*/
void CMeshData::ClearAnimation() {
	m_pMeshData->animationNames.clear();
	m_pMeshData->animationBeginTimes.clear();
	m_pMeshData->animationEndTimes.clear();
}

/**
* 克隆网格对象
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
		memset(dst->currentRotKey, 0, sizeof(size_t) * 4);
		memset(dst->currentPosKey, 0, sizeof(size_t) * 4);
		dst->keyRot = src->keyRot;
		dst->keyPos = src->keyPos;
		meshData->m_pMeshData->joints[i] = dst;
	}
	return meshData;
}
