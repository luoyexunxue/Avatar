//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
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
* 构造函数
*/
CShaderManager::CShaderManager() {
	m_pCurrentShader = 0;
}

/**
* 析构函数
*/
CShaderManager::~CShaderManager() {
	m_pInstance = 0;
}

/**
* 单例实例
*/
CShaderManager* CShaderManager::m_pInstance = 0;

/**
* 销毁着色器管理器实例
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
* 从文件创建着色器程序
* @param name 着色器名称
* @param file (顶点/像素)着色器代码文件
* @return 创建的着色器
*/
CShader* CShaderManager::Create(const string& name, const string file[2]) {
	// 是否已经加载着色器
	map<string, CShader*>::iterator iter = m_mapShader.find(name);
	if (iter != m_mapShader.end()) {
		iter->second->m_iRefCount++;
		iter->second->UseShader();
		return iter->second;
	}
	// 读取 Shader 文件
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
* 从内存创建着色器程序
* @param name 着色器名称
* @param vert 顶点着色器代码字符串
* @param frag 像素着色器代码字符串
* @return 创建的着色器
*/
CShader* CShaderManager::Create(const string& name, const char* vert, const char* frag) {
	// 是否已经加载着色器
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
	// 创建着色器
	CreateShader(pShader);
	return pShader;
}

/**
* 销毁着色器程序
* @param shader 需要销毁的着色器
*/
void CShaderManager::Drop(CShader* shader) {
	if (!shader) return;
	// 引用计数减一
	shader->m_iRefCount--;
	if (shader->m_iRefCount <= 0) {
		m_mapShader.erase(shader->m_strName);
		delete shader;
	}
}

/**
* 更新着色器程序
* @param shader 需要更新的着色器
* @param append 需要添加的宏，逗号隔开
* @param remove 需要移除的宏，逗号隔开
*/
void CShaderManager::Update(CShader* shader, const string& append, const string& remove) {
	if (!shader) return;
	// 保存当前 Uniform 值
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
	// 删除当前的着色器程序
	glDeleteProgram(shader->m_iProgram);
	// 添加和删除宏定义
	ShaderSourceDefine(shader, append, remove);
	// 重新创建着色器
	CreateShader(shader);
	// 设置之前保存的 uniform 值
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
* 获取指定名称的着色器
* @param name 着色器名称
* @return 名称为 name 的着色器
*/
CShader* CShaderManager::GetShader(const string& name) {
	map<string, CShader*>::iterator iter = m_mapShader.find(name);
	if (iter != m_mapShader.end()) {
		return iter->second;
	}
	return 0;
}

/**
* 获取当前在用的着色器
* @return 当前着色器
*/
CShader* CShaderManager::GetCurrentShader() {
	return m_pCurrentShader;
}

/**
* 获取管理的所有着色器列表
* @param shaderList 输出着色器列表
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
* 加载着色器
* @param shader 着色器指针
*/
void CShaderManager::CreateShader(CShader* shader) {
	shader->m_bIsValid = false;
	unsigned int v = glCreateShader(GL_VERTEX_SHADER);
	unsigned int f = glCreateShader(GL_FRAGMENT_SHADER);
	// 编译着色器
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

	// 绑定 Attribute 入口
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

	// 获取错误信息
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
	// 默认使用当前创建的着色器
	glUseProgram(shader->m_iProgram);
	m_pCurrentShader = shader;
}

/**
* 编译着色器
* @param source 着色器代码字符串
* @param shader OpenGL 着色器对象
* @param isFragment 是否像素着色代码
* @return 成功返回 true
*/
bool CShaderManager::CompileShader(const char* source, unsigned int shader, bool isFragment) {
	// 载入 Shader 源代码
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
	// 编译 Shader
	glCompileShader(shader);

	// 获取错误信息
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
* 给指定的着色器绑定 Uniform 变量
* @param shader 着色器指针
*/
void CShaderManager::AttachUniforms(CShader* shader) {
	shader->m_mapUniforms.clear();
	shader->m_mapUniformTypes.clear();
	// 获取 Uniform 个数
	size_t numUniform = 0;
	glGetProgramiv(shader->m_iProgram, GL_ACTIVE_UNIFORMS, (GLint*)&numUniform);
	// 获取每个 Uniform 信息
	GLint uniformSize;
	GLenum uniformType;
	GLchar uniformName[256];
	for (size_t i = 0; i < numUniform; i++) {
		memset(uniformName, 0, 256);
		glGetActiveUniform(shader->m_iProgram, i, 256, 0, &uniformSize, &uniformType, uniformName);
		// 获取 Uniform 类型
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
		// 得到 Uniform 的位置
		int uniformLocation;
		if (uniformSize == 1) {
			uniformLocation = glGetUniformLocation(shader->m_iProgram, uniformName);
			shader->m_mapUniforms.insert(std::pair<string, int>(uniformName, uniformLocation));
			shader->m_mapUniformTypes.insert(std::pair<string, CShader::UniformType>(uniformName, eUniformType));
			continue;
		}
		// 部分驱动获取 Uniform 数组名称为 uniformName[0]
		for (size_t n = strlen(uniformName) - 1; n > 0; n--) {
			if (uniformName[n] == '[') {
				uniformName[n] = '\0';
				break;
			}
		}
		uniformLocation = glGetUniformLocation(shader->m_iProgram, uniformName);
		shader->m_mapUniforms.insert(std::pair<string, int>(uniformName, uniformLocation));
		shader->m_mapUniformTypes.insert(std::pair<string, CShader::UniformType>(uniformName, eUniformType));
		// 获取 Uniform 数组每个元素位置
		for (int n = 0; n < uniformSize; n++) {
			string strName = CStringUtil::Format("%s[%d]", uniformName, n);
			uniformLocation = glGetUniformLocation(shader->m_iProgram, strName.c_str());
			shader->m_mapUniforms.insert(std::pair<string, int>(strName, uniformLocation));
			shader->m_mapUniformTypes.insert(std::pair<string, CShader::UniformType>(strName, eUniformType));
		}
	}
}

/**
* 对着色器代码添加或去掉宏定义
* @param shader 着色器指针
* @param append 需要添加的宏
* @param remove 需要移除的宏
*/
void CShaderManager::ShaderSourceDefine(CShader* shader, const string& append, const string& remove) {
	vector<string> appendArray;
	vector<string> removeArray;
	CStringUtil::Split(appendArray, append, ",", true);
	CStringUtil::Split(removeArray, remove, ",", true);
	// 删除定义的宏
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
	// 添加宏定义
	shader->m_strVertShader = defined + vertShader;
	shader->m_strFragShader = defined + fragShader;
}