//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
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
* @brief 相机类
*/
class AVATAR_EXPORT CCamera {
public:
	//! 默认构造函数
	CCamera();
	//! 默认析构函数
	virtual ~CCamera();

	//! 获取相机名称
	virtual const char* GetName() const;

	//! 控制信息输入
	virtual void Input(CInputManager::SInput* input);
	//! 设置相机高度
	virtual void SetHeight(float height);
	//! 设置相机位置
	virtual void SetPosition(const CVector3& pos);
	//! 设置相机方位
	virtual void SetAngle(float yaw, float pitch, float roll);
	//! 设置相机目标
	virtual void SetTarget(const CVector3& pos);

	//! 更新相机
	virtual void Update(float dt);

public:
	//! 获取视口宽度
	float GetViewWidth() const;
	//! 获取视口高度
	float GetViewHeight() const;
	//! 获取相机视角
	float GetFieldOfView() const;
	//! 获取视口宽高比
	float GetAspectRatio() const;
	//! 获取近裁剪面距离
	float GetNearClipDistance() const;
	//! 获取远裁剪面距离
	float GetFarClipDistance() const;
	//! 是否是正交投影模式
	bool IsOrthoProjection() const;

	//! 设置相机视口大小
	void SetViewSize(float width, float height, bool ignoreOrtho);
	//! 设置相机视角
	void SetFieldOfView(float fov);
	//! 设置裁剪面距离
	void SetClipDistance(float zNear, float zFar);
	//! 设置投影方式
	void SetOrthoProjection(bool ortho);

	//! 更新投影矩阵
	void UpdateProjMatrix();
	//! 更新投影矩阵
	void UpdateProjMatrix(bool ortho);
	//! 更新视图矩阵
	void UpdateViewMatrix();
	//! 更新视锥体
	void UpdateFrustum();

	//! 获取投影矩阵引用
	CMatrix4& GetProjMatrix();
	//! 获取视图矩阵引用
	CMatrix4& GetViewMatrix();
	//! 获取视锥体引用
	const CFrustum& GetFrustum();

	//! 绑定输入控制
	void Control(bool enable);
	//! 绑定至指定场景节点
	void Bind(CSceneNode* sceneNode, const CVector3& pos, const CQuaternion& orient);

	//! 获取相机角度
	static void FromVectorToAngle(const CVector3& look, const CVector3& up, float* yaw, float* pitch, float* roll);
	//! 计算相机正交向量
	static void FromAngleToVector(float yaw, float pitch, float roll, CVector3& look, CVector3& up);

public:
	//! 相机位置
	CVector3 m_cPosition;
	//! 视线方向
	CVector3 m_cLookVector;
	//! 朝上方向
	CVector3 m_cUpVector;

protected:
	//! 方位角
	float m_fYaw;
	//! 俯仰角
	float m_fPitch;
	//! 旋转角
	float m_fRoll;

	//! 用户控制绑定
	bool m_bControlAttached;
	//! 视口宽度
	float m_fViewWidth;
	//! 视口高度
	float m_fViewHeight;
	//! 水平视角 (单位度)
	float m_fFieldOfView;
	//! 近裁剪面距离
	float m_fClipNear;
	//! 远裁剪面距离
	float m_fClipFar;
	//! 是否正交投影
	bool m_bOrthoProject;
	//! 投影矩阵
	CMatrix4 m_cProjMatrix;
	//! 视图矩阵
	CMatrix4 m_cViewMatrix;
	//! 视锥体
	CFrustum m_cFrustum;
	//! 依附的场景节点
	CSceneNode* m_pBindNode;
	//! 依附位置偏移
	CVector3 m_cBindPosition;
	//! 依附角度偏移
	CQuaternion m_cBindOrientation;
};

#endif