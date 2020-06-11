//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEANIMATION_H_
#define _CSCENENODEANIMATION_H_
#include "CSceneNode.h"
#include "CMeshData.h"

/**
* @brief 动画模型场景节点
*/
class AVATAR_EXPORT CSceneNodeAnimation: public CSceneNode {
public:
	//! 构造方法
	CSceneNodeAnimation(const string& name, const string& meshFile, bool start, bool skeleton);

	//! 初始化场景节点
	virtual bool Init();
	//! 销毁场景节点
	virtual void Destroy();
	//! 渲染场景节点
	virtual void Render();
	//! 更新逻辑
	virtual void Update(float dt);
	//! 获取网格数据
	virtual CMeshData* GetMeshData();

	//! 动画播放开始
	void StartAnimation(const string& name, bool loop, float transition);
	//! 混合两个动画
	void BlendAnimation(const string& anim1, const string& anim2, float k);
	//! 动画播放停止
	void StopAnimation();
	//! 设置动画速率
	void SetAnimationFrameRate(float rate);
	//! 设置骨骼显示
	void ShowSkeleton(bool visible, bool skeletonOnly);
	//! 获取动画名称
	string GetAnimationName(int index);
	//! 骨骼注视功能
	void PointFacing(const string& joint, const CVector3& front, const CVector3& point, float angle);

private:
	//! 设置动画帧
	void SetupFrame(float dt);
	//! 设置关节关键帧
	void SetupJointKey();
	//! 计算关节动画变换
	void JointTransform(SJoint* joint, int channel, CQuaternion& rot, CVector3& pos);
	//! 更新网格顶点
	void UpdateVertex();
	//! 绘制骨骼
	void DrawSkeleton(bool topMost);
	//! 关节物理模拟
	void PhysicalSimulation(SJoint* joint, const CVector3& gravity, float dt, CMatrix4& anim);

private:
	//! 模型文件
	string m_strFile;
	//! 模型数据
	CMeshData* m_pMeshData;
	//! 备份网格顶点位置数组
	vector<vector<CVector3>> m_vecMeshVertexPos;
	//! 备份网格顶点法向数组
	vector<vector<CVector3>> m_vecMeshVertexNor;
	//! 关节点数组
	vector<CVertex> m_vecJointVertex;

	//! 骨架显示模式
	unsigned char m_iSkeletonMode;
	//! 指示是否在进行动画
	bool m_bAnimation;
	//! 指示是否循环动画
	bool m_bLoopAnimation;
	//! 动画播放速度
	float m_fAnimationRate;
	//! 动画混合参数
	float m_fBlendFactor;
	//! 动画过渡时间
	float m_fTransition;
	//! 动画过渡总时间倒数
	float m_fTransitionTimeInv;
	//! 动画帧同步信息
	float m_fSyncFactor[2];
	//! 动画时间长度
	float m_fAnimationTime[2];
	//! 动画开始时间
	float m_fAnimationBegin[2];
	//! 当前动画时间
	float m_fCurrentTime[2];
};

#endif