//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CCAMERA_H_
#define _CCAMERA_H_
#include "AvatarConfig.h"
#include "CVector3.h"
#include "CMatrix4.h"
#include "CQuaternion.h"
#include "CFrustum.h"
#include "CSceneNode.h"
#include "CInputManager.h"

/**
* @brief �����
*/
class AVATAR_EXPORT CCamera {
public:
	//! Ĭ�Ϲ��캯��
	CCamera();
	//! Ĭ����������
	virtual ~CCamera();

	//! ��ȡ�������
	virtual const char* GetName() const;

	//! ������Ϣ����
	virtual void Input(CInputManager::SInput* input);
	//! ��������߶�
	virtual void SetHeight(float height);
	//! �������λ��
	virtual void SetPosition(const CVector3& pos);
	//! ���������λ
	virtual void SetAngle(float yaw, float pitch, float roll);
	//! �������Ŀ��
	virtual void SetTarget(const CVector3& pos);

	//! �������
	virtual void Update(float dt);

public:
	//! ��ȡ�ӿڿ��
	int GetViewWidth() const;
	//! ��ȡ�ӿڸ߶�
	int GetViewHeight() const;
	//! ��ȡ����ӽ�
	float GetFieldOfView() const;
	//! ��ȡ�ӿڿ�߱�
	float GetAspectRatio() const;
	//! ��ȡ���ü������
	float GetNearClipDistance() const;
	//! ��ȡԶ�ü������
	float GetFarClipDistance() const;

	//! ��������ӿڴ�С
	void SetViewSize(int width, int height);
	//! ��������ӽ�
	void SetFieldOfView(float fov);
	//! ���òü������
	void SetClipDistance(float zNear, float zFar);

	//! ����ͶӰ����
	void UpdateProjMatrix(bool ortho);
	//! ������ͼ����
	void UpdateViewMatrix();
	//! ������׶��
	void UpdateFrustum();

	//! ��ȡͶӰ��������
	CMatrix4& GetProjMatrix();
	//! ��ȡ��ͼ��������
	CMatrix4& GetViewMatrix();
	//! ��ȡ��׶������
	const CFrustum& GetFrustum();

	//! ���������
	void Control(bool enable);
	//! ����ָ�������ڵ�
	void Bind(CSceneNode* sceneNode, const CVector3& pos, const CQuaternion& orient);

	//! ��ȡ�����λ�ǣ������ǣ���ת��
	static void GetYawPitchRoll(const CVector3& lookVec, const CVector3& upVec, float* yaw, float* pitch, float* roll);
	//! �����������������������
	static void GetLookVecUpVec(float yaw, float pitch, float roll, CVector3& lookVec, CVector3& upVec);

public:
	//! ���λ��
	CVector3 m_cPosition;
	//! ���߷���
	CVector3 m_cLookVector;
	//! ���Ϸ���
	CVector3 m_cUpVector;

protected:
	//! ��λ��
	float m_fYaw;
	//! ������
	float m_fPitch;
	//! ��ת��
	float m_fRoll;

	//! �û����ư�
	bool m_bControlAttached;
	//! �ӿڿ��
	int m_iViewWidth;
	//! �ӿڸ߶�
	int m_iViewHeight;
	//! ˮƽ�ӽ� (��λ��)
	float m_fFieldOfView;
	//! ���ü������
	float m_fClipNear;
	//! Զ�ü������
	float m_fClipFar;
	//! ͶӰ����
	CMatrix4 m_cProjMatrix;
	//! ��ͼ����
	CMatrix4 m_cViewMatrix;
	//! ��׶��
	CFrustum m_cFrustum;
	//! �����ĳ����ڵ�
	CSceneNode* m_pBindNode;
	//! ����λ��ƫ��
	CVector3 m_cBindPosition;
	//! �����Ƕ�ƫ��
	CQuaternion m_cBindOrientation;
};

#endif