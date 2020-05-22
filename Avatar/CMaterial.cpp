//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CMaterial.h"
#include "CEngine.h"
#include <cstring>

/**
* ���캯��
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
	// ��Ⱦģʽ
	m_bCullFace = true;
	m_bUseDepth = true;
	m_bAddColor = false;
}

/**
* ��������
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
}

/**
* ���Ʋ���
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
}

/**
* ʹ�ò���
*/
void CMaterial::UseMaterial() {
	// ��������Ԫ
	if (m_iTextureCount == 1) m_pTexture[0]->UseTexture();
	else if (m_iTextureCount > 1) {
		for (int i = m_iTextureCount - 1; i >= 0; i--) {
			m_pTexture[i]->UseTexture(i);
		}
	}
	// ���� Uniform ����
	if (m_pShader) {
		m_pShader->UseShader();
		m_pShader->SetUniform("uRoughness", m_fRoughness);
		m_pShader->SetUniform("uMetalness", m_fMetalness);
		m_pShader->SetUniform("uBaseColor", m_fColor, 4, 1);
		for (size_t i = 0; i < m_vecUniforms.size(); i++) {
			SUniformValue* item = &m_vecUniforms[i];
			switch (item->type) {
			case CShader::BOOL: m_pShader->SetUniform(item->location, item->value.bValue); break;
			case CShader::INT: m_pShader->SetUniform(item->location, item->value.iValue); break;
			case CShader::FLOAT: m_pShader->SetUniform(item->location, item->value.fValue, 1, 1); break;
			case CShader::VEC2: m_pShader->SetUniform(item->location, item->value.fValue, 2, 1); break;
			case CShader::VEC3: m_pShader->SetUniform(item->location, item->value.fValue, 3, 1); break;
			case CShader::VEC4: m_pShader->SetUniform(item->location, item->value.fValue, 4, 1); break;
			case CShader::MAT3: m_pShader->SetUniform(item->location, item->value.fValue, 9, 1); break;
			case CShader::MAT4: m_pShader->SetUniform(item->location, item->value.fValue, 16, 1); break;
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
* ���ò�����ͼ
*/
void CMaterial::SetTexture(CTexture* texture, int index) {
	if (m_pTexture[index]) {
		m_iTextureCount -= 1;
	}
	CEngine::GetTextureManager()->Drop(m_pTexture[index]);
	if (texture) {
		m_pTexture[index] = texture->AddReference();
		m_iTextureCount += 1;
	} else {
		m_pTexture[index] = 0;
	}
}

/**
* ���ò�����ͼ
*/
void CMaterial::SetTexture(const string& file, int index) {
	if (!m_pTexture[index]) {
		m_iTextureCount += 1;
	}
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	pTextureMgr->Drop(m_pTexture[index]);
	m_pTexture[index] = pTextureMgr->Create(file);
}

/**
* ���������������ͼ
*/
void CMaterial::SetTexture(const string& name, const string files[6], int index) {
	if (!m_pTexture[index]) {
		m_iTextureCount += 1;
	}
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	pTextureMgr->Drop(m_pTexture[index]);
	m_pTexture[index] = pTextureMgr->Create(name, files);
}

/**
* �����ڴ������ͼ
*/
void CMaterial::SetTexture(const string& name, int width, int height, int channel, const void* data, int index) {
	if (!m_pTexture[index]) {
		m_iTextureCount += 1;
	}
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	pTextureMgr->Drop(m_pTexture[index]);
	m_pTexture[index] = pTextureMgr->Create(name, width, height, channel, data, false);
}

/**
* ������ɫ��
*/
void CMaterial::SetShader(const string& name) {
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	pShaderMgr->Drop(m_pShader);
	CShader* shader = pShaderMgr->GetShader(name);
	m_pShader = shader ? shader->AddReference() : 0;
}

/**
* ������ɫ��
*/
void CMaterial::SetShader(CShader* shader) {
	CEngine::GetShaderManager()->Drop(m_pShader);
	m_pShader = shader ? shader->AddReference() : 0;
}

/**
* ���� Uniform ֵ��bool
*/
void CMaterial::PassUniform(const string& name, bool value) {
	if (m_pShader) {
		SUniformValue uniformValue;
		uniformValue.location = m_pShader->GetUniform(name);
		uniformValue.type = CShader::BOOL;
		uniformValue.value.bValue = value;
		m_vecUniforms.push_back(uniformValue);
	}
}

/**
* ���� Uniform ֵ��int
*/
void CMaterial::PassUniform(const string& name, int value) {
	if (m_pShader) {
		SUniformValue uniformValue;
		uniformValue.location = m_pShader->GetUniform(name);
		uniformValue.type = CShader::INT;
		uniformValue.value.iValue = value;
		m_vecUniforms.push_back(uniformValue);
	}
}

/**
* ���� Uniform ֵ��float
*/
void CMaterial::PassUniform(const string& name, float value) {
	if (m_pShader) {
		SUniformValue uniformValue;
		uniformValue.location = m_pShader->GetUniform(name);
		uniformValue.type = CShader::FLOAT;
		uniformValue.value.fValue[0] = value;
		m_vecUniforms.push_back(uniformValue);
	}
}

/**
* ���� Uniform ֵ��vec2
*/
void CMaterial::PassUniform(const string& name, const CVector2& value) {
	if (m_pShader) {
		SUniformValue uniformValue;
		uniformValue.location = m_pShader->GetUniform(name);
		uniformValue.type = CShader::VEC2;
		uniformValue.value.fValue[0] = value.m_fValue[0];
		uniformValue.value.fValue[1] = value.m_fValue[1];
		m_vecUniforms.push_back(uniformValue);
	}
}

/**
* ���� Uniform ֵ��vec3
*/
void CMaterial::PassUniform(const string& name, const CVector3& value) {
	if (m_pShader) {
		SUniformValue uniformValue;
		uniformValue.location = m_pShader->GetUniform(name);
		uniformValue.type = CShader::VEC3;
		uniformValue.value.fValue[0] = value.m_fValue[0];
		uniformValue.value.fValue[1] = value.m_fValue[1];
		uniformValue.value.fValue[2] = value.m_fValue[2];
		m_vecUniforms.push_back(uniformValue);
	}
}

/**
* ���� Uniform ֵ��mat4
*/
void CMaterial::PassUniform(const string& name, const CMatrix4& value) {
	if (m_pShader) {
		SUniformValue uniformValue;
		uniformValue.location = m_pShader->GetUniform(name);
		uniformValue.type = CShader::MAT4;
		memcpy(uniformValue.value.fValue, value.m_fValue, sizeof(float) * 16);
		m_vecUniforms.push_back(uniformValue);
	}
}

/**
* ͨ����ʽ���� Uniform ֵ
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
		memcpy(uniformValue.value.fValue, value, sizeof(float) * size);
		m_vecUniforms.push_back(uniformValue);
	}
}

/**
* ������Ⱦģʽ
*/
void CMaterial::SetRenderMode(bool cullFace, bool useDepth, bool addColor) {
	m_bCullFace = cullFace;
	m_bUseDepth = useDepth;
	m_bAddColor = addColor;
}