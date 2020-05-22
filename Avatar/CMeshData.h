//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* @brief ��ת�ؼ�֡
*/
struct SAnimationRotKey {
	//! �ؼ�֡��ʱ��
	float time;
	//! ��ת����
	CQuaternion param;
};

/**
* @brief λ�ƹؼ�֡
*/
struct SAnimationPosKey {
	//! �ؼ�֡��ʱ��
	float time;
	//! λ������
	CVector3 param;
};

/**
* @brief ��̬����
* @note Ϊ����������������ģ��
*/
struct SJointDynamic {
	//! ������
	float mass;
	//! ��ֱ������ϵ��
	float vElasticity;
	//! ˮƽ������ϵ��
	float hElasticity;
	//! �ٶ�����
	float damping;
	//! ��ǰ�ٶ�
	CVector3 velocity;
	//! ����λ��
	CVector3 position;
	//! ���ڵ�λ��
	CVector3 parentPos;
};

/**
* @brief �ؽڶ���
* @attention �ؼ�֡�����ǻ��ھֲ��任����֮�ϵ�
*/
struct SJoint {
	//! ���������±�
	int index;
	//! �ؽ�����
	string name;
	//! �ϼ��ؽ�
	SJoint* parent;
	//! �����ڵ�λ��(����)
	CVector3 position;
	//! �ֲ��任����
	CMatrix4 localMatrix;
	//! ����任����(����)
	CMatrix4 worldMatrix;
	//! �󶨾��������
	CMatrix4 bindMatrixInv;
	//! ���ձ任����(����)
	CMatrix4 finalMatrix;
	//! ��̬����
	SJointDynamic* physics;
	//! ��ǰ��ת�ؼ�֡
	int currentRotKey[4];
	//! ��ǰλ�ƹؼ�֡
	int currentPosKey[4];
	//! ��ת�任�ؼ�֡
	vector<SAnimationRotKey> keyRot;
	//! λ�ñ任�ؼ�֡
	vector<SAnimationPosKey> keyPos;
	//! Ĭ�Ϲ��캯��
	SJoint() : index(0), parent(0), physics(0) {}
	//! Ĭ����������
	~SJoint() { if (physics) delete physics; }
};

/**
* @brief ����������
*/
class AVATAR_EXPORT CMeshData {
public:
	//! ���캯��
	CMeshData();
	//! ��������
	~CMeshData();

	//! ��ȡ��������
	int GetMeshCount() const;
	//! ��ȡ�ؽ�����
	int GetJointCount() const;
	//! ��ȡ��������
	int GetAnimationCount() const;
	//! ��ȡ���񶥵����
	int GetVertexCount() const;
	//! ��ȡ���������μ���
	int GetTriangleCount() const;
	//! ��ȡ��Χ��
	CBoundingBox GetBoundingBox() const;

	//! ��ȡָ������
	CMesh* GetMesh(int index);
	//! ��ȡָ���ؽ�
	SJoint* GetJoint(int index);
	//! ��ȡָ���ؽ�
	SJoint* GetJoint(const string& name);
	//! ��ȡָ�����ƵĶ���
	int GetAnimationIndex(const string& name);
	//! ��ȡ��������
	string& GetAnimationName(int index);
	//! ��ȡ����ʱ��
	float GetAnimationTime(int index);
	//! ��ȡ������ʼʱ��
	float GetAnimationBeginTime(int index);
	//! ��ȡ��������ʱ��
	float GetAnimationEndTime(int index);

	//! �������
	CMeshData* AddMesh(CMesh* mesh);
	//! ��ӹؽ�(���ؽ�����)
	CMeshData* AddJoint(SJoint* joint);
	//! ��Ӷ�����Ϣ
	CMeshData* AddAnimation(const string& name, float beginTime, float endTime);
	//! ��ӹ�������֧��
	CMeshData* AddPhysics(const string& joint, float mass, float hk, float vk, float damping);

	//! �����������
	void ClearMesh();
	//! ������йؽ�
	void ClearJoint();
	//! ������ж���
	void ClearAnimation();

	//! ��¡�������
	CMeshData* Clone() const;

private:
	//! �������ݶ���
	typedef struct _SMeshData {
		vector<CMesh*> meshes;
		vector<SJoint*> joints;
		vector<string> animationNames;
		vector<float> animationBeginTimes;
		vector<float> animationEndTimes;
	} SMeshData;
	//! ��������ָ��
	SMeshData* m_pMeshData;
};

#endif