//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CRIGIDBODY_H_
#define _CRIGIDBODY_H_
#include "AvatarConfig.h"
#include "CSceneNode.h"
#include "CGeometryCreator.h"
#include "CQuaternion.h"
#include "CMatrix4.h"

/**
* @brief 刚体类
*/
class AVATAR_EXPORT CRigidBody {
public:
	//! 构造刚体并绑定场景节点
	CRigidBody(CSceneNode* binding);

	//! 获取绑定的场景节点
	CSceneNode* GetSceneNode() const;

	//! 设置物体属性
	void SetAttribute(float mass, const SGeometry& geometry, bool trigger, bool allowSleep);
	//! 设置阻尼系数
	void SetDamping(float linearDamping, float angularDamping);
	//! 设置摩擦系数
	void SetFriction(float friction);
	//! 设置恢复系数
	void SetRestitution(float restitution);
	//! 设置初速度
	void SetVelocity(const CVector3& linearVel, const CVector3& angularVel);

	//! 复位刚体状态
	void Reset();
	//! 清除所有外力
	void ClearForce();
	//! 施加外力
	void ApplyForce(const CVector3& force, bool local);
	//! 施加外力和外力矩
	void ApplyForce(const CVector3& force, const CVector3& torque, bool local);

	//! 获取刚体几何
	inline const SGeometry& GetGeometry() const { return m_sGeometry; }
	//! 刚体碰撞回调
	inline int& CollideCallback() { return m_iCollideCallback; }

private:
	//! 检查是否需要睡眠
	bool CheckSleeping(float dt);
	//! 计算惯性张量
	void SetupInertiaTensor();

private:
	//! 力的总和
	CVector3 m_cTotalForce;
	//! 加速度
	CVector3 m_cAcceleration;
	//! 线速度
	CVector3 m_cLinearVelocity;
	//! 力矩的总和
	CVector3 m_cTotalTorque;
	//! 角速度
	CVector3 m_cAngularVelocity;
	//! 额外的力
	CVector3 m_cExtraForce;
	//! 额外的力矩
	CVector3 m_cExtraTorque;
	//! 额外的局部力
	CVector3 m_cExtraForceLocal;
	//! 额外的局部力矩
	CVector3 m_cExtraTorqueLocal;
	//! 旋转后的惯性张量逆矩阵
	CMatrix4 m_cInvInertiaTensorWorld;

	//! 线速度阻尼
	float m_fLinearDamping;
	//! 角速度阻尼
	float m_fAngularDamping;
	//! 物体质量
	float m_fMass;
	//! 物体质量倒数
	float m_fInvMass;
	//! 转动惯量
	CVector3 m_cInertiaLocal;
	//! 逆转动惯量
	CVector3 m_cInvInertiaLocal;
	//! 摩擦系数
	float m_fFriction;
	//! 恢复系数
	float m_fRestitution;
	//! 是否是静态物体
	bool m_bStatic;
	//! 是否触发器
	bool m_bIsTrigger;
	//! 是否允许休眠
	bool m_bAllowSleep;
	//! 刚体几何定义
	SGeometry m_sGeometry;

	//! 物体休眠状态
	bool m_bSleeping;
	//! 进入休眠的时间
	float m_fSleepingTime;
	//! 睡眠时的合力
	CVector3 m_cSleepingForce;
	//! 睡眠时的合力矩
	CVector3 m_cSleepingTorque;

	//! 碰撞回调
	int m_iCollideCallback;
	//! 绑定的场景节点
	CSceneNode* m_pSceneNode;
	//! CPhysicsManager 可访问
	friend class CPhysicsManager;
};

#endif