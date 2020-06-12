//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CRigidBody.h"

/**
* 构造函数，绑定场景节点
*/
CRigidBody::CRigidBody(CSceneNode* binding) {
	m_pSceneNode = binding;
	m_bSleeping = false;
	m_fSleepingTime = 0.0f;
	m_fLinearDamping = 0;
	m_fAngularDamping = 0;
	m_fMass = 1;
	m_fInvMass = 1;
	m_fFriction = 0.5f;
	m_fRestitution = 0.5f;
	m_bStatic = false;
	m_bIsTrigger = false;
	m_bAllowSleep = true;
	m_iCollideCallback = -1;
}

/**
* 获取绑定的场景节点
*/
CSceneNode* CRigidBody::GetSceneNode() const {
	return m_pSceneNode;
}

/**
* 设置物体属性
*/
void CRigidBody::SetAttribute(float mass, const SGeometry& geometry, bool trigger, bool allowSleep) {
	// 质量及形状属性
	m_fMass = mass;
	m_fInvMass = mass <= 0.0f ? 0.0f : 1.0f / mass;
	m_bStatic = mass <= 0.0f ? true : false;
	m_sGeometry = geometry;
	m_bIsTrigger = trigger;
	m_bAllowSleep = allowSleep;
	m_bSleeping = m_bStatic && m_bAllowSleep;
	m_fSleepingTime = 0.0f;
	// 计算惯性张量
	SetupInertiaTensor();
	// 计算世界坐标系下的惯性张量
	CMatrix4 orientMatrix = m_pSceneNode->m_cOrientation.ToMatrix();
	m_cInvInertiaTensorWorld = orientMatrix;
	m_cInvInertiaTensorWorld.SetScaled(m_cInvInertiaLocal);
	m_cInvInertiaTensorWorld *= orientMatrix.Transpose();
	// 平面需要特殊处理
	if (m_sGeometry.shape == SGeometry::PLANE) {
		m_pSceneNode->m_cLocalBoundingBox.m_cMin[2] = -1000.0f;
	}
}

/**
* 设置阻尼系数
*/
void CRigidBody::SetDamping(float linearDamping, float angularDamping) {
	m_fLinearDamping = linearDamping;
	m_fAngularDamping = angularDamping;
}

/**
* 设置摩擦系数
*/
void CRigidBody::SetFriction(float friction) {
	m_fFriction = friction;
}

/**
* 设置恢复系数
*/
void CRigidBody::SetRestitution(float restitution) {
	m_fRestitution = restitution;
}

/**
* 设置初速度
*/
void CRigidBody::SetVelocity(const CVector3& linearVel, const CVector3& angularVel) {
	m_cLinearVelocity.SetValue(linearVel.m_fValue);
	m_cAngularVelocity.SetValue(angularVel.m_fValue);
	m_bSleeping = m_bStatic && m_bAllowSleep;
	m_fSleepingTime = 0.0f;
}

/**
* 复位刚体状态
*/
void CRigidBody::Reset() {
	m_cTotalForce.SetValue(0, 0, 0);
	m_cAcceleration.SetValue(0, 0, 0);
	m_cLinearVelocity.SetValue(0, 0, 0);
	m_cTotalTorque.SetValue(0, 0, 0);
	m_cAngularVelocity.SetValue(0, 0, 0);
	m_cExtraForce.SetValue(0, 0, 0);
	m_cExtraTorque.SetValue(0, 0, 0);
	m_cExtraForceLocal.SetValue(0, 0, 0);
	m_cExtraTorqueLocal.SetValue(0, 0, 0);
	m_bSleeping = m_bStatic && m_bAllowSleep;
	m_fSleepingTime = 0.0f;
}

/**
* 清除所有外力
*/
void CRigidBody::ClearForce() {
	m_cExtraForce.SetValue(0, 0, 0);
	m_cExtraTorque.SetValue(0, 0, 0);
	m_cExtraForceLocal.SetValue(0, 0, 0);
	m_cExtraTorqueLocal.SetValue(0, 0, 0);
}

/**
* 施加外力
*/
void CRigidBody::ApplyForce(const CVector3& force, bool local) {
	if (local) m_cExtraForceLocal.SetValue(force);
	else m_cExtraForce.SetValue(force);
}

/**
* 施加外力和外力矩
*/
void CRigidBody::ApplyForce(const CVector3& force, const CVector3& torque, bool local) {
	if (local) {
		m_cExtraForceLocal.SetValue(force);
		m_cExtraTorqueLocal.SetValue(torque);
	} else {
		m_cExtraForce.SetValue(force);
		m_cExtraTorque.SetValue(torque);
	}
}

/**
* 检查是否需要睡眠，返回休眠状态
*/
bool CRigidBody::CheckSleeping(float dt) {
	// 常量定义
	const float sleepTimeElapse = 2.0f;
	const float sleepLinearVelocity = 0.002f;
	const float sleepAngularVelocity = 0.002f;
	if (!m_bAllowSleep) {
		m_bSleeping = false;
		return m_bSleeping;
	}
	float vl2 = m_cLinearVelocity.DotProduct(m_cLinearVelocity);
	float va2 = m_cAngularVelocity.DotProduct(m_cAngularVelocity);
	if (m_bSleeping) {
		if (vl2 > sleepLinearVelocity || va2 > sleepAngularVelocity
			|| m_cSleepingForce != m_cTotalForce || m_cSleepingTorque != m_cTotalTorque) {
			m_bSleeping = false;
			m_fSleepingTime = 0.0f;
		}
	} else if (vl2 <= sleepLinearVelocity && va2 <= sleepAngularVelocity) {
		m_fSleepingTime += dt;
		if (m_fSleepingTime > sleepTimeElapse) {
			m_bSleeping = true;
			m_cSleepingForce = m_cTotalForce;
			m_cSleepingTorque = m_cTotalTorque;
		}
	} else m_fSleepingTime = 0.0f;
	return m_bSleeping;
}

/**
* 计算刚体的惯性张量
*/
void CRigidBody::SetupInertiaTensor() {
	// XYZ 三个坐标轴下的转动惯量
	float Ix = m_fMass;
	float Iy = m_fMass;
	float Iz = m_fMass;
	float sq1, sq2, sq3, len, vs, vc, ms, mc;
	switch (m_sGeometry.shape) {
	case SGeometry::BOX:
		sq1 = m_sGeometry.box.x * m_sGeometry.box.x;
		sq2 = m_sGeometry.box.y * m_sGeometry.box.y;
		sq3 = m_sGeometry.box.z * m_sGeometry.box.z;
		Ix = (1.0f / 12.0f) * m_fMass * (sq2 + sq3);
		Iy = (1.0f / 12.0f) * m_fMass * (sq1 + sq3);
		Iz = (1.0f / 12.0f) * m_fMass * (sq1 + sq2);
		break;
	case SGeometry::SPHERE:
		sq1 = m_sGeometry.sphere.r * m_sGeometry.sphere.r;
		Ix = (2.0f / 5.0f) * m_fMass * sq1;
		Iy = Ix;
		Iz = Ix;
		break;
	case SGeometry::CAPSULE:
		sq1 = m_sGeometry.capsule.r * m_sGeometry.capsule.r;
		len = m_sGeometry.capsule.h - 2.0f * m_sGeometry.capsule.r;
		vs = (4.0f / 3.0f) * 3.14159f * m_sGeometry.capsule.r * sq1;
		vc = 3.14159f * sq1 * len;
		mc = m_fMass * vc / (vs + vc);
		ms = m_fMass * vs / (vs + vc);
		Ix = (1.0f / 4.0f) * mc * sq1 + (1.0f / 12.0f) * mc * len * len;
		Ix += (2.0f / 5.0f) * ms * sq1 + (3.0f / 8.0f) * ms * m_sGeometry.capsule.r * len;
		Ix += (1.0f / 4.0f) * ms * len * len;
		Iy = Ix;
		Iz = (1.0f / 2.0f) * mc * sq1 + (2.0f / 5.0f) * ms * sq1;
		break;
	case SGeometry::CYLINDER:
		sq1 = m_sGeometry.cylinder.r * m_sGeometry.cylinder.r;
		sq2 = m_sGeometry.cylinder.h * m_sGeometry.cylinder.h;
		Ix = (1.0f / 12.0f) * m_fMass * sq2 + (1.0f / 4.0f) * m_fMass * sq1;
		Iy = Ix;
		Iz = (1.0f / 2.0f) * m_fMass * sq1;
		break;
	case SGeometry::TORUS:
		sq1 = m_sGeometry.torus.r * m_sGeometry.torus.r;
		sq2 = m_sGeometry.torus.c * m_sGeometry.torus.c;
		Ix = (5.0f / 8.0f) * m_fMass * sq2 + (1.0f / 2.0f) * m_fMass * sq1;
		Iy = Ix;
		Iz = (3.0f / 4.0f) * m_fMass * sq2 + m_fMass * sq1;
		break;
	case SGeometry::CONE:
		sq1 = m_sGeometry.cone.r * m_sGeometry.cone.r;
		sq2 = m_sGeometry.cone.h * m_sGeometry.cone.h;
		Ix = (3.0f / 20.0f) * m_fMass * sq1 + (3.0f / 80.0f) * m_fMass * sq2;
		Iy = Ix;
		Iz = (3.0f / 10.0f) * m_fMass * sq1;
		break;
	default: break;
	}
	m_cInertiaLocal[0] = Ix;
	m_cInertiaLocal[1] = Iy;
	m_cInertiaLocal[2] = Iz;
	m_cInvInertiaLocal[0] = Ix != 0.0f? 1.0f / Ix: 0.0f;
	m_cInvInertiaLocal[1] = Iy != 0.0f? 1.0f / Iy: 0.0f;
	m_cInvInertiaLocal[2] = Iz != 0.0f? 1.0f / Iz: 0.0f;
}