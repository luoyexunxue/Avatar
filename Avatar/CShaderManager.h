//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* @brief ��ɫ��������
*/
class AVATAR_EXPORT CShaderManager {
public:
	//! ��ȡ������ʵ��
	static CShaderManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CShaderManager();
		return m_pInstance;
	}
	//! ʵ������
	void Destroy();

	//! ���ļ�������ɫ��
	CShader* Create(const string& name, const string file[2]);
	//! ���ַ���������ɫ��
	CShader* Create(const string& name, const char* vert, const char* frag);

	//! ����ָ������ɫ��
	void Drop(CShader* shader);
	//! ������ɫ������
	void Update(CShader* shader, const string& append, const string& remove);

	//! ��ȡָ�����Ƶ���ɫ��
	CShader* GetShader(const string& name);
	//! ��ȡ��ǰ���õ���ɫ��
	CShader* GetCurrentShader();
	//! ��ȡ�����������ɫ���б�
	void GetShaderList(vector<CShader*>& shaderList);

private:
	CShaderManager();
	~CShaderManager();

	//! ������ɫ��
	void CreateShader(CShader* shader);
	//! ������ɫ��
	bool CompileShader(const char* source, unsigned int shader, bool isFragment);
	//! ��ָ������ɫ���� Uniform ����
	void AttachUniforms(CShader* shader);
	//! ����ɫ������궨��
	void ShaderSourceDefine(CShader* shader, const string& append, const string& remove);

private:
	//! ��ǰ��ɫ��
	CShader* m_pCurrentShader;
	//! ��ɫ������
	map<string, CShader*> m_mapShader;
	//! ʵ��
	static CShaderManager* m_pInstance;
	//! ������Ԫ��
	friend class CShader;
};

#endif