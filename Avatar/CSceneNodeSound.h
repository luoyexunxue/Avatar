//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODESOUND_H_
#define _CSCENENODESOUND_H_
#include "CSceneNode.h"

/**
* @brief 声源场景节点
*
* 不可见模型，模拟声源，只能作为其他场景节点的子节点使用
*/
class AVATAR_EXPORT CSceneNodeSound: public CSceneNode {
public:
	//! 构造方法
	CSceneNodeSound(const string& name, const string& soundFile, bool loop, bool start);

	//! 初始化场景节点
	virtual bool Init();
	//! 销毁场景节点
	virtual void Destroy();
	//! 渲染场景节点
	virtual void Render();
	//! 更新逻辑
	virtual void Update(float dt);

	//! 启动或停止播放
	void Play(bool enable);

private:
	//! 音源ID
	int m_iSoundId;
	//! 声音文件
	string m_strSoundFile;
	//! 循环播放
	bool m_bLoopPlay;
	//! 自动播放
	bool m_bAutoPlay;
};

#endif