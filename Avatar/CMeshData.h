//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CMESHDATA_H_
#define _CMESHDATA_H_
#include "AvatarConfig.h"
#include "CMesh.h"
#include "CMatrix4.h"
#include "CVector3.h"
#include "CQuaternion.h"
#include <string>
#include <vector>
using std::string;
using std::vector;

/**
* @brief 旋转关键帧
*/
struct SAnimationRotKey {
	//! 关键帧的时间
	float time;
	//! 旋转数据
	CQuaternion param;
};

/**
* @brief 位移关键帧
*/
struct SAnimationPosKey {
	//! 关键帧的时间
	float time;
	//! 位移数据
	CVector3 param;
};

/**
* @brief 动态骨骼
* @note 为骨骼设置物理引擎模拟
*/
struct SJointDynamic {
	//! 质量绑定
	float mass;
	//! 垂直方向弹性系数
	float vElasticity;
	//! 水平方向弹性系数
	float hElasticity;
	//! 速度阻尼
	float damping;
	//! 当前速度
	CVector3 velocity;
	//! 最终位置
	CVector3 position;
	//! 父节点位置
	CVector3 parentPos;
};

/**
* @brief 关节定义
* @attention 关键帧数据是基于局部变换矩阵之上的
*/
struct SJoint {
	//! 所在数组下标
	int index;
	//! 关节名称
	string name;
	//! 上级关节
	SJoint* parent;
	//! 骨骼节点位置(计算)
	CVector3 position;
	//! 局部变换矩阵
	CMatrix4 localMatrix;
	//! 世界变换矩阵(计算)
	CMatrix4 worldMatrix;
	//! 绑定矩阵逆矩阵
	CMatrix4 bindMatrixInv;
	//! 最终变换矩阵(计算)
	CMatrix4 finalMatrix;
	//! 动态骨骼
	SJointDynamic* physics;
	//! 当前旋转关键帧
	int currentRotKey[4];
	//! 当前位移关键帧
	int currentPosKey[4];
	//! 旋转变换关键帧
	vector<SAnimationRotKey> keyRot;
	//! 位置变换关键帧
	vector<SAnimationPosKey> keyPos;
	//! 默认构造函数
	SJoint() : index(0), parent(0), physics(0) {}
	//! 默认析构函数
	~SJoint() { if (physics) delete physics; }
};

/**
* @brief 网格数据类
*/
class AVATAR_EXPORT CMeshData {
public:
	//! 构造函数
	CMeshData();
	//! 析构函数
	~CMeshData();

	//! 获取网格数量
	int GetMeshCount() const;
	//! 获取关节数量
	int GetJointCount() const;
	//! 获取动画数量
	int GetAnimationCount() const;
	//! 获取网格顶点计数
	int GetVertexCount() const;
	//! 获取网格三角形计数
	int GetTriangleCount() const;
	//! 获取包围盒
	CBoundingBox GetBoundingBox() const;

	//! 获取指定网格
	CMesh* GetMesh(int index);
	//! 获取指定关节
	SJoint* GetJoint(int index);
	//! 获取指定关节
	SJoint* GetJoint(const string& name);
	//! 获取指定名称的动画
	int GetAnimationIndex(const string& name);
	//! 获取动画名称
	string& GetAnimationName(int index);
	//! 获取动画时长
	float GetAnimationTime(int index);
	//! 获取动画开始时间
	float GetAnimationBeginTime(int index);
	//! 获取动画结束时间
	float GetAnimationEndTime(int index);

	//! 添加网格
	CMeshData* AddMesh(CMesh* mesh);
	//! 添加关节(父关节优先)
	CMeshData* AddJoint(SJoint* joint);
	//! 添加动画信息
	CMeshData* AddAnimation(const string& name, float beginTime, float endTime);
	//! 添加骨骼物理支持
	CMeshData* AddPhysics(const string& joint, float mass, float hk, float vk, float damping);

	//! 清空所有网格
	void ClearMesh();
	//! 清空所有关节
	void ClearJoint();
	//! 清空所有动画
	void ClearAnimation();

	//! 克隆网格对象
	CMeshData* Clone() const;

private:
	//! 网格数据定义
	typedef struct _SMeshData {
		vector<CMesh*> meshes;
		vector<SJoint*> joints;
		vector<string> animationNames;
		vector<float> animationBeginTimes;
		vector<float> animationEndTimes;
	} SMeshData;
	//! 网格数据指针
	SMeshData* m_pMeshData;
};

#endif