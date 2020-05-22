//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* @brief ��ɫ����
* @attention ���� Uniform ֵǰ��Ҫ�ȵ��� UseShader()
*/
class AVATAR_EXPORT CShader {
public:
	//! Uniform ��������ö��
	enum UniformType { UNKNOWN, BOOL, INT, FLOAT, VEC2, VEC3, VEC4, MAT3, MAT4 };

public:
	//! ��ɫ���Ƿ���Ч
	bool IsValid() const;
	//! ��ȡ��ɫ������
	const string& GetName() const;

	//! �ж��Ƿ���ָ�����Ƶ� Uniform
	bool IsUniform(const string& name) const;
	//! ��ȡָ�����Ƶ� Uniform λ��
	int GetUniform(const string& name) const;
	//! ��ȡ��ɫ��ID
	unsigned int GetProgramId() const;

	//! ���� Uniform ֵ bool
	void SetUniform(const string& name, bool value) const;
	//! ���� Uniform ֵ int
	void SetUniform(const string& name, int value) const;
	//! ���� Uniform ֵ float
	void SetUniform(const string& name, float value) const;
	//! ���� Uniform ֵ vec2
	void SetUniform(const string& name, const CVector2& value) const;
	//! ���� Uniform ֵ vec3
	void SetUniform(const string& name, const CVector3& value) const;
	//! ���� Uniform ֵ mat4
	void SetUniform(const string& name, const CMatrix4& value) const;
	//! ���� Uniform ֵ float[]
	void SetUniform(const string& name, const float value[], int size, int length) const;

	//! ���� Uniform ֵ bool
	void SetUniform(int location, bool value) const;
	//! ���� Uniform ֵ int
	void SetUniform(int location, int value) const;
	//! ���� Uniform ֵ float
	void SetUniform(int location, float value) const;
	//! ���� Uniform ֵ vec2
	void SetUniform(int location, const CVector2& value) const;
	//! ���� Uniform ֵ vec3
	void SetUniform(int location, const CVector3& value) const;
	//! ���� Uniform ֵ mat4
	void SetUniform(int location, const CMatrix4& value) const;
	//! ���� Uniform ֵ float[]
	void SetUniform(int location, const float value[], int size, int length) const;

	//! ʹ����ɫ��
	void UseShader();
	//! ��������
	CShader* AddReference();

protected:
	//! �ܱ������캯����ֻ��ͨ�� CShaderManager ����
	CShader(class CShaderManager* manager);
	//! �ܱ�����������
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