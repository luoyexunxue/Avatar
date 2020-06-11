//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNode.h"

/**
* 构造函数
*/
CSceneNode::CSceneNode(const string& type, const string& name) {
	m_cScale.SetValue(CVector3::One);
	m_bVisible = true;
	m_bEnabled = true;
	m_bSelected = false;
	m_iNodeId = 0;
	m_strType = type;
	m_strName = name;
	m_pParent = 0;
}

/**
* 析构函数
*/
CSceneNode::~CSceneNode() {
}

/**
* 设置缩放
*/
void CSceneNode::SetScale(const CVector3& scale) {
	m_cScale.SetValue(scale);
	Transform();
}

/**
* 设置朝向
*/
void CSceneNode::SetOrientation(const CQuaternion& orientation) {
	m_cOrientation.SetValue(orientation);
	Transform();
}

/**
* 设置位置
*/
void CSceneNode::SetPosition(const CVector3& position) {
	m_cPosition.SetValue(position);
	Transform();
}

/**
* 更新变换矩阵
*/
void CSceneNode::Transform() {
	// 变换顺序为：缩放-旋转-平移
	m_cModelMatrix.MakeTransform(m_cScale, m_cOrientation, m_cPosition);
	if (!m_pParent) m_cWorldMatrix = m_cModelMatrix;
	else m_cWorldMatrix = m_pParent->m_cWorldMatrix * m_cModelMatrix;
	// 变换子节点
	list<CSceneNode*>::iterator iter = m_lstChildren.begin();
	while (iter != m_lstChildren.end()) {
		(*iter)->Transform();
		++iter;
	}
}

/**
* 获取网格数据
*/
CMeshData* CSceneNode::GetMeshData() {
	return 0;
}

/**
* 获取节点ID
*/
int& CSceneNode::GetId() {
	return m_iNodeId;
}

/**
* 获取节点类型
*/
const string& CSceneNode::GetType() const {
	return m_strType;
}

/**
* 获取节点名称
*/
const string& CSceneNode::GetName() const {
	return m_strName;
}

/**
* 得到世界坐标系下的缩放
*/
CVector3 CSceneNode::GetWorldScale() const {
	if (!m_pParent) return m_cScale;
	const float* m = m_pParent->m_cWorldMatrix.m_fValue;
	float sx = m_cScale.m_fValue[0] * CVector3(m[0], m[1], m[2]).Length();
	float sy = m_cScale.m_fValue[1] * CVector3(m[4], m[5], m[6]).Length();
	float sz = m_cScale.m_fValue[2] * CVector3(m[8], m[9], m[10]).Length();
	return CVector3(sx, sy, sz);
}

/**
* 得到世界坐标系下的朝向
*/
CQuaternion CSceneNode::GetWorldOrientation() const {
	if (!m_pParent) return m_cOrientation;
	CQuaternion rotate;
	rotate.FromMatrix(m_pParent->m_cWorldMatrix);
	return rotate * m_cOrientation;
}

/**
* 得到世界坐标系下的位置
*/
CVector3 CSceneNode::GetWorldPosition() const {
	if (!m_pParent) return m_cPosition;
	return m_pParent->m_cWorldMatrix * m_cPosition;
}

/**
* 获取世界坐标系下的包围盒
*/
CBoundingBox CSceneNode::GetBoundingBox() const {
	if (!m_cLocalBoundingBox.IsValid()) {
		return m_cLocalBoundingBox;
	}
	CBoundingBox aabb;
	for (int i = 0; i < 3; i++) {
		aabb.m_cMin[i] = m_cWorldMatrix.m_fValue[12 + i];
		aabb.m_cMax[i] = m_cWorldMatrix.m_fValue[12 + i];
		for (int j = 0; j < 3; j++) {
			float a = m_cWorldMatrix.m_fValue[i + j * 4] * m_cLocalBoundingBox.m_cMin.m_fValue[j];
			float b = m_cWorldMatrix.m_fValue[i + j * 4] * m_cLocalBoundingBox.m_cMax.m_fValue[j];
			if (a < b) {
				aabb.m_cMin[i] += a;
				aabb.m_cMax[i] += b;
			} else {
				aabb.m_cMin[i] += b;
				aabb.m_cMax[i] += a;
			}
		}
	}
	return aabb;
}