//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeSound.h"
#include "AvatarConfig.h"
#include "CEngine.h"

/**
* 构造函数
*/
CSceneNodeSound::CSceneNodeSound(const string& name, const string& soundFile, bool loop, bool start)
	: CSceneNode("sound", name) {
	m_strSoundFile = soundFile;
	m_bLoopPlay = loop;
	m_bAutoPlay = start;
	m_bVisible = false;
}

/**
* 初始化场景节点
*/
bool CSceneNodeSound::Init() {
	CSoundManager* pSoundMgr = CEngine::GetSoundManager();
	m_iSoundId = pSoundMgr->Create(m_strSoundFile);
	pSoundMgr->SetAttrib(m_iSoundId, m_bLoopPlay);
	if (m_bAutoPlay) pSoundMgr->Play(m_iSoundId);
	return pSoundMgr->IsValid(m_iSoundId);
}

/**
* 销毁场景节点
*/
void CSceneNodeSound::Destroy() {
	CSoundManager* pSoundMgr = CEngine::GetSoundManager();
	pSoundMgr->Stop(m_iSoundId);
	pSoundMgr->Drop(m_iSoundId);
}

/**
* 渲染场景节点
*/
void CSceneNodeSound::Render() {
}

/**
* 逻辑更新
*/
void CSceneNodeSound::Update(float dt) {
	CEngine::GetSoundManager()->SetPosition(m_iSoundId, m_pParent->GetWorldPosition());
}

/**
* 启动或停止播放
*/
void CSceneNodeSound::Play(bool enable) {
	if (enable) CEngine::GetSoundManager()->Play(m_iSoundId);
	else CEngine::GetSoundManager()->Stop(m_iSoundId);
}