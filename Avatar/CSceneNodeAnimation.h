//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEANIMATION_H_
#define _CSCENENODEANIMATION_H_
#include "CSceneNode.h"
#include "CMeshData.h"

/**
* @brief ����ģ�ͳ����ڵ�
*/
class AVATAR_EXPORT CSceneNodeAnimation: public CSceneNode {
public:
	//! ���췽��
	CSceneNodeAnimation(const string& name, const string& meshFile, bool start, bool skeleton);

	//! ��ʼ�������ڵ�
	virtual bool Init();
	//! ���ٳ����ڵ�
	virtual void Destroy();
	//! ��Ⱦ�����ڵ�
	virtual void Render();
	//! �����߼�
	virtual void Update(float dt);
	//! ��ȡ��������
	virtual CMeshData* GetMeshData();

	//! �������ſ�ʼ
	void StartAnimation(const string& name, bool loop, float transition);
	//! �����������
	void BlendAnimation(const string& anim1, const string& anim2, float k);
	//! ��������ֹͣ
	void StopAnimation();
	//! ���ö�������
	void SetAnimationFrameRate(float rate);
	//! ���ù�����ʾ
	void ShowSkeleton(bool visible, bool skeletonOnly);
	//! ��ȡ��������
	string GetAnimationName(int index);
	//! ����ע�ӹ���
	void PointFacing(const string& joint, const CVector3& front, const CVector3& point, float angle);

private:
	//! ���ö���֡
	void SetupFrame(float dt);
	//! ���ùؽڹؼ�֡
	void SetupJointKey();
	//! ����ؽڶ����任
	void JointTransform(SJoint* joint, int channel, CQuaternion& rot, CVector3& pos);
	//! �������񶥵�
	void UpdateVertex();
	//! ���ƹ���
	void DrawSkeleton(bool topMost);
	//! �ؽ�����ģ��
	void PhysicalSimulation(SJoint* joint, const CVector3& gravity, float dt, CMatrix4& anim);

private:
	//! ģ���ļ�
	string m_strFile;
	//! ģ������
	CMeshData* m_pMeshData;
	//! �������񶥵�λ������
	vector<vector<CVector3>> m_vecMeshVertexPos;
	//! �������񶥵㷨������
	vector<vector<CVector3>> m_vecMeshVertexNor;
	//! �ؽڵ�����
	vector<CVertex> m_vecJointVertex;

	//! �Ǽ���ʾģʽ
	unsigned char m_iSkeletonMode;
	//! ָʾ�Ƿ��ڽ��ж���
	bool m_bAnimation;
	//! ָʾ�Ƿ�ѭ������
	bool m_bLoopAnimation;
	//! ���������ٶ�
	float m_fAnimationRate;
	//! ������ϲ���
	float m_fBlendFactor;
	//! ��������ʱ��
	float m_fTransition;
	//! ����������ʱ�䵹��
	float m_fTransitionTimeInv;
	//! ����֡ͬ����Ϣ
	float m_fSyncFactor[2];
	//! ����ʱ�䳤��
	float m_fAnimationTime[2];
	//! ������ʼʱ��
	float m_fAnimationBegin[2];
	//! ��ǰ����ʱ��
	float m_fCurrentTime[2];
};

#endif