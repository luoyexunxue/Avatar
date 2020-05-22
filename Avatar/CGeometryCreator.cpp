//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CGeometryCreator.h"
#include "CQuaternion.h"
#include "CPlane.h"
#include <cmath>

/**
* ����������
* @param dx X �᷽��ĳ���
* @param dy Y �᷽��ĳ���
* @param dz Z �᷽��ĳ���
* @return �������
*/
CMesh* CGeometryCreator::CreateBox(float dx, float dy, float dz) {
	return CreateBox(dx, dy, dz, false);
}

/**
* ���������岢ָ������
* @param dx X �᷽��ĳ���
* @param dy Y �᷽��ĳ���
* @param dz Z �᷽��ĳ���
* @param reversed �淨����
* @return �������
*/
CMesh* CGeometryCreator::CreateBox(float dx, float dy, float dz, bool reversed) {
	return CreateBox(dx, dy, dz, reversed, CColor::White);
}

/**
* ���������岢ָ���������ɫ
* @param dx X �᷽��ĳ���
* @param dy Y �᷽��ĳ���
* @param dz Z �᷽��ĳ���
* @param reversed �淨����
* @param color ��ɫ
* @return �������
*/
CMesh* CGeometryCreator::CreateBox(float dx, float dy, float dz, bool reversed, const CColor& color) {
	CMesh* pMesh = new CMesh();
	pMesh->SetVertexUsage(24);
	dx *= 0.5f;
	dy *= 0.5f;
	dz *= 0.5f;
	// ǰ
	pMesh->AddVertex(CVertex(-dx, -dy, -dz, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex( dx, -dy, -dz, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex(-dx, -dy,  dz, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex( dx, -dy,  dz, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, color.m_fValue));
	// ��
	pMesh->AddVertex(CVertex( dx, -dy, -dz, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex( dx,  dy, -dz, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex( dx, -dy,  dz, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex( dx,  dy,  dz, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, color.m_fValue));
	// ��
	pMesh->AddVertex(CVertex( dx,  dy, -dz, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex(-dx,  dy, -dz, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex( dx,  dy,  dz, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex(-dx,  dy,  dz, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, color.m_fValue));
	// ��
	pMesh->AddVertex(CVertex(-dx,  dy, -dz, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex(-dx, -dy, -dz, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex(-dx,  dy,  dz, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, color.m_fValue));
	pMesh->AddVertex(CVertex(-dx, -dy,  dz, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, color.m_fValue));
	// ��
	pMesh->AddVertex(CVertex(-dx, -dy,  dz, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, color.m_fValue));
	pMesh->AddVertex(CVertex( dx, -dy,  dz, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, color.m_fValue));
	pMesh->AddVertex(CVertex(-dx,  dy,  dz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, color.m_fValue));
	pMesh->AddVertex(CVertex( dx,  dy,  dz, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, color.m_fValue));
	// ��
	pMesh->AddVertex(CVertex(-dx,  dy, -dz, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, color.m_fValue));
	pMesh->AddVertex(CVertex( dx,  dy, -dz, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, color.m_fValue));
	pMesh->AddVertex(CVertex(-dx, -dy, -dz, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, color.m_fValue));
	pMesh->AddVertex(CVertex( dx, -dy, -dz, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, color.m_fValue));
	// ��������
	for (int i = 0; i < 24; i += 4) {
		pMesh->AddTriangle(i + 0, i + 1, i + 2);
		pMesh->AddTriangle(i + 1, i + 3, i + 2);
	}
	pMesh->Reverse(reversed, reversed, false);
	pMesh->Create(false);
	return pMesh;
}

/**
* ��������
* @param radius ����뾶
* @param slices ����ϸ����
* @param stacks ����ϸ����
* @return �������
*/
CMesh* CGeometryCreator::CreateSphere(float radius, int slices, int stacks) {
	return CreateSphere(radius, slices, stacks, false);
}

/**
* �������岢ָ������
* @param radius ����뾶
* @param slices ����ϸ����
* @param stacks ����ϸ����
* @param reversed �淨����
* @return �������
*/
CMesh* CGeometryCreator::CreateSphere(float radius, int slices, int stacks, bool reversed) {
	return CreateSphere(radius, slices, stacks, reversed, CColor::White);
}

/**
* �������岢ָ���������ɫ
* @param radius ����뾶
* @param slices ����ϸ����
* @param stacks ����ϸ����
* @param reversed �淨����
* @param color ��ɫ
* @return �������
*/
CMesh* CGeometryCreator::CreateSphere(float radius, int slices, int stacks, bool reversed, const CColor& color) {
	CMesh* pMesh = new CMesh();
	pMesh->SetVertexUsage((slices + 1) * (stacks + 2));
	const float PI = 3.14159265f;
	const float stepZ = PI / (stacks + 1);
	const float stepXY = 2 * PI / slices;

	for (int i = 0; i < stacks + 2; i++) {
		float nxy = sinf(i * stepZ);
		float nz = -cosf(i * stepZ);
		float z = nz * radius;
		float t = 1.0f - 1.0f * i / (stacks + 1);
		for (int j = 0; j <= slices; j++) {
			float nx = cosf(j * stepXY) * nxy;
			float ny = sinf(j * stepXY) * nxy;
			float s = 1.0f * j / slices;
			pMesh->AddVertex(CVertex(nx * radius, ny * radius, z, s, t, nx, ny, nz, color.m_fValue));
		}
	}
	// ��������������
	for (int i = 0; i <= stacks; i++) {
		for (int j = 0; j < slices; j++) {
			int index1 = (slices + 1) * i + j;
			int index2 = index1 + 1;
			int index3 = (slices + 1) * (i + 1) + j;
			int index4 = index3 + 1;
			pMesh->AddTriangle(index1, index2, index3);
			pMesh->AddTriangle(index2, index4, index3);
		}
	}
	pMesh->Reverse(reversed, reversed, false);
	pMesh->Create(false);
	return pMesh;
}

/**
* ����������
* @param radius ���������뾶
* @param height ������߶�
* @param slices ϸ�ֲ���
* @return �������
*/
CMesh* CGeometryCreator::CreateCapsule(float radius, float height, int slices) {
	return CreateCapsule(radius, height, slices, false);
}

/**
* ���������岢ָ������
* @param radius ���������뾶
* @param height ������߶�
* @param slices ϸ�ֲ���
* @param reversed �淨����
* @return �������
*/
CMesh* CGeometryCreator::CreateCapsule(float radius, float height, int slices, bool reversed) {
	return CreateCapsule(radius, height, slices, reversed, CColor::White);
}

/**
* ���������岢ָ���������ɫ
* @param radius ���������뾶
* @param height ������߶�
* @param slices ϸ�ֲ���
* @param reversed �淨����
* @param color ��ɫ
* @return �������
*/
CMesh* CGeometryCreator::CreateCapsule(float radius, float height, int slices, bool reversed, const CColor& color) {
	CMesh* pMesh = new CMesh();
	pMesh->SetVertexUsage(2 * (slices + 1) * slices);
	const float PI = 3.14159265f;
	const float stepZ = PI / (2 * slices);
	const float stepXY = 2 * PI / slices;

	for (int i = 0; i <= slices; i++) {
		float nxy = sinf(stepZ * i);
		float nz = cosf(stepZ * i);
		float z = radius * nz + 0.5f * height;
		float t = radius * (1.0f - nz) / (height + 2.0f * radius);
		for (int j = 0; j <= slices; j++) {
			float nx = cosf(stepXY * j) * nxy;
			float ny = sinf(stepXY * j) * nxy;
			float s = 1.0f * j / slices;
			pMesh->AddVertex(CVertex(nx * radius, ny * radius, z, s, t, nx, ny, nz, color.m_fValue));
			pMesh->AddVertex(CVertex(nx * radius, ny * radius, -z, s, 1.0f - t, nx, ny, -nz, color.m_fValue));
		}
	}
	// ��������������
	for (int i = 0; i < slices; i++) {
		for (int j = 0; j < slices; j++) {
			int index1 = i * (slices + 1) * 2 + j * 2;
			int index2 = (slices + 1) * 2 + index1;
			pMesh->AddTriangle(index1, index2, index2 + 2);
			pMesh->AddTriangle(index1, index2 + 2, index1 + 2);
			pMesh->AddTriangle(index2 + 1, index1 + 1, index1 + 3);
			pMesh->AddTriangle(index2 + 1, index1 + 3, index2 + 3);
		}
		// ����������
		int idx = 2 * slices * (slices + 1) + i * 2;
		pMesh->AddTriangle(idx, idx + 1, idx + 3);
		pMesh->AddTriangle(idx, idx + 3, idx + 2);
	}
	pMesh->Reverse(reversed, reversed, false);
	pMesh->Create(false);
	return pMesh;
}

/**
* ����Բ����
* @param radius Բ�������뾶
* @param height Բ����߶�
* @param slices ϸ�ֲ���
* @return �������
*/
CMesh* CGeometryCreator::CreateCylinder(float radius, float height, int slices) {
	return CreateCylinder(radius, height, slices, false);
}

/**
* ����Բ���岢ָ������
* @param radius Բ�������뾶
* @param height Բ����߶�
* @param slices ϸ�ֲ���
* @param reversed �淨����
* @return �������
*/
CMesh* CGeometryCreator::CreateCylinder(float radius, float height, int slices, bool reversed) {
	return CreateCylinder(radius, height, slices, reversed, CColor::White);
}

/**
* ����Բ���岢ָ���������ɫ
* @param radius Բ�������뾶
* @param height Բ����߶�
* @param slices ϸ�ֲ���
* @param reversed �淨����
* @param color ��ɫ
* @return �������
*/
CMesh* CGeometryCreator::CreateCylinder(float radius, float height, int slices, bool reversed, const CColor& color) {
	CMesh* pMesh = new CMesh();
	pMesh->SetVertexUsage((slices + 1) * 4);
	const float PI = 3.14159265f;
	const float step = 2 * PI / slices;
	const float z = height / 2;

	for (int i = 0; i <= slices; i++) {
		float nx = cosf(i * step);
		float ny = sinf(i * step);
		float x = nx * radius;
		float y = ny * radius;
		float s = 1.0f * i / slices;
		// �����϶�����¶���
		pMesh->AddVertex(CVertex(x, y, z, s, 0, nx, ny, 0, color.m_fValue));
		pMesh->AddVertex(CVertex(x, y, -z, s, 1, nx, ny, 0, color.m_fValue));
		// �����͵ײ��涥��
		if (i != slices) {
			pMesh->AddVertex(CVertex(x, y, z, 0, 0, 0, 0, 1, color.m_fValue));
			pMesh->AddVertex(CVertex(x, y, -z, 0, 0, 0, 0, -1, color.m_fValue));
		}
	}
	// �����͵ײ����Ķ���
	int baseIndex = pMesh->GetVertexCount();
	pMesh->AddVertex(CVertex(0, 0, z, 0, 0, 0, 0, 1, color.m_fValue));
	pMesh->AddVertex(CVertex(0, 0, -z, 0, 0, 0, 0, -1, color.m_fValue));

	// ����������
	for (int i = 0; i < slices; i++) {
		int index = i << 2;
		pMesh->AddTriangle(index + 1, index + 5, index);
		pMesh->AddTriangle(index + 5, index + 4, index);
	}
	// �ײ��Ͷ���������
	for (int i = 0; i < slices; i++) {
		int index = (i << 2) + 2;
		if (i + 1 == slices) {
			pMesh->AddTriangle(2, baseIndex, index);
			pMesh->AddTriangle(index + 1, baseIndex + 1, 3);
		} else {
			pMesh->AddTriangle(index + 4, baseIndex, index);
			pMesh->AddTriangle(index + 1, baseIndex + 1, index + 5);
		}
	}
	pMesh->Reverse(reversed, reversed, false);
	pMesh->Create(false);
	return pMesh;
}

/**
* ����Բ��
* @param radius Բ���뾶
* @param thickness ����뾶
* @param slices ϸ�ֲ���
* @return �������
*/
CMesh* CGeometryCreator::CreateTorus(float radius, float thickness, int slices) {
	return CreateTorus(radius, thickness, slices, false);
}

/**
* ����Բ����ָ������
* @param radius Բ���뾶
* @param thickness ����뾶
* @param slices ϸ�ֲ���
* @param reversed �淨����
* @return �������
*/
CMesh* CGeometryCreator::CreateTorus(float radius, float thickness, int slices, bool reversed) {
	return CreateTorus(radius, thickness, slices, reversed, CColor::White);
}

/**
* ����Բ����ָ���������ɫ
* @param radius Բ���뾶
* @param thickness ����뾶
* @param slices ϸ�ֲ���
* @param reversed �淨����
* @param color ��ɫ
* @return �������
*/
CMesh* CGeometryCreator::CreateTorus(float radius, float thickness, int slices, bool reversed, const CColor& color) {
	CMesh* pMesh = new CMesh();
	pMesh->SetVertexUsage((slices + 1) * (slices + 1));
	const float PI = 3.14159265f;

	for (int i = 0; i <= slices; i++) {
		float s = 1.0f * i / slices;
		CMatrix4 transform;
		transform.Translate(radius, 0, 0);
		transform.RotateZ(PI * 2.0f * i / slices);
		// ���涥��ͨ���任Բ�ܶ�������õ�
		for (int j = 0; j <= slices; j++) {
			float t = 1.0f - 1.0f * j / slices;
			float theta = PI * 2.0f * j / slices + PI;
			CVector3 n(cosf(theta), 0, sinf(theta), 0);
			CVector3 p(n[0] * thickness, 0, n[2] * thickness, 1);
			// �Խ��淨���������б任
			n = transform * n;
			p = transform * p;
			pMesh->AddVertex(CVertex(p[0], p[1], p[2], s, t, n[0], n[1], n[2], color.m_fValue));
		}
	}
	// ��������������
	for (int i = 0; i < slices; i++) {
		for (int j = 0; j < slices; j++) {
			unsigned int index = (slices + 1) * i + j;
			pMesh->AddTriangle(index, index + slices + 1, index + 1);
			pMesh->AddTriangle(index + slices + 1, index + slices + 2, index + 1);
		}
	}
	pMesh->Reverse(reversed, reversed, false);
	pMesh->Create(false);
	return pMesh;
}

/**
* ����׵��
* @param radius ׵�����뾶
* @param height ׵��߶�
* @param slices ϸ�ֲ���
* @return �������
*/
CMesh* CGeometryCreator::CreateCone(float radius, float height, int slices) {
	return CreateCone(radius, height, slices, false);
}

/**
* ����׵�岢ָ������
* @param radius ׵�����뾶
* @param height ׵��߶�
* @param slices ϸ�ֲ���
* @param reversed �淨����
* @return �������
*/
CMesh* CGeometryCreator::CreateCone(float radius, float height, int slices, bool reversed) {
	return CreateCone(radius, height, slices, reversed, CColor::White);
}

/**
* ����׵�岢ָ���������ɫ
* @param radius ׵�����뾶
* @param height ׵��߶�
* @param slices ϸ�ֲ���
* @param reversed �淨����
* @param color ��ɫ
* @return �������
*/
CMesh* CGeometryCreator::CreateCone(float radius, float height, int slices, bool reversed, const CColor& color) {
	CMesh* pMesh = new CMesh();
	pMesh->SetVertexUsage((slices + 1) * (slices + 2) + 1);
	const float PI = 3.14159265f;
	const float step = 2 * PI / slices;
	const float nz = atan2f(radius, height);
	const float offset = -height / 4.0f;
	// ��Ϊ����͵���
	for (int i = 0; i <= slices; i++) {
		float nx = cosf(i * step);
		float ny = sinf(i * step);
		float x = nx * radius;
		float y = ny * radius;
		float s = 1.0f * i / slices;
		for (int j = 0; j <= slices; j++) {
			float t = 1.0f - 1.0f * j / slices;
			float z = height * (1.0f - t);
			pMesh->AddVertex(CVertex(x * t, y * t, z + offset, s, t, nx, ny, nz, color.m_fValue));
		}
		pMesh->AddVertex(CVertex(x, y, offset, 0, 0, 0, 0, -1, color.m_fValue));
	}
	// �������ĵ�
	pMesh->AddVertex(CVertex(0, 0, offset, 0, 0, 0, 0, -1, color.m_fValue));
	// ���������
	unsigned int bottomCenter = (slices + 1) * (slices + 2);
	for (int i = 0; i < slices; i++) {
		int baseIndex1 = i * (slices + 2);
		int baseIndex2 = (i + 1) * (slices + 2);
		for (int j = 0; j < slices; j++) {
			int index1 = baseIndex1 + j;
			int index2 = baseIndex2 + j;
			pMesh->AddTriangle(index1, index2, index1 + 1);
			pMesh->AddTriangle(index1 + 1, index2, index2 + 1);
		}
		pMesh->AddTriangle(bottomCenter, baseIndex2 + slices + 1, baseIndex1 + slices + 1);
	}
	pMesh->Reverse(reversed, reversed, false);
	pMesh->Create(false);
	return pMesh;
}

/**
* ����ƽ��
* @param width ƽ����
* @param height ƽ��߶�
* @param axis ƽ�淨����������ķ���1,2,3,-1,-2,-3 �ֱ��ʾ XYZ ��������͸�����
* @return �������
*/
CMesh* CGeometryCreator::CreatePlane(float width, float height, int axis) {
	return CreatePlane(width, height, axis, false);
}

/**
* ����ƽ�沢ָ������
* @param width ƽ����
* @param height ƽ��߶�
* @param axis ƽ�淨����������ķ���1,2,3,-1,-2,-3 �ֱ��ʾ XYZ ��������͸�����
* @param reversed �淨����
* @return �������
*/
CMesh* CGeometryCreator::CreatePlane(float width, float height, int axis, bool reversed) {
	return CreatePlane(width, height, axis, reversed, CColor::White);
}

/**
* ����ƽ�沢ָ���������ɫ
* @param width ƽ����
* @param height ƽ��߶�
* @param axis ƽ�淨����������ķ���1,2,3,-1,-2,-3 �ֱ��ʾ XYZ ��������͸�����
* @param reversed �淨����
* @param color ��ɫ
* @return �������
*/
CMesh* CGeometryCreator::CreatePlane(float width, float height, int axis, bool reversed, const CColor& color) {
	CMesh* pMesh = new CMesh();
	pMesh->SetVertexUsage(4);
	width *= 0.5f;
	height *= 0.5f;
	switch (axis) {
	case -1: case 1:
		pMesh->AddVertex(CVertex(0, -width, -height, 0, 1, 1, 0, 0, color.m_fValue));
		pMesh->AddVertex(CVertex(0,  width, -height, 1, 1, 1, 0, 0, color.m_fValue));
		pMesh->AddVertex(CVertex(0, -width,  height, 0, 0, 1, 0, 0, color.m_fValue));
		pMesh->AddVertex(CVertex(0,  width,  height, 1, 0, 1, 0, 0, color.m_fValue));
		break;
	case -2: case 2:
		pMesh->AddVertex(CVertex( width, 0, -height, 0, 1, 0, 1, 0, color.m_fValue));
		pMesh->AddVertex(CVertex(-width, 0, -height, 1, 1, 0, 1, 0, color.m_fValue));
		pMesh->AddVertex(CVertex( width, 0,  height, 0, 0, 0, 1, 0, color.m_fValue));
		pMesh->AddVertex(CVertex(-width, 0,  height, 1, 0, 0, 1, 0, color.m_fValue));
		break;
	case -3: case 3:
		pMesh->AddVertex(CVertex(-width, -height, 0, 0, 1, 0, 0, 1, color.m_fValue));
		pMesh->AddVertex(CVertex( width, -height, 0, 1, 1, 0, 0, 1, color.m_fValue));
		pMesh->AddVertex(CVertex(-width,  height, 0, 0, 0, 0, 0, 1, color.m_fValue));
		pMesh->AddVertex(CVertex( width,  height, 0, 1, 0, 0, 0, 1, color.m_fValue));
		break;
	}
	pMesh->AddTriangle(0, 1, 2);
	pMesh->AddTriangle(1, 3, 2);
	if (axis < 0) reversed = !reversed;
	pMesh->Reverse(reversed, reversed, false);
	pMesh->Create(false);
	return pMesh;
}

/**
* ����Բ
* @param radius Բ�뾶
* @param axis ƽ�淨����������ķ���1,2,3,-1,-2,-3 �ֱ��ʾ XYZ ��������͸�����
* @param slices Բ��ϸ����
* @return �������
*/
CMesh* CGeometryCreator::CreateCircle(float radius, int axis, int slices) {
	return CreateCircle(radius, axis, slices, false);
}

/**
* ����Բ��ָ������
* @param radius Բ�뾶
* @param axis ƽ�淨����������ķ���1,2,3,-1,-2,-3 �ֱ��ʾ XYZ ��������͸�����
* @param slices Բ��ϸ����
* @param reversed �淨����
* @return �������
*/
CMesh* CGeometryCreator::CreateCircle(float radius, int axis, int slices, bool reversed) {
	return CreateCircle(radius, axis, slices, reversed, CColor::White);
}

/**
* ����Բ��ָ���������ɫ
* @param radius Բ�뾶
* @param axis ƽ�淨����������ķ���1,2,3,-1,-2,-3 �ֱ��ʾ XYZ ��������͸�����
* @param slices Բ��ϸ����
* @param reversed �淨����
* @param color ��ɫ
* @return �������
*/
CMesh* CGeometryCreator::CreateCircle(float radius, int axis, int slices, bool reversed, const CColor& color) {
	CMesh* pMesh = new CMesh();
	pMesh->SetVertexUsage(slices + 2);
	const float PI = 3.14159265f;
	const float step = 2 * PI / slices;
	// ���Բ��
	switch (axis) {
	case -1: case 1: pMesh->AddVertex(CVertex(0, 0, 0, 0.5f, 0.5f, 1, 0, 0, color.m_fValue)); break;
	case -2: case 2: pMesh->AddVertex(CVertex(0, 0, 0, 0.5f, 0.5f, 0, 1, 0, color.m_fValue)); break;
	case -3: case 3: pMesh->AddVertex(CVertex(0, 0, 0, 0.5f, 0.5f, 0, 0, 1, color.m_fValue)); break;
	}
	// Բ���з�
	for (int i = 0; i <= slices; i++) {
		float x = cosf(i * step);
		float y = sinf(i * step);
		float s = 0.5f + x * 0.5f;
		float t = 0.5f - y * 0.5f;
		switch (axis) {
		case -1: case 1: pMesh->AddVertex(CVertex(0, x * radius, y * radius, s, t, 1, 0, 0, color.m_fValue)); break;
		case -2: case 2: pMesh->AddVertex(CVertex(x * radius, 0, y * radius, s, t, 0, 1, 0, color.m_fValue)); break;
		case -3: case 3: pMesh->AddVertex(CVertex(x * radius, y * radius, 0, s, t, 0, 0, 1, color.m_fValue)); break;
		}
	}
	// ���������
	for (int i = 1; i <= slices; i++) {
		pMesh->AddTriangle(i, i + 1, 0);
	}
	if (axis < 0) reversed = !reversed;
	pMesh->Reverse(reversed, reversed, false);
	pMesh->Create(false);
	return pMesh;
}

/**
* ������ͷ����ͷ�������׵����϶���
* @param r1 ����뾶
* @param r2 ׵��뾶
* @param h1 �����
* @param h2 ׵���
* @param slices ϸ�ֲ���
* @return �������
*/
CMesh* CGeometryCreator::CreateArrow(float r1, float r2, float h1, float h2, int slices) {
	return CreateArrow(r1, r2, h1, h2, slices, false);
}

/**
* ������ͷ��ָ������
* @param r1 ����뾶
* @param r2 ׵��뾶
* @param h1 �����
* @param h2 ׵���
* @param slices ϸ�ֲ���
* @param reversed �淨����
* @return �������
*/
CMesh* CGeometryCreator::CreateArrow(float r1, float r2, float h1, float h2, int slices, bool reversed) {
	return CreateArrow(r1, r2, h1, h2, slices, reversed, CColor::White);
}

/**
* ������ͷ��ָ���������ɫ
* @param r1 ����뾶
* @param r2 ׵��뾶
* @param h1 �����
* @param h2 ׵���
* @param slices ϸ�ֲ���
* @param reversed �淨����
* @param color ��ɫ
* @return �������
*/
CMesh* CGeometryCreator::CreateArrow(float r1, float r2, float h1, float h2, int slices, bool reversed, const CColor& color) {
	CMesh* pMesh = new CMesh();
	pMesh->SetVertexUsage((slices + 1) * 6 + 2);
	const float PI = 3.14159265f;
	const float step = 2 * PI / slices;
	const float nz = atan2f(r2, h2);
	const float t = h2 / (h1 + h2);

	for (int i = 0; i <= slices; i++) {
		float nx = cosf(i * step);
		float ny = sinf(i * step);
		float x1 = nx * r1;
		float y1 = ny * r1;
		float x2 = nx * r2;
		float y2 = ny * r2;
		float s = 1.0f * i / slices;
		// ���嶥��
		pMesh->AddVertex(CVertex(x1, y1, 0, s, 1, nx, ny, 0, color.m_fValue));
		pMesh->AddVertex(CVertex(x1, y1, h1, s, t, nx, ny, 0, color.m_fValue));
		pMesh->AddVertex(CVertex(x1, y1, 0, 0, 0, 0, 0, -1, color.m_fValue));
		// ׵�嶥��
		pMesh->AddVertex(CVertex(x2, y2, h1, s, t, nx, ny, nz, color.m_fValue));
		pMesh->AddVertex(CVertex(0, 0, h1 + h2, s, 0, nx, ny, nz, color.m_fValue));
		pMesh->AddVertex(CVertex(x2, y2, h1, 0, 0, 0, 0, -1, color.m_fValue));
	}
	// ����ײ����Ķ���
	pMesh->AddVertex(CVertex(0, 0, 0, 0, 0, 0, 0, -1, color.m_fValue));
	// ׵��������Ķ���
	pMesh->AddVertex(CVertex(0, 0, h1, 0, 0, 0, 0, -1, color.m_fValue));

	// ���������
	unsigned int base1 = (slices + 1) * 6;
	unsigned int base2 = base1 + 1;
	for (int i = 0; i < slices; i++) {
		unsigned int index = i * 6;
		pMesh->AddTriangle(index, index + 6, index + 1);
		pMesh->AddTriangle(index + 6, index + 7, index + 1);
		pMesh->AddTriangle(index + 3, index + 9, index + 4);
		pMesh->AddTriangle(index + 9, index + 10, index + 4);
		// ����
		pMesh->AddTriangle(index + 2, base1, index + 8);
		pMesh->AddTriangle(index + 5, base2, index + 11);
	}
	pMesh->Reverse(reversed, reversed, false);
	pMesh->Create(false);
	return pMesh;
}

/**
* ����������
* @param polygon ����涥�㣬����պ�
* @param count ����涥�����
* @param line �����߶�
* @param length �����߶ζ�����
* @return �������
*/
CMesh* CGeometryCreator::CreateExtrude(const CVector2* polygon, int count, const CVector3* line, int length) {
	return CreateExtrude(polygon, count, line, length, false);
}

/**
* ���������壬�������Ƿ�ƽ������
* @param polygon ����涥�㣬����պ�
* @param count ����涥�����
* @param line �����߶�
* @param length �����߶ζ�����
* @param smooth �Ƿ�ƽ������
* @return �������
*/
CMesh* CGeometryCreator::CreateExtrude(const CVector2* polygon, int count, const CVector3* line, int length, bool smooth) {
	return CreateExtrude(polygon, count, line, length, smooth, CColor::White);
}

/**
* ���������岢ָ����ɫ
* @param polygon ����涥�㣬����պ�
* @param count ����涥�����
* @param line �����߶�
* @param length �����߶ζ�����
* @param smooth �Ƿ�ƽ������
* @param color ��ɫ
* @return �������
*/
CMesh* CGeometryCreator::CreateExtrude(const CVector2* polygon, int count, const CVector3* line, int length, bool smooth, const CColor& color) {
	// ȥ���߶��ײ���ͬ�ĵ�
	while (length > 1 && line[1] == line[0]) { ++line; --length; }
	if (count < 2 || length < 2) return 0;

	CMesh* pMesh = new CMesh();
	pMesh->SetVertexUsage(smooth ? count * length * 2 : (count - 2) * length * 4);
	// ���������ת
	CVector3 dir = line[1] - line[0];
	CVector3 axis = CVector3::Z.CrossProduct(dir);
	if (axis.Length() == 0.0f) axis.SetValue(dir.Tangent());
	float cosa = CVector3::Z.DotProduct(dir.Normalize());
	if (cosa < -1.0f) cosa = -1.0f;
	else if (cosa > 1.0f) cosa = 1.0f;
	CQuaternion rotate;
	rotate.FromAxisAngle(axis, acosf(cosa));
	// ��������ܳ�����������U
	float* perimeter = new float[count];
	float* texCoordU = new float[count];
	perimeter[0] = 0.0f;
	texCoordU[0] = 0.0f;
	for (int i = 1; i < count; i++) perimeter[i] = perimeter[i - 1] + (polygon[i] - polygon[i - 1]).Length();
	for (int i = 1; i < count; i++) texCoordU[i] = perimeter[i] / perimeter[count - 1];
	// ��ʼѭ������յ���涥��
	float lineLengthSum = 0.0f;
	for (int i = 0; i < length; i++) {
		// �������棬���淨��Ϊǰһ���߶���������һ���߶�����֮��
		CVector3 next = line[i + 1 == length? i: i + 1] - line[i];
		float distance = next.Length();
		if (distance == 0.0f) next.SetValue(dir);
		CPlane clipPlane(line[i], dir + next.Normalize());
		CPlane projPlane1(line[i], dir);
		CPlane projPlane2(line[i], distance == 0.0f? dir: next);
		float cosin = dir.DotProduct(next);
		if (cosin < -1.0f) cosin = -1.0f;
		else if (cosin > 1.0f) cosin = 1.0f;
		const float rotateAngle = acosf(cosin);
		for (int j = 0; j < count; j++) {
			// ���潻��ͨ������任��Ľ��涥�����߶η���������󽻵õ�
			CVector3 prj;
			CVector3 org = line[i] + rotate * CVector3(polygon[j].m_fValue[0], polygon[j].m_fValue[1], 0.0f);
			clipPlane.IntersectLine(org, dir, prj);
			// ���߶γ���Ϊ��׼���Ժ�����ܳ�Ϊ��������ȷ�������췽�����ͼ����
			float texCoordV1 = (lineLengthSum + projPlane1.Distance(prj)) / perimeter[count - 1];
			float texCoordV2 = (lineLengthSum + projPlane2.Distance(prj)) / perimeter[count - 1];
			pMesh->AddVertex(CVertex(prj[0], prj[1], prj[2], texCoordU[j], texCoordV1, 0, 0, 0, color.m_fValue));
			pMesh->AddVertex(CVertex(prj[0], prj[1], prj[2], texCoordU[j], texCoordV2, 0, 0, 0, color.m_fValue));
			if (!smooth && j > 0 && j < count - 1) {
				pMesh->AddVertex(CVertex(prj[0], prj[1], prj[2], texCoordU[j], texCoordV1, 0, 0, 0, color.m_fValue));
				pMesh->AddVertex(CVertex(prj[0], prj[1], prj[2], texCoordU[j], texCoordV2, 0, 0, 0, color.m_fValue));
			}
		}
		lineLengthSum += distance;
		// ���㵽��һ���յ����ת
		if (fabs(rotateAngle) > 1E-9) {
			CQuaternion rot;
			rotate = rot.FromAxisAngle(dir.CrossProduct(next), rotateAngle) * rotate;
			rotate.Normalize();
		}
		dir.SetValue(next);
	}
	// ���������
	for (int i = 0; i < length - 1; i++) {
		for (int j = 0; j < count - 1; j++) {
			int index1 = 2 * (count * i + j) + 1;
			int index2 = 2 * (count * (i + 1) + j);
			if (!smooth) {
				index1 = 4 * ((count - 1) * i + j) + 1;
				index2 = 4 * ((count - 1) * (i + 1) + j);
			}
			pMesh->AddTriangle(index1, index1 + 2, index2);
			pMesh->AddTriangle(index1 + 2, index2 + 2, index2);
		}
	}
	delete[] perimeter;
	delete[] texCoordU;
	pMesh->SetupNormal();
	// ƽ������
	if (smooth) {
		for (int i = 0; i < length; i++) {
			int m = 2 * count * i;
			int n = 2 * count * (i + 1);
			CVertex* v11 = pMesh->GetVertex(m);
			CVertex* v12 = pMesh->GetVertex(n - 2);
			CVertex* v21 = pMesh->GetVertex(m + 1);
			CVertex* v22 = pMesh->GetVertex(n - 1);
			CVector3 n1 = CVector3(v11->m_fNormal) + CVector3(v12->m_fNormal);
			CVector3 n2 = CVector3(v21->m_fNormal) + CVector3(v22->m_fNormal);
			v11->SetNormal(n1.Normalize());
			v12->SetNormal(n1);
			v21->SetNormal(n2.Normalize());
			v22->SetNormal(n2);
		}
	}
	pMesh->Create(false);
	return pMesh;
}