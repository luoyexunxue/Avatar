//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CIKSolver.h"
#include <algorithm>
#include <cmath>

/**
* 默认构造函数
* @param joint 末端效应器关节节点
* @param affect 受影响的父关节数量
*/
CIKSolver::CIKSolver(SJoint* joint, int affect) {
	m_cTarget[0] = joint->worldMatrix[12];
	m_cTarget[1] = joint->worldMatrix[13];
	m_cTarget[2] = joint->worldMatrix[14];
	SIKNode node;
	node.joint = joint;
	node.constraint = 0;
	m_vecKinematicChain.push_back(node);
	joint = joint->parent;
	while (joint && affect--) {
		joint->jointIK = true;
		node.joint = joint;
		node.constraint = 0;
		node.localRot.FromMatrix(joint->localMatrix);
		node.worldRot.FromMatrix(joint->worldMatrix);
		m_vecKinematicChain.push_back(node);
		joint = joint->parent;
	}
}

/**
* 默认析构函数
*/
CIKSolver::~CIKSolver() {
	for (size_t i = 0; i < m_vecKinematicChain.size(); i++) {
		if (m_vecKinematicChain[i].constraint) {
			delete m_vecKinematicChain[i].constraint;
		}
	}
}

/**
* 设置目标位置
* @param target 目标位置(模型坐标系)
*/
void CIKSolver::SetTarget(const CVector3& target) {
	m_cTarget.SetValue(target);
}

/**
* 设置铰链关节约束
* @param joint 关节指针
* @param axis 旋转轴(关节局部坐标系)
* @param min 最小角度
* @param max 最大角度
*/
void CIKSolver::SetSwingLimit(SJoint* joint, const CVector3& axis, float min, float max) {
	for (size_t i = 1; i < m_vecKinematicChain.size(); i++) {
		if (m_vecKinematicChain[i].joint == joint) {
			if (!m_vecKinematicChain[i].constraint) m_vecKinematicChain[i].constraint = new SConstraint();
			m_vecKinematicChain[i].constraint->type = SWINGLIMIT;
			m_vecKinematicChain[i].constraint->minAngle = min;
			m_vecKinematicChain[i].constraint->maxAngle = max;
			m_vecKinematicChain[i].constraint->axis.SetValue(axis);
			m_vecKinematicChain[i].constraint->axis.Normalize();
			CVector3 direction = CVector3(&m_vecKinematicChain[i - 1].joint->localMatrix[12]).Normalize();
			CVector3 right = m_vecKinematicChain[i].constraint->axis.CrossProduct(direction);
			if (right.DotProduct(right) < 1E-6f) right = m_vecKinematicChain[i].constraint->axis.Tangent();
			CVector3 destination = right.CrossProduct(m_vecKinematicChain[i].constraint->axis);
			m_vecKinematicChain[i].constraint->original.SetValue(destination);
			m_vecKinematicChain[i].constraint->initial.SetValue(direction);
			m_vecKinematicChain[i].animationRot.FromVector(direction, destination);
			break;
		}
	}
}

/**
* 设置球状关节约束
* @param joint 关节指针
* @param axis 中心轴(关节局部坐标系)
* @param angle 最大偏转角度
*/
void CIKSolver::SetTwistLimit(SJoint* joint, const CVector3& axis, float angle) {
	for (size_t i = 1; i < m_vecKinematicChain.size(); i++) {
		if (m_vecKinematicChain[i].joint == joint) {
			if (!m_vecKinematicChain[i].constraint) m_vecKinematicChain[i].constraint = new SConstraint();
			m_vecKinematicChain[i].constraint->type = TWISTLIMIT;
			m_vecKinematicChain[i].constraint->minAngle = 0.0f;
			m_vecKinematicChain[i].constraint->maxAngle = angle;
			m_vecKinematicChain[i].constraint->axis.SetValue(axis);
			m_vecKinematicChain[i].constraint->axis.Normalize();
			CVector3 direction = CVector3(&m_vecKinematicChain[i - 1].joint->localMatrix[12]).Normalize();
			m_vecKinematicChain[i].constraint->original.SetValue(m_vecKinematicChain[i].constraint->axis);
			m_vecKinematicChain[i].constraint->initial.SetValue(direction);
			m_vecKinematicChain[i].animationRot.FromVector(direction, m_vecKinematicChain[i].constraint->axis);
			break;
		}
	}
}

/**
* 迭代解析
* @param maxStep 最大迭代次数
* @remark 采用CCD方法
*/
void CIKSolver::Solve(int maxStep) {
	for (size_t i = 0; i < m_vecKinematicChain.size(); i++) {
		SJoint* joint = m_vecKinematicChain[i].joint;
		joint->position[0] = joint->worldMatrix[12];
		joint->position[1] = joint->worldMatrix[13];
		joint->position[2] = joint->worldMatrix[14];
	}
	SJoint* endEffector = m_vecKinematicChain[0].joint;
	while (maxStep--) {
		for (size_t i = 1; i < m_vecKinematicChain.size(); i++) {
			SJoint* joint = m_vecKinematicChain[i].joint;
			CVector3 src = endEffector->position - joint->position;
			CVector3 dst = m_cTarget - joint->position;
			src.Normalize();
			dst.Normalize();
			// 将旋转轴变换至关节局部坐标系
			CQuaternion world2local = m_vecKinematicChain[i].animationRot * CQuaternion(m_vecKinematicChain[i].worldRot).Conjugate();
			CVector3 axis = world2local * src.CrossProduct(dst);
			float angle = acosf(std::min(std::max(src.DotProduct(dst), -1.0f), 1.0f));
			CQuaternion local_rotate;
			CQuaternion world_rotate;
			local_rotate.FromAxisAngle(axis, angle);
			if (m_vecKinematicChain[i].constraint) ConstraintSolve(i, local_rotate);
			m_vecKinematicChain[i].animationRot = local_rotate * m_vecKinematicChain[i].animationRot;
			m_vecKinematicChain[i].animationRot.Normalize();
			world_rotate.FromAxisAngle(world2local.Conjugate() * axis, angle);
			for (size_t j = 0; j < i; j++) {
				SJoint* child = m_vecKinematicChain[j].joint;
				child->position = world_rotate * (child->position - joint->position) + joint->position;
			}
		}
		// 更新Q_world = parent.Q_world * Q_local * Q_anim
		size_t idx = m_vecKinematicChain.size() - 1;
		if (m_vecKinematicChain[idx].joint->parent) {
			SJoint* parent = m_vecKinematicChain[idx].joint->parent;
			m_vecKinematicChain[idx].worldRot = CQuaternion().FromMatrix(parent->worldMatrix) * m_vecKinematicChain[idx].localRot;
		} else m_vecKinematicChain[idx].worldRot = m_vecKinematicChain[idx].localRot;
		m_vecKinematicChain[idx].worldRot *= m_vecKinematicChain[idx].animationRot;
		m_vecKinematicChain[idx].worldRot.Normalize();
		while (--idx > 0) {
			m_vecKinematicChain[idx].worldRot = m_vecKinematicChain[idx + 1].worldRot * m_vecKinematicChain[idx].localRot;
			m_vecKinematicChain[idx].worldRot *= m_vecKinematicChain[idx].animationRot;
			m_vecKinematicChain[idx].worldRot.Normalize();
		}
		// 检查是否非常接近目标
		CVector3 distance = m_cTarget - endEffector->position;
		if (distance.DotProduct(distance) < 0.001f) break;
	}
	for (size_t i = 0; i < m_vecKinematicChain.size(); i++) {
		size_t index = m_vecKinematicChain.size() - 1 - i;
		SJoint* joint = m_vecKinematicChain[index].joint;
		joint->worldMatrix.SetValue(joint->localMatrix * m_vecKinematicChain[index].animationRot.ToMatrix());
		if (joint->parent) joint->worldMatrix = joint->parent->worldMatrix * joint->worldMatrix;
	}
}

/**
* 约束计算
* @param index IK链数组下标
* @param rotation 局部坐标系下的旋转
* @remark 目前支持两种约束方式，通过对rotation参数进行修正以达到约束效果
*/
void CIKSolver::ConstraintSolve(size_t index, CQuaternion& rotation) {
	SConstraint* constraint = m_vecKinematicChain[index].constraint;
	if (constraint->type == SWINGLIMIT) {
		CQuaternion axis_rotate;
		CVector3 current = m_vecKinematicChain[index].animationRot * constraint->initial;
		CVector3 right = constraint->axis.CrossProduct(rotation * current);
		CVector3 project = right.CrossProduct(constraint->axis);
		float angle = acosf(std::max(std::min(constraint->original.DotProduct(project), 1.0f), -1.0f));
		if (constraint->original.CrossProduct(project).DotProduct(constraint->axis) < 0.0f) angle = -angle;
		angle = std::min(std::max(angle, constraint->minAngle), constraint->maxAngle);
		axis_rotate.FromAxisAngle(constraint->axis, angle);
		rotation.FromVector(current, axis_rotate * constraint->original);
	} else if (constraint->type == TWISTLIMIT) {
		CQuaternion axis_rotate;
		CVector3 current = m_vecKinematicChain[index].animationRot * constraint->initial;
		axis_rotate.FromVector(constraint->axis, rotation * current);
		float angle = acosf(axis_rotate[3]) * 2.0f;
		angle = std::min(angle, constraint->maxAngle);
		axis_rotate.FromAxisAngle(CVector3(axis_rotate.m_fValue), angle);
		rotation.FromVector(current, axis_rotate * constraint->original);
		m_vecKinematicChain[index].animationRot.FromVector(constraint->initial, current);
	}
}