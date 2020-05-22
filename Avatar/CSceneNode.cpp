//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNode.h"

/**
* ���캯��
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
* ��������
*/
CSceneNode::~CSceneNode() {
}

/**
* ��������
*/
void CSceneNode::SetScale(const CVector3& scale) {
	m_cScale.SetValue(scale);
	Transform();
}

/**
* ���ó���
*/
void CSceneNode::SetOrientation(const CQuaternion& orientation) {
	m_cOrientation.SetValue(orientation);
	Transform();
}

/**
* ����λ��
*/
void CSceneNode::SetPosition(const CVector3& position) {
	m_cPosition.SetValue(position);
	Transform();
}

/**
* ���±任����
*/
void CSceneNode::Transform() {
	// �任˳��Ϊ������-��ת-ƽ��
	m_cModelMatrix.MakeTransform(m_cScale, m_cOrientation, m_cPosition);
	if (!m_pParent) m_cWorldMatrix = m_cModelMatrix;
	else m_cWorldMatrix = m_pParent->m_cWorldMatrix * m_cModelMatrix;
	// �任�ӽڵ�
	list<CSceneNode*>::iterator iter = m_lstChildren.begin();
	while (iter != m_lstChildren.end()) {
		(*iter)->Transform();
		++iter;
	}
}

/**
* ��ȡ��������
*/
CMeshData* CSceneNode::GetMeshData() {
	return 0;
}

/**
* ��ȡ�ڵ�ID
*/
int& CSceneNode::GetId() {
	return m_iNodeId;
}

/**
* ��ȡ�ڵ�����
*/
const string& CSceneNode::GetType() const {
	return m_strType;
}

/**
* ��ȡ�ڵ�����
*/
const string& CSceneNode::GetName() const {
	return m_strName;
}

/**
* �õ���������ϵ�µ�����
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
* �õ���������ϵ�µĳ���
*/
CQuaternion CSceneNode::GetWorldOrientation() const {
	if (!m_pParent) return m_cOrientation;
	CQuaternion rotate;
	rotate.FromMatrix(m_pParent->m_cWorldMatrix);
	return rotate * m_cOrientation;
}

/**
* �õ���������ϵ�µ�λ��
*/
CVector3 CSceneNode::GetWorldPosition() const {
	if (!m_pParent) return m_cPosition;
	return m_pParent->m_cWorldMatrix * m_cPosition;
}

/**
* ��ȡ��������ϵ�µİ�Χ��
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