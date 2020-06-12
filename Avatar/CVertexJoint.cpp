//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CVertexJoint.h"

/**
* 默认构造函数
*/
CVertexJoint::CVertexJoint() {
	m_iCount = 0;
}

/**
* 复制构造函数
*/
CVertexJoint::CVertexJoint(const CVertexJoint& joint) {
	m_iCount = joint.m_iCount;
	m_iJointId[0] = joint.m_iJointId[0];
	m_iJointId[1] = joint.m_iJointId[1];
	m_iJointId[2] = joint.m_iJointId[2];
	m_iJointId[3] = joint.m_iJointId[3];
	m_fWeight[0] = joint.m_fWeight[0];
	m_fWeight[1] = joint.m_fWeight[1];
	m_fWeight[2] = joint.m_fWeight[2];
	m_fWeight[3] = joint.m_fWeight[3];
}

/**
* 仅有一个骨骼影响
*/
CVertexJoint::CVertexJoint(int jointId) {
	m_iCount = 1;
	m_iJointId[0] = jointId;
	m_fWeight[0] = 1.0f;
}

/**
* 构造四个绑定信息实例
*/
CVertexJoint::CVertexJoint(const int jointId[4], const float weight[4]) {
	m_iCount = 4;
	m_iJointId[0] = jointId[0];
	m_iJointId[1] = jointId[1];
	m_iJointId[2] = jointId[2];
	m_iJointId[3] = jointId[3];
	m_fWeight[0] = weight[0];
	m_fWeight[1] = weight[1];
	m_fWeight[2] = weight[2];
	m_fWeight[3] = weight[3];
}

/**
* 添加绑定
*/
CVertexJoint& CVertexJoint::AddBind(int jointId, float weight) {
	if (m_iCount < 4) {
		m_iJointId[m_iCount] = jointId;
		m_fWeight[m_iCount] = weight;
		m_iCount += 1;
	} else {
		float min = weight;
		int index = -1;
		for (int i = 0; i < 4; i++) {
			if (m_fWeight[i] < min) {
				min = m_fWeight[i];
				index = i;
			}
		}
		if (index >= 0) {
			m_iJointId[index] = jointId;
			m_fWeight[index] = weight;
		}
	}
	return *this;
}

/**
* 添加绑定
*/
CVertexJoint& CVertexJoint::AddBind(const CVertexJoint& bind, float scale) {
	for (int i = 0; i < bind.m_iCount; i++) {
		int existJoint = -1;
		for (int j = 0; j < m_iCount; j++) {
			if (m_iJointId[j] == bind.m_iJointId[i]) {
				existJoint = j;
				break;
			}
		}
		if (existJoint >= 0) m_fWeight[existJoint] += bind.m_fWeight[i] * scale;
		else AddBind(bind.m_iJointId[i], bind.m_fWeight[i] * scale);
	}
	return *this;
}

/**
* 单位化权重
*/
CVertexJoint& CVertexJoint::Normalize() {
	float total = 0.0;
	for (int i = 0; i < m_iCount; i++) total += m_fWeight[i];
	for (int i = 0; i < m_iCount; i++) m_fWeight[i] /= total;
	return *this;
}

/**
* 运算符重载 =
*/
CVertexJoint& CVertexJoint::operator = (const CVertexJoint& joint) {
	m_iCount = joint.m_iCount;
	m_iJointId[0] = joint.m_iJointId[0];
	m_iJointId[1] = joint.m_iJointId[1];
	m_iJointId[2] = joint.m_iJointId[2];
	m_iJointId[3] = joint.m_iJointId[3];
	m_fWeight[0] = joint.m_fWeight[0];
	m_fWeight[1] = joint.m_fWeight[1];
	m_fWeight[2] = joint.m_fWeight[2];
	m_fWeight[3] = joint.m_fWeight[3];
	return *this;
}