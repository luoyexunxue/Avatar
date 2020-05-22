//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* @brief ����ģ�������
*/
class AVATAR_EXPORT CPhysicsManager {
public:
	//! ��ȡ������ʵ��
	static CPhysicsManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CPhysicsManager();
		return m_pInstance;
	}

	//! ʵ������
	void Destroy();
	//! ����ģ�����
	void Update(float dt);

	//! ��Ӹ���
	void AddRigidBody(CRigidBody* body);
	//! ɾ������
	void DelRigidBody(CRigidBody* body);
	//! ������и���
	void ClearRigidBody();
	//! ͨ���󶨵ĳ����ڵ��ȡ����
	CRigidBody* GetRigidBody(CSceneNode* node);
	//! ��ȡ��������и����б�
	void GetRigidBodyList(vector<CRigidBody*>& rigidList);

	//! �����������ٶ�
	void SetGravity(const CVector3& gravity);
	//! ��ȡ�������ٶ�
	CVector3 GetGravity();

	//! ʩ��������������
	void ApplyForce(CRigidBody* body, const CVector3& force);
	//! ʩ����������
	void ApplyForce(CRigidBody* body, const CVector3& force, const CVector3& relPos);

	//! ʩ�ӳ�������������
	void ApplyImpulse(CRigidBody* body, const CVector3& impulse);
	//! ʩ�ӳ���������
	void ApplyImpulse(CRigidBody* body, const CVector3& impulse, const CVector3& pos);

private:
	CPhysicsManager();
	~CPhysicsManager();

	//! �����ٶ�
	void IntegrateForce();
	//! ����λ��
	void IntegrateVelocity();
	//! ��ײ���
	void CollisionDetect();
	//! ����Լ��
	void SolveConstraint();

	//! Ԥ����Ӵ�Լ��
	void PreSolve(SArbiter& arbiter);
	//! ����Ӵ�Լ��
	void Solve(SArbiter& arbiter);
	//! ������ײ�¼�
	void CollideEvent();

private:
	//! ��ײ��Ϣ
	typedef struct _SCollideInfo{
		bool enter;
		CRigidBody* body1;
		CRigidBody* body2;
	} SCollideInfo;

	//! ִ�е�ʱ����
	const float m_fTimeStep;
	//! ����
	CVector3 m_cGravity;
	//! �����б�
	list<CRigidBody*> m_lstRigidBody;
	//! DBVT ��ײ�����
	vector<CDynamicBvt::SCollidePair> m_vecCollidePair;
	//! �Ӵ��б�
	vector<SArbiter> m_vecArbiterList;
	//! ��ײ����
	map<size_t, SCollideInfo> m_mapCollide;
	//! ��̬��Χ�����
	CDynamicBvt* m_pDynamicBvt;
	//! ʵ��
	static CPhysicsManager* m_pInstance;
};

#endif