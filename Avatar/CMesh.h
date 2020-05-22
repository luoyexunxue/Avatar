//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CMESH_H_
#define _CMESH_H_
#include "AvatarConfig.h"
#include "CMaterial.h"
#include "CVertex.h"
#include "CVertexJoint.h"
#include "CBoundingBox.h"
#include "CRay.h"
#include "CMatrix4.h"
#include <vector>
using std::vector;

/**
* @brief ��������
*/
class AVATAR_EXPORT CMesh {
public:
	//! Ĭ�Ϲ��캯��
	CMesh();
	//! Ĭ����������
	~CMesh();

public:
	//! ��ȡ����
	inline CMaterial* GetMaterial() const { return m_pMaterial; }

	//! ���ö�������
	void SetVertexUsage(unsigned int count);
	//! ���һ������
	void AddVertex(const CVertex& vertex);
	//! ���һ�����㲢ָ����������Ϣ
	void AddVertex(const CVertex& vertex, const CVertexJoint& bind);
	//! ʹ��������ʽ���һ��������
	void AddTriangle(unsigned int a, unsigned int b, unsigned int c);
	//! ʹ�ö��㷽ʽ���һ��������
	void AddTriangle(const CVertex& a, const CVertex& b, const CVertex& c);

	//! �Ƴ����񶥵�
	void RemoveVertex(unsigned int index, int count);
	//! �Ƴ�����������
	void RemoveTriangle(unsigned int index, int count);

	//! ����һ������
	void Append(const CMesh* mesh);
	//! �����ж�����о���任
	void Transform(const CMatrix4& matrix);
	//! �Զ����㶥�㷨����
	void SetupNormal();
	//! ��ת���㷨�����������
	void Reverse(bool normal, bool texCoordU, bool texCoordV);

	//! ��ȡ�������
	int GetVertexCount() const;
	//! ��ȡ�����θ���
	int GetTriangleCount() const;
	//! ��ȡ�󶨹�����������
	int GetBindCount() const;
	//! ��ȡ���񶥵�
	CVertex* GetVertex(unsigned int index);
	//! ��ȡ���񶥵�
	CVertex* GetVertex(unsigned int face, unsigned int index);
	//! ��ȡ�����ζ���
	void GetTriangle(unsigned int index, CVertex* vertices[3]);
	//! ��ȡ�����ζ�������
	void GetTriangle(unsigned int index, unsigned int vertices[3]);
	//! ��ȡ���������
	CVertexJoint* GetBind(unsigned int index);

	//! ��������
	void Create(bool dynamic);
	//! ���»���
	void Update(int bufferType);
	//! ��Ⱦ����
	void Render(bool material = true);
	//! ����ϸ��
	void Subdivision();

	//! ��ȡ��Χ��
	CBoundingBox GetBoundingBox() const;
	//! ����ʰȡ�����ؾ���
	float Intersects(const CRay& ray, int* face, float* bu, float* bv) const;
	//! ���������������������
	float Volume(CVector3& centroid) const;
	//! ��������
	float SurfaceArea() const;

	//! ��¡�������
	CMesh* Clone() const;

private:
	//! ��������
	vector<CVertex> m_vecVertexArray;
	//! ������������
	vector<unsigned int> m_vecIndexArray;
	//! ����������
	vector<CVertexJoint> m_vecBindArray;
	//! ��Ƕ������ݴ�С
	size_t m_iVertexArraySize;
	//! ����������ݴ�С
	size_t m_iIndexArraySize;
	//! ������Ϣ
	CMaterial* m_pMaterial;

	//! �Ƿ�̬��������
	bool m_bDynamic;
	//! ����Ƿ������ɻ���
	bool m_bCreated;

	//! ���㻺��ID
	unsigned int m_iVertexBuffer;
	//! ��������ID
    unsigned int m_iIndexBuffer;
};

#endif