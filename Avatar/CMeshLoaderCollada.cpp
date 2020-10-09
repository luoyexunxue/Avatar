//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CMeshLoaderCollada.h"
#include "CEngine.h"
#include "CStringUtil.h"
#include "CLog.h"
#include <cstring>
#include <sstream>
using std::istringstream;

/**
* 加载 DAE 模型
*/
CMeshData* CMeshLoaderCollada::LoadFile(const string& filename, uint8_t* data, size_t size) {
	char* xml = new char[size + 1];
	memcpy(xml, data, size);
	xml[size] = '\0';
	xml_document<> doc;
	try { doc.parse<0>(xml); } catch(...) { delete[] xml; return 0; }
	xml_node<>* root = doc.first_node("COLLADA");
	if (!root) { delete[] xml; return 0; }
	const char* version = root->first_attribute("version")->value();
	if (strncmp(version, "1.4", 3) && strncmp(version, "1.5", 3)) { delete[] xml; return 0; }
	xml_node<>* up_axis = root->first_node("asset")->first_node("up_axis");
	const char* axis = up_axis? up_axis->value(): "Y_UP";
	// 读取数据节点
	xml_node<>* library_images = root->first_node("library_images");
	xml_node<>* library_materials = root->first_node("library_materials");
	xml_node<>* library_effects = root->first_node("library_effects");
	xml_node<>* library_geometries = root->first_node("library_geometries");
	xml_node<>* library_controllers = root->first_node("library_controllers");
	xml_node<>* library_animations = root->first_node("library_animations");
	xml_node<>* library_visual_scenes = root->first_node("library_visual_scenes");
	m_strBaseDir = CFileManager::GetDirectory(filename);
	m_mapImage.clear();
	m_mapMaterial.clear();
	m_mapMaterialMapper.clear();
	m_mapMeshTransform.clear();
	m_mapJoints.clear();
	m_mapJointsMapper.clear();
	m_mapJointsName.clear();
	m_mapVertexJoint.clear();
	// 向上方向修正（存在动画的情况下不能使用，避免造成动画错误）
	CMatrix4 model_transform;
	if (!library_animations) {
		if (!strcmp(axis, "Y_UP")) model_transform.RotateX(1.570796f);
		if (!strcmp(axis, "X_UP")) model_transform.RotateY(-1.570796f);
	}
	// 读取节点数据
	CMeshData* pMeshData = new CMeshData();
	if (library_images) ReadImages(library_images);
	if (library_materials) ReadMaterials(library_materials);
	// 由于部分 Collada 文件骨骼节点未标明 type="JOINT"
	// 因此使用动画数据中的骨骼名称来确定哪些是骨骼节点
	if (library_animations) ReadAnimationJoint(library_animations);
	if (library_visual_scenes) ReadVisualScenes(library_visual_scenes, model_transform, pMeshData);
	if (library_controllers) ReadControllers(library_controllers, pMeshData);
	if (library_geometries) ReadGeometries(library_geometries, pMeshData);
	if (library_effects) ReadEffects(library_effects, pMeshData);
	if (library_animations) ReadAnimations(library_animations, pMeshData);
	// 创建网格对象
	for (size_t i = 0; i < pMeshData->GetMeshCount(); i++) {
		pMeshData->GetMesh(i)->Create(library_animations != 0);
	}
	delete[] xml;
	return pMeshData;
}

/**
* 读取图片节点
*/
void CMeshLoaderCollada::ReadImages(xml_node<>* images) {
	xml_node<>* image = images->first_node();
	while (image) {
		string id = image->first_attribute("id")->value();
		string init_from = image->first_node("init_from")->value();
		if (init_from.find("file://") == 0) init_from = init_from.substr(7);
		if (CFileManager::IsFullPath(init_from)) m_mapImage[id] = init_from;
		else m_mapImage[id] = m_strBaseDir + init_from;
		image = image->next_sibling();
	}
}

/**
* 读取材质节点
*/
void CMeshLoaderCollada::ReadMaterials(xml_node<>* materials) {
	xml_node<>* material = materials->first_node();
	while (material) {
		string id = material->first_attribute("id")->value();
		string url = material->first_node()->first_attribute("url")->value();
		m_mapMaterialMapper["#" + id] = url;
		material = material->next_sibling();
	}
}

/**
* 读取场景节点
*/
void CMeshLoaderCollada::ReadVisualScenes(xml_node<>* visualScenes, const CMatrix4& matrix, CMeshData* meshData) {
	// 读取场景数据
	xml_node<>* visual_scene = visualScenes->first_node();
	while (visual_scene) {
		ReadNode(visual_scene, matrix, 0);
		visual_scene = visual_scene->next_sibling();
	}
	// 添加关节信息
	map<string, SJoint*> jointMap;
	for (map<string, SJoint*>::iterator i = m_mapJoints.begin(); i != m_mapJoints.end(); ++i) {
		jointMap.insert(std::pair<string, SJoint*>(i->second->name, i->second));
	}
	// 按父关节优先的顺序添加
	vector<SJoint*> jointList;
	map<string, SJoint*>::iterator iter = jointMap.begin();
	while (iter != jointMap.end()) {
		SJoint* joint = iter->second;
		while (joint && jointMap.count(joint->name) > 0) {
			jointMap.erase(joint->name);
			jointList.push_back(joint);
			joint = joint->parent;
		}
		for (size_t i = jointList.size(); i > 0; i--) {
			meshData->AddJoint(jointList[i - 1]);
		}
		jointList.clear();
		iter = jointMap.begin();
	}
}

/**
* 读取控制节点
*/
void CMeshLoaderCollada::ReadControllers(xml_node<>* controllers, CMeshData* meshData) {
	xml_node<>* controller = controllers->first_node();
	while (controller) {
		string id = controller->first_attribute("id")->value();
		xml_node<>* skin = controller->first_node();
		if (skin) ReadSkin(skin, id, meshData);
		controller = controller->next_sibling();
	}
}

/**
* 读取几何节点
*/
void CMeshLoaderCollada::ReadGeometries(xml_node<>* geometries, CMeshData* meshData) {
	xml_node<>* geometry = geometries->first_node();
	while (geometry) {
		string id = geometry->first_attribute("id")->value();
		xml_node<>* mesh = geometry->first_node();
		if (mesh) ReadMesh(mesh, id, meshData);
		geometry = geometry->next_sibling();
	}
}

/**
* 读取效果节点
*/
void CMeshLoaderCollada::ReadEffects(xml_node<>* effects, CMeshData* meshData) {
	xml_node<>* effect = effects->first_node();
	while (effect) {
		string id = effect->first_attribute("id")->value();
		xml_node<>* profile_COMMON = effect->first_node("profile_COMMON");
		if (profile_COMMON) ReadProfile(profile_COMMON, id, meshData);
		effect = effect->next_sibling();
	}
}

/**
* 读取动画节点
*/
void CMeshLoaderCollada::ReadAnimations(xml_node<>* animations, CMeshData* meshData) {
	xml_node<>* animation = animations->first_node();
	while (animation) {
		xml_node<>* subset = animation->first_node("animation");
		if (subset) {
			while (subset) {
				ReadAnimationData(subset);
				subset = subset->next_sibling();
			}
		} else ReadAnimationData(animation);
		animation = animation->next_sibling();
	}
	float animationEndTime = 0.0f;
	for (map<string, SJoint*>::iterator i = m_mapJoints.begin(); i != m_mapJoints.end(); ++i) {
		if (!i->second->keyPos.empty()) {
			float endTime = i->second->keyPos.back().time;
			if (endTime > animationEndTime) animationEndTime = endTime;
		}
	}
	meshData->AddAnimation("default", 0.0f, animationEndTime);
}

/**
* 解析节点数据
*/
void CMeshLoaderCollada::ReadNode(xml_node<>* node, const CMatrix4& matrix, SJoint* joint) {
	CMatrix4 transform;
	xml_node<>* current = node->first_node();
	while (current) {
		const char* name = current->name();
		float x, y, z, a;
		if (!strcmp(name, "translate")) {
			istringstream reader(current->value());
			reader >> x >> y >> z;
			transform *= CMatrix4().Translate(x, y, z);
		} else if (!strcmp(name, "rotate")) {
			istringstream reader(current->value());
			reader >> x >> y >> z >> a;
			transform *= CQuaternion().FromAxisAngle(CVector3(x, y, z), a).ToMatrix();
		} else if (!strcmp(name, "scale")) {
			istringstream reader(current->value());
			reader >> x >> y >> z;
			transform *= CMatrix4().Scale(x, y, z);
		} else if (!strcmp(name, "matrix")) {
			istringstream reader(current->value());
			reader >> transform[0] >> transform[4] >> transform[8] >> transform[12];
			reader >> transform[1] >> transform[5] >> transform[9] >> transform[13];
			reader >> transform[2] >> transform[6] >> transform[10] >> transform[14];
			reader >> transform[3] >> transform[7] >> transform[11] >> transform[15];
		} else if (!strcmp(name, "instance_geometry")) {
			const char* geometry = current->first_attribute("url")->value();
			m_mapMeshTransform.insert(std::pair<string, CMatrix4>(geometry, matrix * transform));
			xml_node<>* bind_material = current->first_node("bind_material");
			if (bind_material) MapMaterial(bind_material);
		} else if (!strcmp(name, "instance_controller")) {
			xml_node<>* bind_material = current->first_node("bind_material");
			if (bind_material) MapMaterial(bind_material);
		} else if (!strcmp(name, "node")) {
			xml_attribute<>* attrib_id = current->first_attribute("id");
			xml_attribute<>* attrib_type = current->first_attribute("type");
			bool isJoint1 = attrib_id && m_mapJointsName.count(attrib_id->value()) != 0;
			bool isJoint2 = attrib_type && strcmp(attrib_type->value(), "JOINT") == 0;
			if (isJoint1 || isJoint2) {
				const char* id = attrib_id->value();
				const char* sid = id;
				xml_attribute<>* node_sid = current->first_attribute("sid");
				if (node_sid) sid = node_sid->value();
				SJoint* pJoint = new SJoint();
				pJoint->name = current->first_attribute("name")->value();
				pJoint->parent = joint;
				m_mapJoints.insert(std::pair<string, SJoint*>(sid, pJoint));
				m_mapJointsMapper.insert(std::pair<string, string>(id, sid));
				ReadNode(current, matrix * transform, pJoint);
			} else ReadNode(current, matrix * transform, joint);
		}
		current = current->next_sibling();
	}
	if (joint) {
		joint->localMatrix = transform;
		joint->worldMatrix = matrix * transform;
	}
}

/**
* 解析蒙皮数据
*/
void CMeshLoaderCollada::ReadSkin(xml_node<>* skin, const string& id, CMeshData* meshData) {
	const string mesh_id = skin->first_attribute("source")->value();
	xml_node<>* vertex_weights = skin->first_node("vertex_weights");
	m_mapVertexJoint[mesh_id].resize(atoi(vertex_weights->first_attribute("count")->value()));
	// 获取绑定矩阵
	xml_node<>* bind_shape_matrix = skin->first_node("bind_shape_matrix");
	if (bind_shape_matrix) {
		float mat[16];
		istringstream stream_bind_shape_mat(bind_shape_matrix->value());
		for (int i = 0; i < 16; i++) stream_bind_shape_mat >> mat[i];
		m_mapMeshTransform[mesh_id] = CMatrix4(mat).Transpose() * m_mapMeshTransform[mesh_id];
	}
	// 获取输入源
	map<string, xml_node<>*> sourceList;
	xml_node<>* source = skin->first_node("source");
	while (source) {
		string source_id = source->first_attribute("id")->value();
		xml_node<>* Name_array = source->first_node("Name_array");
		xml_node<>* float_array = source->first_node("float_array");
		source = source->next_sibling("source");
		if (Name_array) sourceList["#" + source_id] = Name_array;
		else sourceList["#" + source_id] = float_array;
	}
	xml_node<>* source_joint = 0;
	xml_node<>* source_weight = 0;
	xml_node<>* source_bind = 0;
	int source_stride = 1;
	int source_offset[2];
	xml_node<>* weights_input = vertex_weights->first_node("input");
	while (weights_input) {
		const char* input_semantic = weights_input->first_attribute("semantic")->value();
		const char* input_source = weights_input->first_attribute("source")->value();
		int input_offset = atoi(weights_input->first_attribute("offset")->value());
		weights_input = weights_input->next_sibling("input");
		if (source_stride < input_offset + 1) source_stride = input_offset + 1;
		if (!strcmp("JOINT", input_semantic)) {
			source_joint = sourceList[input_source];
			source_offset[0] = input_offset;
		} else if (!strcmp("WEIGHT", input_semantic)) {
			source_weight = sourceList[input_source];
			source_offset[1] = input_offset;
		}
	}
	// 获取绑定逆矩阵
	xml_node<>* joints_input = skin->first_node("joints")->first_node("input");
	while (joints_input) {
		const char* input_semantic = joints_input->first_attribute("semantic")->value();
		const char* input_source = joints_input->first_attribute("source")->value();
		joints_input = joints_input->next_sibling("input");
		if (!strcmp("INV_BIND_MATRIX", input_semantic)) {
			source_bind = sourceList[input_source];
			break;
		}
	}
	vector<string> vecJoint;
	vector<float> vecWeight;
	vector<float> vecBind;
	// 得到关节名称数组，关节绑定矩阵数组，顶点权重数组
	vecJoint.resize(atoi(source_joint->first_attribute("count")->value()));
	vecWeight.resize(atoi(source_weight->first_attribute("count")->value()));
	vecBind.resize(atoi(source_bind->first_attribute("count")->value()));
	istringstream stream_joint(source_joint->value());
	istringstream stream_weight(source_weight->value());
	istringstream stream_bind(source_bind->value());
	istringstream stream_vcount(vertex_weights->first_node("vcount")->value());
	istringstream stream_v(vertex_weights->first_node("v")->value());
	for (size_t i = 0; i < vecJoint.size(); i++) stream_joint >> vecJoint[i];
	for (size_t i = 0; i < vecWeight.size(); i++) stream_weight >> vecWeight[i];
	for (size_t i = 0; i < vecBind.size(); i++) stream_bind >> vecBind[i];
	vector<CVertexJoint>& vertexJoint = m_mapVertexJoint[mesh_id];
	vector<unsigned int> weight(source_stride);
	for (size_t i = 0; i < vertexJoint.size(); i++) {
		int affected = 0;
		stream_vcount >> affected;
		for (int j = 0; j < affected; j++) {
			for (int k = 0; k < source_stride; k++) stream_v >> weight[k];
			vertexJoint[i].AddBind(weight[source_offset[0]], vecWeight[weight[source_offset[1]]]);
		}
		vertexJoint[i].Normalize();
	}
	// 设置关节绑定逆矩阵
	for (size_t i = 0; i < vecJoint.size(); i++) {
		map<string, SJoint*>::iterator iter = m_mapJoints.find(vecJoint[i]);
		if (iter != m_mapJoints.end()) {
			iter->second->bindMatrixInv.SetValue(&vecBind[i * 16]);
			iter->second->bindMatrixInv.Transpose();
		} else {
			CLog::Warn("Cannot find joint '%s'", vecJoint[i].c_str());
			CLog::Info("Make sure that the bone name should not contain any space char");
		}
	}
	MapVertexJoint(mesh_id, vecJoint, meshData);
}

/**
* 解析网格数据
*/
void CMeshLoaderCollada::ReadMesh(xml_node<>* mesh, const string& id, CMeshData* meshData) {
	xml_node<>* triangles = mesh->first_node("triangles");
	xml_node<>* polylist = mesh->first_node("polylist");
	xml_node<>* polygons = mesh->first_node("polygons");
	xml_node<>* element = triangles;
	if (!element) element = polylist;
	if (!element) element = polygons;
	if (!element) return;
	// 获取输入源
	vector<CVertexJoint>& vertexJoint = m_mapVertexJoint["#" + id];
	map<string, vector<float>> sourceList;
	map<string, int> sourceStride;
	xml_node<>* source = mesh->first_node("source");
	while (source) {
		string source_id = source->first_attribute("id")->value();
		xml_node<>* float_array = source->first_node("float_array");
		xml_node<>* technique_common = source->first_node("technique_common");
		source = source->next_sibling("source");
		if (float_array) {
			source_id = "#" + CStringUtil::Replace(source_id, " ", "%20");
			vector<float>& vecData = sourceList[source_id];
			vecData.resize(atoi(float_array->first_attribute("count")->value()));
			istringstream stream(float_array->value());
			for (size_t i = 0; i < vecData.size(); i++) stream >> vecData[i];
			if (technique_common) {
				sourceStride[source_id] = atoi(technique_common->first_node("accessor")->first_attribute("stride")->value());
			}
		}
	}
	// 解析数据
	while (element) {
		xml_node<>* element_p = element->first_node("p");
		xml_attribute<>* element_count = element->first_attribute("count");
		xml_attribute<>* element_material = element->first_attribute("material");
		int face_count = atoi(element_count->value());
		string material_name = element_material? m_mapMaterial[element_material->value()]: "";
		// 四种输入源 0-POSITION, 1-NORMAL, 2-TEXCOORD, 3-COLOR
		string input_source[4];
		int input_offset[4];
		int input_stride[4];
		int stride = GetElementInput(element, input_source, input_offset);
		input_stride[0] = sourceStride.count(input_source[0]) ? sourceStride[input_source[0]] : 3;
		input_stride[1] = sourceStride.count(input_source[1]) ? sourceStride[input_source[1]] : 3;
		input_stride[2] = sourceStride.count(input_source[2]) ? sourceStride[input_source[2]] : 2;
		input_stride[3] = sourceStride.count(input_source[3]) ? sourceStride[input_source[3]] : 4;
		const vector<float>& vecPos = sourceList[input_source[0]];
		const vector<float>& vecNor = sourceList[input_source[1]];
		const vector<float>& vecTex = sourceList[input_source[2]];
		const vector<float>& vecClr = sourceList[input_source[3]];
		vector<unsigned int> vecIdx;
		istringstream stream_v(polylist? polylist->first_node("vcount")->value(): "");
		istringstream stream_p(element_p->value());
		while (!stream_p.eof()) {
			int temp = -1;
			stream_p >> temp;
			if (temp >= 0) vecIdx.push_back((unsigned int)temp);
		}
		// 创建网格对象
		CMesh* pMesh = new CMesh();
		pMesh->GetMaterial()->GetName() = material_name;
		pMesh->GetMaterial()->SetTexture("");
		size_t currentPos = 0;
		vector<unsigned int> vertex(3);
		for (int i = 0; i < face_count; i++) {
			size_t face_vert_count = 3;
			if (polylist) stream_v >> face_vert_count;
			if (polygons) face_vert_count = vecIdx.size() / stride;
			vertex.resize(face_vert_count);
			// 添加顶点和三角形
			for (size_t j = 0; j < face_vert_count; j++) {
				CVertex vert;
				vertex[j] = (unsigned int)pMesh->GetVertexCount();
				size_t baseIndex = j * stride + currentPos;
				if (input_offset[0] >= 0) vert.SetPosition(&vecPos[vecIdx[baseIndex + input_offset[0]] * input_stride[0]]);
				if (input_offset[1] >= 0) vert.SetNormal(&vecNor[vecIdx[baseIndex + input_offset[1]] * input_stride[1]]);
				if (input_offset[2] >= 0) vert.SetTexCoord(&vecTex[vecIdx[baseIndex + input_offset[2]] * input_stride[2]]);
				if (input_offset[3] >= 0) vert.SetColor(&vecClr[vecIdx[baseIndex + input_offset[3]] * input_stride[3]]);
				if (vertexJoint.empty()) pMesh->AddVertex(vert);
				else pMesh->AddVertex(vert, vertexJoint[vecIdx[baseIndex + input_offset[0]]]);
			}
			for (size_t j = 2; j < face_vert_count; j++) {
				pMesh->AddTriangle(vertex[0], vertex[j - 1], vertex[j]);
			}
			currentPos += face_vert_count * stride;
			if (polygons) {
				element_p = element_p->next_sibling();
				if (element_p) {
					vecIdx.clear();
					currentPos = 0;
					stream_p.clear();
					stream_p.str(element_p->value());
					while (!stream_p.eof()) {
						int temp = -1;
						stream_p >> temp;
						if (temp >= 0) vecIdx.push_back((unsigned int)temp);
					}
				}
			}
		}
		pMesh->Transform(m_mapMeshTransform["#" + id]);
		pMesh->Reverse(false, false, true);
		meshData->AddMesh(pMesh);
		// 下一个网格
		if (triangles) element = element->next_sibling("triangles");
		else if (polylist) element = element->next_sibling("polylist");
		else if (polygons) element = element->next_sibling("polygons");
		else element = 0;
	}
}

/**
* 解析材质数据
*/
void CMeshLoaderCollada::ReadProfile(xml_node<>* profile, const string& id, CMeshData* meshData) {
	string url = "#" + id;
	for (size_t i = 0; i < meshData->GetMeshCount(); i++) {
		CMesh* mesh = meshData->GetMesh(i);
		CMaterial* material = mesh->GetMaterial();
		if (material->GetName() == url) {
			xml_node<>* diffuse = profile->first_node("technique")->first_node()->first_node("diffuse");
			if (diffuse) {
				if (diffuse->first_node("texture")) {
					string texture = diffuse->first_node("texture")->first_attribute("texture")->value();
					map<string, string> mapper;
					xml_node<>* newparam = profile->first_node("newparam");
					while (newparam) {
						string sid = newparam->first_attribute("sid")->value();
						xml_node<>* sampler2D = newparam->first_node("sampler2D");
						xml_node<>* surface = newparam->first_node("surface");
						if (sampler2D) mapper[sid] = sampler2D->first_node("source")->value();
						if (surface) mapper[sid] = surface->first_node("init_from")->value();
						newparam = newparam->next_sibling("newparam");
					}
					// 部分文件 image 可能包含在 library_effects/effect/profile_COMMON 内
					xml_node<>* image = profile->first_node("image");
					while (image) {
						string id = image->first_attribute("id")->value();
						string init_from = image->first_node("init_from")->value();
						if (init_from.find("file://") == 0) init_from = init_from.substr(7);
						if (CFileManager::IsFullPath(init_from)) m_mapImage[id] = init_from;
						else m_mapImage[id] = m_strBaseDir + init_from;
						image = image->next_sibling("image");
					}
					map<string, string>::iterator iter = mapper.find(texture);
					while (iter != mapper.end()) {
						texture = iter->second;
						iter = mapper.find(texture);
					}
					if (m_mapImage.count(texture) > 0) {
						material->SetTexture(m_mapImage[texture]);
					}
				}
				if (diffuse->first_node("color")) {
					istringstream reader;
					reader.str(diffuse->first_node("color")->value());
					reader >> material->m_fColor[0] >> material->m_fColor[1];
					reader >> material->m_fColor[2] >> material->m_fColor[3];
				}
			}
		}
	}
}

/**
* 读取动画数据
*/
void CMeshLoaderCollada::ReadAnimationData(xml_node<>* animation) {
	// 获取输入源
	map<string, xml_node<>*> samplerList;
	map<string, vector<float>> sourceList;
	xml_node<>* source = animation->first_node("source");
	while (source) {
		string source_id = source->first_attribute("id")->value();
		xml_node<>* float_array = source->first_node("float_array");
		source = source->next_sibling("source");
		if (float_array) {
			vector<float>& vecData = sourceList["#" + source_id];
			vecData.resize(atoi(float_array->first_attribute("count")->value()));
			istringstream stream(float_array->value());
			for (size_t i = 0; i < vecData.size(); i++) stream >> vecData[i];
		}
	}
	xml_node<>* sampler = animation->first_node("sampler");
	while (sampler) {
		string sampler_id = sampler->first_attribute("id")->value();
		samplerList["#" + sampler_id] = sampler;
		sampler = sampler->next_sibling("sampler");
	}
	// 动画通道
	xml_node<>* channel = animation->first_node("channel");
	while (channel) {
		SJoint* pJoint = 0;
		string channel_source = channel->first_attribute("source")->value();
		string channel_target = channel->first_attribute("target")->value();
		channel = channel->next_sibling("channel");
		string joint = m_mapJointsMapper[channel_target.substr(0, channel_target.rfind('/'))];
		string trans = channel_target.substr(channel_target.find('/') + 1);
		map<string, SJoint*>::iterator iter = m_mapJoints.find(joint);
		if (iter != m_mapJoints.end()) pJoint = iter->second;
		if (!pJoint) continue;
		// 不支持非矩阵的变换方式
		if (trans != "matrix" && trans != "transform") continue;
		map<string, xml_node<>*>::iterator iter_sampler = samplerList.find(channel_source);
		if (iter_sampler == samplerList.end()) continue;
		const char* source_input = "";
		const char* source_output = "";
		xml_node<>* input = iter_sampler->second->first_node("input");
		while (input) {
			const char* input_semantic = input->first_attribute("semantic")->value();
			const char* input_source = input->first_attribute("source")->value();
			if (!strcmp("INPUT", input_semantic)) source_input = input_source;
			else if (!strcmp("OUTPUT", input_semantic)) source_output = input_source;
			input = input->next_sibling();
		}
		const vector<float>& vecInput = sourceList[source_input];
		const vector<float>& vecOutput = sourceList[source_output];
		// Collada 动画数据为局部变换矩阵，需要转换为相对于关节局部变换矩阵的旋转和平移量
		CMatrix4 localMatrixInv(pJoint->localMatrix);
		localMatrixInv.Invert();
		for (size_t i = 0; i < vecInput.size(); i++) {
			CMatrix4 transform = localMatrixInv * CMatrix4(&vecOutput[i * 16]).Transpose();
			SAnimationRotKey keyRot;
			SAnimationPosKey keyPos;
			keyRot.time = vecInput[i];
			keyPos.time = vecInput[i];
			keyRot.param.FromMatrix(transform);
			keyPos.param.SetValue(transform[12], transform[13], transform[14]);
			pJoint->keyRot.push_back(keyRot);
			pJoint->keyPos.push_back(keyPos);
		}
	}
}

/**
* 读取动画关节
*/
void CMeshLoaderCollada::ReadAnimationJoint(xml_node<>* animations) {
	xml_node<>* animation = animations->first_node();
	while (animation) {
		xml_node<>* subset = animation->first_node("animation");
		if (subset) {
			while (subset) {
				xml_node<>* channel = subset->first_node("channel");
				while (channel) {
					string target = channel->first_attribute("target")->value();
					string joint = target.substr(0, target.rfind('/'));
					channel = channel->next_sibling("channel");
					if (m_mapJointsName.count(joint) == 0) {
						m_mapJointsName.insert(std::pair<string, int>(joint, 0));
					}
				}
				subset = subset->next_sibling();
			}
		} else {
			xml_node<>* channel = animation->first_node("channel");
			while (channel) {
				string target = channel->first_attribute("target")->value();
				string joint = target.substr(0, target.rfind('/'));
				channel = channel->next_sibling("channel");
				if (m_mapJointsName.count(joint) == 0) {
					m_mapJointsName.insert(std::pair<string, int>(joint, 0));
				}
			}
		}
		animation = animation->next_sibling();
	}
}

/**
* 映射材质名称
*/
void CMeshLoaderCollada::MapMaterial(xml_node<>* material) {
	xml_node<>* instance_material = material->first_node()->first_node("instance_material");
	while (instance_material) {
		string symbol = instance_material->first_attribute("symbol")->value();
		string target = instance_material->first_attribute("target")->value();
		instance_material = instance_material->next_sibling("instance_material");
		m_mapMaterial[symbol] = m_mapMaterialMapper[target];
	}
}

/**
* 映射顶点骨骼
*/
void CMeshLoaderCollada::MapVertexJoint(const string& id, const vector<string>& joint, CMeshData* meshData) {
	map<string, size_t> jointIndexMapper;
	map<string, string> jointNameMapper;
	// 关节名称->关节索引
	for (size_t i = 0; i < meshData->GetJointCount(); i++) {
		jointIndexMapper.insert(std::pair<string, size_t>(meshData->GetJoint(i)->name, i));
	}
	// 关节SID->关节名称
	for (map<string, SJoint*>::iterator i = m_mapJoints.begin(); i != m_mapJoints.end(); ++i) {
		jointNameMapper.insert(std::pair<string, string>(i->first, i->second->name));
	}
	// 执行映射，修改顶点对应关节
	vector<int> jointMap(joint.size());
	vector<CVertexJoint>& vertexJoint = m_mapVertexJoint[id];
	for (size_t i = 0; i < joint.size(); i++) {
		jointMap[i] = static_cast<int>(jointIndexMapper[jointNameMapper[joint[i]]]);
	}
	for (size_t i = 0; i < vertexJoint.size(); i++) {
		CVertexJoint* vertJoint = &vertexJoint[i];
		for (int n = 0; n < vertJoint->m_iCount; n++) {
			vertJoint->m_iJointId[n] = jointMap[vertJoint->m_iJointId[n]];
		}
	}
}

/**
* 获取几何数据输入
* 0-POSITION, 1-NORMAL, 2-TEXCOORD, 3-COLOR
*/
int CMeshLoaderCollada::GetElementInput(xml_node<>* element, string source[4], int offset[4]) {
	int stride = 1;
	offset[0] = -1;
	offset[1] = -1;
	offset[2] = -1;
	offset[3] = -1;
	xml_node<>* input = element->first_node("input");
	while (input) {
		int input_offset = atoi(input->first_attribute("offset")->value());
		const char* input_semantic = input->first_attribute("semantic")->value();
		const char* input_source = input->first_attribute("source")->value();
		xml_attribute<>* input_set = input->first_attribute("set");
		input = input->next_sibling("input");
		if (stride < input_offset + 1) stride = input_offset + 1;
		if (!strcmp(input_semantic, "VERTEX")) {
			xml_node<>* vinput = element->previous_sibling("vertices")->first_node("input");
			while (vinput) {
				const char* vsemantic = vinput->first_attribute("semantic")->value();
				const char* vsource = vinput->first_attribute("source")->value();
				vinput = vinput->next_sibling("input");
				if (!strcmp(vsemantic, "POSITION")) {
					source[0] = CStringUtil::Replace(vsource, " ", "%20");
					offset[0] = input_offset;
				} else if (!strcmp(vsemantic, "NORMAL")) {
					source[1] = CStringUtil::Replace(vsource, " ", "%20");
					offset[1] = input_offset;
				} else if (!strcmp(vsemantic, "TEXCOORD")) {
					if (!input_set || offset[2] < 0 || !strcmp(input_set->value(), "0")) {
						source[2] = CStringUtil::Replace(vsource, " ", "%20");
						offset[2] = input_offset;
					}
				} else if (!strcmp(vsemantic, "COLOR")) {
					if (!input_set || offset[3] < 0 || !strcmp(input_set->value(), "0")) {
						source[3] = CStringUtil::Replace(vsource, " ", "%20");
						offset[3] = input_offset;
					}
				}
			}
		} else if (!strcmp(input_semantic, "POSITION")) {
			source[0] = CStringUtil::Replace(input_source, " ", "%20");
			offset[0] = input_offset;
		} else if (!strcmp(input_semantic, "NORMAL")) {
			source[1] = CStringUtil::Replace(input_source, " ", "%20");
			offset[1] = input_offset;
		} else if (!strcmp(input_semantic, "TEXCOORD")) {
			if (!input_set || offset[2] < 0 || !strcmp(input_set->value(), "0")) {
				source[2] = CStringUtil::Replace(input_source, " ", "%20");
				offset[2] = input_offset;
			}
		} else if (!strcmp(input_semantic, "COLOR")) {
			if (!input_set || offset[3] < 0 || !strcmp(input_set->value(), "0")) {
				source[3] = CStringUtil::Replace(input_source, " ", "%20");
				offset[3] = input_offset;
			}
		}
	}
	return stride;
}