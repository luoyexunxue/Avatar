//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CCAMERAFREE_H_
#define _CCAMERAFREE_H_
#include "CCamera.h"
#include "CQuaternion.h"
#include <vector>
using std::vector;

/**
* @brief ȫ���ɶ������
*
* �����ڷ���ģ����Ӧ��
*/
class AVATAR_EXPORT CCameraFree: public CCamera {
public:
	//! Ĭ�Ϲ��캯��
	CCameraFree();

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
	//! ��������ϵ��
	void SetDamping(float k);
	//! ��ʼ����·��
	void StartLineTrack(const vector<CVector3>& track, float speed, bool follow, bool loop);
	//! ֹͣ������·
	void StopLineTrack();
	//! ��������ƫ��
	void OffsetLineTrack(const CVector3& offset);

private:
	//! ���ε㶨��
	typedef struct _STrackPoint {
		float time;
		CVector3 position;
	} STrackPoint;

private:
	//! ���㵱ǰ����λ�úͷ���
	void TrackLine(float dt, CVector3& pos, CQuaternion& orient);

private:
	//! ����ϵ��
	float m_fDamping;
	//! �����λ
	CQuaternion m_cOrientation;
	//! ���Ԥ����λ
	CQuaternion m_cOrientInAdvance;
	//! ���Ԥ��λ��
	CVector3 m_cPosInAdvance;

	//! ������·
	vector<STrackPoint> m_vecTrackLine;
	//! �����ٶ�
	float m_fTrackSpeed;
	//! ����ʱ���߸���
	bool m_bTrackFollow;
	//! ѭ������
	bool m_bTrackLoop;
	//! ��ǰ����ʱ��
	float m_fTrackTime;
	//! ��ǰ���ε�
	int m_iTrackPoint;
	//! ֮ǰ���ε�
	int m_iTrackPointPrev;
	//! ��������ƫ��
	CVector3 m_cTrackOffset;
};

#endif