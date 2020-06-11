//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CCAMERAGAZE_H_
#define _CCAMERAGAZE_H_
#include "CCamera.h"

/**
* @brief 固定目标点型相机
*/
class AVATAR_EXPORT CCameraGaze: public CCamera {
public:
	//! 默认构造函数
	CCameraGaze();

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
	//! 设置相机距离目标最近距离
	void SetMinDistance(float distance);
	//! 设置相机距离目标最大距离
	void SetMaxDistance(float distance);
	//! 设置俯仰角范围
	void SetPitchRange(float min, float max);

private:
	//! 限制方位角
	inline void RestrictYawRange(bool shortest);

private:
	//! 阻尼系数
	float m_fDamping;
	//! 最小距离
	float m_fMinDistance;
	//! 最大距离
	float m_fMaxDistance;
	//! 俯仰角范围
	float m_fPitchRange[2];
	//! 相机预定方位角
	float m_fYawInAdvance;
	//! 相机预定俯仰角
	float m_fPitchInAdvance;
	//! 相机预定翻滚角
	float m_fRollInAdvance;
	//! 目标位置
	CVector3 m_cTargetPos;
	//! 相机预定位置
	CVector3 m_cPosInAdvance;
};

#endif