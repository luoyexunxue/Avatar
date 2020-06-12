//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CCAMERAFREE_H_
#define _CCAMERAFREE_H_
#include "CCamera.h"
#include "CQuaternion.h"
#include <vector>
using std::vector;

/**
* @brief 全自由度型相机
*
* 适用于飞行模拟类应用
*/
class AVATAR_EXPORT CCameraFree: public CCamera {
public:
	//! 默认构造函数
	CCameraFree();

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
	//! 开始漫游路线
	void StartLineTrack(const vector<CVector3>& track, float speed, bool follow, bool loop);
	//! 停止漫游线路
	void StopLineTrack();
	//! 设置漫游偏移
	void OffsetLineTrack(const CVector3& offset);

private:
	//! 漫游点定义
	typedef struct _STrackPoint {
		float time;
		CVector3 position;
	} STrackPoint;

private:
	//! 计算当前漫游位置和方向
	void TrackLine(float dt, CVector3& pos, CQuaternion& orient);

private:
	//! 阻尼系数
	float m_fDamping;
	//! 相机方位
	CQuaternion m_cOrientation;
	//! 相机预定方位
	CQuaternion m_cOrientInAdvance;
	//! 相机预定位置
	CVector3 m_cPosInAdvance;

	//! 漫游线路
	vector<STrackPoint> m_vecTrackLine;
	//! 漫游速度
	float m_fTrackSpeed;
	//! 漫游时视线跟随
	bool m_bTrackFollow;
	//! 循环漫游
	bool m_bTrackLoop;
	//! 当前漫游时间
	float m_fTrackTime;
	//! 当前漫游点
	int m_iTrackPoint;
	//! 之前漫游点
	int m_iTrackPointPrev;
	//! 漫游坐标偏移
	CVector3 m_cTrackOffset;
};

#endif