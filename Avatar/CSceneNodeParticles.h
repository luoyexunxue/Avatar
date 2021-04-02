//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEPARTICLES_H_
#define _CSCENENODEPARTICLES_H_
#include "CSceneNode.h"
#include <vector>
using std::vector;

/**
* @brief 粒子群场景节点
*/
class AVATAR_EXPORT CSceneNodeParticles: public CSceneNode {
public:
	//! 构造方法
	CSceneNodeParticles(const string& name, const string& texture, const CColor& color,
		float size, bool dark, int count, bool loop);

	//! 初始化场景节点
	virtual bool Init();
	//! 销毁场景节点
	virtual void Destroy();
	//! 渲染场景节点
	virtual void Render();
	//! 更新场景节点
	virtual void Update(float dt);

	//! 初始化粒子
	void SetupSpeed(const CVector3& emit, float spread, float fade);
	//! 设置是否受重力加速度影响
	void SetGravityEffect(bool enable);

private:
	//! 随机数生成函数
	inline float Random();

private:
	//! 粒子结构体
	typedef struct _SParticle {
		bool active;
		float fade;
		float scale;
		float color[4];
		float speed[3];
		float position[3];
		float distance;
	} SParticle;

	//! 是否循环
	bool m_bLoop;
	//! 暗混合模式
	bool m_bDarkBlend;
	//! 重力作用
	bool m_bGravityEffect;
	//! 消失速度
	float m_fFadeSpeed;
	//! 粒子扩散速度
	float m_fSpreadSpeed;
	//! 粒子射出速度
	float m_fEmitSpeed[3];
	//! 粒子大小
	float m_fParticleSize;
	//! 粒子初始颜色
	float m_fParticleColor[4];
	//! 粒子纹理
	string m_strTexture;
	//! 粒子群网格对象
	CMesh* m_pMesh;
	//! 粒子数组
	vector<SParticle> m_vecParticles;
	//! 粒子排序索引
	vector<size_t> m_vecSortIndex;
};

#endif