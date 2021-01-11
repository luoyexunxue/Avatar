//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPostProcessManager.h"
#include "CEngine.h"
#include "CLog.h"
#include "CPostProcessPass.h"
#include "CPostProcessOculus.h"
#include "CPostProcessBlur.h"
#include "CPostProcessGray.h"
#include "CPostProcessAnaglyph.h"
#include "CPostProcessEmboss.h"
#include "CPostProcessFxaa.h"
#include "CPostProcessEdge.h"
#include "CPostProcessCartoon.h"
#include "CPostProcessHdr.h"
#include "CPostProcessSsao.h"
#include "CPostProcessBeam.h"
#include "CPostProcessSnow.h"
#include "CPostProcessRain.h"
#include "CPostProcessBloom.h"
#include "CPostProcessPanorama.h"

/**
* 构造函数
*/
CPostProcessManager::CPostProcessManager() {
	m_pSavedRenderTarget = 0;
	m_pRenderMesh = new CMesh();
	m_pRenderMesh->SetVertexUsage(4);
	m_pRenderMesh->AddVertex(CVertex(-1.0f, 1.0f, 0.0f, 0.0f, 1.0f));
	m_pRenderMesh->AddVertex(CVertex(1.0f, 1.0f, 0.0f, 1.0f, 1.0f));
	m_pRenderMesh->AddVertex(CVertex(1.0f, -1.0f, 0.0f, 1.0f, 0.0f));
	m_pRenderMesh->AddVertex(CVertex(-1.0f, -1.0f, 0.0f, 0.0f, 0.0f));
	m_pRenderMesh->AddTriangle(3, 2, 0);
	m_pRenderMesh->AddTriangle(2, 1, 0);
	m_pRenderMesh->Create(false);
}

/**
* 析构函数
*/
CPostProcessManager::~CPostProcessManager() {
	delete m_pRenderMesh;
	m_pInstance = 0;
}

/**
* 单例实例
*/
CPostProcessManager* CPostProcessManager::m_pInstance = 0;

/**
* 实例后处理管理器
*/
void CPostProcessManager::Destroy() {
	map<string, CPostProcess*>::iterator iter = m_mapPostProcess.begin();
	while (iter != m_mapPostProcess.end()) {
		iter->second->Enable(false);
		iter->second->Destroy();
		delete iter->second;
		++iter;
	}
	delete this;
}

/**
* 注册后处理过程
*/
bool CPostProcessManager::Register(const string& name, CPostProcess* post) {
	// 删除已注册的同名称后处理
	if (m_mapPostProcess.count(name) > 0) {
		list<string>::iterator iter = m_lstEnabledPostProcess.begin();
		while (m_lstEnabledPostProcess.end() != iter++) {
			if (*iter == name) {
				m_lstEnabledPostProcess.erase(iter);
				break;
			}
		}
		CPostProcess* item = m_mapPostProcess[name];
		m_mapPostProcess.erase(name);
		item->Enable(false);
		item->Destroy();
		delete item;
	}
	int width, height;
	CEngine::GetGraphicsManager()->GetWindowSize(&width, &height);
	if (!post->Init(width, height)) {
		post->Destroy();
		delete post;
		return false;
	}
	post->m_iScreenSize[0] = width;
	post->m_iScreenSize[1] = height;
	m_mapPostProcess.insert(std::pair<string, CPostProcess*>(name, post));
	return true;
}

/**
* 附加或取消指定的图像后处理
*/
bool CPostProcessManager::Enable(const string& name, bool enable) {
	if (m_mapPostProcess.count(name) == 0 && !RegisterDefault(name)) return false;
	list<string>::iterator iter = m_lstEnabledPostProcess.begin();
	while (iter != m_lstEnabledPostProcess.end()) {
		if (*iter == name) {
			m_lstEnabledPostProcess.erase(iter);
			if (enable) m_lstEnabledPostProcess.push_back(name);
			else m_mapPostProcess[name]->Enable(false);
			return true;
		}
		++iter;
	}
	if (enable) {
		m_mapPostProcess[name]->Enable(true);
		m_lstEnabledPostProcess.push_back(name);
	}
	return true;
}

/**
* 指定的后处理是否启用
*/
bool CPostProcessManager::IsEnabled(const string& name) {
	list<string>::iterator iter = m_lstEnabledPostProcess.begin();
	while (iter != m_lstEnabledPostProcess.end()) {
		if (*iter == name) return true;
		++iter;
	}
	return false;
}

/**
* 获取已注册的后处理列表
*/
void CPostProcessManager::GetPostProcessList(vector<string>& postList) {
	postList.clear();
	map<string, CPostProcess*>::iterator iter = m_mapPostProcess.begin();
	while (iter != m_mapPostProcess.end()) {
		postList.push_back(iter->first);
		++iter;
	}
}

/**
* 准备后处理
*/
bool CPostProcessManager::PrepareFrame(CTexture* renderTarget, int width, int height) {
	if (m_lstEnabledPostProcess.empty()) return false;
	// 通知屏幕大小
	list<string>::iterator iter = m_lstEnabledPostProcess.begin();
	while (iter != m_lstEnabledPostProcess.end()) {
		CPostProcess* item = m_mapPostProcess[*iter++];
		if (item->m_iScreenSize[0] != width || item->m_iScreenSize[1] != height) {
			item->Resize(width, height);
			item->m_iScreenSize[0] = width;
			item->m_iScreenSize[1] = height;
		}
	}
	// 将第一个后处理设置为渲染目标
	CPostProcess* first = m_mapPostProcess[m_lstEnabledPostProcess.front()];
	m_pSavedRenderTarget = renderTarget;
	renderTarget = first->m_pRenderTexture;
	CEngine::GetGraphicsManager()->SetRenderTarget(renderTarget, 0, true, true);
	return true;
}

/**
* 进行后处理
* @note 后处理逐个执行，后一个的输入纹理作为前一个的渲染目标
*/
void CPostProcessManager::ApplyFrame() {
	if (!m_lstEnabledPostProcess.empty()) {
		list<string>::iterator iter = m_lstEnabledPostProcess.begin();
		CPostProcess* current = m_mapPostProcess[*iter];
		while (++iter != m_lstEnabledPostProcess.end()) {
			CPostProcess* next = m_mapPostProcess[*iter];
			current->Apply(next->m_pRenderTexture, m_pRenderMesh);
			current = next;
		}
		current->Apply(m_pSavedRenderTarget, m_pRenderMesh);
	}
}

/**
* 设置后处理参数，int
*/
void CPostProcessManager::SetProcessParam(const string& name, const string& key, int value) {
	map<string, CPostProcess*>::iterator iter = m_mapPostProcess.find(name);
	if (iter != m_mapPostProcess.end()) {
		CPostProcess* pPostProcess = iter->second;
		pPostProcess->m_pPostProcessShader->UseShader();
		pPostProcess->m_pPostProcessShader->SetUniform(key, value);
	}
}

/**
* 设置后处理参数，float
*/
void CPostProcessManager::SetProcessParam(const string& name, const string& key, float value) {
	map<string, CPostProcess*>::iterator iter = m_mapPostProcess.find(name);
	if (iter != m_mapPostProcess.end()) {
		CPostProcess* pPostProcess = iter->second;
		pPostProcess->m_pPostProcessShader->UseShader();
		pPostProcess->m_pPostProcessShader->SetUniform(key, value);
	}
}

/**
* 设置后处理参数，vec2
*/
void CPostProcessManager::SetProcessParam(const string& name, const string& key, const CVector2& value) {
	map<string, CPostProcess*>::iterator iter = m_mapPostProcess.find(name);
	if (iter != m_mapPostProcess.end()) {
		CPostProcess* pPostProcess = iter->second;
		pPostProcess->m_pPostProcessShader->UseShader();
		pPostProcess->m_pPostProcessShader->SetUniform(key, value);
	}
}

/**
* 设置后处理参数，vec3
*/
void CPostProcessManager::SetProcessParam(const string& name, const string& key, const CVector3& value) {
	map<string, CPostProcess*>::iterator iter = m_mapPostProcess.find(name);
	if (iter != m_mapPostProcess.end()) {
		CPostProcess* pPostProcess = iter->second;
		pPostProcess->m_pPostProcessShader->UseShader();
		pPostProcess->m_pPostProcessShader->SetUniform(key, value);
	}
}

/**
* 设置后处理参数，mat4
*/
void CPostProcessManager::SetProcessParam(const string& name, const string& key, const CMatrix4& value) {
	map<string, CPostProcess*>::iterator iter = m_mapPostProcess.find(name);
	if (iter != m_mapPostProcess.end()) {
		CPostProcess* pPostProcess = iter->second;
		pPostProcess->m_pPostProcessShader->UseShader();
		pPostProcess->m_pPostProcessShader->SetUniform(key, value);
	}
}

/**
* 设置后处理参数，通用形式
*/
void CPostProcessManager::SetProcessParam(const string& name, const string& key, const float value[], int size) {
	map<string, CPostProcess*>::iterator iter = m_mapPostProcess.find(name);
	if (iter != m_mapPostProcess.end()) {
		CPostProcess* pPostProcess = iter->second;
		pPostProcess->m_pPostProcessShader->UseShader();
		pPostProcess->m_pPostProcessShader->SetUniform(key, value, size, 1);
	}
}

/**
* 注册指定的系统内置后处理
*/
bool CPostProcessManager::RegisterDefault(const string& name) {
	if (name == "anaglyph") return Register(name, new CPostProcessAnaglyph());
	else if (name == "beam") return Register(name, new CPostProcessBeam());
	else if (name == "bloom") return Register(name, new CPostProcessBloom());
	else if (name == "blur") return Register(name, new CPostProcessBlur());
	else if (name == "cartoon") return Register(name, new CPostProcessCartoon());
	else if (name == "edge") return Register(name, new CPostProcessEdge());
	else if (name == "emboss") return Register(name, new CPostProcessEmboss());
	else if (name == "fxaa") return Register(name, new CPostProcessFxaa());
	else if (name == "gray") return Register(name, new CPostProcessGray());
	else if (name == "hdr") return Register(name, new CPostProcessHdr());
	else if (name == "oculus") return Register(name, new CPostProcessOculus());
	else if (name == "panorama") return Register(name, new CPostProcessPanorama());
	else if (name == "pass") return Register(name, new CPostProcessPass());
	else if (name == "rain") return Register(name, new CPostProcessRain());
	else if (name == "snow") return Register(name, new CPostProcessSnow());
	else if (name == "ssao") return Register(name, new CPostProcessSsao());
	return false;
}