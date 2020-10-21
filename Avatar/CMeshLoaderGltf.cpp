//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CMeshLoaderGltf.h"
#include "CEngine.h"
#include "CBase64.h"
#include <cstdint>
#include <cstring>
#include <algorithm>

/**
* 加载 GLTF 模型
*/
CMeshData* CMeshLoaderGltf::LoadFile(const string& filename, uint8_t* data, size_t size) {
	if (size == 0) return 0;
	const char* json = reinterpret_cast<char*>(data);
	bool isBinary = strncmp(json, "glTF", 4) == 0;
	size_t jsonLength = size;
	m_vecBuffer.clear();
	m_setJoints.clear();
	m_mapJoints.clear();
	m_mapLocalInv.clear();
	m_mapBindInv.clear();
	m_strFilename = filename;
	m_strBaseDir = CFileManager::GetDirectory(filename);
	// 二进制 glb 文件
	if (isBinary) {
		uint32_t chunkLength = *(uint32_t*)(data + 12);
		uint32_t chunkType = *(uint32_t*)(data + 16);
		if (chunkType != 0x4E4F534A) return 0;
		json = reinterpret_cast<char*>(data + 20);
		jsonLength = chunkLength;
		unsigned int offset = 20 + chunkLength;
		if (offset < size) {
			chunkLength = *(uint32_t*)(data + offset);
			chunkType = *(uint32_t*)(data + offset + 4);
			if (chunkType == 0x004E4942) {
				SBufferBin bin;
				bin.allocated = false;
				bin.data = data + offset + 8;
				bin.size = chunkLength;
				m_vecBuffer.push_back(bin);
			}
		}
	}
	// 解析 Json 对象
	if (!m_cJsonObject.Parse(json, jsonLength)) return 0;
	if (m_cJsonObject["asset"]["version"].ToString() != "2.0") return 0;
	if (!m_cJsonObject.IsContain("scenes")) return 0;
	// 解析 GLTF 数据
	if (!isBinary) ReadBuffers();
	CMatrix4 transform;
	transform.RotateX(1.570796f);
	m_pMeshData = new CMeshData();
	ReadSkin();
	CJsonObject& nodes = m_cJsonObject["scenes"][0]["nodes"];
	for (size_t i = 0; i < nodes.GetCount(); i++) ReadJointNode(nodes[i].ToInt(), transform, 0);
	for (size_t i = 0; i < nodes.GetCount(); i++) ReadMeshNode(nodes[i].ToInt(), transform);
	ReadAnimation();
	for (size_t i = 0; i < m_vecBuffer.size(); i++) {
		if (m_vecBuffer[i].allocated) delete[] m_vecBuffer[i].data;
	}
	return m_pMeshData;
}

/**
* 读取二进制缓冲区
*/
void CMeshLoaderGltf::ReadBuffers() {
	size_t count = m_cJsonObject["buffers"].GetCount();
	for (size_t i = 0; i < count; i++) {
		SBufferBin bin;
		bin.allocated = false;
		bin.data = 0;
		bin.size = 0;
		string uri = m_cJsonObject["buffers"][i]["uri"].ToString();
		if (uri.find("data:") == 0) {
			size_t pos = uri.find(";base64,");
			if (pos != string::npos) {
				bin.allocated = true;
				bin.size = (uri.length() - pos - 8) * 3 / 4;
				bin.data = new unsigned char[bin.size];
				bin.size = CBase64::Decode(uri.c_str() + pos + 8, bin.data, bin.size);
				m_vecBuffer.push_back(bin);
				continue;
			}
		}
		CFileManager::CBinaryFile file;
		if (CEngine::GetFileManager()->ReadFile(m_strBaseDir + uri, &file)) {
			bin.allocated = true;
			bin.size = file.size;
			bin.data = new unsigned char[bin.size];
			memcpy(bin.data, file.contents, file.size);
		}
		m_vecBuffer.push_back(bin);
	}
}

/**
* 读取骨骼蒙皮数据
*/
void CMeshLoaderGltf::ReadSkin() {
	if (!m_cJsonObject.IsContain("skins")) return;
	for (size_t i = 0; i < m_cJsonObject["skins"].GetCount(); i++) {
		CJsonObject& skin = m_cJsonObject["skins"][i];
		CJsonObject& joints = skin["joints"];
		if (skin.IsContain("skeleton")) m_setJoints.insert(std::pair<int, bool>(skin["skeleton"].ToInt(), false));
		for (size_t j = 0; j < joints.GetCount(); j++) m_setJoints.insert(std::pair<int, bool>(joints[j].ToInt(), false));
		// 获取关节的逆绑定矩阵
		if (skin.IsContain("inverseBindMatrices")) {
			CJsonObject& accessor = m_cJsonObject["accessors"][skin["inverseBindMatrices"].ToInt()];
			CJsonObject& bufferView = m_cJsonObject["bufferViews"][accessor["bufferView"].ToInt()];
			unsigned char* buffer = m_vecBuffer[bufferView["buffer"].ToInt()].data;
			if (!buffer) continue;
			if (accessor["type"].ToString() != "MAT4") continue;
			int componentType = accessor["componentType"].ToInt();
			size_t count = accessor["count"].ToInt();
			int byteOffset = bufferView["byteOffset"].ToInt();
			if (accessor.IsContain("byteOffset")) byteOffset += accessor["byteOffset"].ToInt();
			if (count != joints.GetCount()) continue;
			if (componentType == TYPE_FLOAT) {
				int byteStride = bufferView.IsContain("byteStride") ? bufferView["byteStride"].ToInt() : 16 * sizeof(float);
				for (size_t i = 0; i < count; i++) {
					float* data = reinterpret_cast<float*>(buffer + byteOffset + i * byteStride);
					m_mapBindInv[joints[i].ToInt()] = CMatrix4(data);
				}
			}
		}
	}
}

/**
* 读取骨骼节点
*/
void CMeshLoaderGltf::ReadJointNode(int index, const CMatrix4& matrix, SJoint* parent) {
	CMatrix4 local;
	CJsonObject& node = m_cJsonObject["nodes"][index];
	if (node.IsContain("matrix")) for (int i = 0; i < 16; i++) local[i] = node["matrix"][i].ToFloat();
	else if (node.IsContain("translation") || node.IsContain("rotation") || node.IsContain("scale")) {
		CVector3 translation;
		CQuaternion rotation;
		CVector3 scale(1.0f, 1.0f, 1.0f);
		if (node.IsContain("translation")) for (int i = 0; i < 3; i++) translation[i] = node["translation"][i].ToFloat();
		if (node.IsContain("rotation")) for (int i = 0; i < 4; i++) rotation[i] = node["rotation"][i].ToFloat();
		if (node.IsContain("scale")) for (int i = 0; i < 3; i++) scale[i] = node["scale"][i].ToFloat();
		local.MakeTransform(scale, rotation, translation);
	}
	const CMatrix4 transform = matrix * local;
	if (m_setJoints.count(index) > 0) {
		SJoint* pJoint = new SJoint();
		m_pMeshData->AddJoint(pJoint);
		pJoint->name = node["name"].ToString();
		pJoint->parent = parent;
		pJoint->localMatrix = parent ? local : transform;
		pJoint->worldMatrix = transform;
		pJoint->bindMatrixInv = m_mapBindInv[index];
		m_mapLocalInv.insert(std::pair<int, CMatrix4>(index, local.Invert()));
		m_mapJoints.insert(std::pair<int, SJoint*>(index, pJoint));
		parent = pJoint;
	}
	if (node.IsContain("children")) {
		size_t children_count = node["children"].GetCount();
		for (size_t i = 0; i < children_count; i++) {
			ReadJointNode(node["children"][i].ToInt(), transform, parent);
		}
	}
}

/**
* 读取网格节点
*/
void CMeshLoaderGltf::ReadMeshNode(int index, const CMatrix4& matrix) {
	int mesh_index = -1;
	int skin_index = -1;
	CMatrix4 local;
	CJsonObject& node = m_cJsonObject["nodes"][index];
	if (node.IsContain("mesh")) mesh_index = node["mesh"].ToInt();
	if (node.IsContain("skin")) skin_index = node["skin"].ToInt();
	if (node.IsContain("matrix")) for (int i = 0; i < 16; i++) local[i] = node["matrix"][i].ToFloat();
	else if (node.IsContain("translation") || node.IsContain("rotation") || node.IsContain("scale")) {
		CVector3 translation;
		CQuaternion rotation;
		CVector3 scale(1.0f, 1.0f, 1.0f);
		if (node.IsContain("translation")) for (int i = 0; i < 3; i++) translation[i] = node["translation"][i].ToFloat();
		if (node.IsContain("rotation")) for (int i = 0; i < 4; i++) rotation[i] = node["rotation"][i].ToFloat();
		if (node.IsContain("scale")) for (int i = 0; i < 3; i++) scale[i] = node["scale"][i].ToFloat();
		local.MakeTransform(scale, rotation, translation);
	}
	const CMatrix4 transform = matrix * local;
	if (node.IsContain("children")) {
		size_t children_count = node["children"].GetCount();
		for (size_t i = 0; i < children_count; i++) {
			ReadMeshNode(node["children"][i].ToInt(), transform);
		}
	}
	if (mesh_index >= 0) {
		map<size_t, int> skinMapper;
		if (skin_index >= 0) {
			CMatrix4 globalTransformInv = CMatrix4(transform).Invert();
			CJsonObject& joints = m_cJsonObject["skins"][skin_index]["joints"];
			for (size_t i = 0; i < joints.GetCount(); i++) {
				int node_index = joints[i].ToInt();
				skinMapper.insert(std::pair<size_t, int>(i, node_index));
				if (!m_setJoints[node_index]) {
					m_mapJoints[node_index]->bindMatrixInv *= globalTransformInv;
					m_setJoints[node_index] = true;
				}
			}
		}
		ReadMesh(m_cJsonObject["meshes"][mesh_index], transform, skinMapper);
	}
}

/**
* 读取网格
*/
void CMeshLoaderGltf::ReadMesh(CJsonObject& mesh, const CMatrix4& matrix, map<size_t, int>& skinMapper) {
	size_t primitive_count = mesh["primitives"].GetCount();
	for (size_t i = 0; i < primitive_count; i++) {
		CJsonObject& primitive = mesh["primitives"][i];
		CJsonObject& attributes = primitive["attributes"];
		int material_index = primitive.IsContain("material") ? primitive["material"].ToInt() : -1;
		int mesh_indices = primitive["indices"].ToInt();
		int mesh_position = attributes["POSITION"].ToInt();
		int mesh_normal = attributes.IsContain("NORMAL") ? attributes["NORMAL"].ToInt() : -1;
		int mesh_texcoord = attributes.IsContain("TEXCOORD_0") ? attributes["TEXCOORD_0"].ToInt() : -1;
		int mesh_color = attributes.IsContain("COLOR_0") ? attributes["COLOR_0"].ToInt() : -1;
		int mesh_joints = attributes.IsContain("JOINTS_0") ? attributes["JOINTS_0"].ToInt() : -1;
		int mesh_weights = attributes.IsContain("WEIGHTS_0") ? attributes["WEIGHTS_0"].ToInt() : -1;
		CMesh* pMesh = new CMesh();
		CMaterial* pMaterial = pMesh->GetMaterial();
		if (material_index >= 0) {
			int texcoord = 0;
			ReadMaterial(material_index, pMaterial, &texcoord);
			mesh_texcoord = attributes["TEXCOORD_" + std::to_string(texcoord)].ToInt();
		}
		AddTriangles(pMesh, mesh_indices);
		AddVertices(pMesh, mesh_position, mesh_joints, mesh_weights, skinMapper);
		// 设置顶点属性
		if (mesh_normal >= 0) SetupVerticesNormal(pMesh, mesh_normal);
		if (mesh_texcoord >= 0) SetupVerticesTexCoord(pMesh, mesh_texcoord);
		if (mesh_color >= 0) SetupVerticesColor(pMesh, mesh_color);
		if (pMaterial->GetTextureCount() == 0) pMaterial->SetTexture("");
		pMesh->Transform(matrix);
		pMesh->Create(mesh_joints >= 0);
		m_pMeshData->AddMesh(pMesh);
	}
}

/**
* 读取材质信息
*/
void CMeshLoaderGltf::ReadMaterial(int index, CMaterial* material, int* texcoord) {
	CJsonObject& node = m_cJsonObject["materials"][index];
	material->GetName() = node["name"].ToString();
	if (node.IsContain("doubleSided")) material->SetRenderMode(!node["doubleSided"].ToBool(), true, false);
	if (node.IsContain("pbrMetallicRoughness")) {
		CJsonObject& pbr = node["pbrMetallicRoughness"];
		if (pbr.IsContain("metallicFactor")) material->m_fMetalness = pbr["metallicFactor"].ToFloat();
		if (pbr.IsContain("roughnessFactor")) material->m_fRoughness = pbr["roughnessFactor"].ToFloat();
		if (pbr.IsContain("baseColorFactor")) for (int i = 0; i < 4; i++) material->m_fColor[i] = pbr["baseColorFactor"][i].ToFloat();
		if (pbr.IsContain("baseColorTexture")) {
			CJsonObject& texture = m_cJsonObject["textures"][pbr["baseColorTexture"]["index"].ToInt()];
			*texcoord = pbr["baseColorTexture"]["texCoord"].ToInt();
			ReadTexture(texture, material);
		}
	}
	if (material->GetTextureCount() == 0 && node.IsContain("extensions")) {
		if (node["extensions"].IsContain("KHR_materials_pbrSpecularGlossiness")) {
			CJsonObject& pbr = node["extensions"]["KHR_materials_pbrSpecularGlossiness"];
			if (pbr.IsContain("diffuseFactor")) for (int i = 0; i < 4; i++) material->m_fColor[i] = pbr["diffuseFactor"][i].ToFloat();
			if (pbr.IsContain("diffuseTexture")) {
				CJsonObject& texture = m_cJsonObject["textures"][pbr["diffuseTexture"]["index"].ToInt()];
				*texcoord = pbr["diffuseTexture"]["texCoord"].ToInt();
				ReadTexture(texture, material);
			}
		} else if (node["extensions"].IsContain("KHR_techniques_webgl")) {
			CJsonObject& webgl = node["extensions"]["KHR_techniques_webgl"];
			CJsonObject& texture = m_cJsonObject["textures"][webgl["values"][0]["index"].ToInt()];
			*texcoord = webgl["values"][0]["texCoord"].ToInt();
			ReadTexture(texture, material);
		}
	}
}

/**
* 读取纹理数据
*/
void CMeshLoaderGltf::ReadTexture(CJsonObject& texture, CMaterial* material) {
	CJsonObject& image = m_cJsonObject["images"][texture["source"].ToInt()];
	CTexture* pTexture = 0;
	if (image.IsContain("uri")) {
		pTexture = CEngine::GetTextureManager()->Create(m_strBaseDir + image["uri"].ToString());
	} else if (image.IsContain("bufferView")) {
		string name = m_strFilename + "#";
		if (image.IsContain("name")) name += image["name"].ToString();
		else if (texture.IsContain("name")) name += texture["name"].ToString();
		string mime = image["mimeType"].ToString();
		CJsonObject& bufferView = m_cJsonObject["bufferViews"][image["bufferView"].ToInt()];
		unsigned char* buffer = m_vecBuffer[bufferView["buffer"].ToInt()].data;
		if (!buffer) {
			material->SetTexture("");
			return;
		}
		unsigned char* data = buffer + bufferView["byteOffset"].ToInt();
		unsigned int length = static_cast<unsigned int>(bufferView["byteLength"].ToInt());
		CFileManager::FileType fileType = CFileManager::BIN;
		if (mime == "image/bmp") fileType = CFileManager::BMP;
		else if (mime == "image/tga") fileType = CFileManager::TGA;
		else if (mime == "image/png") fileType = CFileManager::PNG;
		else if (mime == "image/jpeg" || mime == "image/jpg") fileType = CFileManager::JPG;
		else {
			material->SetTexture("");
			return;
		}
		CFileManager::CImageFile image(fileType);
		CEngine::GetFileManager()->ReadFile(data, length, &image);
		pTexture = CEngine::GetTextureManager()->Create(name, image.width, image.height, image.channels, image.contents, true);
	} else return;
	if (texture.IsContain("sampler")) {
		CJsonObject& sampler = m_cJsonObject["samplers"][texture["sampler"].ToInt()];
		int wrapModeS = sampler.IsContain("wrapS") ? sampler["wrapS"].ToInt() : 10497;
		int wrapModeT = sampler.IsContain("wrapT") ? sampler["wrapT"].ToInt() : 10497;
		pTexture->SetWrapModeClampToEdge(wrapModeS == 33071, wrapModeT == 33071);
		pTexture->SetWrapModeMirroredRepeat(wrapModeS == 33648, wrapModeT == 33648);
		pTexture->SetWrapModeRepeat(wrapModeS == 10497, wrapModeT == 10497);
	}
	material->SetTexture(pTexture);
	CEngine::GetTextureManager()->Drop(pTexture);
}

/**
* 读取动画数据
*/
void CMeshLoaderGltf::ReadAnimation() {
	if (!m_cJsonObject.IsContain("animations")) return;
	size_t count = m_cJsonObject["animations"].GetCount();
	float time_begin = 0.0f;
	float time_end = 0.0f;
	for (size_t i = 0; i < count; i++) {
		CJsonObject& animation = m_cJsonObject["animations"][i];
		CJsonObject& channels = animation["channels"];
		string name = animation.IsContain("name") ? animation["name"].ToString() : std::to_string(i);
		for (size_t j = 0; j < channels.GetCount(); j++) {
			int target_joint = channels[j]["target"]["node"].ToInt();
			map<int, SJoint*>::iterator iter = m_mapJoints.find(target_joint);
			if (iter == m_mapJoints.end()) continue;
			SJoint* pJoint = iter->second;
			CJsonObject& sampler = animation["samplers"][channels[j]["sampler"].ToInt()];
			string path = channels[j]["target"]["path"].ToString();
			CJsonObject& accessor_input = m_cJsonObject["accessors"][sampler["input"].ToInt()];
			CJsonObject& accessor_output = m_cJsonObject["accessors"][sampler["output"].ToInt()];
			CJsonObject& bufferView_input = m_cJsonObject["bufferViews"][accessor_input["bufferView"].ToInt()];
			CJsonObject& bufferView_output = m_cJsonObject["bufferViews"][accessor_output["bufferView"].ToInt()];
			unsigned char* buffer_input = m_vecBuffer[bufferView_input["buffer"].ToInt()].data;
			unsigned char* buffer_output = m_vecBuffer[bufferView_output["buffer"].ToInt()].data;
			if (!buffer_input || !buffer_output) continue;
			if (accessor_input["type"].ToString() != "SCALAR") continue;
			if (accessor_input["componentType"].ToInt() != TYPE_FLOAT) continue;
			if (accessor_output["componentType"].ToInt() != TYPE_FLOAT) continue;
			int count = accessor_input["count"].ToInt();
			int byteOffset_input = bufferView_input["byteOffset"].ToInt();
			int byteOffset_output = bufferView_output["byteOffset"].ToInt();
			if (accessor_input.IsContain("byteOffset")) byteOffset_input += accessor_input["byteOffset"].ToInt();
			if (accessor_output.IsContain("byteOffset")) byteOffset_output += accessor_output["byteOffset"].ToInt();
			int output_stride = accessor_output["type"].ToString() == "VEC3" ? 3 : 4;
			int byteStride_input = bufferView_input.IsContain("byteStride") ? bufferView_input["byteStride"].ToInt() : sizeof(float);
			int byteStride_output = bufferView_output.IsContain("byteStride") ? bufferView_output["byteStride"].ToInt() : output_stride * sizeof(float);
			if (path == "translation") {
				for (int k = 0; k < count; k++) {
					float* data_input = reinterpret_cast<float*>(buffer_input + byteOffset_input + k * byteStride_input);
					float* data_output = reinterpret_cast<float*>(buffer_output + byteOffset_output + k * byteStride_output);
					SAnimationPosKey key;
					key.time = time_begin + data_input[0];
					key.param.SetValue(m_mapLocalInv[target_joint] * CVector3(data_output));
					pJoint->keyPos.push_back(key);
					time_end = std::max(time_end, key.time);
				}
			} else if (path == "rotation") {
				for (int k = 0; k < count; k++) {
					float* data_input = reinterpret_cast<float*>(buffer_input + byteOffset_input + k * byteStride_input);
					float* data_output = reinterpret_cast<float*>(buffer_output + byteOffset_output + k * byteStride_output);
					SAnimationRotKey key;
					key.time = time_begin + data_input[0];
					key.param.SetValue(m_mapLocalInv[target_joint] * CQuaternion(data_output));
					pJoint->keyRot.push_back(key);
					time_end = std::max(time_end, key.time);
				}
			}
		}
		m_pMeshData->AddAnimation(name, time_begin, time_end);
		time_begin = time_end + 1.0f;
	}
}

/**
* 添加网格三角形
*/
void CMeshLoaderGltf::AddTriangles(CMesh* mesh, int accessoIndex) {
	CJsonObject& accessor = m_cJsonObject["accessors"][accessoIndex];
	CJsonObject& bufferView = m_cJsonObject["bufferViews"][accessor["bufferView"].ToInt()];
	unsigned char* buffer = m_vecBuffer[bufferView["buffer"].ToInt()].data;
	if (!buffer) return;
	if (accessor["type"].ToString() != "SCALAR") return;
	int componentType = accessor["componentType"].ToInt();
	int count = accessor["count"].ToInt();
	int byteOffset = bufferView["byteOffset"].ToInt();
	if (accessor.IsContain("byteOffset")) byteOffset += accessor["byteOffset"].ToInt();
	if (componentType == TYPE_UNSIGNED_BYTE) {
		unsigned char* data = buffer + byteOffset;
		for (int i = 0; i < count; i += 3) mesh->AddTriangle(data[i], data[i + 1], data[i + 2]);
	} else if (componentType == TYPE_UNSIGNED_SHORT) {
		unsigned short* data = reinterpret_cast<unsigned short*>(buffer + byteOffset);
		for (int i = 0; i < count; i += 3) mesh->AddTriangle(data[i], data[i + 1], data[i + 2]);
	} else if (componentType == TYPE_UNSIGNED_INT) {
		unsigned int* data = reinterpret_cast<unsigned int*>(buffer + byteOffset);
		for (int i = 0; i < count; i += 3) mesh->AddTriangle(data[i], data[i + 1], data[i + 2]);
	}
}

/**
* 添加网格顶点
*/
void CMeshLoaderGltf::AddVertices(CMesh* mesh, int position, int joints, int weights, map<size_t, int>& skinMapper) {
	CJsonObject& accessor_pos = m_cJsonObject["accessors"][position];
	CJsonObject& bufferView_pos = m_cJsonObject["bufferViews"][accessor_pos["bufferView"].ToInt()];
	unsigned char* buffer_pos = m_vecBuffer[bufferView_pos["buffer"].ToInt()].data;
	if (!buffer_pos) return;
	if (accessor_pos["type"].ToString() != "VEC3") return;
	int componentType_pos = accessor_pos["componentType"].ToInt();
	int count = accessor_pos["count"].ToInt();
	int byteOffset_pos = bufferView_pos["byteOffset"].ToInt();
	if (accessor_pos.IsContain("byteOffset")) byteOffset_pos += accessor_pos["byteOffset"].ToInt();
	if (componentType_pos != TYPE_FLOAT) return;
	int byteStride_pos = bufferView_pos.IsContain("byteStride") ? bufferView_pos["byteStride"].ToInt() : 3 * sizeof(float);
	mesh->SetVertexUsage(count);
	if (joints >= 0 && weights >= 0) {
		CJsonObject& accessor_joint = m_cJsonObject["accessors"][joints];
		CJsonObject& accessor_weight = m_cJsonObject["accessors"][weights];
		CJsonObject& bufferView_joint = m_cJsonObject["bufferViews"][accessor_joint["bufferView"].ToInt()];
		CJsonObject& bufferView_weight = m_cJsonObject["bufferViews"][accessor_weight["bufferView"].ToInt()];
		unsigned char* buffer_joint = m_vecBuffer[bufferView_joint["buffer"].ToInt()].data;
		unsigned char* buffer_weight = m_vecBuffer[bufferView_weight["buffer"].ToInt()].data;
		if (!buffer_joint || !buffer_weight) return;
		int componentType_joint = accessor_joint["componentType"].ToInt();
		int componentType_weight = accessor_weight["componentType"].ToInt();
		int byteOffset_joint = bufferView_joint["byteOffset"].ToInt();
		int byteOffset_weight = bufferView_weight["byteOffset"].ToInt();
		if (accessor_joint.IsContain("byteOffset")) byteOffset_joint += accessor_joint["byteOffset"].ToInt();
		if (accessor_weight.IsContain("byteOffset")) byteOffset_weight += accessor_weight["byteOffset"].ToInt();
		if (accessor_joint["type"].ToString() == "VEC4" && accessor_joint["count"].ToInt() == count &&
			accessor_weight["type"].ToString() == "VEC4" && accessor_weight["count"].ToInt() == count &&
			componentType_weight == TYPE_FLOAT) {
			int byteStride_weight = bufferView_weight.IsContain("byteStride") ? bufferView_weight["byteStride"].ToInt() : 4 * sizeof(float);
			if (componentType_joint == TYPE_UNSIGNED_BYTE) {
				int byteStride_joint = bufferView_joint.IsContain("byteStride") ? bufferView_joint["byteStride"].ToInt() : 4;
				for (int i = 0; i < count; i++) {
					float* data_pos = reinterpret_cast<float*>(buffer_pos + byteOffset_pos + i * byteStride_pos);
					float* data_weight = reinterpret_cast<float*>(buffer_weight + byteOffset_weight + i * byteStride_weight);
					unsigned char* data_joint = buffer_joint + byteOffset_joint + i * byteStride_joint;
					CVertexJoint joint;
					joint.AddBind(m_mapJoints[skinMapper[(size_t)data_joint[0]]]->index, data_weight[0]);
					joint.AddBind(m_mapJoints[skinMapper[(size_t)data_joint[1]]]->index, data_weight[1]);
					joint.AddBind(m_mapJoints[skinMapper[(size_t)data_joint[2]]]->index, data_weight[2]);
					joint.AddBind(m_mapJoints[skinMapper[(size_t)data_joint[3]]]->index, data_weight[3]);
					mesh->AddVertex(CVertex(data_pos[0], data_pos[1], data_pos[2]), joint.Normalize());
				}
			} else if (componentType_joint == TYPE_UNSIGNED_SHORT) {
				int byteStride_joint = bufferView_joint.IsContain("byteStride") ? bufferView_joint["byteStride"].ToInt() : 4 * sizeof(unsigned short);
				for (int i = 0; i < count; i++) {
					float* data_pos = reinterpret_cast<float*>(buffer_pos + byteOffset_pos + i * byteStride_pos);
					float* data_weight = reinterpret_cast<float*>(buffer_weight + byteOffset_weight + i * byteStride_weight);
					unsigned short* data_joint = reinterpret_cast<unsigned short*>(buffer_joint + byteOffset_joint + i * byteStride_joint);
					CVertexJoint joint;
					joint.AddBind(m_mapJoints[skinMapper[(size_t)data_joint[0]]]->index, data_weight[0]);
					joint.AddBind(m_mapJoints[skinMapper[(size_t)data_joint[1]]]->index, data_weight[1]);
					joint.AddBind(m_mapJoints[skinMapper[(size_t)data_joint[2]]]->index, data_weight[2]);
					joint.AddBind(m_mapJoints[skinMapper[(size_t)data_joint[3]]]->index, data_weight[3]);
					mesh->AddVertex(CVertex(data_pos[0], data_pos[1], data_pos[2]), joint.Normalize());
				}
			}
			return;
		}
	}
	for (int i = 0; i < count; i++) {
		float* data_pos = reinterpret_cast<float*>(buffer_pos + byteOffset_pos + i * byteStride_pos);
		mesh->AddVertex(CVertex(data_pos[0], data_pos[1], data_pos[2]));
	}
}

/**
* 设置顶点法相
*/
void CMeshLoaderGltf::SetupVerticesNormal(CMesh* mesh, int accessorIndex) {
	CJsonObject& accessor = m_cJsonObject["accessors"][accessorIndex];
	CJsonObject& bufferView = m_cJsonObject["bufferViews"][accessor["bufferView"].ToInt()];
	unsigned char* buffer = m_vecBuffer[bufferView["buffer"].ToInt()].data;
	if (!buffer) return;
	if (accessor["type"].ToString() != "VEC3") return;
	int componentType = accessor["componentType"].ToInt();
	size_t count = accessor["count"].ToInt();
	int byteOffset = bufferView["byteOffset"].ToInt();
	if (accessor.IsContain("byteOffset")) byteOffset += accessor["byteOffset"].ToInt();
	if (count != mesh->GetVertexCount()) return;
	if (componentType == TYPE_FLOAT) {
		int byteStride = bufferView.IsContain("byteStride") ? bufferView["byteStride"].ToInt() : 3 * sizeof(float);
		for (size_t i = 0; i < count; i++) {
			float* data = reinterpret_cast<float*>(buffer + byteOffset + i * byteStride);
			mesh->GetVertex(i)->SetNormal(data[0], data[1], data[2]);
		}
	}
}

/**
* 设置顶点纹理坐标
*/
void CMeshLoaderGltf::SetupVerticesTexCoord(CMesh* mesh, int accessorIndex) {
	CJsonObject& accessor = m_cJsonObject["accessors"][accessorIndex];
	CJsonObject& bufferView = m_cJsonObject["bufferViews"][accessor["bufferView"].ToInt()];
	unsigned char* buffer = m_vecBuffer[bufferView["buffer"].ToInt()].data;
	if (!buffer) return;
	if (accessor["type"].ToString() != "VEC2") return;
	int componentType = accessor["componentType"].ToInt();
	size_t count = accessor["count"].ToInt();
	int byteOffset = bufferView["byteOffset"].ToInt();
	if (accessor.IsContain("byteOffset")) byteOffset += accessor["byteOffset"].ToInt();
	if (count != mesh->GetVertexCount()) return;
	if (componentType == TYPE_UNSIGNED_BYTE) {
		const float scale = 0.00392f;
		int byteStride = bufferView.IsContain("byteStride") ? bufferView["byteStride"].ToInt() : 2;
		for (size_t i = 0; i < count; i++) {
			unsigned char* data = buffer + byteOffset + i * byteStride;
			mesh->GetVertex(i)->SetTexCoord(data[0] * scale, data[1] * scale);
		}
	} else if (componentType == TYPE_UNSIGNED_SHORT) {
		const float scale = 1.5259E-5f;
		int byteStride = bufferView.IsContain("byteStride") ? bufferView["byteStride"].ToInt() : 2 * sizeof(unsigned short);
		for (size_t i = 0; i < count; i++) {
			unsigned short* data = reinterpret_cast<unsigned short*>(buffer + byteOffset + i * byteStride);
			mesh->GetVertex(i)->SetTexCoord(data[0] * scale, data[1] * scale);
		}
	} else if (componentType == TYPE_FLOAT) {
		int byteStride = bufferView.IsContain("byteStride") ? bufferView["byteStride"].ToInt() : 2 * sizeof(float);
		for (size_t i = 0; i < count; i++) {
			float* data = reinterpret_cast<float*>(buffer + byteOffset + i * byteStride);
			mesh->GetVertex(i)->SetTexCoord(data[0], data[1]);
		}
	}
}

/**
* 设置顶点颜色
*/
void CMeshLoaderGltf::SetupVerticesColor(CMesh* mesh, int accessorIndex) {
	CJsonObject& accessor = m_cJsonObject["accessors"][accessorIndex];
	CJsonObject& bufferView = m_cJsonObject["bufferViews"][accessor["bufferView"].ToInt()];
	unsigned char* buffer = m_vecBuffer[bufferView["buffer"].ToInt()].data;
	if (!buffer) return;
	string type = accessor["type"].ToString();
	if (type != "VEC3" && type != "VEC4") return;
	int componentType = accessor["componentType"].ToInt();
	size_t count = accessor["count"].ToInt();
	int byteOffset = bufferView["byteOffset"].ToInt();
	if (accessor.IsContain("byteOffset")) byteOffset += accessor["byteOffset"].ToInt();
	if (count != mesh->GetVertexCount()) return;
	int stride = type == "VEC3" ? 3 : 4;
	if (componentType == TYPE_UNSIGNED_BYTE) {
		const float scale = 0.00392f;
		int byteStride = bufferView.IsContain("byteStride") ? bufferView["byteStride"].ToInt() : stride;
		for (size_t i = 0; i < count; i++) {
			unsigned char* data = buffer + byteOffset + i * byteStride;
			float r = data[0] * scale;
			float g = data[1] * scale;
			float b = data[2] * scale;
			float a = stride == 4 ? data[3] * scale : 1.0f;
			mesh->GetVertex(i)->SetColor(r, g, b, a);
		}
	} else if (componentType == TYPE_UNSIGNED_SHORT) {
		const float scale = 1.5259E-5f;
		int byteStride = bufferView.IsContain("byteStride") ? bufferView["byteStride"].ToInt() : stride * sizeof(unsigned short);
		for (size_t i = 0; i < count; i++) {
			unsigned short* data = reinterpret_cast<unsigned short*>(buffer + byteOffset + i * byteStride);
			float r = data[0] * scale;
			float g = data[1] * scale;
			float b = data[2] * scale;
			float a = stride == 4 ? data[3] * scale : 1.0f;
			mesh->GetVertex(i)->SetColor(r, g, b, a);
		}
	} else if (componentType == TYPE_FLOAT) {
		int byteStride = bufferView.IsContain("byteStride") ? bufferView["byteStride"].ToInt() : stride * sizeof(float);
		for (size_t i = 0; i < count; i++) {
			float* data = reinterpret_cast<float*>(buffer + byteOffset + i * byteStride);
			float r = data[0];
			float g = data[1];
			float b = data[2];
			float a = stride == 4 ? data[3] : 1.0f;
			mesh->GetVertex(i)->SetColor(r, g, b, a);
		}
	}
}