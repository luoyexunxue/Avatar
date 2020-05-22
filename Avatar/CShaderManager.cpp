//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CShaderManager.h"
#include "AvatarConfig.h"
#include "CEngine.h"
#include "CStringUtil.h"
#include "CLog.h"
#include <cstring>

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
* ���캯��
*/
CShaderManager::CShaderManager() {
	m_pCurrentShader = 0;
}

/**
* ��������
*/
CShaderManager::~CShaderManager() {
	m_pInstance = 0;
}

/**
* ����ʵ��
*/
CShaderManager* CShaderManager::m_pInstance = 0;

/**
* ������ɫ��������ʵ��
*/
void CShaderManager::Destroy() {
	glUseProgram(0);
	map<string, CShader*>::iterator iter = m_mapShader.begin();
	while (iter != m_mapShader.end()) {
		delete iter->second;
		++iter;
	}
	delete this;
}

/**
* ���ļ�������ɫ������
* @param name ��ɫ������
* @param file (����/����)��ɫ�������ļ�
* @return ��������ɫ��
*/
CShader* CShaderManager::Create(const string& name, const string file[2]) {
	// �Ƿ��Ѿ�������ɫ��
	map<string, CShader*>::iterator iter = m_mapShader.find(name);
	if (iter != m_mapShader.end()) {
		iter->second->m_iRefCount++;
		iter->second->UseShader();
		return iter->second;
	}
	// ��ȡ Shader �ļ�
	CFileManager::CTextFile text[2];
	for (int i = 0; i < 2; i++) {
		if (!CEngine::GetFileManager()->ReadFile(file[i], &text[i])) {
			CLog::Error("Read shader file '%s' error", file[i].c_str());
			CShader* pShader = new CShader(this);
			pShader->m_strName = name;
			pShader->m_strVertShader = file[0];
			pShader->m_strFragShader = file[1];
			m_mapShader.insert(std::pair<string, CShader*>(name, pShader));
			return pShader;
		}
	}
	return Create(name, (char*)text[0].contents, (char*)text[1].contents);
}

/**
* ���ڴ洴����ɫ������
* @param name ��ɫ������
* @param vert ������ɫ�������ַ���
* @param frag ������ɫ�������ַ���
* @return ��������ɫ��
*/
CShader* CShaderManager::Create(const string& name, const char* vert, const char* frag) {
	// �Ƿ��Ѿ�������ɫ��
	map<string, CShader*>::iterator iter = m_mapShader.find(name);
	if (iter != m_mapShader.end()) {
		iter->second->m_iRefCount++;
		iter->second->UseShader();
		return iter->second;
	}
	CShader* pShader = new CShader(this);
	pShader->m_strName = name;
	pShader->m_strVertShader = vert;
	pShader->m_strFragShader = frag;
	m_mapShader.insert(std::pair<string, CShader*>(name, pShader));
	// ������ɫ��
	CreateShader(pShader);
	return pShader;
}

/**
* ������ɫ������
* @param shader ��Ҫ���ٵ���ɫ��
*/
void CShaderManager::Drop(CShader* shader) {
	if (!shader) return;
	// ���ü�����һ
	shader->m_iRefCount--;
	if (shader->m_iRefCount <= 0) {
		m_mapShader.erase(shader->m_strName);
		delete shader;
	}
}

/**
* ������ɫ������
* @param shader ��Ҫ���µ���ɫ��
* @param append ��Ҫ��ӵĺ꣬���Ÿ���
* @param remove ��Ҫ�Ƴ��ĺ꣬���Ÿ���
*/
void CShaderManager::Update(CShader* shader, const string& append, const string& remove) {
	if (!shader) return;
	// ���浱ǰ Uniform ֵ
	struct SUniformValue {
		string name;
		CShader::UniformType type;
		union {
			int iValue[1];
			float fValue[16];
		} value;
	};
	vector<SUniformValue> uniformValues;
	map<string, int>::iterator iter = shader->m_mapUniforms.begin();
	while (iter != shader->m_mapUniforms.end()) {
		SUniformValue uniform;
		uniform.name = iter->first;
		uniform.type = shader->m_mapUniformTypes[iter->first];
		switch (uniform.type) {
		case CShader::BOOL:
		case CShader::INT:
			glGetUniformiv(shader->m_iProgram, iter->second, uniform.value.iValue);
			break;
		case CShader::FLOAT:
		case CShader::VEC2:
		case CShader::VEC3:
		case CShader::VEC4:
		case CShader::MAT3:
		case CShader::MAT4:
			glGetUniformfv(shader->m_iProgram, iter->second, uniform.value.fValue);
			break;
		default: break;
		}
		uniformValues.push_back(uniform);
		++iter;
	}
	// ɾ����ǰ����ɫ������
	glDeleteProgram(shader->m_iProgram);
	// ��Ӻ�ɾ���궨��
	ShaderSourceDefine(shader, append, remove);
	// ���´�����ɫ��
	CreateShader(shader);
	// ����֮ǰ����� uniform ֵ
	for (size_t i = 0; i < uniformValues.size(); i++) {
		SUniformValue* item = &uniformValues[i];
		if (shader->IsUniform(item->name)) {
			switch (item->type) {
			case CShader::BOOL: shader->SetUniform(item->name, item->value.iValue[0]); break;
			case CShader::INT: shader->SetUniform(item->name, item->value.iValue[0]); break;
			case CShader::FLOAT: shader->SetUniform(item->name, item->value.fValue, 1, 1); break;
			case CShader::VEC2: shader->SetUniform(item->name, item->value.fValue, 2, 1); break;
			case CShader::VEC3: shader->SetUniform(item->name, item->value.fValue, 3, 1); break;
			case CShader::VEC4: shader->SetUniform(item->name, item->value.fValue, 4, 1); break;
			case CShader::MAT3: shader->SetUniform(item->name, item->value.fValue, 9, 1); break;
			case CShader::MAT4: shader->SetUniform(item->name, item->value.fValue, 16, 1); break;
			default: break;
			}
		}
	}
}

/**
* ��ȡָ�����Ƶ���ɫ��
* @param name ��ɫ������
* @return ����Ϊ name ����ɫ��
*/
CShader* CShaderManager::GetShader(const string& name) {
	map<string, CShader*>::iterator iter = m_mapShader.find(name);
	if (iter != m_mapShader.end()) {
		return iter->second;
	}
	return 0;
}

/**
* ��ȡ��ǰ���õ���ɫ��
* @return ��ǰ��ɫ��
*/
CShader* CShaderManager::GetCurrentShader() {
	return m_pCurrentShader;
}

/**
* ��ȡ�����������ɫ���б�
* @param shaderList �����ɫ���б�
*/
void CShaderManager::GetShaderList(vector<CShader*>& shaderList) {
	shaderList.resize(m_mapShader.size());
	map<string, CShader*>::iterator iter = m_mapShader.begin();
	int index = 0;
	while (iter != m_mapShader.end()) {
		shaderList[index++] = iter->second;
		++iter;
	}
}

/**
* ������ɫ��
* @param shader ��ɫ��ָ��
*/
void CShaderManager::CreateShader(CShader* shader) {
	shader->m_bIsValid = false;
	unsigned int v = glCreateShader(GL_VERTEX_SHADER);
	unsigned int f = glCreateShader(GL_FRAGMENT_SHADER);
	// ������ɫ��
	bool bVert = CompileShader(shader->m_strVertShader.c_str(), v, false);
	bool bFrag = CompileShader(shader->m_strFragShader.c_str(), f, true);
	if (!bVert || !bFrag) {
		glDeleteShader(v);
		glDeleteShader(f);
		CLog::Warn("Load shader '%s' failed", shader->m_strName.c_str());
		return;
	}
	shader->m_iProgram = glCreateProgram();
	glAttachShader(shader->m_iProgram, f);
	glAttachShader(shader->m_iProgram, v);

	// �� Attribute ���
	glBindAttribLocation(shader->m_iProgram, 0, "aPosition");
	glBindAttribLocation(shader->m_iProgram, 1, "aTexCoord");
	glBindAttribLocation(shader->m_iProgram, 2, "aNormal");
	glBindAttribLocation(shader->m_iProgram, 3, "aColor");
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glLinkProgram(shader->m_iProgram);
	glDeleteShader(v);
	glDeleteShader(f);

	// ��ȡ������Ϣ
	int linkStatus = 0;
	glGetProgramiv(shader->m_iProgram, GL_LINK_STATUS, &linkStatus);
	if (!linkStatus) {
		GLint infoLen = 1024;
		glGetProgramiv(shader->m_iProgram, GL_INFO_LOG_LENGTH, &infoLen);
		char* logInfo = new char[infoLen];
		glGetProgramInfoLog(shader->m_iProgram, infoLen, 0, logInfo);
		CLog::Warn("Load shader '%s' error: %s", shader->m_strName.c_str(), logInfo);
		delete[] logInfo;
	} else {
		shader->m_bIsValid = true;
		AttachUniforms(shader);
	}
	// Ĭ��ʹ�õ�ǰ��������ɫ��
	glUseProgram(shader->m_iProgram);
	m_pCurrentShader = shader;
}

/**
* ������ɫ��
* @param source ��ɫ�������ַ���
* @param shader OpenGL ��ɫ������
* @param isFragment �Ƿ�������ɫ����
* @return �ɹ����� true
*/
bool CShaderManager::CompileShader(const char* source, unsigned int shader, bool isFragment) {
	// ���� Shader Դ����
#if (defined AVATAR_WINDOWS) || (defined AVATAR_LINUX)
	const char* GLSL_VERSION = "#version 330\n";
#else
	const char* GLSL_VERSION = "#version 300 es\n";
#endif
	if (isFragment) {
		const char* PRECISION = "\
			#ifdef GL_FRAGMENT_PRECISION_HIGH\n\
			precision highp float;\n\
			#else\n\
			precision mediump float;\n\
			#endif\n";
		const char* code[3] = { GLSL_VERSION, PRECISION, source };
		const int length[3] = { (int)strlen(GLSL_VERSION), (int)strlen(PRECISION), (int)strlen(source) };
		glShaderSource(shader, 3, code, length);
	} else {
		const char* code[2] = { GLSL_VERSION, source };
		const int length[2] = { (int)strlen(GLSL_VERSION), (int)strlen(source) };
		glShaderSource(shader, 2, code, length);
	}
	// ���� Shader
	glCompileShader(shader);

	// ��ȡ������Ϣ
	int compileStatus = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (!compileStatus) {
		GLint infoLen = 1024;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
		char* logInfo = new char[infoLen];
		glGetShaderInfoLog(shader, infoLen, 0, logInfo);
		CLog::Warn("Compile shader error: %s", logInfo);
		delete[] logInfo;
		return false;
	}
	return true;
}

/**
* ��ָ������ɫ���� Uniform ����
* @param shader ��ɫ��ָ��
*/
void CShaderManager::AttachUniforms(CShader* shader) {
	shader->m_mapUniforms.clear();
	shader->m_mapUniformTypes.clear();
	// ��ȡ Uniform ����
	size_t numUniform = 0;
	glGetProgramiv(shader->m_iProgram, GL_ACTIVE_UNIFORMS, (GLint*)&numUniform);
	// ��ȡÿ�� Uniform ��Ϣ
	GLint uniformSize;
	GLenum uniformType;
	GLchar uniformName[256];
	for (size_t i = 0; i < numUniform; i++) {
		memset(uniformName, 0, 256);
		glGetActiveUniform(shader->m_iProgram, i, 256, 0, &uniformSize, &uniformType, uniformName);
		// ��ȡ Uniform ����
		CShader::UniformType eUniformType;
		switch (uniformType) {
		case GL_BOOL: eUniformType = CShader::BOOL; break;
		case GL_INT: eUniformType = CShader::INT; break;
		case GL_FLOAT: eUniformType = CShader::FLOAT; break;
		case GL_FLOAT_VEC2: eUniformType = CShader::VEC2; break;
		case GL_FLOAT_VEC3: eUniformType = CShader::VEC3; break;
		case GL_FLOAT_VEC4: eUniformType = CShader::VEC4; break;
		case GL_FLOAT_MAT3: eUniformType = CShader::MAT3; break;
		case GL_FLOAT_MAT4: eUniformType = CShader::MAT4; break;
		case GL_SAMPLER_2D: eUniformType = CShader::INT; break;
		case GL_SAMPLER_CUBE: eUniformType = CShader::INT; break;
		default: eUniformType = CShader::UNKNOWN; break;
		}
		// �õ� Uniform ��λ��
		int uniformLocation;
		if (uniformSize == 1) {
			uniformLocation = glGetUniformLocation(shader->m_iProgram, uniformName);
			shader->m_mapUniforms.insert(std::pair<string, int>(uniformName, uniformLocation));
			shader->m_mapUniformTypes.insert(std::pair<string, CShader::UniformType>(uniformName, eUniformType));
			continue;
		}
		// ����������ȡ Uniform ��������Ϊ uniformName[0]
		for (size_t n = strlen(uniformName) - 1; n > 0; n--) {
			if (uniformName[n] == '[') {
				uniformName[n] = '\0';
				break;
			}
		}
		uniformLocation = glGetUniformLocation(shader->m_iProgram, uniformName);
		shader->m_mapUniforms.insert(std::pair<string, int>(uniformName, uniformLocation));
		shader->m_mapUniformTypes.insert(std::pair<string, CShader::UniformType>(uniformName, eUniformType));
		// ��ȡ Uniform ����ÿ��Ԫ��λ��
		for (int n = 0; n < uniformSize; n++) {
			string strName = CStringUtil::Format("%s[%d]", uniformName, n);
			uniformLocation = glGetUniformLocation(shader->m_iProgram, strName.c_str());
			shader->m_mapUniforms.insert(std::pair<string, int>(strName, uniformLocation));
			shader->m_mapUniformTypes.insert(std::pair<string, CShader::UniformType>(strName, eUniformType));
		}
	}
}

/**
* ����ɫ��������ӻ�ȥ���궨��
* @param shader ��ɫ��ָ��
* @param append ��Ҫ��ӵĺ�
* @param remove ��Ҫ�Ƴ��ĺ�
*/
void CShaderManager::ShaderSourceDefine(CShader* shader, const string& append, const string& remove) {
	vector<string> appendArray;
	vector<string> removeArray;
	CStringUtil::Split(appendArray, append, ",", true);
	CStringUtil::Split(removeArray, remove, ",", true);
	// ɾ������ĺ�
	string defined = "";
	string vertShader = shader->m_strVertShader;
	string fragShader = shader->m_strFragShader;
	for (size_t i = 0; i < appendArray.size(); i++) {
		string item = "#define " + appendArray[i] + "\n";
		vertShader = CStringUtil::Remove(vertShader, item.c_str());
		fragShader = CStringUtil::Remove(fragShader, item.c_str());
		defined += item;
	}
	for (size_t i = 0; i < removeArray.size(); i++) {
		string item = "#define " + removeArray[i] + "\n";
		vertShader = CStringUtil::Remove(vertShader, item.c_str());
		fragShader = CStringUtil::Remove(fragShader, item.c_str());
	}
	// ��Ӻ궨��
	shader->m_strVertShader = defined + vertShader;
	shader->m_strFragShader = defined + fragShader;
}