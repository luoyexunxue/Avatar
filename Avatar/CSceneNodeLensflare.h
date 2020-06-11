//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODELENSFLARE_H_
#define _CSCENENODELENSFLARE_H_
#include "CSceneNode.h"

/**
* @brief 镜头光晕场景节点
*/
class AVATAR_EXPORT CSceneNodeLensflare: public CSceneNode {
public:
	//! 构造方法
	CSceneNodeLensflare(const string& name);

	//! 初始化场景节点
	virtual bool Init();
	//! 销毁场景节点
	virtual void Destroy();
	//! 渲染场景节点
	virtual void Render();
	//! 更新场景节点
	virtual void Update(float dt);
	//! 重载坐标变换
	virtual void Transform() {}

private:
	//! 光晕结构体
	typedef struct _SLensFlare {
		float offset;
		float size;
		float color[4];
		CTexture* texture;
		/**
		* 设置光晕参数
		*/
		void SetParameter(float offset, float size, float r, float g, float b, float a, CTexture* texture) {
			this->offset = offset;
			this->size = size;
			this->color[0] = r;
			this->color[1] = g;
			this->color[2] = b;
			this->color[3] = a;
			this->texture = texture;
		}
	} SLensFlare;

private:
	//! 光晕列表
	SLensFlare m_sLensFlares[6];
	//! 太阳纹理
	CTexture* m_pTextureFlare;
	//! 光晕纹理1
	CTexture* m_pTextureGlow;
	//! 光晕纹理2
	CTexture* m_pTextureHalo;
	//! 光晕渲染网格
	CMesh* m_pMesh;
	//! 太阳的位置
	CVector3 m_cSunPosition;
	//! 屏幕中心位置
	CVector2 m_cCenterPos;
	//! 太阳在屏幕中位置
	CVector3 m_cScreenPos;
};

#endif