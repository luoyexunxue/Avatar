//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEBLAST_H_
#define _CSCENENODEBLAST_H_
#include "CSceneNode.h"

/**
* @brief 爆炸模拟场景节点
*/
class AVATAR_EXPORT CSceneNodeBlast: public CSceneNode {
public:
	//! 构造方法
	CSceneNodeBlast(const string& name, const string& texture, int row, int column);

	//! 初始化场景节点
	virtual bool Init();
	//! 销毁场景节点
	virtual void Destroy();
	//! 渲染场景节点
	virtual void Render();
	//! 模拟更新
	virtual void Update(float dt);

	//! 爆炸初始化
	void InitBlast(float duration, float size);
	//! 判断是否结束
	bool IsFinished() { return !m_bBlast; }

private:
	//! 爆炸模拟
	bool m_bBlast;
	//! 爆炸纹理
	string m_strTexture;
	//! 模拟时间
	float m_fTimeElapse;
	//! 持续时间
	float m_fDuration;
	//! 纵向帧
	int m_iFrameRow;
	//! 横向帧
	int m_iFrameCol;
	//! 总行数
	int m_iTotalRow;
	//! 总列数
	int m_iTotalCol;
	//! 网格对象
	CMesh* m_pMesh;
};

#endif