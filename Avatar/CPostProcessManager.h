//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSMANAGER_H_
#define _CPOSTPROCESSMANAGER_H_
#include "AvatarConfig.h"
#include "CPostProcess.h"
#include "CTexture.h"
#include "CMesh.h"
#include <string>
#include <vector>
#include <map>
#include <list>
using std::string;
using std::vector;
using std::map;
using std::list;

/**
* @brief ͼ����������
*/
class AVATAR_EXPORT CPostProcessManager {
public:
	//! ��ȡ������ʵ��
	static CPostProcessManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CPostProcessManager();
		return m_pInstance;
	}
	//! ʵ������
	void Destroy();

	//! ע�����
	bool Register(const string& name, CPostProcess* postProcess);
	//! ���ӻ�ȡ��ָ����ͼ�����
	bool Enable(const string& name, bool enable);
	//! ָ���ĺ����Ƿ�����
	bool IsEnabled(const string& name);
	//! ��ȡ��ע��ĺ����б�
	void GetPostProcessList(vector<string>& postList);

	//! ׼������
	bool PrepareFrame(CTexture* renderTarget);
	//! ���к���
	void ApplyFrame();
	//! ���º���ͼƬ��С
	void UpdateSize(int width, int height);

	//! ���ú������ int
	void SetProcessParam(const string& name, const string& key, int value);
	//! ���ú������ float
	void SetProcessParam(const string& name, const string& key, float value);
	//! ���ú������ vec2
	void SetProcessParam(const string& name, const string& key, const CVector2& value);
	//! ���ú������ vec3
	void SetProcessParam(const string& name, const string& key, const CVector3& value);
	//! ���ú������ mat4
	void SetProcessParam(const string& name, const string& key, const CMatrix4& value);
	//! ���ú������ float[]
	void SetProcessParam(const string& name, const string& key, const float value[], int size);

private:
	CPostProcessManager();
	~CPostProcessManager();

	//! ע�����ú���
	bool RegisterDefault(const string& name);

private:
	//! ������Ļ��С
	int m_iScreenSize[2];
	//! �����Դ��ȾĿ��
	CTexture* m_pSavedRenderTarget;
	//! �����б�
	map<string, CPostProcess*> m_mapPostProcess;
	//! ���õĺ���
	list<CPostProcess*> m_lstEnabledPostProcess;
	//! ���ں�����Ⱦ���������
	CMesh* m_pRenderMesh;

	//! ʵ��
	static CPostProcessManager* m_pInstance;
};

#endif