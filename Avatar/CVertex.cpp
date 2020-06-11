//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CVertex.h"

/**
* 构造函数
*/
CVertex::CVertex() {
	m_fPosition[0] = 0.0f;
	m_fPosition[1] = 0.0f;
	m_fPosition[2] = 0.0f;
	m_fPosition[3] = 1.0f;
	m_fTexCoord[0] = 0.0f;
	m_fTexCoord[1] = 0.0f;
	m_fNormal[0] = 0.0f;
	m_fNormal[1] = 0.0f;
	m_fNormal[2] = 1.0f;
	m_fNormal[3] = 0.0f;
	m_fColor[0] = 1.0f;
	m_fColor[1] = 1.0f;
	m_fColor[2] = 1.0f;
	m_fColor[3] = 1.0f;
}

/**
* 复制构造函数
*/
CVertex::CVertex(const CVertex& vertex) {
	m_fPosition[0] = vertex.m_fPosition[0];
	m_fPosition[1] = vertex.m_fPosition[1];
	m_fPosition[2] = vertex.m_fPosition[2];
	m_fPosition[3] = vertex.m_fPosition[3];
	m_fTexCoord[0] = vertex.m_fTexCoord[0];
	m_fTexCoord[1] = vertex.m_fTexCoord[1];
	m_fNormal[0] = vertex.m_fNormal[0];
	m_fNormal[1] = vertex.m_fNormal[1];
	m_fNormal[2] = vertex.m_fNormal[2];
	m_fNormal[3] = vertex.m_fNormal[3];
	m_fColor[0] = vertex.m_fColor[0];
	m_fColor[1] = vertex.m_fColor[1];
	m_fColor[2] = vertex.m_fColor[2];
	m_fColor[3] = vertex.m_fColor[3];
}

/**
* 构造函数，仅指定顶点坐标
*/
CVertex::CVertex(float x, float y, float z) {
	m_fPosition[0] = x;
	m_fPosition[1] = y;
	m_fPosition[2] = z;
	m_fPosition[3] = 1.0f;
	m_fTexCoord[0] = 0.0f;
	m_fTexCoord[1] = 0.0f;
	m_fNormal[0] = 0.0f;
	m_fNormal[1] = 0.0f;
	m_fNormal[2] = 1.0f;
	m_fNormal[3] = 0.0f;
	m_fColor[0] = 1.0f;
	m_fColor[1] = 1.0f;
	m_fColor[2] = 1.0f;
	m_fColor[3] = 1.0f;
}

/**
* 构造函数，指定顶点坐标及纹理坐标
*/
CVertex::CVertex(float x, float y, float z, float s, float t) {
	m_fPosition[0] = x;
	m_fPosition[1] = y;
	m_fPosition[2] = z;
	m_fPosition[3] = 1.0f;
	m_fTexCoord[0] = s;
	m_fTexCoord[1] = t;
	m_fNormal[0] = 0.0f;
	m_fNormal[1] = 0.0f;
	m_fNormal[2] = 1.0f;
	m_fNormal[3] = 0.0f;
	m_fColor[0] = 1.0f;
	m_fColor[1] = 1.0f;
	m_fColor[2] = 1.0f;
	m_fColor[3] = 1.0f;
}

/**
* 构造函数，指定顶点坐标和纹理坐标以及法向量
*/
CVertex::CVertex(float x, float y, float z, float s, float t, float nx, float ny, float nz) {
	m_fPosition[0] = x;
	m_fPosition[1] = y;
	m_fPosition[2] = z;
	m_fPosition[3] = 1.0f;
	m_fTexCoord[0] = s;
	m_fTexCoord[1] = t;
	m_fNormal[0] = nx;
	m_fNormal[1] = ny;
	m_fNormal[2] = nz;
	m_fNormal[3] = 0.0f;
	m_fColor[0] = 1.0f;
	m_fColor[1] = 1.0f;
	m_fColor[2] = 1.0f;
	m_fColor[3] = 1.0f;
}

/**
* 构造函数，指定顶点坐标和纹理坐标和法向量以及颜色
*/
CVertex::CVertex(float x, float y, float z, float s, float t, float nx, float ny, float nz, const float color[4]) {
	m_fPosition[0] = x;
	m_fPosition[1] = y;
	m_fPosition[2] = z;
	m_fPosition[3] = 1.0f;
	m_fTexCoord[0] = s;
	m_fTexCoord[1] = t;
	m_fNormal[0] = nx;
	m_fNormal[1] = ny;
	m_fNormal[2] = nz;
	m_fNormal[3] = 0.0f;
	m_fColor[0] = color[0];
	m_fColor[1] = color[1];
	m_fColor[2] = color[2];
	m_fColor[3] = color[3];
}

/**
* 设置顶点位置
*/
void CVertex::SetPosition(const CVector3& pos) {
	m_fPosition[0] = pos.m_fValue[0];
	m_fPosition[1] = pos.m_fValue[1];
	m_fPosition[2] = pos.m_fValue[2];
}

/**
* 设置顶点位置
*/
void CVertex::SetPosition(const float pos[3]) {
	m_fPosition[0] = pos[0];
	m_fPosition[1] = pos[1];
	m_fPosition[2] = pos[2];
}

/**
* 设置顶点位置
*/
void CVertex::SetPosition(float x, float y, float z) {
	m_fPosition[0] = x;
	m_fPosition[1] = y;
	m_fPosition[2] = z;
}

/**
* 设置顶点纹理坐标
*/
void CVertex::SetTexCoord(const CVector2& texCoord) {
	m_fTexCoord[0] = texCoord.m_fValue[0];
	m_fTexCoord[1] = texCoord.m_fValue[1];
}

/**
* 设置顶点纹理坐标
*/
void CVertex::SetTexCoord(const float texCoord[2]) {
	m_fTexCoord[0] = texCoord[0];
	m_fTexCoord[1] = texCoord[1];
}

/**
* 设置顶点纹理坐标
*/
void CVertex::SetTexCoord(float s, float t) {
	m_fTexCoord[0] = s;
	m_fTexCoord[1] = t;
}

/**
* 设置顶点位置
*/
void CVertex::SetNormal(const CVector3& normal) {
	m_fNormal[0] = normal.m_fValue[0];
	m_fNormal[1] = normal.m_fValue[1];
	m_fNormal[2] = normal.m_fValue[2];
}

/**
* 设置顶点法相
*/
void CVertex::SetNormal(const float normal[3]) {
	m_fNormal[0] = normal[0];
	m_fNormal[1] = normal[1];
	m_fNormal[2] = normal[2];
}

/**
* 设置顶点法相
*/
void CVertex::SetNormal(float nx, float ny, float nz) {
	m_fNormal[0] = nx;
	m_fNormal[1] = ny;
	m_fNormal[2] = nz;
}

/**
* 设置顶点颜色
*/
void CVertex::SetColor(const CColor& color) {
	m_fColor[0] = color.m_fValue[0];
	m_fColor[1] = color.m_fValue[1];
	m_fColor[2] = color.m_fValue[2];
	m_fColor[3] = color.m_fValue[3];
}

/**
* 设置顶点颜色
*/
void CVertex::SetColor(const float color[4]) {
	m_fColor[0] = color[0];
	m_fColor[1] = color[1];
	m_fColor[2] = color[2];
	m_fColor[3] = color[3];
}

/**
* 设置顶点颜色
*/
void CVertex::SetColor(float r, float g, float b, float a) {
	m_fColor[0] = r;
	m_fColor[1] = g;
	m_fColor[2] = b;
	m_fColor[3] = a;
}

/**
* 翻转法向量
*/
void CVertex::FlipNormal() {
	m_fNormal[0] = -m_fNormal[0];
	m_fNormal[1] = -m_fNormal[1];
	m_fNormal[2] = -m_fNormal[2];
}

/**
* 线性插值
*/
CVertex CVertex::Interpolate(const CVertex& vertex, float t) {
	float x = vertex.m_fPosition[0] * t + m_fPosition[0] * (1.0f - t);
	float y = vertex.m_fPosition[1] * t + m_fPosition[1] * (1.0f - t);
	float z = vertex.m_fPosition[2] * t + m_fPosition[2] * (1.0f - t);
	float u = vertex.m_fTexCoord[0] * t + m_fTexCoord[0] * (1.0f - t);
	float v = vertex.m_fTexCoord[1] * t + m_fTexCoord[1] * (1.0f - t);
	float nx = vertex.m_fNormal[0] * t + m_fNormal[0] * (1.0f - t);
	float ny = vertex.m_fNormal[1] * t + m_fNormal[1] * (1.0f - t);
	float nz = vertex.m_fNormal[2] * t + m_fNormal[2] * (1.0f - t);
	float color[4] = {
		vertex.m_fColor[0] * t + m_fColor[0] * (1.0f - t),
		vertex.m_fColor[1] * t + m_fColor[1] * (1.0f - t),
		vertex.m_fColor[2] * t + m_fColor[2] * (1.0f - t),
		vertex.m_fColor[3] * t + m_fColor[3] * (1.0f - t)
	};
	return CVertex(x, y, z, u, v, nx, ny, nz, color);
}

/**
* 运算符重载 =
*/
CVertex& CVertex::operator = (const CVertex& vertex) {
	m_fPosition[0] = vertex.m_fPosition[0];
	m_fPosition[1] = vertex.m_fPosition[1];
	m_fPosition[2] = vertex.m_fPosition[2];
	m_fTexCoord[0] = vertex.m_fTexCoord[0];
	m_fTexCoord[1] = vertex.m_fTexCoord[1];
	m_fNormal[0] = vertex.m_fNormal[0];
	m_fNormal[1] = vertex.m_fNormal[1];
	m_fNormal[2] = vertex.m_fNormal[2];
	m_fColor[0] = vertex.m_fColor[0];
	m_fColor[1] = vertex.m_fColor[1];
	m_fColor[2] = vertex.m_fColor[2];
	m_fColor[3] = vertex.m_fColor[3];
	return *this;
}

/**
* 运算符重载 ==
*/
bool CVertex::operator == (const CVertex& vertex) const {
	if (m_fPosition[0] != vertex.m_fPosition[0] ||
		m_fPosition[1] != vertex.m_fPosition[1] ||
		m_fPosition[2] != vertex.m_fPosition[2]) return false;
	if (m_fTexCoord[0] != vertex.m_fTexCoord[0] ||
		m_fTexCoord[1] != vertex.m_fTexCoord[1]) return false;
	if (m_fNormal[0] != vertex.m_fNormal[0] ||
		m_fNormal[1] != vertex.m_fNormal[1] ||
		m_fNormal[2] != vertex.m_fNormal[2]) return false;
	if (m_fColor[0] != vertex.m_fColor[0] ||
		m_fColor[1] != vertex.m_fColor[1] ||
		m_fColor[2] != vertex.m_fColor[2] ||
		m_fColor[3] != vertex.m_fColor[3]) return false;
	return true;
}

/**
* 运算符重载 !=
*/
bool CVertex::operator != (const CVertex& vertex) const {
	return !(*this == vertex);
}