//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CCAMERAVIEWER_H_
#define _CCAMERAVIEWER_H_
#include "CCamera.h"
#include "CQuaternion.h"

/**
* @brief 模型浏览型相机
*
* 全方位观察，可以用它来观测物体，浏览模型
*/
class AVATAR_EXPORT CCameraViewer: public CCamera {
public:
	//! 默认构造函数
	CCameraViewer();

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
	//! 设置阻尼系数
	void SetDamping(float k);

private:
	//! 阻尼系数
	float m_fDamping;
	//! 相机目标距离
	float m_fDistance;
	//! 相机预定目标距离
	float m_fDistanceInAdvance;
	//! 目标位置
	CVector3 m_cTargetPos;
	//! 相机方位
	CQuaternion m_cOrientation;
	//! 相机预定方位
	CQuaternion m_cOrientInAdvance;
};

#endif