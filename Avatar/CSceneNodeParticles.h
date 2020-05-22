//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
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
	CSceneNodeParticles(const string& name, const string& texture, float size, int count, bool dark,
		const CColor& color, const float speed[3], float spread, float fade, bool loop);

	//! 初始化场景节点
	virtual bool Init();
	//! 销毁场景节点
	virtual void Destroy();
	//! 渲染场景节点
	virtual void Render();
	//! 更新场景节点
	virtual void Update(float dt);

	//! 初始化粒子
	void InitParticles(const float initSpeed[3], float spreadSpeed, float fadeSpeed);

private:
	//! 随机数生成函数
	inline float Random();

private:
	//! 粒子结构体
	typedef struct _SParticle {
		bool active;
		float fade;
		float color[4];
		float speed[3];
		float position[3];
		float distance;
	} SParticle;

private:
	//! 是否循环
	bool m_bLoopParticles;
	//! 暗混合模式
	bool m_bDarkBlendMode;
	//! 消失速度
	float m_fFadeSpeed;
	//! 粒子射出速度
	float m_fSpreadSpeed;
	//! 粒子初始速度
	float m_fInitSpeed[3];
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
	size_t* m_pSortIndex;
};

#endif