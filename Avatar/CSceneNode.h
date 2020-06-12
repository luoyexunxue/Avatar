//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODE_H_
#define _CSCENENODE_H_
#include "AvatarConfig.h"
#include "CMatrix4.h"
#include "CVector3.h"
#include "CQuaternion.h"
#include "CBoundingBox.h"
#include "CMeshData.h"
#include <string>
#include <list>
using std::string;
using std::list;

/**
* @brief 场景节点抽象类
*/
class AVATAR_EXPORT CSceneNode {
public:
	//! 构造函数
	CSceneNode(const string& type, const string& name);
	//! 虚析构函数
	virtual ~CSceneNode();

public:
	//! 初始化接口
	virtual bool Init() = 0;
	//! 渲染接口
	virtual void Render() = 0;
	//! 销毁接口
	virtual void Destroy() = 0;

	//! 节点更新
	virtual void Update(float dt) {}
	//! 设置缩放
	virtual void SetScale(const CVector3& scale);
	//! 设置朝向
	virtual void SetOrientation(const CQuaternion& orientation);
	//! 设置位置
	virtual void SetPosition(const CVector3& position);
	//! 更新变换矩阵
	virtual void Transform();

	//! 获取网格数据
	virtual CMeshData* GetMeshData();

	//! 获取节点ID
	int& GetId();
	//! 获取节点类型
	const string& GetType() const;
	//! 获取节点名称
	const string& GetName() const;

	//! 得到世界坐标系下的缩放
	CVector3 GetWorldScale() const;
	//! 得到世界坐标系下的朝向
	CQuaternion GetWorldOrientation() const;
	//! 得到世界坐标系下的位置
	CVector3 GetWorldPosition() const;
	//! 获取世界坐标系下的包围盒
	CBoundingBox GetBoundingBox() const;

public:
	//! 节点缩放
	CVector3 m_cScale;
	//! 节点旋转
	CQuaternion m_cOrientation;
	//! 节点位移
	CVector3 m_cPosition;
	//! 局部坐标系下的包围盒
	CBoundingBox m_cLocalBoundingBox;

	//! 是否可见
	bool m_bVisible;
	//! 是否启用
	bool m_bEnabled;
	//! 是否选中
	bool m_bSelected;

	//! 局部变换矩阵
	CMatrix4 m_cModelMatrix;
	//! 世界变换矩阵
	CMatrix4 m_cWorldMatrix;

protected:
	//! 节点ID
	int m_iNodeId;
	//! 节点类型
	string m_strType;
	//! 节点名称
	string m_strName;
	//! 父节点
	CSceneNode* m_pParent;
	//! 子节点
	list<CSceneNode*> m_lstChildren;

	//! 设置场景管理器可访问
	friend class CSceneManager;
};

#endif