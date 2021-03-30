//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CMaterial.h"
#include "CEngine.h"
#include <cstring>

/**
* 构造函数
*/
CMaterial::CMaterial() {
	m_fColor[0] = 1.0f;
	m_fColor[1] = 1.0f;
	m_fColor[2] = 1.0f;
	m_fColor[3] = 1.0f;
	m_fRoughness = 0.8f;
	m_fMetalness = 0.1f;
	m_iTextureCount = 0;
	m_pTexture[0] = 0;
	m_pTexture[1] = 0;
	m_pTexture[2] = 0;
	m_pTexture[3] = 0;
	m_pTexture[4] = 0;
	m_pTexture[5] = 0;
	m_pTexture[6] = 0;
	m_pTexture[7] = 0;
	m_pShader = 0;
	// 渲染模式
	m_bCullFace = true;
	m_bUseDepth = true;
	m_bAddColor = false;
}

/**
* 析构函数
*/
CMaterial::~CMaterial() {
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	pTextureMgr->Drop(m_pTexture[0]);
	pTextureMgr->Drop(m_pTexture[1]);
	pTextureMgr->Drop(m_pTexture[2]);
	pTextureMgr->Drop(m_pTexture[3]);
	pTextureMgr->Drop(m_pTexture[4]);
	pTextureMgr->Drop(m_pTexture[5]);
	pTextureMgr->Drop(m_pTexture[6]);
	pTextureMgr->Drop(m_pTexture[7]);
	CEngine::GetShaderManager()->Drop(m_pShader);
	map<int, SBufferedTexture>::iterator iter = m_mapBufferedTextures.begin();
	while (iter != m_mapBufferedTextures.end()) {
		if (iter->second.data) delete[] iter->second.data;
		++iter;
	}
}

/**
* 复制材质
*/
void CMaterial::CopyFrom(const CMaterial* material) {
	m_fColor[0] = material->m_fColor[0];
	m_fColor[1] = material->m_fColor[1];
	m_fColor[2] = material->m_fColor[2];
	m_fColor[3] = material->m_fColor[3];
	m_fRoughness = material->m_fRoughness;
	m_fMetalness = material->m_fMetalness;
	m_strName = material->m_strName;
	m_iTextureCount = material->m_iTextureCount;
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	for (int i = 0; i < 8; i++) {
		pTextureMgr->Drop(m_pTexture[i]);
		m_pTexture[i] = material->m_pTexture[i] ? material->m_pTexture[i]->AddReference() : 0;
	}
	CEngine::GetShaderManager()->Drop(m_pShader);
	m_pShader = material->m_pShader ? material->m_pShader->AddReference() : 0;
	m_bCullFace = material->m_bCullFace;
	m_bUseDepth = material->m_bUseDepth;
	m_bAddColor = material->m_bAddColor;
	map<int, SBufferedTexture>::iterator iter1 = m_mapBufferedTextures.begin();
	while (iter1 != m_mapBufferedTextures.end()) {
		if (iter1->second.data) delete[] iter1->second.data;
		++iter1;
	}
	m_mapBufferedTextures.clear();
	map<int, SBufferedTexture>::const_iterator iter2 = material->m_mapBufferedTextures.begin();
	while (iter2 != material->m_mapBufferedTextures.end()) {
		m_mapBufferedTextures[iter2->first] = iter2->second;
		if (iter2->second.data) {
			int size = iter2->second.width * iter2->second.height * iter2->second.channel;
			m_mapBufferedTextures[iter2->first].data = new unsigned char[size];
			std::memcpy(m_mapBufferedTextures[iter2->first].data, iter2->second.data, size);
		}
		++iter2;
	}
}

/**
* 使用材质
*/
void CMaterial::UseMaterial() {
	CreateBufferedTexture();
	// 设置纹理单元
	if (m_iTextureCount == 1) m_pTexture[0]->UseTexture();
	else if (m_iTextureCount > 1) {
		for (int i = m_iTextureCount - 1; i >= 0; i--) {
			m_pTexture[i]->UseTexture(i);
		}
	}
	// 设置 Uniform 变量
	if (m_pShader) {
		m_pShader->UseShader();
		m_pShader->SetUniform("uRoughness", m_fRoughness);
		m_pShader->SetUniform("uMetalness", m_fMetalness);
		m_pShader->SetUniform("uBaseColor", m_fColor, 4, 1);
		for (size_t i = 0; i < m_vecUniforms.size(); i++) {
			SUniformValue* item = &m_vecUniforms[i];
			switch (item->type) {
			case CShader::BOOL: m_pShader->SetUniform(item->location, item->bValue); break;
			case CShader::INT: m_pShader->SetUniform(item->location, item->iValue); break;
			case CShader::FLOAT: m_pShader->SetUniform(item->location, item->fValue, 1, 1); break;
			case CShader::VEC2: m_pShader->SetUniform(item->location, item->fValue, 2, 1); break;
			case CShader::VEC3: m_pShader->SetUniform(item->location, item->fValue, 3, 1); break;
			case CShader::VEC4: m_pShader->SetUniform(item->location, item->fValue, 4, 1); break;
			case CShader::MAT3: m_pShader->SetUniform(item->location, item->fValue, 9, 1); break;
			case CShader::MAT4: m_pShader->SetUniform(item->location, item->fValue, 16, 1); break;
			default: break;
			}
		}
		m_vecUniforms.clear();
	} else {
		CShader* shader = CEngine::GetShaderManager()->GetCurrentShader();
		shader->SetUniform("uRoughness", m_fRoughness);
		shader->SetUniform("uMetalness", m_fMetalness);
		shader->SetUniform("uBaseColor", m_fColor, 4, 1);
	}
}

/**
* 设置材质贴图
*/
void CMaterial::SetTexture(CTexture* texture, int index) {
	if (m_pTexture[index]) m_iTextureCount -= 1;
	CEngine::GetTextureManager()->Drop(m_pTexture[index]);
	m_pTexture[index] = texture ? texture->AddReference() : 0;
	m_iTextureCount += texture ? 1 : 0;
}

/**
* 设置材质贴图
*/
void CMaterial::SetTexture(const string& file, int index) {
	if (!m_pTexture[index]) m_iTextureCount += 1;
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	pTextureMgr->Drop(m_pTexture[index]);
	m_pTexture[index] = pTextureMgr->Create(file);
}

/**
* 设置内存材质贴图
*/
void CMaterial::SetTexture(const string& name, int width, int height, int channel, const void* data, bool mipmap, int index) {
	if (!m_pTexture[index]) m_iTextureCount += 1;
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	pTextureMgr->Drop(m_pTexture[index]);
	m_pTexture[index] = pTextureMgr->Create(name, width, height, channel, data, mipmap);
}

/**
* 缓冲加载纹理
* @note 延迟创建纹理对象，若不在渲染线程中需调用此方法设置材质贴图
*/
void CMaterial::SetTextureBuffered(const string& file, int index) {
	map<int, SBufferedTexture>::iterator iter = m_mapBufferedTextures.find(index);
	if (iter != m_mapBufferedTextures.end() && iter->second.data) delete[] iter->second.data;
	m_mapBufferedTextures[index].name = file;
	m_mapBufferedTextures[index].data = 0;
}

/**
*  缓冲加载纹理
* @note 延迟创建纹理对象，若不在渲染线程中需调用此方法设置材质贴图
*/
void CMaterial::SetTextureBuffered(const string& name, int width, int height, int channel, const void* data, bool mipmap, int index) {
	map<int, SBufferedTexture>::iterator iter = m_mapBufferedTextures.find(index);
	if (iter != m_mapBufferedTextures.end() && iter->second.data) delete[] iter->second.data;
	int size = width * height * channel;
	m_mapBufferedTextures[index].name = name;
	m_mapBufferedTextures[index].width = width;
	m_mapBufferedTextures[index].height = height;
	m_mapBufferedTextures[index].channel = channel;
	m_mapBufferedTextures[index].mipmap = mipmap;
	m_mapBufferedTextures[index].data = new unsigned char[size];
	std::memcpy(m_mapBufferedTextures[index].data, data, size);
}

/**
* 设置缓冲纹理模式
* @param s 纹理坐标S，可选值 1-重复 2-限制到边界 3-镜像重复
* @param t 纹理坐标T，可选值 1-重复 2-限制到边界 3-镜像重复
*/
void CMaterial::SetTextureBufferedWrapMode(int s, int t, int index) {
	map<int, SBufferedTexture>::iterator iter = m_mapBufferedTextures.find(index);
	if (iter != m_mapBufferedTextures.end()) {
		iter->second.wraps = s;
		iter->second.wrapt = t;
	}
}

/**
* 设置着色器
*/
void CMaterial::SetShader(const string& name) {
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	pShaderMgr->Drop(m_pShader);
	CShader* shader = pShaderMgr->GetShader(name);
	m_pShader = shader ? shader->AddReference() : 0;
}

/**
* 设置着色器
*/
void CMaterial::SetShader(CShader* shader) {
	CEngine::GetShaderManager()->Drop(m_pShader);
	m_pShader = shader ? shader->AddReference() : 0;
}

/**
* 传递 Uniform 值，bool
*/
void CMaterial::PassUniform(const string& name, bool value) {
	if (m_pShader) {
		SUniformValue uniformValue;
		uniformValue.location = m_pShader->GetUniform(name);
		uniformValue.type = CShader::BOOL;
		uniformValue.bValue = value;
		m_vecUniforms.push_back(uniformValue);
	}
}

/**
* 传递 Uniform 值，int
*/
void CMaterial::PassUniform(const string& name, int value) {
	if (m_pShader) {
		SUniformValue uniformValue;
		uniformValue.location = m_pShader->GetUniform(name);
		uniformValue.type = CShader::INT;
		uniformValue.iValue = value;
		m_vecUniforms.push_back(uniformValue);
	}
}

/**
* 传递 Uniform 值，float
*/
void CMaterial::PassUniform(const string& name, float value) {
	if (m_pShader) {
		SUniformValue uniformValue;
		uniformValue.location = m_pShader->GetUniform(name);
		uniformValue.type = CShader::FLOAT;
		uniformValue.fValue[0] = value;
		m_vecUniforms.push_back(uniformValue);
	}
}

/**
* 传递 Uniform 值，vec2
*/
void CMaterial::PassUniform(const string& name, const CVector2& value) {
	if (m_pShader) {
		SUniformValue uniformValue;
		uniformValue.location = m_pShader->GetUniform(name);
		uniformValue.type = CShader::VEC2;
		uniformValue.fValue[0] = value.m_fValue[0];
		uniformValue.fValue[1] = value.m_fValue[1];
		m_vecUniforms.push_back(uniformValue);
	}
}

/**
* 传递 Uniform 值，vec3
*/
void CMaterial::PassUniform(const string& name, const CVector3& value) {
	if (m_pShader) {
		SUniformValue uniformValue;
		uniformValue.location = m_pShader->GetUniform(name);
		uniformValue.type = CShader::VEC3;
		uniformValue.fValue[0] = value.m_fValue[0];
		uniformValue.fValue[1] = value.m_fValue[1];
		uniformValue.fValue[2] = value.m_fValue[2];
		m_vecUniforms.push_back(uniformValue);
	}
}

/**
* 传递 Uniform 值，mat4
*/
void CMaterial::PassUniform(const string& name, const CMatrix4& value) {
	if (m_pShader) {
		SUniformValue uniformValue;
		uniformValue.location = m_pShader->GetUniform(name);
		uniformValue.type = CShader::MAT4;
		memcpy(uniformValue.fValue, value.m_fValue, sizeof(float) * 16);
		m_vecUniforms.push_back(uniformValue);
	}
}

/**
* 通用形式传递 Uniform 值
*/
void CMaterial::PassUniform(const string& name, const float value[], int size) {
	if (m_pShader) {
		SUniformValue uniformValue;
		switch (size) {
		case 1: uniformValue.type = CShader::FLOAT; break;
		case 2: uniformValue.type = CShader::VEC2; break;
		case 3: uniformValue.type = CShader::VEC3; break;
		case 4: uniformValue.type = CShader::VEC4; break;
		case 9: uniformValue.type = CShader::MAT3; break;
		case 16: uniformValue.type = CShader::MAT4; break;
		}
		uniformValue.location = m_pShader->GetUniform(name);
		memcpy(uniformValue.fValue, value, sizeof(float) * size);
		m_vecUniforms.push_back(uniformValue);
	}
}

/**
* 设置渲染模式
*/
void CMaterial::SetRenderMode(bool cullFace, bool useDepth, bool addColor) {
	m_bCullFace = cullFace;
	m_bUseDepth = useDepth;
	m_bAddColor = addColor;
}

/**
* 创建已缓冲的纹理对象
* @see SetTextureBuffered
*/
void CMaterial::CreateBufferedTexture() {
	if (m_mapBufferedTextures.empty()) return;
	map<int, SBufferedTexture>::iterator iter = m_mapBufferedTextures.begin();
	while (iter != m_mapBufferedTextures.end()) {
		const SBufferedTexture& t = iter->second;
		if (t.data) {
			SetTexture(t.name, t.width, t.height, t.channel, t.data, t.mipmap, iter->first);
			delete[] t.data;
		} else SetTexture(t.name, iter->first);
		if (t.wraps == 1) m_pTexture[iter->first]->SetWrapModeRepeat(true, false);
		else if (t.wraps == 2) m_pTexture[iter->first]->SetWrapModeClampToEdge(true, false);
		else if (t.wraps == 3) m_pTexture[iter->first]->SetWrapModeMirroredRepeat(true, false);
		if (t.wrapt == 1) m_pTexture[iter->first]->SetWrapModeRepeat(false, true);
		else if (t.wrapt == 2) m_pTexture[iter->first]->SetWrapModeClampToEdge(false, true);
		else if (t.wrapt == 3) m_pTexture[iter->first]->SetWrapModeMirroredRepeat(false, true);
		++iter;
	}
	m_mapBufferedTextures.clear();
}