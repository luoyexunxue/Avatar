//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPHYSICSMANAGER_H_
#define _CPHYSICSMANAGER_H_
#include "AvatarConfig.h"
#include "CRigidBody.h"
#include "CVector3.h"
#include "CDynamicBvt.h"
#include "CContactDetector.h"
#include <map>
#include <list>
#include <vector>
using std::map;
using std::list;
using std::vector;

/**
* @brief 物理模拟管理器
*/
class AVATAR_EXPORT CPhysicsManager {
public:
	//! 获取管理器实例
	static CPhysicsManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CPhysicsManager();
		return m_pInstance;
	}

	//! 实例销毁
	void Destroy();
	//! 物理模拟更新
	void Update(float dt);

	//! 添加刚体
	void AddRigidBody(CRigidBody* body);
	//! 删除刚体
	void DelRigidBody(CRigidBody* body);
	//! 清除所有刚体
	void ClearRigidBody();
	//! 通过绑定的场景节点获取刚体
	CRigidBody* GetRigidBody(CSceneNode* node);
	//! 获取管理的所有刚体列表
	void GetRigidBodyList(vector<CRigidBody*>& rigidList);

	//! 设置重力加速度
	void SetGravity(const CVector3& gravity);
	//! 获取重力加速度
	CVector3 GetGravity();

	//! 施加力到刚体重心
	void ApplyForce(CRigidBody* body, const CVector3& force);
	//! 施加力到刚体
	void ApplyForce(CRigidBody* body, const CVector3& force, const CVector3& relPos);

	//! 施加冲量到刚体重心
	void ApplyImpulse(CRigidBody* body, const CVector3& impulse);
	//! 施加冲量到刚体
	void ApplyImpulse(CRigidBody* body, const CVector3& impulse, const CVector3& pos);

private:
	CPhysicsManager();
	~CPhysicsManager();

	//! 更新速度
	void IntegrateForce();
	//! 更新位置
	void IntegrateVelocity();
	//! 碰撞检测
	void CollisionDetect();
	//! 处理约束
	void SolveConstraint();

	//! 预处理接触约束
	void PreSolve(SArbiter& arbiter);
	//! 处理接触约束
	void Solve(SArbiter& arbiter);
	//! 处理碰撞事件
	void CollideEvent();

private:
	//! 碰撞信息
	typedef struct _SCollideInfo {
		bool enter;
		CRigidBody* body1;
		CRigidBody* body2;
	} SCollideInfo;

	//! 执行的时间间隔
	const float m_fTimeStep;
	//! 重力
	CVector3 m_cGravity;
	//! 刚体列表
	list<CRigidBody*> m_lstRigidBody;
	//! DBVT 碰撞检测结果
	vector<CDynamicBvt::SCollidePair> m_vecCollidePair;
	//! 接触列表
	list<SArbiter> m_lstArbiterList;
	//! 碰撞集合
	map<size_t, SCollideInfo> m_mapCollide;
	//! 动态包围层次树
	CDynamicBvt* m_pDynamicBvt;
	//! 实例
	static CPhysicsManager* m_pInstance;
};

#endif