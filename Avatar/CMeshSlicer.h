//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CMESHSLICER_H_
#define _CMESHSLICER_H_
#include "AvatarConfig.h"
#include "CMesh.h"
#include "CPlane.h"
#include "CSceneNode.h"
#include <vector>
#include <list>
using std::vector;
using std::list;

/**
* @brief 网格切割类
*
* 可以使用指定平面切割网格对象，得到线段轨迹
*/
class AVATAR_EXPORT CMeshSlicer {
public:
	//! 默认构造函数
	CMeshSlicer();
	//! 使用网格对象构造切割类
	CMeshSlicer(const CMesh* mesh);

	//! 添加网格
	void AddMesh(const CMesh* mesh);

	//! 按指定平面切割
	bool Slice(const CPlane& plane);
	//! 按指定坐标轴切割
	bool Slice(int axis, float value);

	//! 获取轨迹个数
	size_t GetTrackCount() const;
	//! 获取一个轨迹的点个数
	size_t GetTrackPointCount(size_t track) const;
	//! 获取一个轨迹的指定点
	const CVector3& GetTrackPoint(size_t track, size_t point) const;

	//! 创建可渲染的场景节点
	CSceneNode* CreateRenderLine(CSceneNode* src, size_t track, bool depth, bool point);

private:
	//! 切割三角形
	void SliceTriangle(const CPlane& plane, const CVector3& a, const CVector3& b, const CVector3& c);
	//! 从切割后的线段生成切面
	void BuildTrackLine();
	//! 判断两点是否相同
	inline bool IsEqual(const CVector3& a, const CVector3& b) const;

private:
	//! 切割线段定义
	typedef struct _STrackSegment {
		CVector3 a;
		CVector3 b;
		_STrackSegment(const CVector3& _a, const CVector3& _b): a(_a), b(_b) {}
	} STrackSegment;

private:
	vector<const CMesh*> m_vecMeshes;
	list<STrackSegment> m_lstTrackSet;
	vector<list<CVector3>> m_vecTrackLines;
};

#endif