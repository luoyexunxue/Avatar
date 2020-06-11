//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CMeshLoader3ds.h"
#include "CEngine.h"
#include "CStreamReader.h"
#include <cstdint>

/**
* 加载 3DS 模型
*/
CMeshData* CMeshLoader3ds::LoadFile(const string& filename, const string& type) {
	CFileManager::CBinaryFile file;
	if (!CEngine::GetFileManager()->ReadFile(filename, &file)) {
		return 0;
	}
	// 不是有效的 3DS 文件
	if (file.size < 16 || file.contents[0] != 0x4D || file.contents[1] != 0x4D) {
		return 0;
	}
	m_strBaseDir = CFileManager::GetDirectory(filename);
	m_vecMeshes.clear();
	m_vecMaterials.clear();
	ReadChunk(file.contents, file.size);
	// 将材质绑定至指定网格对象
	for (size_t i = 0; i < m_vecMaterials.size(); i++) {
		for (size_t j = 0; j < m_vecMeshes.size(); j++) {
			if (m_vecMaterials[i]->GetName() == m_vecMeshes[j]->GetMaterial()->GetName()) {
				m_vecMeshes[j]->GetMaterial()->CopyFrom(m_vecMaterials[i]);
				// 如果没有纹理则使用灰色纹理
				if (!m_vecMaterials[i]->GetTexture()) {
					m_vecMeshes[j]->GetMaterial()->SetTexture("");
				}
			}
		}
		delete m_vecMaterials[i];
	}
	// 创建网格对象
	CMeshData* pMeshData = new CMeshData();
	for (size_t i = 0; i < m_vecMeshes.size(); i++) {
		m_vecMeshes[i]->SetupNormal();
		m_vecMeshes[i]->Create(false);
		if (m_vecMaterials.empty()) m_vecMeshes[i]->GetMaterial()->SetTexture("");
		pMeshData->AddMesh(m_vecMeshes[i]);
	}
	return pMeshData;
}

/**
* 递归读取块
*/
void CMeshLoader3ds::ReadChunk(unsigned char* buffer, unsigned int size) {
	CStreamReader reader(buffer, size);
	while (reader.Available() > 0) {
		int currentPos = reader.GetPosition();
		uint16_t chunkId = reader.GetValue<uint16_t>();
		uint32_t chunkSize = reader.GetValue<uint32_t>();
		switch (chunkId) {
		case 0x4D4D:
		case 0x3D3D:
			ReadChunk(reader.GetPointer(), chunkSize - 6);
			break;
		case 0x4000:
			while (reader.GetValue<char>() != '\0');
			ReadChunk(reader.GetPointer(), chunkSize + currentPos - reader.GetPosition());
			break;
		case 0x4100:
			m_vecMeshes.push_back(new CMesh());
			ReadMeshChunk(reader.GetPointer(), chunkSize - 6, m_vecMeshes.back());
			break;
		case 0xAFFF:
			m_vecMaterials.push_back(new CMaterial());
			ReadMaterialChunk(reader.GetPointer(), chunkSize - 6, m_vecMaterials.back());
			break;
		default:
			break;
		}
		reader.SetPosition(currentPos).Skip(chunkSize);
	}
}

/**
* 读取网格块
*/
void CMeshLoader3ds::ReadMeshChunk(unsigned char* buffer, unsigned int size, CMesh* mesh) {
	int numVertices = 0;
	int numTriangles = 0;
	int numTexCoords = 0;
	CStreamReader reader(buffer, size);
	while (reader.Available() > 0) {
		int currentPos = reader.GetPosition();
		uint16_t chunkId = reader.GetValue<uint16_t>();
		uint32_t chunkSize = reader.GetValue<uint32_t>();
		switch (chunkId) {
		case 0x4110:
			numVertices = reader.GetValue<uint16_t>();
			mesh->SetVertexUsage(numVertices);
			for (int i = 0; i < numVertices; i++) {
				float x, y, z;
				reader >> x >> y >> z;
				mesh->AddVertex(CVertex(x, y, z));
			}
			break;
		case 0x4120:
			numTriangles = reader.GetValue<uint16_t>();
			for (int i = 0; i < numTriangles; i++) {
				uint16_t a, b, c, flag;
				reader >> a >> b >> c >> flag;
				mesh->AddTriangle(a, b, c);
			}
			ReadMeshChunk(reader.GetPointer(), chunkSize + currentPos - reader.GetPosition(), mesh);
			break;
		case 0x4140:
			numTexCoords = reader.GetValue<uint16_t>();
			for (int i = 0; i < numTexCoords && i < numVertices; i++) {
				float s, t;
				reader >> s >> t;
				mesh->GetVertex(i)->SetTexCoord(s, t);
			}
			break;
		case 0x4130:
			mesh->GetMaterial()->GetName() = (char*)reader.GetPointer();
			break;
		default:
			break;
		}
		reader.SetPosition(currentPos).Skip(chunkSize);
	}
}

/**
* 读取材质块
*/
void CMeshLoader3ds::ReadMaterialChunk(unsigned char* buffer, unsigned int size, CMaterial* material) {
	float color[3];
	string textureFile;
	CStreamReader reader(buffer, size);
	while (reader.Available() > 0) {
		int currentPos = reader.GetPosition();
		uint16_t chunkId = reader.GetValue<uint16_t>();
		uint32_t chunkSize = reader.GetValue<uint32_t>();
		switch (chunkId) {
		case 0xA000:
			material->GetName() = (char*)reader.GetPointer();
			break;
		case 0xA020:
			ReadColorChunk(reader.GetPointer(), chunkSize - 6, color);
			material->m_fColor[0] = color[0];
			material->m_fColor[1] = color[1];
			material->m_fColor[2] = color[2];
			break;
		case 0xA200:
			ReadTextureChunk(reader.GetPointer(), chunkSize - 6, textureFile);
			if (!textureFile.empty()) {
				if (!CFileManager::IsFullPath(textureFile)) textureFile = m_strBaseDir + textureFile;
				material->SetTexture(textureFile);
			}
			break;
		default:
			break;
		}
		reader.SetPosition(currentPos).Skip(chunkSize);
	}
}

/**
* 读取颜色块
*/
void CMeshLoader3ds::ReadColorChunk(unsigned char* buffer, unsigned int size, float color[3]) {
	uint8_t r, g, b;
	CStreamReader reader(buffer, size);
	uint16_t chunkId = reader.GetValue<uint16_t>();
	reader.Skip(4);
	switch (chunkId) {
	case 0x0010:
		reader >> color[0] >> color[1] >> color[2];
		break;
	case 0x0011:
		reader >> r >> g >> b;
		color[0] = static_cast<float>(r) / 255.0f;
		color[1] = static_cast<float>(g) / 255.0f;
		color[2] = static_cast<float>(b) / 255.0f;
		break;
	default:
		color[0] = 1.0f;
		color[1] = 1.0f;
		color[2] = 1.0f;
		break;
	}
}

/**
* 读取纹理块
*/
void CMeshLoader3ds::ReadTextureChunk(unsigned char* buffer, unsigned int size, string& texture) {
	CStreamReader reader(buffer, size);
	while (reader.Available() > 0) {
		int currentPos = reader.GetPosition();
		uint16_t chunkId = reader.GetValue<uint16_t>();
		uint32_t chunkSize = reader.GetValue<uint32_t>();
		switch (chunkId) {
		case 0xA300:
			texture = (char*)reader.GetPointer();
			break;
		default:
			texture = "";
			break;
		}
		reader.SetPosition(currentPos).Skip(chunkSize);
	}
}