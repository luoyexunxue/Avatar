//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CCAMERASMOOTH_H_
#define _CCAMERASMOOTH_H_
#include "CCamera.h"

/**
* @brief 平滑式相机
*
* 针对基本相机类型基础上增加了平滑移动效果
*/
class AVATAR_EXPORT CCameraSmooth: public CCamera {
public:
	//! 默认构造函数
	CCameraSmooth();

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
	//! 限制方位角
	inline void RestrictYawRange(bool shortest);

private:
	//! 阻尼系数
	float m_fDamping;
	//! 相机预定方位角
	float m_fYawInAdvance;
	//! 相机预定俯仰角
	float m_fPitchInAdvance;
	//! 相机预定翻滚角
	float m_fRollInAdvance;
	//! 相机预定位置
	CVector3 m_cPosInAdvance;
};

#endif