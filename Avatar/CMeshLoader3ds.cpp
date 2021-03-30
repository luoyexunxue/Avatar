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
CMeshData* CMeshLoader3ds::LoadFile(const string& filename, uint8_t* data, size_t size) {
	// 不是有效的 3DS 文件
	if (size < 16 || data[0] != 0x4D || data[1] != 0x4D) return 0;
	CMeshLoader3ds::SContext context;
	context.baseDir = CFileManager::GetDirectory(filename);
	ReadChunk(&context, data, size);
	CMeshData* pMeshData = new CMeshData();
	// 将材质绑定至指定网格对象
	for (size_t i = 0; i < context.meshes.size(); i++) {
		bool material_flag = true;
		for (size_t j = 0; j < context.materials.size(); j++) {
			if (context.meshes[i]->GetMaterial()->GetName() == context.materials[j]->GetName()) {
				material_flag = false;
				context.meshes[i]->GetMaterial()->CopyFrom(context.materials[j]);
				break;
			}
		}
		if (material_flag) context.meshes[i]->GetMaterial()->SetTextureBuffered("");
		context.meshes[i]->SetupNormal();
		pMeshData->AddMesh(context.meshes[i]);
	}
	for (size_t i = 0; i < context.materials.size(); i++) delete context.materials[i];
	return pMeshData;
}

/**
* 递归读取块
*/
void CMeshLoader3ds::ReadChunk(SContext* context, unsigned char* buffer, size_t size) {
	CStreamReader reader(buffer, size);
	while (reader.Available() > 0) {
		size_t currentPos = reader.GetPosition();
		uint16_t chunkId = reader.GetValue<uint16_t>();
		uint32_t chunkSize = reader.GetValue<uint32_t>();
		switch (chunkId) {
		case 0x4D4D:
		case 0x3D3D:
			ReadChunk(context, reader.GetPointer(), chunkSize - 6);
			break;
		case 0x4000:
			while (reader.GetValue<char>() != '\0');
			ReadChunk(context, reader.GetPointer(), chunkSize + currentPos - reader.GetPosition());
			break;
		case 0x4100:
			context->meshes.push_back(new CMesh());
			ReadMeshChunk(reader.GetPointer(), chunkSize - 6, context->meshes.back());
			break;
		case 0xAFFF:
			context->materials.push_back(new CMaterial());
			ReadMaterialChunk(context, reader.GetPointer(), chunkSize - 6, context->materials.back());
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
void CMeshLoader3ds::ReadMeshChunk(unsigned char* buffer, size_t size, CMesh* mesh) {
	int numVertices = 0;
	int numTriangles = 0;
	int numTexCoords = 0;
	CStreamReader reader(buffer, size);
	while (reader.Available() > 0) {
		size_t currentPos = reader.GetPosition();
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
void CMeshLoader3ds::ReadMaterialChunk(SContext* context, unsigned char* buffer, unsigned int size, CMaterial* material) {
	float color[3];
	string textureFile;
	material->SetTextureBuffered("");
	CStreamReader reader(buffer, size);
	while (reader.Available() > 0) {
		size_t currentPos = reader.GetPosition();
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
				if (!CFileManager::IsFullPath(textureFile)) textureFile = context->baseDir + textureFile;
				material->SetTextureBuffered(textureFile);
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
		size_t currentPos = reader.GetPosition();
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