//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeSound.h"
#include "AvatarConfig.h"
#include "CEngine.h"

/**
* ���캯��
*/
CSceneNodeSound::CSceneNodeSound(const string& name, const string& soundFile, bool loop, bool start)
	: CSceneNode("sound", name) {
	m_strSoundFile = soundFile;
	m_bLoopPlay = loop;
	m_bAutoPlay = start;
	m_bVisible = false;
}

/**
* ��ʼ�������ڵ�
*/
bool CSceneNodeSound::Init() {
	CSoundManager* pSoundMgr = CEngine::GetSoundManager();
	m_iSoundId = pSoundMgr->Create(m_strSoundFile);
	pSoundMgr->SetAttrib(m_iSoundId, m_bLoopPlay);
	if (m_bAutoPlay) pSoundMgr->Play(m_iSoundId);
	return pSoundMgr->IsValid(m_iSoundId);
}

/**
* ���ٳ����ڵ�
*/
void CSceneNodeSound::Destroy() {
	CSoundManager* pSoundMgr = CEngine::GetSoundManager();
	pSoundMgr->Stop(m_iSoundId);
	pSoundMgr->Drop(m_iSoundId);
}

/**
* ��Ⱦ�����ڵ�
*/
void CSceneNodeSound::Render() {
}

/**
* �߼�����
*/
void CSceneNodeSound::Update(float dt) {
	CEngine::GetSoundManager()->SetPosition(m_iSoundId, m_pParent->GetWorldPosition());
}

/**
* ������ֹͣ����
*/
void CSceneNodeSound::Play(bool enable) {
	if (enable) CEngine::GetSoundManager()->Play(m_iSoundId);
	else CEngine::GetSoundManager()->Stop(m_iSoundId);
}