//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CShader.h"
#include "CShaderManager.h"
#include "AvatarConfig.h"

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
* 构造函数
*/
CShader::CShader(CShaderManager* manager) {
	m_pManager = manager;
	m_bIsValid = false;
	m_iProgram = 0;
	m_iRefCount = 1;
}

/**
* 析构函数
*/
CShader::~CShader() {
	if (m_iProgram) glDeleteProgram(m_iProgram);
}

/**
* 着色器是否有效
*/
bool CShader::IsValid() const {
	return m_bIsValid;
}

/**
* 获取着色器名称
*/
const string& CShader::GetName() const {
	return m_strName;
}

/**
* 判断是否含有指定名称的 Uniform
*/
bool CShader::IsUniform(const string& name) const {
	return m_mapUniforms.count(name) > 0;
}

/**
* 获取指定名称的 Uniform 位置
*/
int CShader::GetUniform(const string& name) const {
	map<string, int>::const_iterator iter = m_mapUniforms.find(name);
	if (iter != m_mapUniforms.end()) return iter->second;
	return -1;
}

/**
* 获取着色器ID
*/
unsigned int CShader::GetProgramId() const {
	return m_iProgram;
}

/**
* 设置 bool 类型 Uniform 值
*/
void CShader::SetUniform(const string& name, bool value) const {
	map<string, int>::const_iterator iter = m_mapUniforms.find(name);
	if (iter != m_mapUniforms.end()) {
		glUniform1i(iter->second, value? 1: 0);
	}
}

/**
* 设置 int 类型 Uniform 值
*/
void CShader::SetUniform(const string& name, int value) const {
	map<string, int>::const_iterator iter = m_mapUniforms.find(name);
	if (iter != m_mapUniforms.end()) {
		glUniform1i(iter->second, value);
	}
}

/**
* 设置 float 类型 Uniform 值
*/
void CShader::SetUniform(const string& name, float value) const {
	map<string, int>::const_iterator iter = m_mapUniforms.find(name);
	if (iter != m_mapUniforms.end()) {
		glUniform1f(iter->second, value);
	}
}

/**
* 设置 vec2 类型 Uniform 值
*/
void CShader::SetUniform(const string& name, const CVector2& value) const {
	map<string, int>::const_iterator iter = m_mapUniforms.find(name);
	if (iter != m_mapUniforms.end()) {
		glUniform2fv(iter->second, 1, value.m_fValue);
	}
}

/**
* 设置 vec3 类型 Uniform 值
*/
void CShader::SetUniform(const string& name, const CVector3& value) const {
	map<string, int>::const_iterator iter = m_mapUniforms.find(name);
	if (iter != m_mapUniforms.end()) {
		glUniform3fv(iter->second, 1, value.m_fValue);
	}
}

/**
* 设置 mat4 类型 Uniform 值
*/
void CShader::SetUniform(const string& name, const CMatrix4& value) const {
	map<string, int>::const_iterator iter = m_mapUniforms.find(name);
	if (iter != m_mapUniforms.end()) {
		glUniformMatrix4fv(iter->second, 1, GL_FALSE, value.m_fValue);
	}
}

/**
* 通用方式设置 Uniform 值
*/
void CShader::SetUniform(const string& name, const float value[], int size, int length) const {
	map<string, int>::const_iterator iter = m_mapUniforms.find(name);
	if (iter != m_mapUniforms.end()) {
		switch (size) {
		case 1: glUniform1fv(iter->second, length, value); break;
		case 2: glUniform2fv(iter->second, length, value); break;
		case 3: glUniform3fv(iter->second, length, value); break;
		case 4: glUniform4fv(iter->second, length, value); break;
		case 9: glUniformMatrix3fv(iter->second, length, GL_FALSE, value); break;
		case 16: glUniformMatrix4fv(iter->second, length, GL_FALSE, value); break;
		}
	}
}

/**
* 设置 bool 类型 Uniform 值
*/
void CShader::SetUniform(int location, bool value) const {
	glUniform1i(location, value? 1: 0);
}

/**
* 设置 int 类型 Uniform 值
*/
void CShader::SetUniform(int location, int value) const {
	glUniform1i(location, value);
}

/**
* 设置 float 类型 Uniform 值
*/
void CShader::SetUniform(int location, float value) const {
	glUniform1f(location, value);
}

/**
* 设置 vec2 类型 Uniform 值
*/
void CShader::SetUniform(int location, const CVector2& value) const {
	glUniform2fv(location, 1, value.m_fValue);
}

/**
* 设置 vec3 类型 Uniform 值
*/
void CShader::SetUniform(int location, const CVector3& value) const {
	glUniform3fv(location, 1, value.m_fValue);
}

/**
* 设置 mat4 类型 Uniform 值
*/
void CShader::SetUniform(int location, const CMatrix4& value) const {
	glUniformMatrix4fv(location, 1, GL_FALSE, value.m_fValue);
}

/**
* 通用方式设置 Uniform 值
*/
void CShader::SetUniform(int location, const float value[], int size, int length) const {
	switch (size) {
	case 1: glUniform1fv(location, length, value); break;
	case 2: glUniform2fv(location, length, value); break;
	case 3: glUniform3fv(location, length, value); break;
	case 4: glUniform4fv(location, length, value); break;
	case 9: glUniformMatrix3fv(location, length, GL_FALSE, value); break;
	case 16: glUniformMatrix4fv(location, length, GL_FALSE, value); break;
	}
}

/**
* 使用着色器
*/
void CShader::UseShader() {
	if (m_pManager->m_pCurrentShader != this) {
		glUseProgram(m_iProgram);
		m_pManager->m_pCurrentShader = this;
	}
}

/**
* 增加引用
*/
CShader* CShader::AddReference() {
	m_iRefCount += 1;
	return this;
}