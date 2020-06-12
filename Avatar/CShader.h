//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSHADER_H_
#define _CSHADER_H_
#include "AvatarConfig.h"
#include "CVector2.h"
#include "CVector3.h"
#include "CMatrix4.h"
#include <string>
#include <map>
using std::string;
using std::map;

/**
* @brief 着色器类
* @attention 设置 Uniform 值前需要先调用 UseShader()
*/
class AVATAR_EXPORT CShader {
public:
	//! Uniform 变量类型枚举
	enum UniformType { UNKNOWN, BOOL, INT, FLOAT, VEC2, VEC3, VEC4, MAT3, MAT4 };

public:
	//! 着色器是否有效
	bool IsValid() const;
	//! 获取着色器名称
	const string& GetName() const;

	//! 判断是否含有指定名称的 Uniform
	bool IsUniform(const string& name) const;
	//! 获取指定名称的 Uniform 位置
	int GetUniform(const string& name) const;
	//! 获取着色器ID
	unsigned int GetProgramId() const;

	//! 设置 Uniform 值 bool
	void SetUniform(const string& name, bool value) const;
	//! 设置 Uniform 值 int
	void SetUniform(const string& name, int value) const;
	//! 设置 Uniform 值 float
	void SetUniform(const string& name, float value) const;
	//! 设置 Uniform 值 vec2
	void SetUniform(const string& name, const CVector2& value) const;
	//! 设置 Uniform 值 vec3
	void SetUniform(const string& name, const CVector3& value) const;
	//! 设置 Uniform 值 mat4
	void SetUniform(const string& name, const CMatrix4& value) const;
	//! 设置 Uniform 值 float[]
	void SetUniform(const string& name, const float value[], int size, int length) const;

	//! 设置 Uniform 值 bool
	void SetUniform(int location, bool value) const;
	//! 设置 Uniform 值 int
	void SetUniform(int location, int value) const;
	//! 设置 Uniform 值 float
	void SetUniform(int location, float value) const;
	//! 设置 Uniform 值 vec2
	void SetUniform(int location, const CVector2& value) const;
	//! 设置 Uniform 值 vec3
	void SetUniform(int location, const CVector3& value) const;
	//! 设置 Uniform 值 mat4
	void SetUniform(int location, const CMatrix4& value) const;
	//! 设置 Uniform 值 float[]
	void SetUniform(int location, const float value[], int size, int length) const;

	//! 使用着色器
	void UseShader();
	//! 增加引用
	CShader* AddReference();

protected:
	//! 受保护构造函数，只能通过 CShaderManager 创建
	CShader(class CShaderManager* manager);
	//! 受保护析构函数
	~CShader();

private:
	bool m_bIsValid;
	string m_strName;
	string m_strVertShader;
	string m_strFragShader;

	unsigned int m_iProgram;
	map<string, int> m_mapUniforms;
	map<string, UniformType> m_mapUniformTypes;

	int m_iRefCount;
	class CShaderManager* m_pManager;
	friend class CShaderManager;
};

#endif