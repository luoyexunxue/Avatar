//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CCameraFree.h"
#include "CMatrix4.h"
#include "CVector3.h"
#include <cmath>

/**
* 构造函数
*/
CCameraFree::CCameraFree() {
	m_fDamping = 5.0f;
	m_cPosInAdvance = m_cPosition;
	m_fTrackSpeed = 1.0f;
	m_bTrackFollow = false;
	m_bTrackLoop = false;
	m_fTrackTime = 0.0f;
	m_iTrackPoint = 1;
	m_iTrackPointPrev = 0;
}

/**
* 获取相机名称
* @return free
*/
const char* CCameraFree::GetName() const {
	return "free";
}

/**
* 控制信息输入
*/
void CCameraFree::Input(CInputManager::SInput* input) {
	if (!m_bControlAttached) return;
	if (input->bMove) {
		CMatrix4 viewMat = m_cViewMatrix;
		CVector3 movement = CVector3(input->fRightLeft, input->fUpDown, -input->fForthBack, 0.0f);
		m_cPosInAdvance += viewMat.Transpose() * movement;
	}
	if (input->bTurn && !m_bTrackFollow) {
		CQuaternion rotX = CQuaternion().FromAxisAngle(CVector3::X, input->fPitch);
		CQuaternion rotY = CQuaternion().FromAxisAngle(CVector3::Y, -input->fRoll);
		CQuaternion rotZ = CQuaternion().FromAxisAngle(CVector3::Z, input->fYaw);
		m_cOrientInAdvance = m_cOrientInAdvance * rotY * rotZ * rotX;
	}
}

/**
* 设置相机高度
*/
void CCameraFree::SetHeight(float height) {
	m_cPosInAdvance[2] = height;
}

/**
* 设置相机位置
*/
void CCameraFree::SetPosition(const CVector3& pos) {
	m_cPosInAdvance = pos;
}

/**
* 设置相机方位
*/
void CCameraFree::SetAngle(float yaw, float pitch, float roll) {
	if (!m_bTrackFollow) {
		CMatrix4 rotMat;
		rotMat.RotateY(roll);
		rotMat.RotateX(pitch);
		rotMat.RotateZ(yaw);
		m_cOrientInAdvance.FromMatrix(rotMat);
	}
}

/**
* 设置相机目标
*/
void CCameraFree::SetTarget(const CVector3& pos) {
	CVector3 lookVec = pos - m_cPosInAdvance;
	if (!m_bTrackFollow && lookVec.Length() > 0) {
		// 更新相机方位角
		lookVec.Normalize();
		float yaw;
		float pitch;
		float roll;
		GetYawPitchRoll(lookVec, m_cUpVector, &yaw, &pitch, &roll);
		SetAngle(yaw, pitch, roll);
	}
}

/**
* 更新相机
*/
void CCameraFree::Update(float dt) {
	// 对相机位置和方位进行插值
	float interpolation = m_fDamping * dt;
	if (interpolation > 1.0f) interpolation = 1.0f;
	// 线路漫游
	if (m_vecTrackLine.empty()) {
		m_cPosition = m_cPosition.Lerp(m_cPosInAdvance, interpolation);
		m_cOrientation = m_cOrientation.Slerp(m_cOrientInAdvance, interpolation);
	} else {
		TrackLine(dt, m_cPosInAdvance, m_cOrientInAdvance);
		m_cPosInAdvance += m_cOrientInAdvance * m_cTrackOffset;
		m_cPosition = m_cPosition.Lerp(m_cPosInAdvance, interpolation);
		m_cOrientation = m_cOrientation.SlerpShortest(m_cOrientInAdvance, interpolation);
	}
	// 计算相机方位朝向
	m_cLookVector = m_cOrientation * CVector3::Y;
	m_cUpVector = m_cOrientation * CVector3::Z;
	CCamera::Update(dt);
}

/**
* 设置阻尼系数
* @param k 阻尼系数，越大惯性表现越小
*/
void CCameraFree::SetDamping(float k) {
	m_fDamping = k;
}

/**
* 开始漫游路线
* @param track 线路
* @param speed 漫游速度
* @param follow 保持相机视线跟随
* @param loop 循环漫游
*/
void CCameraFree::StartLineTrack(const vector<CVector3>& track, float speed, bool follow, bool loop) {
	m_vecTrackLine.clear();
	if (track.size() < 2) return;
	STrackPoint start;
	start.time = 0.0f;
	start.position = track[0];
	m_vecTrackLine.push_back(start);
	// 预先计算每个点的漫游到的时间
	for (size_t i = 1; i < track.size(); i++) {
		STrackPoint point;
		point.time = m_vecTrackLine.back().time + (track[i] - track[i - 1]).Length() / speed;
		point.position = track[i];
		m_vecTrackLine.push_back(point);
	}
	m_fTrackSpeed = speed;
	m_bTrackFollow = follow;
	m_bTrackLoop = loop;
	m_fTrackTime = 0.0f;
	m_iTrackPoint = 1;
	m_iTrackPointPrev = 0;
}

/**
* 停止漫游线路
*/
void CCameraFree::StopLineTrack() {
	m_vecTrackLine.clear();
	m_bTrackFollow = false;
}

/**
* 设置漫游偏移
* @param offset 位置偏移
*/
void CCameraFree::OffsetLineTrack(const CVector3& offset) {
	m_cTrackOffset.SetValue(offset);
}

/**
* 计算当前漫游位置和方向
* @param dt 步进时间
* @param pos 插值输出位置
* @param orient 插值输出方向
*/
void CCameraFree::TrackLine(float dt, CVector3& pos, CQuaternion& orient) {
	while ((size_t)m_iTrackPoint < m_vecTrackLine.size()) {
		if (m_fTrackTime < m_vecTrackLine[m_iTrackPoint++].time) {
			m_iTrackPoint -= 1;
			// 线性插值
			float t1 = m_fTrackTime - m_vecTrackLine[m_iTrackPoint - 1].time;
			float t2 = m_vecTrackLine[m_iTrackPoint].time - m_vecTrackLine[m_iTrackPoint - 1].time;
			CVector3 dir = m_vecTrackLine[m_iTrackPoint].position - m_vecTrackLine[m_iTrackPoint - 1].position;
			pos = m_vecTrackLine[m_iTrackPoint - 1].position + dir * (t1 / t2);
			// 计算相机方向
			if (m_bTrackFollow && m_iTrackPointPrev != m_iTrackPoint) {
				float cosa = CVector3::Y.DotProduct(dir);
				if (cosa < -1.0f) cosa = -1.0f;
				else if (cosa > 1.0f) cosa = 1.0f;
				CVector3 axis = CVector3::Y.CrossProduct(dir.Normalize());
				orient.FromAxisAngle(axis, acosf(cosa));
				m_iTrackPointPrev = m_iTrackPoint;
			}
			m_fTrackTime += dt;
			break;
		}
		// 循环漫游线路
		if (m_bTrackLoop && (size_t)m_iTrackPoint == m_vecTrackLine.size()) {
			m_fTrackTime -= m_vecTrackLine.back().time;
			m_iTrackPoint = 1;
			m_iTrackPointPrev = 0;
		}
	}
}