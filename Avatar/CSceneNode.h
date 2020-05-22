//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* @brief �����ڵ������
*/
class AVATAR_EXPORT CSceneNode {
public:
	//! ���캯��
	CSceneNode(const string& type, const string& name);
	//! ����������
	virtual ~CSceneNode();

public:
	//! ��ʼ���ӿ�
	virtual bool Init() = 0;
	//! ��Ⱦ�ӿ�
	virtual void Render() = 0;
	//! ���ٽӿ�
	virtual void Destroy() = 0;

	//! �ڵ����
	virtual void Update(float dt) {}
	//! ��������
	virtual void SetScale(const CVector3& scale);
	//! ���ó���
	virtual void SetOrientation(const CQuaternion& orientation);
	//! ����λ��
	virtual void SetPosition(const CVector3& position);
	//! ���±任����
	virtual void Transform();

	//! ��ȡ��������
	virtual CMeshData* GetMeshData();

	//! ��ȡ�ڵ�ID
	int& GetId();
	//! ��ȡ�ڵ�����
	const string& GetType() const;
	//! ��ȡ�ڵ�����
	const string& GetName() const;

	//! �õ���������ϵ�µ�����
	CVector3 GetWorldScale() const;
	//! �õ���������ϵ�µĳ���
	CQuaternion GetWorldOrientation() const;
	//! �õ���������ϵ�µ�λ��
	CVector3 GetWorldPosition() const;
	//! ��ȡ��������ϵ�µİ�Χ��
	CBoundingBox GetBoundingBox() const;

public:
	//! �ڵ�����
	CVector3 m_cScale;
	//! �ڵ���ת
	CQuaternion m_cOrientation;
	//! �ڵ�λ��
	CVector3 m_cPosition;
	//! �ֲ�����ϵ�µİ�Χ��
	CBoundingBox m_cLocalBoundingBox;

	//! �Ƿ�ɼ�
	bool m_bVisible;
	//! �Ƿ�����
	bool m_bEnabled;
	//! �Ƿ�ѡ��
	bool m_bSelected;

	//! �ֲ��任����
	CMatrix4 m_cModelMatrix;
	//! ����任����
	CMatrix4 m_cWorldMatrix;

protected:
	//! �ڵ�ID
	int m_iNodeId;
	//! �ڵ�����
	string m_strType;
	//! �ڵ�����
	string m_strName;
	//! ���ڵ�
	CSceneNode* m_pParent;
	//! �ӽڵ�
	list<CSceneNode*> m_lstChildren;

	//! ���ó����������ɷ���
	friend class CSceneManager;
};

#endif