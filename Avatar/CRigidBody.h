//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* @brief ������
*/
class AVATAR_EXPORT CRigidBody {
public:
	//! ������岢�󶨳����ڵ�
	CRigidBody(CSceneNode* binding);

	//! ��ȡ�󶨵ĳ����ڵ�
	CSceneNode* GetSceneNode() const;

	//! ������������
	void SetAttribute(float mass, const SGeometry& geometry, bool trigger, bool allowSleep);
	//! ��������ϵ��
	void SetDamping(float linearDamping, float angularDamping);
	//! ����Ħ��ϵ��
	void SetFriction(float friction);
	//! ���ûָ�ϵ��
	void SetRestitution(float restitution);
	//! ���ó��ٶ�
	void SetVelocity(const CVector3& linearVel, const CVector3& angularVel);

	//! ��λ����״̬
	void Reset();
	//! �����������
	void ClearForce();
	//! ʩ������
	void ApplyForce(const CVector3& force, bool local);
	//! ʩ��������������
	void ApplyForce(const CVector3& force, const CVector3& torque, bool local);

	//! ��ȡ���弸��
	inline const SGeometry& GetGeometry() const { return m_sGeometry; }
	//! ������ײ�ص�
	inline int& CollideCallback() { return m_iCollideCallback; }

private:
	//! ����Ƿ���Ҫ˯��
	bool CheckSleeping(float dt);
	//! �����������
	void SetupInertiaTensor();

private:
	//! �����ܺ�
	CVector3 m_cTotalForce;
	//! ���ٶ�
	CVector3 m_cAcceleration;
	//! ���ٶ�
	CVector3 m_cLinearVelocity;
	//! ���ص��ܺ�
	CVector3 m_cTotalTorque;
	//! ���ٶ�
	CVector3 m_cAngularVelocity;
	//! �������
	CVector3 m_cExtraForce;
	//! ���������
	CVector3 m_cExtraTorque;
	//! ����ľֲ���
	CVector3 m_cExtraForceLocal;
	//! ����ľֲ�����
	CVector3 m_cExtraTorqueLocal;
	//! ��ת��Ĺ������������
	CMatrix4 m_cInvInertiaTensorWorld;

	//! ���ٶ�����
	float m_fLinearDamping;
	//! ���ٶ�����
	float m_fAngularDamping;
	//! ��������
	float m_fMass;
	//! ������������
	float m_fInvMass;
	//! ת������
	CVector3 m_cInertiaLocal;
	//! ��ת������
	CVector3 m_cInvInertiaLocal;
	//! Ħ��ϵ��
	float m_fFriction;
	//! �ָ�ϵ��
	float m_fRestitution;
	//! �Ƿ��Ǿ�̬����
	bool m_bStatic;
	//! �Ƿ񴥷���
	bool m_bIsTrigger;
	//! �Ƿ���������
	bool m_bAllowSleep;
	//! ���弸�ζ���
	SGeometry m_sGeometry;

	//! ��������״̬
	bool m_bSleeping;
	//! �������ߵ�ʱ��
	float m_fSleepingTime;
	//! ˯��ʱ�ĺ���
	CVector3 m_cSleepingForce;
	//! ˯��ʱ�ĺ�����
	CVector3 m_cSleepingTorque;

	//! ��ײ�ص�
	int m_iCollideCallback;
	//! �󶨵ĳ����ڵ�
	CSceneNode* m_pSceneNode;
	//! CPhysicsManager �ɷ���
	friend class CPhysicsManager;
};

#endif