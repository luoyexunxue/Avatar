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
	CMeshLoaderGltf::SContext context;
	context.filename = filename;
	context.baseDir = CFileManager::GetDirectory(filename);
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
				context.buffers.push_back(bin);
			}
		}
	}
	// 解析 Json 对象
	if (!context.jsonObject.Parse(json, jsonLength)) return 0;
	if (context.jsonObject["asset"]["version"].ToString() != "2.0") return 0;
	if (!context.jsonObject.IsContain("scenes")) return 0;
	// 解析 GLTF 数据
	if (!isBinary) ReadBuffers(&context);
	CMatrix4 transform;
	transform.RotateX(1.570796f);
	context.meshData = new CMeshData();
	ReadSkin(&context);
	CJsonObject& nodes = context.jsonObject["scenes"][0]["nodes"];
	for (size_t i = 0; i < nodes.GetCount(); i++) ReadJointNode(&context, nodes[i].ToInt(), transform, 0);
	for (size_t i = 0; i < nodes.GetCount(); i++) ReadMeshNode(&context, nodes[i].ToInt(), transform);
	ReadAnimation(&context);
	for (size_t i = 0; i < context.buffers.size(); i++) {
		if (context.buffers[i].allocated) delete[] context.buffers[i].data;
	}
	return context.meshData;
}

/**
* 读取二进制缓冲区
*/
void CMeshLoaderGltf::ReadBuffers(SContext* context) {
	size_t count = context->jsonObject["buffers"].GetCount();
	for (size_t i = 0; i < count; i++) {
		SBufferBin bin;
		bin.allocated = false;
		bin.data = 0;
		bin.size = 0;
		string uri = context->jsonObject["buffers"][i]["uri"].ToString();
		if (uri.find("data:") == 0) {
			size_t pos = uri.find(";base64,");
			if (pos != string::npos) {
				bin.allocated = true;
				bin.size = (uri.length() - pos - 8) * 3 / 4;
				bin.data = new unsigned char[bin.size];
				bin.size = CBase64::Decode(uri.c_str() + pos + 8, bin.data, bin.size);
				context->buffers.push_back(bin);
				continue;
			}
		}
		CFileManager::CBinaryFile file;
		if (CEngine::GetFileManager()->ReadFile(context->baseDir + uri, &file)) {
			bin.allocated = true;
			bin.size = file.size;
			bin.data = new unsigned char[bin.size];
			memcpy(bin.data, file.contents, file.size);
		}
		context->buffers.push_back(bin);
	}
}

/**
* 读取骨骼蒙皮数据
*/
void CMeshLoaderGltf::ReadSkin(SContext* context) {
	if (!context->jsonObject.IsContain("skins")) return;
	for (size_t i = 0; i < context->jsonObject["skins"].GetCount(); i++) {
		CJsonObject& skin = context->jsonObject["skins"][i];
		CJsonObject& joints = skin["joints"];
		if (skin.IsContain("skeleton")) context->jointSet.insert(std::pair<int, bool>(skin["skeleton"].ToInt(), false));
		for (size_t j = 0; j < joints.GetCount(); j++) context->jointSet.insert(std::pair<int, bool>(joints[j].ToInt(), false));
		// 获取关节的逆绑定矩阵
		if (skin.IsContain("inverseBindMatrices")) {
			CJsonObject& accessor = context->jsonObject["accessors"][skin["inverseBindMatrices"].ToInt()];
			CJsonObject& bufferView = context->jsonObject["bufferViews"][accessor["bufferView"].ToInt()];
			unsigned char* buffer = context->buffers[bufferView["buffer"].ToInt()].data;
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
					context->bindInvMap[joints[i].ToInt()] = CMatrix4(data);
				}
			}
		}
	}
}

/**
* 读取骨骼节点
*/
void CMeshLoaderGltf::ReadJointNode(SContext* context, int index, const CMatrix4& matrix, SJoint* parent) {
	CMatrix4 local;
	CJsonObject& node = context->jsonObject["nodes"][index];
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
	if (context->jointSet.count(index) > 0) {
		SJoint* pJoint = new SJoint();
		context->meshData->AddJoint(pJoint);
		pJoint->name = node["name"].ToString();
		pJoint->parent = parent;
		pJoint->localMatrix = parent ? local : transform;
		pJoint->worldMatrix = transform;
		pJoint->bindMatrixInv = context->bindInvMap[index];
		context->localInvMap.insert(std::pair<int, CMatrix4>(index, local.Invert()));
		context->jointMap.insert(std::pair<int, SJoint*>(index, pJoint));
		parent = pJoint;
	}
	if (node.IsContain("children")) {
		size_t children_count = node["children"].GetCount();
		for (size_t i = 0; i < children_count; i++) {
			ReadJointNode(context, node["children"][i].ToInt(), transform, parent);
		}
	}
}

/**
* 读取网格节点
*/
void CMeshLoaderGltf::ReadMeshNode(SContext* context, int index, const CMatrix4& matrix) {
	int mesh_index = -1;
	int skin_index = -1;
	CMatrix4 local;
	CJsonObject& node = context->jsonObject["nodes"][index];
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
			ReadMeshNode(context, node["children"][i].ToInt(), transform);
		}
	}
	if (mesh_index >= 0) {
		map<size_t, int> skinMapper;
		if (skin_index >= 0) {
			CMatrix4 globalTransformInv = CMatrix4(transform).Invert();
			CJsonObject& joints = context->jsonObject["skins"][skin_index]["joints"];
			for (size_t i = 0; i < joints.GetCount(); i++) {
				int node_index = joints[i].ToInt();
				skinMapper.insert(std::pair<size_t, int>(i, node_index));
				if (!context->jointSet[node_index]) {
					context->jointMap[node_index]->bindMatrixInv *= globalTransformInv;
					context->jointSet[node_index] = true;
				}
			}
		}
		ReadMesh(context, mesh_index, transform, skinMapper);
	}
}

/**
* 读取网格
*/
void CMeshLoaderGltf::ReadMesh(SContext* context, int index, const CMatrix4& matrix, map<size_t, int>& skinMapper) {
	CJsonObject& mesh = context->jsonObject["meshes"][index];
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
		pMaterial->SetTextureBuffered("");
		if (material_index >= 0) {
			int texcoord = 0;
			ReadMaterial(context, material_index, pMaterial, &texcoord);
			mesh_texcoord = attributes["TEXCOORD_" + std::to_string(texcoord)].ToInt();
		}
		AddTriangles(context, pMesh, mesh_indices);
		AddVertices(context, pMesh, mesh_position, mesh_joints, mesh_weights, skinMapper);
		// 设置顶点属性
		if (mesh_normal >= 0) SetupVerticesNormal(context, pMesh, mesh_normal);
		if (mesh_texcoord >= 0) SetupVerticesTexCoord(context, pMesh, mesh_texcoord);
		if (mesh_color >= 0) SetupVerticesColor(context, pMesh, mesh_color);
		pMesh->Transform(matrix);
		context->meshData->AddMesh(pMesh);
	}
}

/**
* 读取材质信息
*/
void CMeshLoaderGltf::ReadMaterial(SContext* context, int index, CMaterial* material, int* texcoord) {
	CJsonObject& node = context->jsonObject["materials"][index];
	material->GetName() = node["name"].ToString();
	if (node.IsContain("doubleSided")) material->SetRenderMode(!node["doubleSided"].ToBool(), true, false);
	if (node.IsContain("pbrMetallicRoughness")) {
		CJsonObject& pbr = node["pbrMetallicRoughness"];
		if (pbr.IsContain("metallicFactor")) material->m_fMetalness = pbr["metallicFactor"].ToFloat();
		if (pbr.IsContain("roughnessFactor")) material->m_fRoughness = pbr["roughnessFactor"].ToFloat();
		if (pbr.IsContain("baseColorFactor")) for (int i = 0; i < 4; i++) material->m_fColor[i] = pbr["baseColorFactor"][i].ToFloat();
		if (pbr.IsContain("baseColorTexture")) {
			CJsonObject& texture = context->jsonObject["textures"][pbr["baseColorTexture"]["index"].ToInt()];
			*texcoord = pbr["baseColorTexture"]["texCoord"].ToInt();
			if (ReadTexture(context, texture, material)) return;
		}
	}
	if (node.IsContain("extensions")) {
		if (node["extensions"].IsContain("KHR_materials_pbrSpecularGlossiness")) {
			CJsonObject& pbr = node["extensions"]["KHR_materials_pbrSpecularGlossiness"];
			if (pbr.IsContain("diffuseFactor")) for (int i = 0; i < 4; i++) material->m_fColor[i] = pbr["diffuseFactor"][i].ToFloat();
			if (pbr.IsContain("diffuseTexture")) {
				CJsonObject& texture = context->jsonObject["textures"][pbr["diffuseTexture"]["index"].ToInt()];
				*texcoord = pbr["diffuseTexture"]["texCoord"].ToInt();
				ReadTexture(context, texture, material);
			}
		} else if (node["extensions"].IsContain("KHR_techniques_webgl")) {
			CJsonObject& webgl = node["extensions"]["KHR_techniques_webgl"];
			CJsonObject& texture = context->jsonObject["textures"][webgl["values"][0]["index"].ToInt()];
			*texcoord = webgl["values"][0]["texCoord"].ToInt();
			ReadTexture(context, texture, material);
		}
	}
}

/**
* 读取纹理数据
*/
bool CMeshLoaderGltf::ReadTexture(SContext* context, CJsonObject& texture, CMaterial* material) {
	CJsonObject& image = context->jsonObject["images"][texture["source"].ToInt()];
	if (image.IsContain("uri")) {
		material->SetTextureBuffered(context->baseDir + image["uri"].ToString());
	} else if (image.IsContain("bufferView")) {
		string name = context->filename + "#";
		if (image.IsContain("name")) name += image["name"].ToString();
		else if (texture.IsContain("name")) name += texture["name"].ToString();
		string mime = image["mimeType"].ToString();
		CJsonObject& bufferView = context->jsonObject["bufferViews"][image["bufferView"].ToInt()];
		unsigned char* buffer = context->buffers[bufferView["buffer"].ToInt()].data;
		if (!buffer) {
			material->SetTextureBuffered("");
			return false;
		}
		unsigned char* data = buffer + bufferView["byteOffset"].ToInt();
		unsigned int length = static_cast<unsigned int>(bufferView["byteLength"].ToInt());
		CFileManager::FileType fileType = CFileManager::BIN;
		if (mime == "image/bmp") fileType = CFileManager::BMP;
		else if (mime == "image/tga") fileType = CFileManager::TGA;
		else if (mime == "image/png") fileType = CFileManager::PNG;
		else if (mime == "image/jpeg" || mime == "image/jpg") fileType = CFileManager::JPG;
		else {
			material->SetTextureBuffered("");
			return false;
		}
		CFileManager::CImageFile image(fileType);
		CEngine::GetFileManager()->ReadFile(data, length, &image);
		material->SetTextureBuffered(name, image.width, image.height, image.channels, image.contents, true);
	} else return false;
	if (texture.IsContain("sampler")) {
		CJsonObject& sampler = context->jsonObject["samplers"][texture["sampler"].ToInt()];
		int wrapModeS = sampler.IsContain("wrapS") ? sampler["wrapS"].ToInt() : 10497;
		int wrapModeT = sampler.IsContain("wrapT") ? sampler["wrapT"].ToInt() : 10497;
		int mode_s = wrapModeS == 10497 ? 1 : (wrapModeS == 33071 ? 2 : (wrapModeS == 33648 ? 3 : 0));
		int mode_t = wrapModeT == 10497 ? 1 : (wrapModeT == 33071 ? 2 : (wrapModeT == 33648 ? 3 : 0));
		material->SetTextureBufferedWrapMode(mode_s, mode_t);
	}
	return true;
}

/**
* 读取动画数据
*/
void CMeshLoaderGltf::ReadAnimation(SContext* context) {
	if (!context->jsonObject.IsContain("animations")) return;
	size_t count = context->jsonObject["animations"].GetCount();
	float time_begin = 0.0f;
	float time_end = 0.0f;
	for (size_t i = 0; i < count; i++) {
		CJsonObject& animation = context->jsonObject["animations"][i];
		CJsonObject& channels = animation["channels"];
		string name = animation.IsContain("name") ? animation["name"].ToString() : std::to_string(i);
		for (size_t j = 0; j < channels.GetCount(); j++) {
			int target_joint = channels[j]["target"]["node"].ToInt();
			map<int, SJoint*>::iterator iter = context->jointMap.find(target_joint);
			if (iter == context->jointMap.end()) continue;
			SJoint* pJoint = iter->second;
			CJsonObject& sampler = animation["samplers"][channels[j]["sampler"].ToInt()];
			string path = channels[j]["target"]["path"].ToString();
			CJsonObject& accessor_input = context->jsonObject["accessors"][sampler["input"].ToInt()];
			CJsonObject& accessor_output = context->jsonObject["accessors"][sampler["output"].ToInt()];
			CJsonObject& bufferView_input = context->jsonObject["bufferViews"][accessor_input["bufferView"].ToInt()];
			CJsonObject& bufferView_output = context->jsonObject["bufferViews"][accessor_output["bufferView"].ToInt()];
			unsigned char* buffer_input = context->buffers[bufferView_input["buffer"].ToInt()].data;
			unsigned char* buffer_output = context->buffers[bufferView_output["buffer"].ToInt()].data;
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
					key.param.SetValue(context->localInvMap[target_joint] * CVector3(data_output));
					pJoint->keyPos.push_back(key);
					time_end = std::max(time_end, key.time);
				}
			} else if (path == "rotation") {
				for (int k = 0; k < count; k++) {
					float* data_input = reinterpret_cast<float*>(buffer_input + byteOffset_input + k * byteStride_input);
					float* data_output = reinterpret_cast<float*>(buffer_output + byteOffset_output + k * byteStride_output);
					SAnimationRotKey key;
					key.time = time_begin + data_input[0];
					key.param.SetValue(context->localInvMap[target_joint] * CQuaternion(data_output));
					pJoint->keyRot.push_back(key);
					time_end = std::max(time_end, key.time);
				}
			}
		}
		context->meshData->AddAnimation(name, time_begin, time_end);
		time_begin = time_end + 1.0f;
	}
}

/**
* 添加网格三角形
*/
void CMeshLoaderGltf::AddTriangles(SContext* context, CMesh* mesh, int accessorIndex) {
	CJsonObject& accessor = context->jsonObject["accessors"][accessorIndex];
	CJsonObject& bufferView = context->jsonObject["bufferViews"][accessor["bufferView"].ToInt()];
	unsigned char* buffer = context->buffers[bufferView["buffer"].ToInt()].data;
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
void CMeshLoaderGltf::AddVertices(SContext* context, CMesh* mesh, int position, int joints, int weights, map<size_t, int>& skinMapper) {
	CJsonObject& accessor_pos = context->jsonObject["accessors"][position];
	CJsonObject& bufferView_pos = context->jsonObject["bufferViews"][accessor_pos["bufferView"].ToInt()];
	unsigned char* buffer_pos = context->buffers[bufferView_pos["buffer"].ToInt()].data;
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
		CJsonObject& accessor_joint = context->jsonObject["accessors"][joints];
		CJsonObject& accessor_weight = context->jsonObject["accessors"][weights];
		CJsonObject& bufferView_joint = context->jsonObject["bufferViews"][accessor_joint["bufferView"].ToInt()];
		CJsonObject& bufferView_weight = context->jsonObject["bufferViews"][accessor_weight["bufferView"].ToInt()];
		unsigned char* buffer_joint = context->buffers[bufferView_joint["buffer"].ToInt()].data;
		unsigned char* buffer_weight = context->buffers[bufferView_weight["buffer"].ToInt()].data;
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
					joint.AddBind(context->jointMap[skinMapper[(size_t)data_joint[0]]]->index, data_weight[0]);
					joint.AddBind(context->jointMap[skinMapper[(size_t)data_joint[1]]]->index, data_weight[1]);
					joint.AddBind(context->jointMap[skinMapper[(size_t)data_joint[2]]]->index, data_weight[2]);
					joint.AddBind(context->jointMap[skinMapper[(size_t)data_joint[3]]]->index, data_weight[3]);
					mesh->AddVertex(CVertex(data_pos[0], data_pos[1], data_pos[2]), joint.Normalize());
				}
			} else if (componentType_joint == TYPE_UNSIGNED_SHORT) {
				int byteStride_joint = bufferView_joint.IsContain("byteStride") ? bufferView_joint["byteStride"].ToInt() : 4 * sizeof(unsigned short);
				for (int i = 0; i < count; i++) {
					float* data_pos = reinterpret_cast<float*>(buffer_pos + byteOffset_pos + i * byteStride_pos);
					float* data_weight = reinterpret_cast<float*>(buffer_weight + byteOffset_weight + i * byteStride_weight);
					unsigned short* data_joint = reinterpret_cast<unsigned short*>(buffer_joint + byteOffset_joint + i * byteStride_joint);
					CVertexJoint joint;
					joint.AddBind(context->jointMap[skinMapper[(size_t)data_joint[0]]]->index, data_weight[0]);
					joint.AddBind(context->jointMap[skinMapper[(size_t)data_joint[1]]]->index, data_weight[1]);
					joint.AddBind(context->jointMap[skinMapper[(size_t)data_joint[2]]]->index, data_weight[2]);
					joint.AddBind(context->jointMap[skinMapper[(size_t)data_joint[3]]]->index, data_weight[3]);
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
void CMeshLoaderGltf::SetupVerticesNormal(SContext* context, CMesh* mesh, int accessorIndex) {
	CJsonObject& accessor = context->jsonObject["accessors"][accessorIndex];
	CJsonObject& bufferView = context->jsonObject["bufferViews"][accessor["bufferView"].ToInt()];
	unsigned char* buffer = context->buffers[bufferView["buffer"].ToInt()].data;
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
void CMeshLoaderGltf::SetupVerticesTexCoord(SContext* context, CMesh* mesh, int accessorIndex) {
	CJsonObject& accessor = context->jsonObject["accessors"][accessorIndex];
	CJsonObject& bufferView = context->jsonObject["bufferViews"][accessor["bufferView"].ToInt()];
	unsigned char* buffer = context->buffers[bufferView["buffer"].ToInt()].data;
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
void CMeshLoaderGltf::SetupVerticesColor(SContext* context, CMesh* mesh, int accessorIndex) {
	CJsonObject& accessor = context->jsonObject["accessors"][accessorIndex];
	CJsonObject& bufferView = context->jsonObject["bufferViews"][accessor["bufferView"].ToInt()];
	unsigned char* buffer = context->buffers[bufferView["buffer"].ToInt()].data;
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