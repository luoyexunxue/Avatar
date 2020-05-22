//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* ���캯��
*/
CPostProcessManager::CPostProcessManager() {
	m_iScreenSize[0] = 1;
	m_iScreenSize[1] = 1;
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
* ��������
*/
CPostProcessManager::~CPostProcessManager() {
	delete m_pRenderMesh;
	m_pInstance = 0;
}

/**
* ����ʵ��
*/
CPostProcessManager* CPostProcessManager::m_pInstance = 0;

/**
* ʵ�����������
*/
void CPostProcessManager::Destroy() {
	map<string, CPostProcess*>::iterator iter = m_mapPostProcess.begin();
	while (iter != m_mapPostProcess.end()) {
		iter->second->Destroy();
		delete iter->second;
		++iter;
	}
	delete this;
}

/**
* ע��������
*/
bool CPostProcessManager::Register(const string& name, CPostProcess* postProcess) {
	map<string, CPostProcess*>::iterator iter = m_mapPostProcess.find(name);
	if (iter != m_mapPostProcess.end()) {
		// ɾ����ע���ͬ���ƺ���
		list<CPostProcess*>::iterator listIter = m_lstEnabledPostProcess.begin();
		while (listIter != m_lstEnabledPostProcess.end()) {
			if (*listIter == iter->second) {
				m_lstEnabledPostProcess.erase(listIter);
				break;
			}
			++listIter;
		}
		iter->second->Destroy();
		delete iter->second;
		m_mapPostProcess.erase(iter);
	}
	int width, height;
	CEngine::GetGraphicsManager()->GetWindowSize(&width, &height);
	if (!postProcess->Init(width, height)) {
		postProcess->Destroy();
		delete postProcess;
		return false;
	}
	postProcess->m_iSavedScreenSize[0] = width;
	postProcess->m_iSavedScreenSize[1] = height;
	m_mapPostProcess.insert(std::pair<string, CPostProcess*>(name, postProcess));
	return true;
}

/**
* ���ӻ�ȡ��ָ����ͼ�����
*/
bool CPostProcessManager::Enable(const string& name, bool enable) {
	map<string, CPostProcess*>::iterator iter = m_mapPostProcess.find(name);
	if (iter == m_mapPostProcess.end()) {
		if (!RegisterDefault(name)) return false;
		iter = m_mapPostProcess.find(name);
	}
	if (iter != m_mapPostProcess.end()) {
		list<CPostProcess*>::iterator listIter = m_lstEnabledPostProcess.begin();
		while (listIter != m_lstEnabledPostProcess.end()) {
			if (*listIter == iter->second) {
				if (!enable) m_lstEnabledPostProcess.erase(listIter);
				return true;
			}
			++listIter;
		}
		if (enable) m_lstEnabledPostProcess.push_back(iter->second);
		return true;
	}
	return false;
}

/**
* ָ���ĺ����Ƿ�����
*/
bool CPostProcessManager::IsEnabled(const string& name) {
	map<string, CPostProcess*>::iterator iter = m_mapPostProcess.find(name);
	if (iter != m_mapPostProcess.end()) {
		list<CPostProcess*>::iterator listIter = m_lstEnabledPostProcess.begin();
		while (listIter != m_lstEnabledPostProcess.end()) {
			if (*listIter == iter->second) {
				return true;
			}
			++listIter;
		}
	}
	return false;
}

/**
* ��ȡ��ע��ĺ����б�
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
* ׼������
*/
bool CPostProcessManager::PrepareFrame(CTexture* renderTarget) {
	if (m_lstEnabledPostProcess.empty()) return false;
	// ֪ͨ��Ļ��С
	list<CPostProcess*>::iterator iter = m_lstEnabledPostProcess.begin();
	while (iter != m_lstEnabledPostProcess.end()) {
		CPostProcess* item = *iter++;
		if (item->m_iSavedScreenSize[0] != m_iScreenSize[0] || item->m_iSavedScreenSize[1] != m_iScreenSize[1]) {
			item->Resize(m_iScreenSize[0], m_iScreenSize[1]);
			item->m_iSavedScreenSize[0] = m_iScreenSize[0];
			item->m_iSavedScreenSize[1] = m_iScreenSize[1];
		}
	}
	// ����һ����������Ϊ��ȾĿ��
	m_pSavedRenderTarget = renderTarget;
	renderTarget = m_lstEnabledPostProcess.front()->m_pRenderTexture;
	CEngine::GetGraphicsManager()->SetRenderTarget(renderTarget, 0, true, true, true);
	return true;
}

/**
* ���к���
*/
void CPostProcessManager::ApplyFrame() {
	if (!m_lstEnabledPostProcess.empty()) {
		// ������к�����һ��������������Ϊǰһ������ȾĿ��
		list<CPostProcess*>::iterator iter = m_lstEnabledPostProcess.begin();
		CPostProcess* current = *iter;
		while (++iter != m_lstEnabledPostProcess.end()) {
			CPostProcess* next = *iter;
			current->Apply(next->m_pRenderTexture, m_pRenderMesh);
			current = next;
		}
		current->Apply(m_pSavedRenderTarget, m_pRenderMesh);
	}
}

/**
* ���º���ͼƬ��С
*/
void CPostProcessManager::UpdateSize(int width, int height) {
	m_iScreenSize[0] = width;
	m_iScreenSize[1] = height;
}

/**
* ���ú��������int
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
* ���ú��������float
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
* ���ú��������vec2
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
* ���ú��������vec3
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
* ���ú��������mat4
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
* ���ú��������ͨ����ʽ
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
* ע��ָ����ϵͳ���ú���
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