//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* @brief �����и���
*
* ����ʹ��ָ��ƽ���и�������󣬵õ��߶ι켣
*/
class AVATAR_EXPORT CMeshSlicer {
public:
	//! Ĭ�Ϲ��캯��
	CMeshSlicer();
	//! ʹ������������и���
	CMeshSlicer(const CMesh* mesh);

	//! �������
	void AddMesh(const CMesh* mesh);

	//! ��ָ��ƽ���и�
	bool Slice(const CPlane& plane);
	//! ��ָ���������и�
	bool Slice(int axis, float value);

	//! ��ȡ�켣����
	int GetTrackCount() const;
	//! ��ȡһ���켣�ĵ����
	int GetTrackPointCount(int track) const;
	//! ��ȡһ���켣��ָ����
	const CVector3& GetTrackPoint(int track, int point) const;

	//! ��������Ⱦ�ĳ����ڵ�
	CSceneNode* CreateRenderLine(CSceneNode* src, int track, bool depth, bool point);

private:
	//! �и�������
	void SliceTriangle(const CPlane& plane, const CVector3& a, const CVector3& b, const CVector3& c);
	//! ���и����߶���������
	void BuildTrackLine();
	//! �ж������Ƿ���ͬ
	inline bool IsEqual(const CVector3& a, const CVector3& b) const;

private:
	//! �и��߶ζ���
	typedef struct _STrackSegment {
		CVector3 a;
		CVector3 b;
		_STrackSegment(const CVector3& _a, const CVector3& _b): a(_a), b(_b) {}
	} STrackSegment;

private:
	vector<const CMesh*> m_vecMeshes;
	list<STrackSegment> m_lstTrackSet;
	vector< list<CVector3>> m_vecTrackLines;
};

#endif