//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CCONTACTDETECTOR_H_
#define _CCONTACTDETECTOR_H_
#include "CRigidBody.h"
#include "CVector3.h"

/**
* @brief �Ӵ���
*/
struct SContact {
	//! ��ײ��
	CVector3 point;
	//! ָ�����1����ײ����
	CVector3 normal;
	//! �ظ���2����ײ����
	CVector3 tangent;
	//! ����1���Ӵ�������
	CVector3 rel1;
	//! ����2���Ӵ�������
	CVector3 rel2;
	//! �Ӵ����(��ֵ)
	float penetration;

	//! ��������������
	float massNormal;
	//! ��������������
	float massTangent;
	//! �Ӵ���Ȳ������ٶ�
	float biasVelocity;
	//! ��������ۻ�
	float accumulatedNormalImpulse;
	//! ��������ۻ�
	float accumulatedTangentImpulse;
};

/**
* @brief ����Ӵ���Ϣ
*/
struct SArbiter {
	//! ����1
	CRigidBody* body1;
	//! ����2
	CRigidBody* body2;
	//! �ָ�ϵ��
	float restitution;
	//! Ħ��ϵ��
	float friction;
	//! �Ӵ�������
	int numContacts;
	//! �Ӵ����б�
	SContact contact[8];
};

/**
* @brief ��ײ�Ӵ������
*/
class CContactDetector {
public:
	//! ����Ӵ����
	static bool Detect(const CRigidBody* body1, const CRigidBody* body2, SArbiter* arbiter);

private:
	//! ������������
	static int BoxBoxTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! �������������
	static int BoxSphereTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! ���������ƽ��
	static int BoxPlaneTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! ��������������
	static int SphereSphereTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! ���������뽺����
	static int SphereCapsuleTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! ����������Բ����
	static int SphereCylinderTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! ����������Բ��
	static int SphereTorusTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! ����������Բ׶��
	static int SphereConeTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! ����������ƽ��
	static int SpherePlaneTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! ���㽺�����뽺����
	static int CapsuleCapsuleTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! ���㽺������ƽ��
	static int CapsulePlaneTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! ����Բ������ƽ��
	static int CylinderPlaneTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! ����Բ����ƽ��
	static int TorusPlaneTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
	//! ����Բ׶����ƽ��
	static int ConePlaneTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact);
};

#endif