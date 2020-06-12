//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CCONTACTDETECTOR_H_
#define _CCONTACTDETECTOR_H_
#include "CRigidBody.h"
#include "CVector3.h"

/**
* @brief 接触点
*/
struct SContact {
	//! 碰撞点
	CVector3 point;
	//! 指向刚体1的碰撞法向
	CVector3 normal;
	//! 沿刚体2的碰撞切向
	CVector3 tangent;
	//! 刚体1到接触点向量
	CVector3 rel1;
	//! 刚体2到接触点向量
	CVector3 rel2;
	//! 接触深度(负值)
	float penetration;

	//! 法向方向质量当量
	float massNormal;
	//! 切向方向质量当量
	float massTangent;
	//! 接触深度产生的速度
	float biasVelocity;
	//! 法向冲量累积
	float accumulatedNormalImpulse;
	//! 切向冲量累积
	float accumulatedTangentImpulse;
};

/**
* @brief 刚体接触信息
*/
struct SArbiter {
	//! 刚体1
	CRigidBody* body1;
	//! 刚体2
	CRigidBody* body2;
	//! 恢复系数
	float restitution;
	//! 摩擦系数
	float friction;
	//! 接触点数量
	int numContacts;
	//! 接触点列表
	SContact contact[8];
};

/**
* @brief 碰撞接触检测器
*/
class CContactDetector {
public:
	//! 刚体接触检测
	static bool Detect(const CRigidBody* body1, const CRigidBody* body2, SArbiter* arbiter);

private:
	//! 计算盒体与盒体
	static int BoxBoxTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! 计算盒体与球体
	static int BoxSphereTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! 计算盒体与平面
	static int BoxPlaneTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! 计算球体与球体
	static int SphereSphereTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! 计算球体与胶囊体
	static int SphereCapsuleTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! 计算球体与圆柱体
	static int SphereCylinderTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! 计算球体与圆环
	static int SphereTorusTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! 计算球体与圆锥体
	static int SphereConeTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! 计算球体与平面
	static int SpherePlaneTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! 计算胶囊体与胶囊体
	static int CapsuleCapsuleTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! 计算胶囊体与平面
	static int CapsulePlaneTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! 计算圆柱体与平面
	static int CylinderPlaneTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! 计算圆环与平面
	static int TorusPlaneTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! 计算圆锥体与平面
	static int ConePlaneTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
};

#endif