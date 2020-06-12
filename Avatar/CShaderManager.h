//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSHADERMANAGER_H_
#define _CSHADERMANAGER_H_
#include "AvatarConfig.h"
#include "CShader.h"
#include <string>
#include <map>
#include <vector>
using std::string;
using std::map;
using std::vector;

/**
* @brief 着色器管理器
*/
class AVATAR_EXPORT CShaderManager {
public:
	//! 获取管理器实例
	static CShaderManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CShaderManager();
		return m_pInstance;
	}
	//! 实例销毁
	void Destroy();

	//! 从文件创建着色器
	CShader* Create(const string& name, const string file[2]);
	//! 从字符串创建着色器
	CShader* Create(const string& name, const char* vert, const char* frag);

	//! 销毁指定的着色器
	void Drop(CShader* shader);
	//! 更新着色器特性
	void Update(CShader* shader, const string& append, const string& remove);

	//! 获取指定名称的着色器
	CShader* GetShader(const string& name);
	//! 获取当前在用的着色器
	CShader* GetCurrentShader();
	//! 获取管理的所有着色器列表
	void GetShaderList(vector<CShader*>& shaderList);

private:
	CShaderManager();
	~CShaderManager();

	//! 加载着色器
	void CreateShader(CShader* shader);
	//! 编译着色器
	bool CompileShader(const char* source, unsigned int shader, bool isFragment);
	//! 给指定的着色器绑定 Uniform 变量
	void AttachUniforms(CShader* shader);
	//! 对着色器代码宏定义
	void ShaderSourceDefine(CShader* shader, const string& append, const string& remove);

private:
	//! 当前着色器
	CShader* m_pCurrentShader;
	//! 着色器集合
	map<string, CShader*> m_mapShader;
	//! 实例
	static CShaderManager* m_pInstance;
	//! 设置友元类
	friend class CShader;
};

#endif