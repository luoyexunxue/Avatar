//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CMesh.h"
#include "AvatarConfig.h"
#include "CLog.h"
#include <algorithm>
#include <iterator>
#include <cmath>
#include <cstring>
#include <map>
#include <list>
using std::map;
using std::list;

#ifdef AVATAR_WINDOWS
#include "thirdparty/glew/glew.h"
#include <GL/gl.h>
#endif
#ifdef AVATAR_LINUX
#include "thirdparty/glew/glew.h"
#include <GL/gl.h>
#endif
#ifdef AVATAR_ANDROID
#include <GLES3/gl3.h>
#endif

/**
* 构造函数
*/
CMesh::CMesh() {
	m_iVertexArraySize = 0;
	m_iIndexArraySize = 0;
	m_pMaterial = new CMaterial();
	m_bDynamic = false;
	m_bCreated = false;
	m_iVertexBuffer = 0;
	m_iIndexBuffer = 0;
}

/**
* 析构函数
*/
CMesh::~CMesh() {
	delete m_pMaterial;
	if (m_bCreated) {
		glDeleteBuffers(1, &m_iVertexBuffer);
		glDeleteBuffers(1, &m_iIndexBuffer);
	}
}

/**
* 设置顶点使用个数
* @param count 顶点个数
*/
void CMesh::SetVertexUsage(size_t count) {
	m_vecVertexArray.reserve(count);
}

/**
* 添加一个顶点
* @param vertex 顶点
*/
void CMesh::AddVertex(const CVertex& vertex) {
	m_vecVertexArray.push_back(vertex);
}

/**
* 添加一个顶点并指定骨骼绑定
* @param vertex 顶点
* @param bind 骨骼绑定
*/
void CMesh::AddVertex(const CVertex& vertex, const CVertexJoint& bind) {
	m_vecVertexArray.push_back(vertex);
	m_vecBindArray.push_back(bind);
}

/**
* 添加一个三角形（索引方式）
* @param a 顶点索引1
* @param b 顶点索引2
* @param c 顶点索引3
*/
void CMesh::AddTriangle(unsigned int a, unsigned int b, unsigned int c) {
	m_vecIndexArray.push_back(a);
	m_vecIndexArray.push_back(b);
	m_vecIndexArray.push_back(c);
}

/**
* 添加一个三角形（顶点方式）
* @param a 顶点1
* @param b 顶点2
* @param c 顶点3
*/
void CMesh::AddTriangle(const CVertex& a, const CVertex& b, const CVertex& c) {
	// 添加顶点
	m_vecVertexArray.push_back(a);
	m_vecVertexArray.push_back(b);
	m_vecVertexArray.push_back(c);
	// 三角形
	unsigned int baseIndex = (unsigned int)m_vecVertexArray.size();
	m_vecIndexArray.push_back(baseIndex - 3);
	m_vecIndexArray.push_back(baseIndex - 2);
	m_vecIndexArray.push_back(baseIndex - 1);
}

/**
* 移除网格顶点
* @param index 起始位置
* @param count 顶点个数
*/
void CMesh::RemoveVertex(size_t index, int count) {
	const size_t vertexCount = m_vecVertexArray.size();
	const size_t bindCount = m_vecBindArray.size();
	const int maxCount = static_cast<int>(vertexCount - index);
	if (count < 0 || count > maxCount) count = maxCount;
	vector<CVertex>::iterator it1 = m_vecVertexArray.begin() + index;
	vector<CVertex>::iterator it2 = m_vecVertexArray.begin() + index + count;
	m_vecVertexArray.erase(it1, it2);
	if (count + index <= bindCount) {
		vector<CVertexJoint>::iterator it3 = m_vecBindArray.begin() + index;
		vector<CVertexJoint>::iterator it4 = m_vecBindArray.begin() + index + count;
		m_vecBindArray.erase(it3, it4);
	}
}

/**
* 移除网格三角形
* @param index 起始位置
* @param count 三角形个数
*/
void CMesh::RemoveTriangle(size_t index, int count) {
	const size_t triangleCount = m_vecIndexArray.size() / 3;
	const int maxCount = static_cast<int>(triangleCount - index);
	if (count < 0 || count > maxCount) count = maxCount;
	vector<unsigned int>::iterator it1 = m_vecIndexArray.begin() + index * 3;
	vector<unsigned int>::iterator it2 = m_vecIndexArray.begin() + (index + count) * 3;
	m_vecIndexArray.erase(it1, it2);
}

/**
* 附加一个网格
* @param mesh 需要添加的网格
*/
void CMesh::Append(const CMesh* mesh) {
	size_t nv = m_vecVertexArray.size();
	size_t ni = m_vecIndexArray.size();
	size_t nb = m_vecBindArray.size();
	m_vecVertexArray.reserve(nv + mesh->m_vecVertexArray.size());
	m_vecIndexArray.reserve(ni + mesh->m_vecIndexArray.size());
	m_vecBindArray.reserve(nb + mesh->m_vecBindArray.size());
	std::copy(mesh->m_vecVertexArray.begin(), mesh->m_vecVertexArray.end(), std::back_inserter(m_vecVertexArray));
	std::copy(mesh->m_vecIndexArray.begin(), mesh->m_vecIndexArray.end(), std::back_inserter(m_vecIndexArray));
	std::copy(mesh->m_vecBindArray.begin(), mesh->m_vecBindArray.end(), std::back_inserter(m_vecBindArray));
	for (size_t i = ni; i < m_vecIndexArray.size(); i++) m_vecIndexArray[i] += (unsigned int)nv;
	if (m_bCreated) Update(0);
}

/**
* 对所有顶点进行矩阵变换
* @param matrix 变换矩阵
*/
void CMesh::Transform(const CMatrix4& matrix) {
	for (size_t i = 0; i < m_vecVertexArray.size(); i++) {
		CVertex* vert = &m_vecVertexArray[i];
		CVector3 position = matrix * CVector3(vert->m_fPosition);
		CVector3 normal = matrix * CVector3(vert->m_fNormal, vert->m_fNormal[3]);
		// 单位化法向并齐次化坐标
		normal.Normalize();
		float t = 1.0f / position[3];
		// 设置变换后的顶点坐标和法向量
		vert->m_fPosition[0] = position[0] * t;
		vert->m_fPosition[1] = position[1] * t;
		vert->m_fPosition[2] = position[2] * t;
		vert->m_fNormal[0] = normal[0];
		vert->m_fNormal[1] = normal[1];
		vert->m_fNormal[2] = normal[2];
		vert->m_fNormal[3] = 0.0f;
	}
	if (m_bCreated) Update(1);
}

/**
* 自动计算顶点法向量
* @note 以平滑的方式计算所有顶点法向量
*/
void CMesh::SetupNormal() {
	for (size_t i = 0; i < m_vecVertexArray.size(); i++) {
		m_vecVertexArray[i].m_fNormal[0] = 0.0f;
		m_vecVertexArray[i].m_fNormal[1] = 0.0f;
		m_vecVertexArray[i].m_fNormal[2] = 0.0f;
	}
	for (size_t i = 0; i < m_vecIndexArray.size(); i += 3) {
		CVertex* va = &m_vecVertexArray[m_vecIndexArray[i + 0]];
		CVertex* vb = &m_vecVertexArray[m_vecIndexArray[i + 1]];
		CVertex* vc = &m_vecVertexArray[m_vecIndexArray[i + 2]];
		const CVector3 e1 = CVector3(va->m_fPosition) - CVector3(vb->m_fPosition);
		const CVector3 e2 = CVector3(vc->m_fPosition) - CVector3(vb->m_fPosition);
		const CVector3 no = e2.CrossProduct(e1);
		va->m_fNormal[0] += no.m_fValue[0];
		va->m_fNormal[1] += no.m_fValue[1];
		va->m_fNormal[2] += no.m_fValue[2];
		vb->m_fNormal[0] += no.m_fValue[0];
		vb->m_fNormal[1] += no.m_fValue[1];
		vb->m_fNormal[2] += no.m_fValue[2];
		vc->m_fNormal[0] += no.m_fValue[0];
		vc->m_fNormal[1] += no.m_fValue[1];
		vc->m_fNormal[2] += no.m_fValue[2];
	}
	for (size_t i = 0; i < m_vecVertexArray.size(); i++) {
		float* normal = m_vecVertexArray[i].m_fNormal;
		float scale = 1.0f / sqrtf(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
		normal[0] = normal[0] * scale;
		normal[1] = normal[1] * scale;
		normal[2] = normal[2] * scale;
	}
	if (m_bCreated) Update(1);
}

/**
* 反转顶点法向和纹理坐标
* @param normal 反转顶点法向
* @param texCoordU 水平反转纹理
* @param texCoordV 垂直反转纹理
*/
void CMesh::Reverse(bool normal, bool texCoordU, bool texCoordV) {
	// 反转法向量
	if (normal) {
		for (size_t i = 0; i < m_vecIndexArray.size(); i += 3) {
			unsigned int a = m_vecIndexArray[i + 0];
			unsigned int c = m_vecIndexArray[i + 2];
			m_vecIndexArray[i + 0] = c;
			m_vecIndexArray[i + 2] = a;
		}
		for (size_t i = 0; i < m_vecVertexArray.size(); i++) {
			CVertex* vert = &m_vecVertexArray[i];
			vert->m_fNormal[0] = -vert->m_fNormal[0];
			vert->m_fNormal[1] = -vert->m_fNormal[1];
			vert->m_fNormal[2] = -vert->m_fNormal[2];
		}
	}
	// 反转纹理坐标
	if (texCoordU || texCoordV) {
		for (size_t i = 0; i < m_vecVertexArray.size(); i++) {
			CVertex* vert = &m_vecVertexArray[i];
			if (texCoordU) vert->m_fTexCoord[0] = 1.0f - vert->m_fTexCoord[0];
			if (texCoordV) vert->m_fTexCoord[1] = 1.0f - vert->m_fTexCoord[1];
		}
	}
	if (m_bCreated) Update(normal ? 0 : 1);
}

/**
* 获取顶点个数
* @return 顶点个数
*/
size_t CMesh::GetVertexCount() const {
	return m_vecVertexArray.size();
}

/**
* 获取三角形个数
* @return 三角形个数
*/
size_t CMesh::GetTriangleCount() const {
	return m_vecIndexArray.size() / 3;
}

/**
* 获取绑定骨骼的顶点数量
* @return 绑定骨骼顶点数量
*/
size_t CMesh::GetBindCount() const {
	return m_vecBindArray.size();
}

/**
* 获取网格顶点
* @param index 顶点索引
* @return 顶点
*/
CVertex* CMesh::GetVertex(size_t index) {
	return &m_vecVertexArray[index];
}

/**
* 获取网格顶点
* @param face 三角形索引
* @param index 顶点索引(0,1,2)
* @return 顶点
*/
CVertex* CMesh::GetVertex(size_t face, size_t index) {
	return &m_vecVertexArray[m_vecIndexArray[face * 3 + index]];
}

/**
* 获取三角形顶点
* @param index 三角形索引
* @param vertices 输出三角形的三个顶点
*/
void CMesh::GetTriangle(size_t index, CVertex* vertices[3]) {
	vertices[0] = &m_vecVertexArray[m_vecIndexArray[index * 3 + 0]];
	vertices[1] = &m_vecVertexArray[m_vecIndexArray[index * 3 + 1]];
	vertices[2] = &m_vecVertexArray[m_vecIndexArray[index * 3 + 2]];
}

/**
* 获取三角形顶点索引
* @param index 三角形索引
* @param vertices 输出三角形的三个顶点索引
*/
void CMesh::GetTriangle(size_t index, unsigned int vertices[3]) {
	vertices[0] = m_vecIndexArray[index * 3 + 0];
	vertices[1] = m_vecIndexArray[index * 3 + 1];
	vertices[2] = m_vecIndexArray[index * 3 + 2];
}

/**
* 获取顶点骨骼绑定信息
* @param index 顶点索引
* @return 骨骼绑定信息
*/
CVertexJoint* CMesh::GetBind(size_t index) {
	return &m_vecBindArray[index];
}

/**
* 生成网格
* @param dynamic 指定为动态网格
* @note 对不需要经常更新的网格指定为静态以提高性能
*/
void CMesh::Create(bool dynamic) {
	if (m_bCreated) return;
	glGenBuffers(1, &m_iVertexBuffer);
	glGenBuffers(1, &m_iIndexBuffer);
	m_bDynamic = dynamic;
	m_bCreated = true;
	if (m_vecVertexArray.empty() || m_vecIndexArray.empty()) return;
	GLenum usage = dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
	// 创建顶点缓冲
	size_t vertex_array_size = m_vecVertexArray.size() * sizeof(CVertex);
	glBindBuffer(GL_ARRAY_BUFFER, m_iVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_array_size, &m_vecVertexArray[0], usage);
	// 创建索引缓冲
	if (m_vecVertexArray.size() > 0x10000) {
		size_t index_array_size = m_vecIndexArray.size() * sizeof(unsigned int);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iIndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_array_size, &m_vecIndexArray[0], usage);
	} else {
		vector<unsigned short> temp(m_vecIndexArray.size());
		for (size_t i = 0; i < m_vecIndexArray.size(); i++) temp[i] = (unsigned short)m_vecIndexArray[i];
		size_t index_array_size = m_vecIndexArray.size() * sizeof(unsigned short);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iIndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_array_size, &temp[0], usage);
	}
	m_iVertexArraySize = m_vecVertexArray.size();
	m_iIndexArraySize = m_vecIndexArray.size();
}

/**
* 更新缓冲
* @param bufferType 要更新的缓冲（0-全部，1-顶点缓冲，2-索引缓冲）
* @note 如果定点缓冲或索引缓冲大小改变，则不受 bufferType 控制
*/
void CMesh::Update(int bufferType) {
	if (!m_bCreated) return;
	if (m_vecVertexArray.empty() || m_vecIndexArray.empty()) return;
	GLenum usage = m_bDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
	// 更新顶点缓冲
	if (m_iVertexArraySize != m_vecVertexArray.size()) {
		size_t vertex_array_size = m_vecVertexArray.size() * sizeof(CVertex);
		glBindBuffer(GL_ARRAY_BUFFER, m_iVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, vertex_array_size, &m_vecVertexArray[0], usage);
		m_iVertexArraySize = m_vecVertexArray.size();
	} else if (0 == bufferType || 1 == bufferType) {
		size_t vertex_array_size = m_vecVertexArray.size() * sizeof(CVertex);
		glBindBuffer(GL_ARRAY_BUFFER, m_iVertexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_array_size, &m_vecVertexArray[0]);
	}
	// 更新索引缓冲
	if (m_iIndexArraySize != m_vecIndexArray.size() || 0 == bufferType || 2 == bufferType) {
		void* buffer = m_vecIndexArray.data();
		size_t index_array_size = m_vecIndexArray.size() * sizeof(unsigned int);
		vector<unsigned short> temp;
		if (m_iVertexArraySize <= 0x10000) {
			index_array_size >>= 1;
			temp.resize(m_vecIndexArray.size());
			for (size_t i = 0; i < m_vecIndexArray.size(); i++) temp[i] = (unsigned short)m_vecIndexArray[i];
			buffer = temp.data();
		}
		if (m_iIndexArraySize != m_vecIndexArray.size()) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iIndexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_array_size, buffer, usage);
			m_iIndexArraySize = m_vecIndexArray.size();
		} else {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iIndexBuffer);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, index_array_size, buffer);
		}
	}
}

/**
* 渲染网格
* @param material 使用网格材质
*/
void CMesh::Render(bool material) {
	if (!m_bCreated) return;
	if (material) {
		m_pMaterial->UseMaterial();
		if (!m_pMaterial->m_bCullFace) glDisable(GL_CULL_FACE);
		if (!m_pMaterial->m_bUseDepth) glDisable(GL_DEPTH_TEST);
		if (m_pMaterial->m_bAddColor) glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_iVertexBuffer);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(CVertex), (GLvoid*)(sizeof(float) * 0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(CVertex), (GLvoid*)(sizeof(float) * 4));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(CVertex), (GLvoid*)(sizeof(float) * 6));
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(CVertex), (GLvoid*)(sizeof(float) * 10));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iIndexBuffer);
	glDrawElements(GL_TRIANGLES, (GLsizei)m_iIndexArraySize, m_iVertexArraySize > 0x10000 ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT, 0);
	if (material) {
		if (!m_pMaterial->m_bCullFace) glEnable(GL_CULL_FACE);
		if (!m_pMaterial->m_bUseDepth) glEnable(GL_DEPTH_TEST);
		if (m_pMaterial->m_bAddColor) glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

/**
* 网格细分
* @note 使用 Loop 算法将每个三角形细分为四个三角形
*/
void CMesh::Subdivision() {
	// 生成边-面和边-顶点以及顶点-边的查找表
	bool haveJoint = m_vecVertexArray.size() == m_vecBindArray.size();
	const size_t vertexCount = m_vecVertexArray.size();
	const size_t triangleCount = m_vecIndexArray.size() / 3;
	vector<list<size_t>> vertex_edges(vertexCount);
	map<size_t, list<size_t>> edge_triangles;
	map<size_t, std::pair<unsigned int, unsigned int>> edge_vertex;
	for (size_t t = 0; t < triangleCount; t++) {
		for (int v = 0; v < 3; v++) {
			unsigned int v1 = m_vecIndexArray[t * 3 + v];
			unsigned int v2 = m_vecIndexArray[t * 3 + (v + 1) % 3];
			size_t key = v1 < v2 ? v2 + v1 * vertexCount : v1 + v2 * vertexCount;
			vertex_edges[v1].push_back(key);
			edge_triangles[key].push_back(t);
			edge_vertex[key] = std::pair<unsigned int, unsigned int>(v1, v2);
		}
	}
	// 对每条边生成新顶点
	vector<CVertex> vertexArray;
	vector<CVertexJoint> bindArray;
	vector<unsigned int> triangleArray;
	map<size_t, size_t> edgePointsLookup;
	map<size_t, list<size_t>>::iterator iter = edge_triangles.begin();
	while (iter != edge_triangles.end()) {
		float edgeVertexWeight = iter->second.size() != 2 ? 0.5f : (3.0f / 8.0f);
		float adjacentVertexWeight = iter->second.size() != 2 ? 0.0f : (1.0f / 8.0f);
		std::pair<unsigned int, unsigned int>& line = edge_vertex[iter->first];
		const CVertex& v1 = m_vecVertexArray[line.first];
		const CVertex& v2 = m_vecVertexArray[line.second];
		const CVertexJoint& j1 = haveJoint ? m_vecBindArray[line.first] : CVertexJoint();
		const CVertexJoint& j2 = haveJoint ? m_vecBindArray[line.second] : CVertexJoint();
		CVector3 position = CVector3(v1.m_fPosition).Add(v2.m_fPosition).Scale(edgeVertexWeight);
		CVector2 texCoord = CVector2(v1.m_fTexCoord).Add(v2.m_fTexCoord).Scale(edgeVertexWeight);
		CColor vertColor = CColor(CColor::Black).Add(v1.m_fColor, edgeVertexWeight).Add(v2.m_fColor, edgeVertexWeight);
		CVertexJoint vertJoint = CVertexJoint().AddBind(j1, edgeVertexWeight).AddBind(j2, edgeVertexWeight);
		list<size_t>::iterator it = iter->second.begin();
		while (it != iter->second.end()) {
			for (int v = 0; v < 3; v++) {
				unsigned int vi = m_vecIndexArray[*it * 3 + v];
				if (vi != line.first && vi != line.second) {
					position.Add(CVector3(m_vecVertexArray[vi].m_fPosition).Scale(adjacentVertexWeight));
					texCoord.Add(CVector2(m_vecVertexArray[vi].m_fTexCoord).Scale(adjacentVertexWeight));
					vertColor.Add(m_vecVertexArray[vi].m_fColor, adjacentVertexWeight);
					if (haveJoint) vertJoint.AddBind(m_vecBindArray[vi], adjacentVertexWeight);
					break;
				}
			}
			++it;
		}
		CVertex vertex;
		vertex.SetPosition(position);
		vertex.SetTexCoord(texCoord);
		vertex.SetColor(vertColor);
		vertexArray.push_back(vertex);
		if (haveJoint) bindArray.push_back(vertJoint.Normalize());
		edgePointsLookup[iter->first] = vertexArray.size() - 1;
		++iter;
	}
	// 更新原来的顶点坐标
	unsigned int edgeVertexCount = static_cast<unsigned int>(vertexArray.size());
	for (size_t v = 0; v < vertexCount; v++) {
		float vertexWeight = 0.0f;
		size_t connectingEdges = vertex_edges[v].size();
		if (connectingEdges == 2) vertexWeight = 1.0f / 8.0f;
		else if (connectingEdges == 3) vertexWeight = 3.0f / 16.0f;
		else if (connectingEdges > 3) vertexWeight = 3.0f / (8.0f * connectingEdges);
		float edgeWeight = 1.0f - connectingEdges * vertexWeight;
		const CVertex& vert = m_vecVertexArray[v];
		const CVertexJoint& bind = haveJoint ? m_vecBindArray[v] : CVertexJoint();
		CVector3 position = CVector3(vert.m_fPosition).Scale(edgeWeight);
		CVector2 texCoord = CVector2(vert.m_fTexCoord).Scale(edgeWeight);
		CColor vertColor = CColor(CColor::Black).Add(vert.m_fColor, edgeWeight);
		CVertexJoint vertJoint = CVertexJoint().AddBind(bind, edgeWeight);
		list<size_t>::iterator iter = vertex_edges[v].begin();
		while (iter != vertex_edges[v].end()) {
			std::pair<unsigned int, unsigned int>& line = edge_vertex[*iter];
			unsigned int nearby = line.first == v ? line.second : line.first;
			position.Add(CVector3(m_vecVertexArray[nearby].m_fPosition).Scale(vertexWeight));
			texCoord.Add(CVector2(m_vecVertexArray[nearby].m_fTexCoord).Scale(vertexWeight));
			vertColor.Add(m_vecVertexArray[nearby].m_fColor, vertexWeight);
			if (haveJoint) vertJoint.AddBind(m_vecBindArray[nearby], vertexWeight);
			++iter;
		}
		CVertex vertex;
		vertex.SetPosition(position);
		vertex.SetTexCoord(texCoord);
		vertex.SetColor(vertColor);
		vertexArray.push_back(vertex);
		if (haveJoint) bindArray.push_back(vertJoint.Normalize());
	}
	// 在原顶点和边顶点之间生成新的三角形面
	for (size_t t = 0; t < triangleCount; t++) {
		unsigned int* v = &m_vecIndexArray[t * 3];
		unsigned int cv1 = static_cast<unsigned int>(edgePointsLookup[v[0] < v[1] ? v[1] + v[0] * vertexCount : v[0] + v[1] * vertexCount]);
		unsigned int cv2 = static_cast<unsigned int>(edgePointsLookup[v[1] < v[2] ? v[2] + v[1] * vertexCount : v[1] + v[2] * vertexCount]);
		unsigned int cv3 = static_cast<unsigned int>(edgePointsLookup[v[2] < v[0] ? v[0] + v[2] * vertexCount : v[2] + v[0] * vertexCount]);
		triangleArray.push_back(edgeVertexCount + v[0]); triangleArray.push_back(cv1); triangleArray.push_back(cv3);
		triangleArray.push_back(edgeVertexCount + v[1]); triangleArray.push_back(cv2); triangleArray.push_back(cv1);
		triangleArray.push_back(edgeVertexCount + v[2]); triangleArray.push_back(cv3); triangleArray.push_back(cv2);
		triangleArray.push_back(cv1); triangleArray.push_back(cv2); triangleArray.push_back(cv3);
	}
	// 更新原网格顶点数组和三角形数组
	m_vecVertexArray.clear();
	m_vecIndexArray.clear();
	m_vecBindArray.clear();
	for (size_t n = 0; n < vertexArray.size(); n++) m_vecVertexArray.push_back(vertexArray[n]);
	for (size_t n = 0; n < triangleArray.size(); n++) m_vecIndexArray.push_back(triangleArray[n]);
	for (size_t n = 0; n < bindArray.size(); n++) m_vecBindArray.push_back(bindArray[n]);
	SetupNormal();
	Update(0);
}

/**
* 获取包围盒
* @return 包含所有顶点的最小包围盒
*/
CBoundingBox CMesh::GetBoundingBox() const {
	CBoundingBox boundingBox;
	for (size_t i = 0; i < m_vecVertexArray.size(); i++) {
		const float* v = m_vecVertexArray[i].m_fPosition;
		if (v[0] > boundingBox.m_cMax[0]) boundingBox.m_cMax[0] = v[0];
		if (v[0] < boundingBox.m_cMin[0]) boundingBox.m_cMin[0] = v[0];
		if (v[1] > boundingBox.m_cMax[1]) boundingBox.m_cMax[1] = v[1];
		if (v[1] < boundingBox.m_cMin[1]) boundingBox.m_cMin[1] = v[1];
		if (v[2] > boundingBox.m_cMax[2]) boundingBox.m_cMax[2] = v[2];
		if (v[2] < boundingBox.m_cMin[2]) boundingBox.m_cMin[2] = v[2];
	}
	return boundingBox;
}

/**
* 射线拾取，返回距离，并获取相交三角形索引和质心坐标
* @param ray 拾取射线
* @param face 相交的三角形索引(可为NULL)
* @param bu 质心坐标 u 分量(可为NULL)
* @param bv 质心坐标 v 分量(可为NULL)
* @return 交点距离射线起点的距离，负数表示不相交
* @note 交点坐标 = (1 - u - v)V0 + uV1 + vV2 其中 V0 V1 V2 为三角形三个顶点坐标
*/
float CMesh::Intersects(const CRay& ray, size_t* face, float* bu, float* bv) const {
	float distance = -1.0f;
	for (size_t i = 0; i < m_vecIndexArray.size(); i += 3) {
		const float* v0 = m_vecVertexArray[m_vecIndexArray[i + 0]].m_fPosition;
		const float* v1 = m_vecVertexArray[m_vecIndexArray[i + 1]].m_fPosition;
		const float* v2 = m_vecVertexArray[m_vecIndexArray[i + 2]].m_fPosition;

		CVector3 e1(v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2]);
		CVector3 e2(v2[0] - v0[0], v2[1] - v0[1], v2[2] - v0[2]);
		CVector3 p = ray.m_cDirection.CrossProduct(e2);
		float det = e1.DotProduct(p);
		// 射线平行于三角形
		if (fabs(det) < 0.000001f) continue;
		CVector3 t = ray.m_cOrigin - CVector3(v0);
		// 计算交点质心坐标参数 u
		float u = t.DotProduct(p);
		if (u < 0.0f || u > det) continue;
		CVector3 q = t.CrossProduct(e1);
		// 计算交点质心坐标参数 v
		float v = ray.m_cDirection.DotProduct(q);
		if (v < 0.0f || u + v > det) continue;
		// 计算交点距离
		det = 1.0f / det;
		float d = e2.DotProduct(q) * det;
		// 记录相交距离最小的参数
		if (d < distance || distance < 0) {
			if (face) *face = i / 3;
			if (bu) *bu = u * det;
			if (bv) *bv = v * det;
			distance = d;
		}
	}
	return distance;
}

/**
* 计算体积并返回质心坐标
* @param centroid 返回的质心坐标
* @return 网格体积
*/
float CMesh::Volume(CVector3& centroid) const {
	size_t indexCount = m_vecIndexArray.size();
	float volume = 0.0f;
	centroid.SetValue(0.0f, 0.0f, 0.0f);
	for (size_t i = 0; i < indexCount; i += 3) {
		CVector3 v1(m_vecVertexArray[m_vecIndexArray[i + 0]].m_fPosition);
		CVector3 v2(m_vecVertexArray[m_vecIndexArray[i + 1]].m_fPosition);
		CVector3 v3(m_vecVertexArray[m_vecIndexArray[i + 2]].m_fPosition);
		CVector3 a = v2 - v1;
		CVector3 b = v3 - v1;
		float vol = (1.0f / 6.0f) * a.CrossProduct(b).DotProduct(v1);
		centroid += v1.Add(v2).Add(v3).Scale(0.25f * vol);
		volume += vol;
	}
	centroid.Scale(1.0f / volume);
	return volume;
}

/**
* 计算表面积
* @return 网格表面积
*/
float CMesh::SurfaceArea() const {
	size_t indexCount = m_vecIndexArray.size();
	float area = 0.0f;
	for (size_t i = 0; i < indexCount; i += 3) {
		CVector3 v1(m_vecVertexArray[m_vecIndexArray[i + 0]].m_fPosition);
		CVector3 v2(m_vecVertexArray[m_vecIndexArray[i + 1]].m_fPosition);
		CVector3 v3(m_vecVertexArray[m_vecIndexArray[i + 2]].m_fPosition);
		CVector3 a = v2 - v1;
		CVector3 b = v3 - v1;
		area += 0.5f * a.CrossProduct(b).Length();
	}
	return area;
}

/**
* 克隆网格对象
*/
CMesh* CMesh::Clone() const {
	CMesh* mesh = new CMesh();
	mesh->m_vecVertexArray = m_vecVertexArray;
	mesh->m_vecIndexArray = m_vecIndexArray;
	mesh->m_vecBindArray = m_vecBindArray;
	mesh->m_pMaterial->CopyFrom(m_pMaterial);
	if (m_bCreated) mesh->Create(m_bDynamic);
	return mesh;
}