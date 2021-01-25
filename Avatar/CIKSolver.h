//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CIKSOLVER_H_
#define _CIKSOLVER_H_
#include "AvatarConfig.h"
#include "CMeshData.h"
#include "CVector3.h"
#include "CQuaternion.h"
#include <vector>
using std::vector;

/**
* @brief IK 解析器
*/
class AVATAR_EXPORT CIKSolver {
public:
	//! 默认构造函数
	CIKSolver(SJoint* joint, int affect);
	//! 默认析构函数
	~CIKSolver();

public:
	//! 设置目标位置
	void SetTarget(const CVector3& target);
	//! 设置铰链关节约束
	void SetSwingLimit(SJoint* joint, const CVector3& axis, float min, float max);
	//! 设置球状关节约束
	void SetTwistLimit(SJoint* joint, const CVector3& axis, float angle);
	//! 迭代解析
	void Solve(int maxStep);

private:
	//! 支持的约束方式
	enum ConstrainType { SWINGLIMIT, TWISTLIMIT };
	//! 约束定义
	typedef struct _SConstraint {
		ConstrainType type;
		CVector3 axis;
		CVector3 original;
		CVector3 initial;
		float minAngle;
		float maxAngle;
	} SConstraint;
	//! IK节点
	typedef struct _SIKNode {
		SJoint* joint;
		SConstraint* constraint;
		CQuaternion localRot;
		CQuaternion worldRot;
		CQuaternion animationRot;
	} SIKNode;

	//! 约束计算
	void ConstraintSolve(size_t index, CQuaternion& rotation);

private:
	//! 目标位置
	CVector3 m_cTarget;
	//! IK骨骼链
	vector<SIKNode> m_vecKinematicChain;
};

#endif