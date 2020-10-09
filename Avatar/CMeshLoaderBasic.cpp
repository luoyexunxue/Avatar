//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CMeshLoaderBasic.h"
#include "CEngine.h"
#include "CStreamReader.h"
#include "CStringUtil.h"
#include "CMeshLoaderGltf.h"
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <map>
#include <vector>
#include <sstream>
using std::map;
using std::istringstream;

/**
* 加载 STL/OBJ/MS3D/BVH/B3DM/I3DM 模型
*/
CMeshData* CMeshLoaderBasic::LoadFile(const string& filename, uint8_t* data, size_t size) {
	string ext = CStringUtil::UpperCase(CFileManager::GetExtension(filename));
	if (ext == "STL") return LoadStlFile(filename, data, size);
	if (ext == "OBJ") return LoadObjFile(filename, data, size);
	if (ext == "MS3D") return LoadMs3dFile(filename, data, size);
	if (ext == "BVH") return LoadBvhFile(filename, data, size);
	if (ext == "B3DM") return LoadB3dmFile(filename, data, size);
	if (ext == "I3DM") return LoadI3dmFile(filename, data, size);
	return 0;
}

/**
* 加载 STL 模型
*/
CMeshData* CMeshLoaderBasic::LoadStlFile(const string& filename, uint8_t* data, size_t size) {
	CStreamReader reader(data, size);
	// 跳过80字节的头部
	reader.Skip(80);
	// 三角形数量
	int32_t triangleCount = reader.GetValue<int32_t>();
	// 不是有效的 STL 文件
	if (reader.Available() <  triangleCount * 50) {
		return 0;
	}
	CMeshData* pMeshData = new CMeshData();
	CMesh* pMesh = new CMesh();
	pMeshData->AddMesh(pMesh);
	pMesh->SetVertexUsage(triangleCount * 3);
	while (triangleCount--) {
		float n[3];
		float v[3][3];
		reader >> n[0] >> n[1] >> n[2];
		reader >> v[0][0] >> v[0][1] >> v[0][2];
		reader >> v[1][0] >> v[1][1] >> v[1][2];
		reader >> v[2][0] >> v[2][1] >> v[2][2];
		reader.Skip(2);
		CVertex v1(v[0][0], v[0][1], v[0][2], 0, 0, n[0], n[1], n[2]);
		CVertex v2(v[1][0], v[1][1], v[1][2], 0, 0, n[0], n[1], n[2]);
		CVertex v3(v[2][0], v[2][1], v[2][2], 0, 0, n[0], n[1], n[2]);
		pMesh->AddTriangle(v1, v2, v3);
	}
	pMesh->Create(false);
	pMesh->GetMaterial()->SetTexture("");
	return pMeshData;
}

/**
* 加载 OBJ 模型
*/
CMeshData* CMeshLoaderBasic::LoadObjFile(const string& filename, uint8_t* data, size_t size) {
	// 顶点索引
	struct SVertIndex {
		int vp;
		int vt;
		int vn;
		SVertIndex(int index[3]) :vp(index[0]), vt(index[1]), vn(index[2]) {}
	};
	// map 的比较函数，加载 OBJ 文件时用来消除重复顶点
	struct SComparekey {
		bool operator () (const SVertIndex& v1, const SVertIndex& v2) const {
			if (v1.vp != v2.vp) return v1.vp < v2.vp;
			if (v1.vt != v2.vt) return v1.vt < v2.vt;
			if (v1.vn != v2.vn) return v1.vn < v2.vn;
			return false;
		}
	};
	vector<float> vecVertices;
	vector<float> vecTexCoords;
	vector<float> vecNormals;
	string mtlFilePath;
	string currentMtl = "";
	CMesh* currentMesh = 0;
	map<SVertIndex, unsigned int, SComparekey> mapVertices;
	CMeshData* pMeshData = new CMeshData();
	string baseDir = CFileManager::GetDirectory(filename);
	istringstream stream(string((const char*)data, size));
	istringstream reader;
	while (!stream.eof()) {
		char line[256];
		stream.getline(line, 256);
		// 无效行
		if (strlen(line) == 0 || line[0] == '#') continue;
		reader.clear();
		reader.str(line);
		string key;
		reader >> key;
		if (key == "mtllib") {
			reader.getline(line, 256);
			char* p = line;
			char* q = line + strlen(line) - 1;
			while (*p == ' ' || *p == '\t') ++p;
			while (*q == ' ' || *q == '\t' || *q == '\r' || *q == '\n') *q = '\0';
			mtlFilePath = baseDir + p;
		} else if (key == "v") {
			size_t index = vecVertices.size();
			vecVertices.resize(index + 3);
			reader >> vecVertices[index] >> vecVertices[index + 1] >> vecVertices[index + 2];
		} else if (key == "vt") {
			size_t index = vecTexCoords.size();
			vecTexCoords.resize(index + 2);
			reader >> vecTexCoords[index] >> vecTexCoords[index + 1];
		} else if (key == "vn") {
			size_t index = vecNormals.size();
			vecNormals.resize(index + 3);
			reader >> vecNormals[index] >> vecNormals[index + 1] >> vecNormals[index + 2];
		} else if (key == "g" || key == "o" || key == "usemtl") {
			currentMesh = 0;
			if (key == "usemtl") reader >> currentMtl;
		} else if (key == "f") {
			if (!currentMesh) {
				currentMesh = new CMesh();
				currentMesh->GetMaterial()->GetName() = currentMtl;
				currentMesh->GetMaterial()->SetTexture("");
				pMeshData->AddMesh(currentMesh);
				mapVertices.clear();
			}
			string face;
			vector<SVertIndex> vertIndex;
			while (!reader.eof()) {
				reader >> face;
				int index[3] = { -1, -1, -1 };
				int length = ParseObjFace(face, index);
				if (length > 0) index[0] = index[0] < 0 ? index[0] * 3 + (int)vecVertices.size() : (index[0] - 1) * 3;
				if (length > 1) index[1] = index[1] < 0 ? index[1] * 2 + (int)vecTexCoords.size() : (index[1] - 1) * 2;
				if (length > 2) index[2] = index[2] < 0 ? index[2] * 3 + (int)vecNormals.size() : (index[2] - 1) * 3;
				vertIndex.push_back(SVertIndex(index));
			}
			if (vertIndex.size() < 3) continue;
			unsigned int triangle[3];
			for (size_t i = 0; i < vertIndex.size(); i++) {
				SVertIndex& index = vertIndex[i];
				CVertex vert;
				if (index.vp >= 0) vert.SetPosition(&vecVertices[index.vp]);
				if (index.vt >= 0) vert.SetTexCoord(&vecTexCoords[index.vt]);
				if (index.vn >= 0) vert.SetNormal(&vecNormals[index.vn]);
				unsigned int current = 0;
				map<SVertIndex, unsigned int, SComparekey>::iterator iter = mapVertices.find(index);
				if (iter != mapVertices.end()) current = iter->second;
				else {
					current = (unsigned int)currentMesh->GetVertexCount();
					mapVertices.insert(std::pair<SVertIndex, unsigned int>(index, current));
					currentMesh->AddVertex(vert);
				}
				if (i > 2) triangle[1] = triangle[2];
				if (i > 1) {
					triangle[2] = current;
					currentMesh->AddTriangle(triangle[0], triangle[1], triangle[2]);
				} else triangle[i] = current;
			}
		}
	}
	// 加载材质
	LoadObjMaterial(mtlFilePath, pMeshData);
	// 创建网格
	for (size_t i = 0; i < pMeshData->GetMeshCount(); i++) {
		CMesh* pMesh = pMeshData->GetMesh(i);
		CVertex* v = pMesh->GetVertex(0);
		if (vecNormals.empty() || (v->m_fNormal[0] == 0.0f && v->m_fNormal[1] == 0.0f && v->m_fNormal[2] == 0.0f)) {
			pMesh->SetupNormal();
		}
		pMesh->Reverse(false, false, true);
		pMesh->Create(false);
	}
	return pMeshData;
}

/**
* 加载 MS3D 模型
*/
CMeshData* CMeshLoaderBasic::LoadMs3dFile(const string& filename, uint8_t* data, size_t size) {
	string baseDir = CFileManager::GetDirectory(filename);
	CStreamReader reader(data, size);
	// 读取文件头
	char magicId[10];
	reader.Read((unsigned char*)magicId, 10);
	int version = reader.GetValue<int32_t>();
	if (strncmp(magicId, "MS3D000000", 10) != 0 || version < 3 || version > 4) {
		return 0;
	}
	vector<int> vecMeshMaterials;
	vector<string> vecJointParents;
	CMeshData* pMeshData = new CMeshData();

	int numVertices = reader.GetValue<uint16_t>();
	CStreamReader vertexReader(reader.GetPointer(), numVertices * 15);
	reader.Skip(vertexReader.Available());
	int numTriangles = reader.GetValue<uint16_t>();
	CStreamReader triangleReader(reader.GetPointer(), numTriangles * 70);
	reader.Skip(triangleReader.Available());

	int numMeshes = reader.GetValue<uint16_t>();
	for (int i = 0; i < numMeshes; i++) {
		CMesh* pMesh = new CMesh();
		unsigned int triangleCount = reader.Skip(33).GetValue<uint16_t>();
		for (unsigned int j = 0; j < triangleCount; j++) {
			uint16_t vertexIndices[3];
			CVertex vertex[3];
			int8_t boneId[3];
			triangleReader.SetPosition(reader.GetValue<uint16_t>() * 70 + 2);
			triangleReader >> vertexIndices[0] >> vertexIndices[1] >> vertexIndices[2];
			for (int n = 0; n < 3; n++) {
				vertexReader.SetPosition(vertexIndices[n] * 15 + 1);
				vertexReader >> vertex[n].m_fPosition[0] >> vertex[n].m_fPosition[1] >> vertex[n].m_fPosition[2];
				vertexReader >> boneId[n];
			}
			triangleReader >> vertex[0].m_fNormal[0] >> vertex[0].m_fNormal[1] >> vertex[0].m_fNormal[2];
			triangleReader >> vertex[1].m_fNormal[0] >> vertex[1].m_fNormal[1] >> vertex[1].m_fNormal[2];
			triangleReader >> vertex[2].m_fNormal[0] >> vertex[2].m_fNormal[1] >> vertex[2].m_fNormal[2];
			triangleReader >> vertex[0].m_fTexCoord[0] >> vertex[1].m_fTexCoord[0] >> vertex[2].m_fTexCoord[0];
			triangleReader >> vertex[0].m_fTexCoord[1] >> vertex[1].m_fTexCoord[1] >> vertex[2].m_fTexCoord[1];
			pMesh->AddVertex(vertex[0], CVertexJoint(boneId[0]));
			pMesh->AddVertex(vertex[1], CVertexJoint(boneId[1]));
			pMesh->AddVertex(vertex[2], CVertexJoint(boneId[2]));
			pMesh->AddTriangle(j * 3 + 0, j * 3 + 1, j * 3 + 2);
		}
		pMeshData->AddMesh(pMesh);
		vecMeshMaterials.push_back(reader.GetValue<int8_t>());
	}
	int numMaterials = reader.GetValue<uint16_t>();
	CStreamReader materialReader(reader.GetPointer(), numMaterials * 361);
	reader.Skip(materialReader.Available());
	for (int i = 0; i < numMeshes; i++) {
		if (vecMeshMaterials[i] < 0) continue;
		CMesh* pMesh = pMeshData->GetMesh(i);
		CMaterial* mtl = pMesh->GetMaterial();
		materialReader.SetPosition(vecMeshMaterials[i] * 361);
		char name[32];
		char texture[128];
		materialReader.Read((unsigned char*)name, 32);
		mtl->GetName() = name;
		materialReader.Skip(16);
		materialReader >> mtl->m_fColor[0] >> mtl->m_fColor[1] >> mtl->m_fColor[2] >> mtl->m_fColor[3];
		materialReader.Skip(41).Read((unsigned char*)texture, 128);
		// 设置纹理图片，如果没有则使用纯灰色
		if (strlen(texture) > 0) mtl->SetTexture(baseDir + texture);
		else mtl->SetTexture("");
		pMesh->Create(true);
	}
	int numJoints = reader.Skip(12).GetValue<uint16_t>();
	for (int i = 0; i < numJoints; i++) {
		SJoint* pJoint = new SJoint();
		char name[32];
		char parentName[32];
		float rotation[3];
		float translation[3];
		reader.Skip(1).Read((unsigned char*)name, 32);
		reader.Read((unsigned char*)parentName, 32);
		reader >> rotation[0] >> rotation[1] >> rotation[2];
		reader >> translation[0] >> translation[1] >> translation[2];
		pJoint->name = name;
		pJoint->localMatrix.RotateX(rotation[0]).RotateY(rotation[1]).RotateZ(rotation[2]);
		pJoint->localMatrix.Translate(translation[0], translation[1], translation[2]);
		pJoint->worldMatrix = pJoint->localMatrix;
		// 关节动画变换关键帧
		int numRotationKeyframes = reader.GetValue<uint16_t>();
		int numTranslationKeyframes = reader.GetValue<uint16_t>();
		for (int j = 0; j < numRotationKeyframes; j++) {
			float x, y, z;
			SAnimationRotKey keyframe;
			reader >> keyframe.time >> x >> y >> z;
			keyframe.param.FromEulerAngles(x, y, z);
			pJoint->keyRot.push_back(keyframe);
		}
		for (int j = 0; j < numTranslationKeyframes; j++) {
			float x, y, z;
			SAnimationPosKey keyframe;
			reader >> keyframe.time >> x >> y >> z;
			keyframe.param.SetValue(x, y, z);
			pJoint->keyPos.push_back(keyframe);
		}
		pMeshData->AddJoint(pJoint);
		vecJointParents.push_back(parentName);
	}
	// 对骨骼节点生成父级关系，ms3d 里的第一个节点就是根节点
	for (int i = 0; i < numJoints; i++) {
		SJoint* pJoint = pMeshData->GetJoint(i);
		for (int j = 0; j < numJoints; j++) {
			SJoint* pParent = pMeshData->GetJoint(j);
			if (i != j && pParent->name == vecJointParents[i]) {
				pJoint->parent = pParent;
				pJoint->worldMatrix = pParent->worldMatrix * pJoint->localMatrix;
				break;
			}
		}
		pJoint->bindMatrixInv.SetValue(pJoint->worldMatrix);
		pJoint->bindMatrixInv.Invert();
	}
	// 添加动画信息
	float endTime = 0.0f;
	for (int i = 0; i < numJoints; i++) {
		SJoint* pJoint = pMeshData->GetJoint(i);
		if (endTime < pJoint->keyPos.back().time) endTime = pJoint->keyPos.back().time;
		if (endTime < pJoint->keyRot.back().time) endTime = pJoint->keyRot.back().time;
	}
	if (endTime > 0.0f) pMeshData->AddAnimation("default", 0.0f, endTime);
	return pMeshData;
}

/**
* 加载 BVH 动画
*/
CMeshData* CMeshLoaderBasic::LoadBvhFile(const string& filename, uint8_t* data, size_t size) {
	string line;
	istringstream stream(string((char*)data, size));
	stream >> line;
	// 读取骨骼结构
	if (line != "HIERARCHY") return 0;
	CMeshData* pMeshData = new CMeshData();
	map<string, vector<string>> jointChannels;
	SJoint* pParent = 0;
	int braceCount = 0;
	do {
		string keyword;
		stream >> keyword;
		float offset[] = { 0, 0, 0 };
		if ("End" == keyword) {
			stream >> keyword >> keyword;
			stream >> keyword >> offset[0] >> offset[1] >> offset[2];
			stream >> keyword;
		} else if ("ROOT" == keyword || "JOINT" == keyword) {
			++braceCount;
			SJoint* pJoint  = new SJoint();
			pJoint->parent = pParent;
			stream >> pJoint->name >> keyword;
			int numChannel;
			stream >> keyword >> offset[0] >> offset[1] >> offset[2];
			stream >> keyword >> numChannel;
			jointChannels[pJoint->name].resize(numChannel);
			for (int i = 0; i < numChannel; i++) stream >> jointChannels[pJoint->name][i];
			pJoint->localMatrix.Translate(offset[0], offset[1], offset[2]);
			if (pJoint->parent) pJoint->worldMatrix = pJoint->parent->worldMatrix * pJoint->localMatrix;
			else pJoint->worldMatrix = pJoint->localMatrix;
			pJoint->bindMatrixInv.SetValue(pJoint->worldMatrix);
			pJoint->bindMatrixInv.Invert();
			pMeshData->AddJoint(pJoint);
			pParent = pJoint;
		} else if ("}" == keyword) {
			--braceCount;
			pParent = pParent->parent;
		}
	} while (braceCount > 0 && !stream.eof());
	// 读取动画数据
	stream >> line;
	if (line == "MOTION") {
		int frameCount;
		float frameTime;
		string keyword;
		stream >> keyword >> frameCount;
		stream >> keyword >> keyword >> frameTime;
		for (int i = 0; i < frameCount; i++) {
			for (size_t j = 0; j < pMeshData->GetJointCount(); j++) {
				SJoint* pJoint = pMeshData->GetJoint(j);
				vector<string>& channels = jointChannels[pJoint->name];
				SAnimationPosKey posKey;
				SAnimationRotKey rotKey;
				rotKey.time = posKey.time = i * frameTime;
				for (size_t n = 0; n < channels.size(); n++) {
					if (channels[n] == "Xposition") stream >> posKey.param[0];
					else if (channels[n] == "Yposition") stream >> posKey.param[1];
					else if (channels[n] == "Zposition") stream >> posKey.param[2];
					else if (channels[n] == "Xrotation") stream >> rotKey.param[0];
					else if (channels[n] == "Yrotation") stream >> rotKey.param[1];
					else if (channels[n] == "Zrotation") stream >> rotKey.param[2];
					else stream >> keyword;
				}
				CMatrix4 rot;
				rot.RotateY(rotKey.param[1] * 0.017453f);
				rot.RotateX(rotKey.param[0] * 0.017453f);
				rot.RotateZ(rotKey.param[2] * 0.017453f);
				rotKey.param.FromMatrix(rot);
				pJoint->keyPos.push_back(posKey);
				pJoint->keyRot.push_back(rotKey);
			}
		}
		pMeshData->AddAnimation("MOTION", 0, (frameCount - 1) * frameTime);
	}
	return pMeshData;
}

/**
* 加载 B3DM 文件
*/
CMeshData* CMeshLoaderBasic::LoadB3dmFile(const string& filename, uint8_t* data, size_t size) {
	if (!memcpy(data, "b3dm", 4)) return 0;
	CStreamReader reader(data + 4, size - 4);
	if (reader.GetValue<uint32_t>() != 1) return 0;
	if (reader.GetValue<uint32_t>() != size) return 0;
	uint32_t featureTableJSONByteLength = reader.GetValue<uint32_t>();
	uint32_t featureTableBinaryByteLength = reader.GetValue<uint32_t>();
	uint32_t batchTableJSONByteLength = reader.GetValue<uint32_t>();
	uint32_t batchTableBinaryByteLength = reader.GetValue<uint32_t>();
	reader.Skip(featureTableJSONByteLength + featureTableBinaryByteLength);
	reader.Skip(batchTableJSONByteLength + batchTableBinaryByteLength);
	return CMeshLoaderGltf().LoadFile(filename, reader.GetPointer(), reader.Available());
}

/**
* 加载 I3DM 文件
*/
CMeshData* CMeshLoaderBasic::LoadI3dmFile(const string& filename, uint8_t* data, size_t size) {
	if (!memcpy(data, "i3dm", 4)) return 0;
	CStreamReader reader(data + 4, size - 4);
	if (reader.GetValue<uint32_t>() != 1) return 0;
	if (reader.GetValue<uint32_t>() != size) return 0;
	uint32_t featureTableJSONByteLength = reader.GetValue<uint32_t>();
	uint32_t featureTableBinaryByteLength = reader.GetValue<uint32_t>();
	uint32_t batchTableJSONByteLength = reader.GetValue<uint32_t>();
	uint32_t batchTableBinaryByteLength = reader.GetValue<uint32_t>();
	uint32_t gltfFormat = reader.GetValue<uint32_t>();
	reader.Skip(featureTableJSONByteLength + featureTableBinaryByteLength);
	reader.Skip(batchTableJSONByteLength + batchTableBinaryByteLength);
	if (gltfFormat == 0) {
		string uri = string((char*)reader.GetPointer(), reader.Available());
		return CMeshLoader::Load(uri, true);
	}
	return CMeshLoaderGltf().LoadFile(filename, reader.GetPointer(), reader.Available());
}

/**
* 加载 OBJ 材质信息
*/
void CMeshLoaderBasic::LoadObjMaterial(const string& filename, CMeshData* meshData) {
	CFileManager::CTextFile file;
	if (!CEngine::GetFileManager()->ReadFile(filename, &file)) {
		return;
	}
	string baseDir = CFileManager::GetDirectory(filename);
	map<string, CMaterial*> mapMaterials;
	CMaterial* pCurrentMaterial = 0;

	istringstream stream((const char*)file.contents);
	istringstream reader;
	while (!stream.eof()) {
		char line[256];
		stream.getline(line, 256);
		// 无效行
		if (strlen(line) == 0 || line[0] == '#') continue;
		reader.clear();
		reader.str(line);
		string key;
		reader >> key;
		if (key == "newmtl") {
			pCurrentMaterial = new CMaterial();
			reader >> pCurrentMaterial->GetName();
			mapMaterials.insert(std::pair<string, CMaterial*>(pCurrentMaterial->GetName(), pCurrentMaterial));
		} else if (key == "Kd" && pCurrentMaterial) {
			reader >> pCurrentMaterial->m_fColor[0];
			reader >> pCurrentMaterial->m_fColor[1];
			reader >> pCurrentMaterial->m_fColor[2];
		} else if (key == "map_Kd" && pCurrentMaterial) {
			reader.getline(line, 256);
			char* p = line;
			char* q = line + strlen(line) - 1;
			while (*p == ' ' || *p == '\t') ++p;
			while (*q == ' ' || *q == '\t' || *q == '\r' || *q == '\n') *q = '\0';
			if (CFileManager::IsFullPath(p)) pCurrentMaterial->SetTexture(p);
			else pCurrentMaterial->SetTexture(baseDir + p);
		} else if (key == "map_Ka" && pCurrentMaterial) {
			reader.getline(line, 256);
			char* p = line;
			char* q = line + strlen(line) - 1;
			while (*p == ' ' || *p == '\t') ++p;
			while (*q == ' ' || *q == '\t' || *q == '\r' || *q == '\n') *q = '\0';
			if (CFileManager::IsFullPath(p)) pCurrentMaterial->SetTexture(p);
			else pCurrentMaterial->SetTexture(baseDir + p);
		}
	}
	// 对网格赋予材质
	for (size_t i = 0; i < meshData->GetMeshCount(); i++) {
		map<string, CMaterial*>::iterator iter = mapMaterials.find(meshData->GetMesh(i)->GetMaterial()->GetName());
		if (iter != mapMaterials.end()) {
			CMesh* mesh = meshData->GetMesh(i);
			mesh->GetMaterial()->CopyFrom(iter->second);
			// 如果纹理为空则设置为默认的灰色纹理
			if (!mesh->GetMaterial()->GetTexture()) {
				mesh->GetMaterial()->SetTexture("");
			}
		}
	}
	// 释放材质数组占用的内存
	map<string, CMaterial*>::iterator iter = mapMaterials.begin();
	while (iter != mapMaterials.end()) {
		delete iter->second;
		++iter;
	}
}

/**
* 解析 OBJ 文件面定义
*/
int CMeshLoaderBasic::ParseObjFace(const string& face, int index[3]) {
	char temp[32];
	int count = 0;
	size_t start = 0;
	size_t current = 0;
	while (start < face.length() && count < 3) {
		while (current < face.length() && face[current] != '/') {
			temp[current - start] = face[current];
			current += 1;
		}
		temp[current - start] = '\0';
		current += 1;
		start = current;
		index[count++] = atoi(temp);
	}
	return count;
}