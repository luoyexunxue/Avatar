//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CMeshLoader.h"
#include "CMeshLoaderBasic.h"
#include "CMeshLoader3ds.h"
#include "CMeshLoaderCollada.h"
#include "CMeshLoaderGltf.h"
#include "CEngine.h"
#include "CStringUtil.h"
#include "CFileManager.h"
#include "CStreamReader.h"
#include "CStreamWriter.h"
#include "CLog.h"
#include <cstdint>
#include <cstring>
#include <vector>
#include <set>
#include <tuple>
using std::vector;
using std::set;
using std::tuple;

/**
* 已注册的模型加载器
*/
map<string, CMeshLoader*> CMeshLoader::m_mapMeshLoader;

/**
* 已缓存的网格模型
*/
map<string, CMeshData*> CMeshLoader::m_mapMeshDataCache;

/**
* 已缓存的网格模型引用
*/
map<CMeshData*, int> CMeshLoader::m_mapCacheRefCount;

/**
* 注册模型加载器
*/
bool CMeshLoader::Register(const string& type, CMeshLoader* loader) {
	if (!m_mapMeshLoader.size()) {
		RegisterLoader();
	}
	string ext = CStringUtil::UpperCase(type);
	if (m_mapMeshLoader.count(ext) == 0) {
		m_mapMeshLoader.insert(std::pair<string, CMeshLoader*>(ext, loader));
		return true;
	}
	CLog::Warn("Mesh loader for type '%s' already exist", ext.c_str());
	return false;
}

/**
* 销毁已注册的加载器
*/
void CMeshLoader::Destroy() {
	set<CMeshLoader*> loaders;
	map<string, CMeshLoader*>::iterator it1 = m_mapMeshLoader.begin();
	while (it1 != m_mapMeshLoader.end()) {
		if (loaders.count(it1->second) == 0) {
			loaders.insert(it1->second);
			delete it1->second;
		}
		++it1;
	}
	map<string, CMeshData*>::iterator it2 = m_mapMeshDataCache.begin();
	while (it2 != m_mapMeshDataCache.end()) {
		delete it2->second;
		++it2;
	}
	m_mapMeshLoader.clear();
	m_mapMeshDataCache.clear();
	m_mapCacheRefCount.clear();
}

/**
* 加载网格模型
* @param filename 模型文件
* @param cache 是否缓存
* @return 网格模型对象指针
* @attention 当使用缓存时，必须使用 CMeshLoader::Remove() 释放资源，并且应该避免修改网格对象内部数据。
*	使用模型缓存避免重复加载相同模型，以优化内存使用率
*/
CMeshData* CMeshLoader::Load(const string& filename, bool cache) {
	if (!m_mapMeshLoader.size()) {
		RegisterLoader();
	}
	CMeshData* meshData = 0;
	string cacheName = CFileManager::IsFullPath(filename) ? filename : CEngine::GetFileManager()->GetDataDirectory() + filename;
	map<string, CMeshData*>::iterator iter = m_mapMeshDataCache.find(cacheName);
	if (iter != m_mapMeshDataCache.end()) {
		meshData = iter->second;
		if (cache) m_mapCacheRefCount.find(meshData)->second += 1;
		else meshData = meshData->Clone();
		return meshData;
	}
	string ext = CStringUtil::UpperCase(CFileManager::GetExtension(filename));
	if (ext == "AVT") meshData = LoadAvatar(filename);
	else {
		map<string, CMeshLoader*>::iterator iter = m_mapMeshLoader.find(ext);
		if (iter != m_mapMeshLoader.end()) meshData = iter->second->LoadFile(filename, ext);
		else CLog::Warn("Could not find a mesh loader for type '%s'", ext.c_str());
	}
	if (meshData && cache) {
		m_mapMeshDataCache.insert(std::pair<string, CMeshData*>(cacheName, meshData));
		m_mapCacheRefCount.insert(std::pair<CMeshData*, int>(meshData, 1));
	}
	return meshData;
}

/**
* 保存网格模型
* @param filename 保存的文件名
* @param meshData 模型对象
* @return 成功返回true
*/
bool CMeshLoader::Save(const string& filename, CMeshData* meshData) {
	string ext = CStringUtil::UpperCase(CFileManager::GetExtension(filename));
	if (ext == "AVT") {
		return SaveAvatar(filename, meshData);
	}
	CLog::Warn("Could not save mesh data for type '%s'", ext.c_str());
	return false;
}

/**
* 移除已缓存的模型
* @param meshData 需要移除的网格对象
* @attention 当使用 CMeshLoader::Load() 且参数 cache 为 true 时必须使用此方法释放模型对象
*/
void CMeshLoader::Remove(CMeshData* meshData) {
	map<CMeshData*, int>::iterator iter = m_mapCacheRefCount.find(meshData);
	if (iter != m_mapCacheRefCount.end()) {
		iter->second -= 1;
		if (iter->second == 0) {
			map<string, CMeshData*>::iterator it = m_mapMeshDataCache.begin();
			while (it != m_mapMeshDataCache.end()) {
				if (it->second == meshData) {
					m_mapMeshDataCache.erase(it);
					m_mapCacheRefCount.erase(iter);
					delete meshData;
					return;
				}
				++it;
			}
		}
	} else delete meshData;
}

/**
* 注册内置加载器
*/
void CMeshLoader::RegisterLoader() {
	CMeshLoader* basicLoader = new CMeshLoaderBasic();
	CMeshLoader* gltfLoader = new CMeshLoaderGltf();
	m_mapMeshLoader.insert(std::pair<string, CMeshLoader*>("STL", basicLoader));
	m_mapMeshLoader.insert(std::pair<string, CMeshLoader*>("OBJ", basicLoader));
	m_mapMeshLoader.insert(std::pair<string, CMeshLoader*>("MS3D", basicLoader));
	m_mapMeshLoader.insert(std::pair<string, CMeshLoader*>("BVH", basicLoader));
	m_mapMeshLoader.insert(std::pair<string, CMeshLoader*>("3DS", new CMeshLoader3ds()));
	m_mapMeshLoader.insert(std::pair<string, CMeshLoader*>("DAE", new CMeshLoaderCollada()));
	m_mapMeshLoader.insert(std::pair<string, CMeshLoader*>("GLB", gltfLoader));
	m_mapMeshLoader.insert(std::pair<string, CMeshLoader*>("GLTF", gltfLoader));
}

/**
* 加载 AVT 文件
*/
CMeshData* CMeshLoader::LoadAvatar(const string& filename) {
	CFileManager::CBinaryFile file;
	if (!CEngine::GetFileManager()->ReadFile(filename, &file)) {
		return 0;
	}
	// 检验文件是否有效
	CStreamReader reader(file.contents, file.size);
	if (memcmp("AVATAR", reader.GetPointer(), 6) != 0) return 0;
	if (reader.Skip(6).GetValue<uint16_t>() != 0x0001) return 0;
	vector<tuple<CMaterial*, uint8_t, string>> materialTextures;
	vector<int16_t> jointParentIndex;
	map<string, CTexture*> textureMapper;
	CMeshData* meshData = new CMeshData();
	// 读取网格数据
	uint32_t meshCount = reader.GetValue<uint32_t>();
	for (uint32_t i = 0; i < meshCount; i++) {
		uint8_t mesh_flag = reader.GetValue<uint8_t>();
		uint32_t mesh_vertex = reader.GetValue<uint32_t>();
		uint32_t mesh_triangle = reader.GetValue<uint32_t>();
		CMesh* mesh = new CMesh();
		mesh->SetVertexUsage(mesh_vertex);
		for (uint32_t v = 0; v < mesh_vertex; v++) {
			CVertex vert;
			reader >> vert.m_fPosition[0] >> vert.m_fPosition[1] >> vert.m_fPosition[2];
			reader >> vert.m_fTexCoord[0] >> vert.m_fTexCoord[1];
			reader >> vert.m_fNormal[0] >> vert.m_fNormal[1] >> vert.m_fNormal[2];
			reader >> vert.m_fColor[0] >> vert.m_fColor[1] >> vert.m_fColor[2] >> vert.m_fColor[3];
			if (mesh_flag & 0x01) {
				CVertexJoint bind;
				bind.m_iCount = static_cast<int>(reader.GetValue<uint8_t>());
				bind.m_iJointId[0] = static_cast<int>(reader.GetValue<uint16_t>());
				bind.m_iJointId[1] = static_cast<int>(reader.GetValue<uint16_t>());
				bind.m_iJointId[2] = static_cast<int>(reader.GetValue<uint16_t>());
				bind.m_iJointId[3] = static_cast<int>(reader.GetValue<uint16_t>());
				reader >> bind.m_fWeight[0] >> bind.m_fWeight[1] >> bind.m_fWeight[2] >> bind.m_fWeight[3];
				mesh->AddVertex(vert, bind);
			} else {
				mesh->AddVertex(vert);
			}
		}
		for (uint32_t t = 0; t < mesh_triangle; t++) {
			unsigned int tri1 = static_cast<unsigned int>(reader.GetValue<uint32_t>());
			unsigned int tri2 = static_cast<unsigned int>(reader.GetValue<uint32_t>());
			unsigned int tri3 = static_cast<unsigned int>(reader.GetValue<uint32_t>());
			mesh->AddTriangle(tri1, tri2, tri3);
		}
		// 材质数据
		CMaterial* material = mesh->GetMaterial();
		material->GetName() = (char*)reader.GetPointer();
		reader.Skip(32);
		reader >> material->m_fColor[0] >> material->m_fColor[1] >> material->m_fColor[2] >> material->m_fColor[3];
		reader >> material->m_fRoughness >> material->m_fMetalness;
		uint8_t mesh_texture = reader.GetValue<uint8_t>();
		for (uint8_t t = 0; t < mesh_texture; t++) {
			char textureId[33] = { 0 };
			reader.Read((unsigned char*)textureId, 32);
			materialTextures.push_back(std::make_tuple(material, t, textureId));
		}
		mesh->Create((mesh_flag & 0x01) != 0);
		meshData->AddMesh(mesh);
	}
	// 读骨骼数据
	uint32_t jointCount = reader.GetValue<uint32_t>();
	for (uint32_t i = 0; i < jointCount; i++) {
		SJoint* joint = new SJoint();
		joint->name = (char*)reader.GetPointer();
		jointParentIndex.push_back(reader.Skip(32).GetValue<int16_t>());
		for (int m = 0; m < 16; m++) reader >> joint->localMatrix[m];
		for (int m = 0; m < 16; m++) reader >> joint->worldMatrix[m];
		for (int m = 0; m < 16; m++) reader >> joint->bindMatrixInv[m];
		uint8_t joint_flag = reader.GetValue<uint8_t>();
		if (joint_flag & 0x80) {
			joint->physics = new SJointDynamic();
			joint->physics->enabled = (joint_flag & 0x01) != 0x00;
			joint->physics->isFacing = false;
			reader >> joint->physics->mass;
			reader >> joint->physics->vElasticity;
			reader >> joint->physics->hElasticity;
			reader >> joint->physics->damping;
		}
		meshData->AddJoint(joint);
	}
	// 读动画数据
	for (uint32_t i = 0; i < jointCount; i++) {
		SJoint* joint = meshData->GetJoint(i);
		uint32_t numKeyRot = reader.GetValue<uint32_t>();
		uint32_t numKeyPos = reader.GetValue<uint32_t>();
		joint->keyRot.resize(numKeyRot);
		joint->keyPos.resize(numKeyPos);
		for (uint32_t k = 0; k < numKeyRot; k++) {
			reader >> joint->keyRot[k].time;
			reader >> joint->keyRot[k].param[0];
			reader >> joint->keyRot[k].param[1];
			reader >> joint->keyRot[k].param[2];
			reader >> joint->keyRot[k].param[3];
		}
		for (uint32_t k = 0; k < numKeyPos; k++) {
			reader >> joint->keyPos[k].time;
			reader >> joint->keyPos[k].param[0];
			reader >> joint->keyPos[k].param[1];
			reader >> joint->keyPos[k].param[2];
		}
		if (jointParentIndex[i] >= 0) {
			joint->parent = meshData->GetJoint(jointParentIndex[i]);
		}
	}
	// 读动画信息
	uint32_t animCount = reader.GetValue<uint32_t>();
	for (uint32_t i = 0; i < animCount; i++) {
		const char* name = (char*)reader.GetPointer();
		float beginTime = reader.Skip(32).GetValue<float>();
		float endTime = reader.GetValue<float>();
		meshData->AddAnimation(name, beginTime, endTime);
	}
	// 读纹理数据
	uint32_t textureCount = reader.GetValue<uint32_t>();
	for (uint32_t i = 0; i < textureCount; i++) {
		char textureId[33] = { 0 };
		reader.Read((unsigned char*)textureId, 32);
		string name = textureId;
		uint32_t fileSize = reader.GetValue<uint32_t>();
		uint16_t width = reader.GetValue<uint16_t>();
		uint16_t height = reader.GetValue<uint16_t>();
		uint16_t channel = reader.GetValue<uint16_t>();
		uint16_t type = reader.GetValue<uint16_t>();
		CFileManager::FileType fileType = CFileManager::BIN;
		if (type == 1) fileType = CFileManager::BMP;
		else if (type == 2) fileType = CFileManager::TGA;
		else if (type == 3) fileType = CFileManager::PNG;
		else if (type == 4) fileType = CFileManager::JPG;
		if (fileSize > 0) {
			CFileManager::CImageFile image(fileType);
			CEngine::GetFileManager()->ReadFile(reader.GetPointer(), fileSize, &image);
			textureMapper[name] = CEngine::GetTextureManager()->Create(name, width, height, channel, image.contents, true);
			reader.Skip(fileSize);
		} else {
			textureMapper[name] = CEngine::GetTextureManager()->Create("");
		}
	}
	// 纹理赋给网格对象
	for (size_t i = 0; i < materialTextures.size(); i++) {
		map<string, CTexture*>::iterator target = textureMapper.find(std::get<2>(materialTextures[i]));
		if (target != textureMapper.end()) {
			std::get<0>(materialTextures[i])->SetTexture(target->second, std::get<1>(materialTextures[i]));
		}
	}
	map<string, CTexture*>::iterator iter = textureMapper.begin();
	while (iter != textureMapper.end()) {
		CEngine::GetTextureManager()->Drop(iter->second);
		++iter;
	}
	return meshData;
}

/**
* 保存 AVT 文件
*/
bool CMeshLoader::SaveAvatar(const string& filename, CMeshData* meshData) {
	map<CTexture*, string> textureMapper;
	map<string, CFileManager::CBinaryFile*> textureBuffer;
	map<string, uint32_t> textureSize;
	uint32_t meshCount = meshData->GetMeshCount();
	uint32_t jointCount = meshData->GetJointCount();
	uint32_t animCount = meshData->GetAnimationCount();
	for (uint32_t i = 0; i < meshCount; i++) {
		CMaterial* material = meshData->GetMesh(i)->GetMaterial();
		for (int t = 0; t < material->GetTextureCount(); t++) {
			CTexture* texture = material->GetTexture(t);
			if (texture && textureMapper.count(texture) == 0) {
				string textureId = CStringUtil::Guid();
				textureMapper.insert(std::pair<CTexture*, string>(texture, textureId));
				if (texture->IsValid() && !CEngine::GetFileManager()->FileExists(texture->GetFilePath())) {
					unsigned int imageSize = 4 * texture->GetWidth() * texture->GetHeight();
					CFileManager::CImageFile image(CFileManager::PNG, imageSize);
					image.channels = 4;
					image.width = texture->GetWidth();
					image.height = texture->GetHeight();
					CFileManager::CBinaryFile* buffer = new CFileManager::CBinaryFile(imageSize);
					CEngine::GetTextureManager()->ReadData(texture, 0, 0, image.contents);
					buffer->size = CEngine::GetFileManager()->WriteFile(&image, buffer->contents, buffer->size);
					textureBuffer[textureId] = buffer;
				}
			}
		}
	}
	// 计算文件大小
	uint32_t size = 24;
	for (uint32_t i = 0; i < meshCount; i++) {
		CMesh* mesh = meshData->GetMesh(i);
		size += 9 + mesh->GetVertexCount() * 48;
		size += mesh->GetBindCount() > 0 ? mesh->GetVertexCount() * 25 : 0;
		size += mesh->GetTriangleCount() * 12;
		size += 57 + mesh->GetMaterial()->GetTextureCount() * 32;
	}
	for (uint32_t i = 0; i < jointCount; i++) {
		SJoint* joint = meshData->GetJoint(i);
		size += 227 + (joint->physics ? 16 : 0);
		size += 8 + joint->keyRot.size() * 20 + joint->keyPos.size() * 16;
	}
	size += animCount * 40;
	map<CTexture*, string>::iterator iter = textureMapper.begin();
	while (iter != textureMapper.end()) {
		unsigned int fileSize = CEngine::GetFileManager()->FileSize(iter->first->GetFilePath());
		if (!fileSize && textureBuffer.count(iter->second) > 0) fileSize = textureBuffer[iter->second]->size;
		textureSize.insert(std::pair<string, uint32_t>(iter->second, fileSize));
		size += 44 + fileSize;
		++iter;
	}
	CFileManager::CBinaryFile file(size);
	CStreamWriter writer(file.contents, file.size);
	// 写文件头
	const uint16_t version = 0x0001;
	writer.Write((unsigned char*)"AVATAR", 6);
	writer << version;
	// 写网格数据
	writer << meshCount;
	for (uint32_t i = 0; i < meshCount; i++) {
		CMesh* mesh = meshData->GetMesh(i);
		uint8_t mesh_flag = mesh->GetBindCount() > 0 ? 0x01 : 0x00;
		uint32_t mesh_vertex = mesh->GetVertexCount();
		uint32_t mesh_triangle = mesh->GetTriangleCount();
		writer << mesh_flag << mesh_vertex << mesh_triangle;
		for (uint32_t v = 0; v < mesh_vertex; v++) {
			CVertex* vert = mesh->GetVertex(v);
			writer << vert->m_fPosition[0] << vert->m_fPosition[1] << vert->m_fPosition[2];
			writer << vert->m_fTexCoord[0] << vert->m_fTexCoord[1];
			writer << vert->m_fNormal[0] << vert->m_fNormal[1] << vert->m_fNormal[2];
			writer << vert->m_fColor[0] << vert->m_fColor[1] << vert->m_fColor[2] << vert->m_fColor[3];
			if (mesh_flag & 0x01) {
				CVertexJoint* bind = mesh->GetBind(v);
				writer.SetValue((uint8_t&)bind->m_iCount);
				writer.SetValue((uint16_t&)bind->m_iJointId[0]);
				writer.SetValue((uint16_t&)bind->m_iJointId[1]);
				writer.SetValue((uint16_t&)bind->m_iJointId[2]);
				writer.SetValue((uint16_t&)bind->m_iJointId[3]);
				writer << bind->m_fWeight[0] << bind->m_fWeight[1] << bind->m_fWeight[2] << bind->m_fWeight[3];
			}
		}
		for (uint32_t t = 0; t < mesh_triangle; t++) {
			unsigned int vertices[3];
			mesh->GetTriangle(t, vertices);
			writer.SetValue((uint32_t&)vertices[0]);
			writer.SetValue((uint32_t&)vertices[1]);
			writer.SetValue((uint32_t&)vertices[2]);
		}
		// 材质数据
		CMaterial* material = mesh->GetMaterial();
		char materialName[32] = { 0 };
		strncpy(materialName, material->GetName().c_str(), 31);
		writer.Write((unsigned char*)materialName, 32);
		writer << material->m_fColor[0] << material->m_fColor[1] << material->m_fColor[2] << material->m_fColor[3];
		writer << material->m_fRoughness << material->m_fMetalness;
		uint8_t mesh_texture = (uint8_t)material->GetTextureCount();
		writer << mesh_texture;
		for (uint8_t t = 0; t < mesh_texture; t++) {
			char textureId[32] = { 0 };
			strncpy(textureId, textureMapper[material->GetTexture(t)].c_str(), 32);
			writer.Write((unsigned char*)textureId, 32);
		}
	}
	// 写骨骼数据
	writer << jointCount;
	for (uint32_t i = 0; i < jointCount; i++) {
		SJoint* joint = meshData->GetJoint(i);
		char jointName[32] = { 0 };
		strncpy(jointName, joint->name.c_str(), 31);
		writer.Write((unsigned char*)jointName, 32);
		int16_t parentIndex = -1;
		for (uint32_t j = 0; j < jointCount; j++) {
			if (joint->parent == meshData->GetJoint(j)) {
				parentIndex = (int16_t)j;
				break;
			}
		}
		writer << parentIndex;
		for (int m = 0; m < 16; m++) writer << joint->localMatrix[m];
		for (int m = 0; m < 16; m++) writer << joint->worldMatrix[m];
		for (int m = 0; m < 16; m++) writer << joint->bindMatrixInv[m];
		uint8_t joint_flag = joint->physics ? (joint->physics->enabled ? 0x81 : 0x80) : 0x00;
		writer.SetValue(joint_flag);
		if (joint->physics) {
			writer << joint->physics->mass;
			writer << joint->physics->vElasticity;
			writer << joint->physics->hElasticity;
			writer << joint->physics->damping;
		}
	}
	// 写动画数据
	for (uint32_t i = 0; i < jointCount; i++) {
		SJoint* joint = meshData->GetJoint(i);
		uint32_t numKeyRot = joint->keyRot.size();
		uint32_t numKeyPos = joint->keyPos.size();
		writer << numKeyRot << numKeyPos;
		for (uint32_t k = 0; k < numKeyRot; k++) {
			writer << joint->keyRot[k].time;
			writer << joint->keyRot[k].param[0];
			writer << joint->keyRot[k].param[1];
			writer << joint->keyRot[k].param[2];
			writer << joint->keyRot[k].param[3];
		}
		for (uint32_t k = 0; k < numKeyPos; k++) {
			writer << joint->keyPos[k].time;
			writer << joint->keyPos[k].param[0];
			writer << joint->keyPos[k].param[1];
			writer << joint->keyPos[k].param[2];
		}
	}
	// 写动画信息
	writer << animCount;
	for (uint32_t i = 0; i < animCount; i++) {
		char animationName[32] = { 0 };
		strncpy(animationName, meshData->GetAnimationName(i).c_str(), 31);
		writer.Write((unsigned char*)animationName, 32);
		float beginTime = meshData->GetAnimationBeginTime(i);
		float endTime = meshData->GetAnimationEndTime(i);
		writer << beginTime << endTime;
	}
	// 写纹理数据
	uint32_t textureCount = textureMapper.size();
	writer << textureCount;
	for (iter = textureMapper.begin(); iter != textureMapper.end(); ++iter) {
		string ext = CStringUtil::UpperCase(CFileManager::GetExtension(iter->first->GetFilePath()));
		uint32_t fileSize = textureSize[iter->second];
		uint16_t width = (uint16_t)iter->first->GetWidth();
		uint16_t height = (uint16_t)iter->first->GetHeight();
		uint16_t channel = (uint16_t)iter->first->GetChannel();
		uint16_t type = 0;
		if (ext == "BMP") type = 1;
		else if (ext == "TGA") type = 2;
		else if (ext == "PNG") type = 3;
		else if (ext == "JPG" || ext == "JPEG") type = 4;
		if (textureBuffer.count(iter->second) > 0) type = 3;
		writer.Write((unsigned char*)iter->second.c_str(), 32);
		writer << fileSize << width << height << channel << type;
		if (fileSize > 0) {
			if (textureBuffer.count(iter->second) > 0) {
				CFileManager::CBinaryFile* buffer = textureBuffer[iter->second];
				writer.Write(buffer->contents, buffer->size);
			} else {
				CFileManager::CBinaryFile bin;
				CEngine::GetFileManager()->ReadFile(iter->first->GetFilePath(), &bin);
				writer.Write(bin.contents, bin.size > fileSize ? fileSize : bin.size);
				writer.Skip(bin.size < fileSize ? fileSize - bin.size : 0);
			}
		}
	}
	map<string, CFileManager::CBinaryFile*>::iterator it = textureBuffer.begin();
	while (it != textureBuffer.end()) { delete it->second; ++it; }
	return CEngine::GetFileManager()->WriteFile(&file, filename) > 0;
}