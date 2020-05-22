//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODESOUND_H_
#define _CSCENENODESOUND_H_
#include "CSceneNode.h"

/**
* @brief ��Դ�����ڵ�
*
* ���ɼ�ģ�ͣ�ģ����Դ��ֻ����Ϊ���������ڵ���ӽڵ�ʹ��
*/
class AVATAR_EXPORT CSceneNodeSound: public CSceneNode {
public:
	//! ���췽��
	CSceneNodeSound(const string& name, const string& soundFile, bool loop, bool start);

	//! ��ʼ�������ڵ�
	virtual bool Init();
	//! ���ٳ����ڵ�
	virtual void Destroy();
	//! ��Ⱦ�����ڵ�
	virtual void Render();
	//! �����߼�
	virtual void Update(float dt);

	//! ������ֹͣ����
	void Play(bool enable);

private:
	//! ��ԴID
	int m_iSoundId;
	//! �����ļ�
	string m_strSoundFile;
	//! ѭ������
	bool m_bLoopPlay;
	//! �Զ�����
	bool m_bAutoPlay;
};

#endif