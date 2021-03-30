//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPhysicsManager.h"
#include "CEngine.h"
#include "CLog.h"
#include <algorithm>

/**
* 构造函数
*/
CPhysicsManager::CPhysicsManager(): m_fTimeStep(1.0f / 100.0f) {
	m_pDynamicBvt = new CDynamicBvt();
}

/**
* 析构函数
*/
CPhysicsManager::~CPhysicsManager() {
	m_pInstance = 0;
}

/**
* 单例实例
*/
CPhysicsManager* CPhysicsManager::m_pInstance = 0;

/**
* 销毁物理管理器
*/
void CPhysicsManager::Destroy() {
	list<CRigidBody*>::iterator iter = m_lstRigidBody.begin();
	while (iter != m_lstRigidBody.end()) {
		delete *iter;
		++iter;
	}
	m_lstRigidBody.clear();
	delete m_pDynamicBvt;
	delete this;
}

/**
* 物理模拟更新
* @param dt 时间步长
*/
void CPhysicsManager::Update(float dt) {
	if (!m_lstRigidBody.empty()) {
		// 上次剩余时间
		static float lastTimeRemain = 0.0f;
		dt += lastTimeRemain;
		// 需要分解执行的步数
		int steps = static_cast<int>(dt / m_fTimeStep);
		for (int i = 0; i < steps; i++) {
			IntegrateForce();
			CollisionDetect();
			SolveConstraint();
			IntegrateVelocity();
		}
		// 剩余时间放在下次执行
		lastTimeRemain = dt - steps * m_fTimeStep;
	}
}

/**
* 添加刚体
* @param body 刚体对象指针
*/
void CPhysicsManager::AddRigidBody(CRigidBody* body) {
	list<CRigidBody*>::iterator iter = m_lstRigidBody.begin();
	while (iter != m_lstRigidBody.end()) {
		if (body == *iter) {
			CLog::Warn("Rigid body already exist");
			return;
		}
		++iter;
	}
	m_lstRigidBody.push_back(body);
	if (body->m_sGeometry.shape != SGeometry::UNKNOWN) {
		m_pDynamicBvt->Insert(body, body->m_pSceneNode->GetBoundingBox());
	}
}

/**
* 删除刚体
* @param body 刚体对象指针
*/
void CPhysicsManager::DelRigidBody(CRigidBody* body) {
	list<CRigidBody*>::iterator iter = m_lstRigidBody.begin();
	while (iter != m_lstRigidBody.end()) {
		if (body == *iter) {
			m_lstRigidBody.erase(iter);
			m_pDynamicBvt->Delete(body);
			map<size_t, SCollideInfo>::iterator cur = m_mapCollide.begin();
			while (cur != m_mapCollide.end()) {
				if (cur->second.body1 == body || cur->second.body2 == body) m_mapCollide.erase(cur++);
				else ++cur;
			}
			delete body;
			return;
		}
		++iter;
	}
	CLog::Warn("Rigid body not exist");
}

/**
* 清除所有刚体
*/
void CPhysicsManager::ClearRigidBody() {
	list<CRigidBody*>::iterator iter = m_lstRigidBody.begin();
	while (iter != m_lstRigidBody.end()) {
		delete (*iter);
		++iter;
	}
	m_mapCollide.clear();
	m_lstRigidBody.clear();
	m_pDynamicBvt->Clear();
}

/**
* 通过绑定的场景节点获取刚体
* @param node 场景节点
* @return 刚体对象指针，若没有绑定对应刚体则返回 null
*/
CRigidBody* CPhysicsManager::GetRigidBody(CSceneNode* node) {
	list<CRigidBody*>::iterator iter = m_lstRigidBody.begin();
	while (iter != m_lstRigidBody.end()) {
		if ((*iter)->m_pSceneNode == node) return *iter;
		++iter;
	}
	return 0;
}

/**
* 获取管理的所有刚体列表
* @param rigidList 返回的刚体列表
*/
void CPhysicsManager::GetRigidBodyList(vector<CRigidBody*>& rigidList) {
	rigidList.resize(m_lstRigidBody.size());
	list<CRigidBody*>::iterator iter = m_lstRigidBody.begin();
	int index = 0;
	while (iter != m_lstRigidBody.end()) {
		rigidList[index++] = *iter;
		++iter;
	}
}

/**
* 设置重力加速度
* @param gravity 重力加速度矢量
*/
void CPhysicsManager::SetGravity(const CVector3& gravity) {
	m_cGravity.SetValue(gravity);
}

/**
* 获取重力加速度
* @return 重力加速度
*/
CVector3 CPhysicsManager::GetGravity() {
	return m_cGravity;
}

/**
* 施加力到刚体重心
* @param body 刚体
* @param force 力矢量
* @attention force 方向为世界坐标系
*/
void CPhysicsManager::ApplyForce(CRigidBody* body, const CVector3& force) {
	if (!body->m_bStatic) {
		body->ApplyForce(force, false, false);
	}
}

/**
* 施加力到刚体
* @param body 刚体
* @param force 力矢量
* @param relPos 受力点
* @attention force, relPos 为刚体局部坐标系
*/
void CPhysicsManager::ApplyForce(CRigidBody* body, const CVector3& force, const CVector3& relPos) {
	if (!body->m_bStatic) {
		body->ApplyForce(force, relPos.CrossProduct(force), true, false);
	}
}

/**
* 施加冲量到刚体重心
* @param body 施加冲量的刚体
* @param impulse 冲量
* @attention impulse 为世界坐标系下冲量
*/
void CPhysicsManager::ApplyImpulse(CRigidBody* body, const CVector3& impulse) {
	if (!body->m_bStatic) {
		body->m_cLinearVelocity += impulse * body->m_fInvMass;
		body->m_bSleeping = false;
	}
}

/**
* 施加冲量到刚体
* @param body 施加冲量的刚体
* @param impulse 冲量
* @param pos 施加冲量的位置
* @attention impulse, pos 为世界坐标系下冲量
*/
void CPhysicsManager::ApplyImpulse(CRigidBody* body, const CVector3& impulse, const CVector3& pos) {
	if (!body->m_bStatic) {
		CVector3 relPos = pos - body->m_pSceneNode->GetWorldPosition();
		body->m_cLinearVelocity += impulse * body->m_fInvMass;
		body->m_cAngularVelocity += body->m_cInvInertiaTensorWorld * relPos.CrossProduct(impulse);
		body->m_bSleeping = false;
	}
}

/**
* 对力积分，更新速度
*/
void CPhysicsManager::IntegrateForce() {
	list<CRigidBody*>::iterator iter = m_lstRigidBody.begin();
	while (iter != m_lstRigidBody.end()) {
		CRigidBody* pBody = *iter;
		++iter;
		// 跳过静止物体
		if (pBody->m_bStatic) continue;

		// 施加外力和外力矩作用
		CQuaternion& orientation = pBody->m_pSceneNode->m_cOrientation;
		pBody->m_cTotalForce = pBody->m_cExtraForce;
		pBody->m_cTotalForce += orientation * pBody->m_cExtraForceLocal;
		pBody->m_cTotalForce += m_cGravity * pBody->m_fMass;
		pBody->m_cTotalForce += pBody->m_cLinearVelocity * pBody->m_fLinearDamping;
		pBody->m_cTotalTorque = pBody->m_cExtraTorque;
		pBody->m_cTotalTorque = orientation * pBody->m_cExtraTorqueLocal;
		pBody->m_cTotalTorque += pBody->m_cAngularVelocity * pBody->m_fAngularDamping;
		// 陀螺力矩计算，即求解方程(角动量守恒) dL = I * (ω2 - ω1) + (ω2 × I * ω2) * dt = 0
		// 使用 Newton-Raphson 方法求解新的角速度，x[k+1] = x[k] - Jinv(x[k]) * F(x[k]) 其中 J(x) = dF(x)
		CMatrix4 inertiaTensorLocal;
		inertiaTensorLocal(0, 0) = pBody->m_cInertiaLocal[0];
		inertiaTensorLocal(1, 1) = pBody->m_cInertiaLocal[1];
		inertiaTensorLocal(2, 2) = pBody->m_cInertiaLocal[2];
		CVector3 angularVelocity = CQuaternion(orientation).Conjugate() * pBody->m_cAngularVelocity;
		CVector3 angularMomentum = inertiaTensorLocal * angularVelocity;
		CMatrix4 skew1;
		CMatrix4 skew2;
		skew1.SkewSymmetric(angularVelocity);
		skew2.SkewSymmetric(angularMomentum);
		CMatrix4 jacobian = inertiaTensorLocal + (skew1 * inertiaTensorLocal - skew2) * m_fTimeStep;
		CVector3 fangular = angularVelocity.CrossProduct(angularMomentum) * m_fTimeStep;
		angularVelocity -= jacobian.Invert() * fangular;
		pBody->m_cAngularVelocity = orientation * angularVelocity;

		// 检查是否需要休眠
		if (!pBody->CheckSleeping(m_fTimeStep)) {
			// 新的线速度
			CVector3 acceleration = pBody->m_cTotalForce * pBody->m_fInvMass;
			pBody->m_cLinearVelocity += (pBody->m_cAcceleration + acceleration) * m_fTimeStep * 0.5f;
			pBody->m_cAcceleration = acceleration;
			// 新的角速度
			CVector3 angularMomentum = pBody->m_cTotalTorque * m_fTimeStep;
			pBody->m_cAngularVelocity += pBody->m_cInvInertiaTensorWorld * angularMomentum;
		}
	}
}

/**
* 对速度积分，更新位置
*/
void CPhysicsManager::IntegrateVelocity() {
	list<CRigidBody*>::iterator iter = m_lstRigidBody.begin();
	while (iter != m_lstRigidBody.end()) {
		CRigidBody* pBody = *iter;
		++iter;
		// 跳过休眠的物体
		if (pBody->m_bSleeping) continue;
		// 更新非静止刚体的位置和方向
		CSceneNode* pNode = pBody->m_pSceneNode;
		if (!pBody->m_bStatic) {
			CQuaternion rotation(pBody->m_cAngularVelocity[0], pBody->m_cAngularVelocity[1], pBody->m_cAngularVelocity[2], 0);
			rotation *= 0.5f * m_fTimeStep;
			rotation *= pNode->m_cOrientation;
			pNode->m_cOrientation += rotation;
			pNode->m_cOrientation.Normalize();
			pNode->m_cPosition += pBody->m_cLinearVelocity * m_fTimeStep + pBody->m_cAcceleration * (m_fTimeStep * m_fTimeStep * 0.5f);
			pNode->Transform();
			// 通过当前方向更新世界坐标系下的惯性张量
			// L-world = I-world * ω-world = O * I-local * (O-1 * ω-world) => I-world = O * I-local * OT
			CMatrix4 orientMatrix = pNode->m_cOrientation.ToMatrix();
			pBody->m_cInvInertiaTensorWorld = orientMatrix;
			pBody->m_cInvInertiaTensorWorld.SetScaled(pBody->m_cInvInertiaLocal);
			pBody->m_cInvInertiaTensorWorld *= orientMatrix.Transpose();
		}
		// 更新层次包围盒
		m_pDynamicBvt->Update(pBody, pNode->GetBoundingBox());
	}
}

/**
* 碰撞检测
*/
void CPhysicsManager::CollisionDetect() {
	// 清除之前的接触对
	m_lstArbiterList.clear();
	// 使用 DBVT 进行粗略检查
	m_pDynamicBvt->CollisionPair(m_vecCollidePair);
	// 对每对可能的碰撞进行精确计算得到接触点
	for (size_t i = 0; i < m_vecCollidePair.size(); i++) {
		CDynamicBvt::SCollidePair& pair = m_vecCollidePair[i];
		CRigidBody* bodyA = static_cast<CRigidBody*>(pair.userData1);
		CRigidBody* bodyB = static_cast<CRigidBody*>(pair.userData2);
		// 跳过都在休眠状态的物体
		if (bodyA->m_bSleeping && bodyB->m_bSleeping) continue;
		// 接触检测
		SArbiter arbiter;
		if (CContactDetector::Detect(bodyA, bodyB, &arbiter)) {
			// 唤醒休眠中的物体
			bodyA->m_bSleeping = bodyA->m_bStatic && bodyA->m_bAllowSleep;
			bodyB->m_bSleeping = bodyB->m_bStatic && bodyB->m_bAllowSleep;
			// 计算恢复系数和摩擦系数
			arbiter.restitution = std::min(bodyA->m_fRestitution, bodyB->m_fRestitution);
			arbiter.friction = std::min(bodyA->m_fFriction, bodyB->m_fFriction);
			m_lstArbiterList.push_back(arbiter);
		}
	}
	// 碰撞事件回调
	CollideEvent();
}

/**
* 处理约束
*/
void CPhysicsManager::SolveConstraint() {
	// 迭代次数
	const int iteration = 10;
	// 预处理接触点
	list<SArbiter>::iterator iter = m_lstArbiterList.begin();
	while (iter != m_lstArbiterList.end()) PreSolve(*iter++);
	// 迭代冲量求解
	for (int i = 0; i < iteration; i++) {
		iter = m_lstArbiterList.begin();
		while (iter != m_lstArbiterList.end()) Solve(*iter++);
	}
}

/**
* 预处理接触点约束
* @param arbiter 刚体接触信息
*/
void CPhysicsManager::PreSolve(SArbiter& arbiter) {
	// 常量定义
	const float allowedPenetration = -0.01f;
	const float resutitutionSlop = -0.1f;
	const float biasFactor = 0.25f;

	CRigidBody* body1 = arbiter.body1;
	CRigidBody* body2 = arbiter.body2;
	for (int i = 0; i < arbiter.numContacts; i++) {
		SContact* c = &arbiter.contact[i];

		// 计算接触点切向速度方向
		CVector3 v1 = body1->m_cLinearVelocity + body1->m_cAngularVelocity.CrossProduct(c->rel1);
		CVector3 v2 = body2->m_cLinearVelocity + body2->m_cAngularVelocity.CrossProduct(c->rel2);
		CVector3 vr = v1 - v2;
		float vrn = vr.DotProduct(c->normal);
		c->tangent = (c->normal * vrn - vr).Normalize();
		if (c->tangent[0] == 0.0f && c->tangent[1] == 0.0f && c->tangent[2] == 0.0f) {
			c->tangent.SetValue(1.0f, 0.0f, 0.0f);
		}
		// 碰撞法向质量当量
		CVector3 r1xn = c->rel1.CrossProduct(c->normal);
		CVector3 r2xn = c->rel2.CrossProduct(c->normal);
		float n1 = r1xn.DotProduct(body1->m_cInvInertiaTensorWorld * r1xn);
		float n2 = r2xn.DotProduct(body2->m_cInvInertiaTensorWorld * r2xn);
		c->massNormal = 1.0f / (body1->m_fInvMass + body2->m_fInvMass + n1 + n2);

		// 碰撞切向质量当量
		CVector3 r1xt = c->rel1.CrossProduct(c->tangent);
		CVector3 r2xt = c->rel2.CrossProduct(c->tangent);
		float t1 = r1xt.DotProduct(body1->m_cInvInertiaTensorWorld * r1xt);
		float t2 = r2xt.DotProduct(body2->m_cInvInertiaTensorWorld * r2xt);
		c->massTangent = 1.0f / (body1->m_fInvMass + body2->m_fInvMass + t1 + t2);

		// 接触深度和碰撞恢复产生的速度
		// 恢复系数 restitution 定义 e = -v2/v1
		c->biasVelocity = std::min(c->penetration - allowedPenetration, 0.0f) * (biasFactor / m_fTimeStep);
		c->biasVelocity += std::min(vrn - resutitutionSlop, 0.0f) * arbiter.restitution;

		// 累加冲量清零
		c->accumulatedNormalImpulse = 0.0f;
		c->accumulatedTangentImpulse = 0.0f;
	}
}

/**
* 处理接触点约束
* @param arbiter 刚体接触信息
*/
void CPhysicsManager::Solve(SArbiter& arbiter) {
	CRigidBody* body1 = arbiter.body1;
	CRigidBody* body2 = arbiter.body2;
	for (int i = 0; i < arbiter.numContacts; i++) {
		// J = (vr * n){-(e + 1) / {1/m1 + 1/m2 + n * [(r1 × n) / I1] × r1 + n * [(r2 × n) / I2] × r2}}
		SContact* c = &arbiter.contact[i];

		// 两个刚体在接触点的相对速度
		CVector3 v1 = body1->m_cLinearVelocity + body1->m_cAngularVelocity.CrossProduct(c->rel1);
		CVector3 v2 = body2->m_cLinearVelocity + body2->m_cAngularVelocity.CrossProduct(c->rel2);
		CVector3 vr = v1 - v2;

		// 接触点法向和切向相对速度
		float vrn = vr.DotProduct(c->normal);
		float vrt = vr.DotProduct(c->tangent);

		// 法向和切向冲量大小
		float pn = -c->massNormal * (vrn + c->biasVelocity);
		float pt = -c->massTangent * vrt;

		// 对冲量大小限制，参考 Box2D
		float tmpn = c->accumulatedNormalImpulse;
		c->accumulatedNormalImpulse = tmpn + pn;
		if (c->accumulatedNormalImpulse < 0.0f) c->accumulatedNormalImpulse = 0.0f;
		pn = c->accumulatedNormalImpulse - tmpn;

		// 切向最大冲量
		const float maxPt = arbiter.friction * c->accumulatedNormalImpulse;

		float tmpt = c->accumulatedTangentImpulse;
		c->accumulatedTangentImpulse = tmpt + pt;
		if (c->accumulatedTangentImpulse < -maxPt) c->accumulatedTangentImpulse = -maxPt;
		else if (c->accumulatedTangentImpulse > maxPt) c->accumulatedTangentImpulse = maxPt;
		pt = c->accumulatedTangentImpulse - tmpt;

		// 对刚体1和刚体2分别施加冲量
		CVector3 impulse = c->normal * pn + c->tangent * pt;
		body1->m_cLinearVelocity += impulse * body1->m_fInvMass;
		body1->m_cAngularVelocity += body1->m_cInvInertiaTensorWorld * c->rel1.CrossProduct(impulse);
		body2->m_cLinearVelocity -= impulse * body2->m_fInvMass;
		body2->m_cAngularVelocity -= body2->m_cInvInertiaTensorWorld * c->rel2.CrossProduct(impulse);
	}
}

/**
* 处理碰撞事件
*/
void CPhysicsManager::CollideEvent() {
	// 标记全部碰撞为无效
	map<size_t, SCollideInfo>::iterator iter = m_mapCollide.begin();
	while (iter != m_mapCollide.end()) {
		iter->second.enter = false;
		++iter;
	}
	// 遍历全部接触列表
	CScriptManager* pScriptMgr = CEngine::GetScriptManager();
	list<SArbiter>::iterator it = m_lstArbiterList.begin();
	while (it != m_lstArbiterList.end()) {
		SArbiter& arbiter = *it++;
		if (arbiter.body1->m_bIsTrigger || arbiter.body2->m_bIsTrigger) {
			arbiter.numContacts = 0;
		}
		const int callback1 = arbiter.body1->CollideCallback();
		const int callback2 = arbiter.body2->CollideCallback();
		if (callback1 < 0 && callback2 < 0) continue;
		size_t body1 = reinterpret_cast<size_t>(arbiter.body1);
		size_t body2 = reinterpret_cast<size_t>(arbiter.body2);
		size_t key = (body1 << 4) + body2;
		iter = m_mapCollide.find(key);
		if (iter != m_mapCollide.end()) {
			iter->second.enter = true;
			continue;
		}
		SCollideInfo info;
		info.enter = true;
		info.body1 = arbiter.body1;
		info.body2 = arbiter.body2;
		m_mapCollide.insert(std::pair<size_t, SCollideInfo>(key, info));
		// 脚本回调
		if (callback1 >= 0) {
			CSceneNode* node = arbiter.body1->m_pSceneNode;
			pScriptMgr->CollideEnter(callback1, node->GetName(), arbiter.contact[0].point);
		}
		if (callback2 >= 0) {
			CSceneNode* node = arbiter.body2->m_pSceneNode;
			pScriptMgr->CollideEnter(callback2, node->GetName(), arbiter.contact[0].point);
		}
	}
	// 移除无效的碰撞
	iter = m_mapCollide.begin();
	while (iter != m_mapCollide.end()) {
		if (!iter->second.enter) {
			const int callback1 = iter->second.body1->CollideCallback();
			const int callback2 = iter->second.body2->CollideCallback();
			if (callback1 >= 0) pScriptMgr->CollideLeave(callback1, iter->second.body1->m_pSceneNode->GetName());
			if (callback2 >= 0) pScriptMgr->CollideLeave(callback2, iter->second.body2->m_pSceneNode->GetName());
			m_mapCollide.erase(iter++);
		} else ++iter;
	}
}