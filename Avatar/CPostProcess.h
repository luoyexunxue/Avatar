//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESS_H_
#define _CPOSTPROCESS_H_
#include "CTexture.h"
#include "CShader.h"
#include "CMesh.h"

/**
* @brief 后处理抽象类
*/
class CPostProcess {
public:
	//! 默认构造函数
	CPostProcess();
	//! 默认虚析构函数
	virtual ~CPostProcess() {}

	//! 初始化后处理对象
	virtual bool Init(int width, int height) = 0;
	//! 渲染区域大小改变
	virtual void Resize(int width, int height) = 0;
	//! 销毁后处理对象
	virtual void Destroy() = 0;
	//! 应用当前后处理
	virtual void Apply(CTexture* target, CMesh* mesh);
	//! 启用或禁用后处理
	virtual void Enable(bool enable) {};

protected:
	//! 获取顶点着色器代码
	const char* GetVertexShader();

protected:
	//! 后处理着色器
	CShader* m_pPostProcessShader;
	//! 后处理渲染纹理
	CTexture* m_pRenderTexture;

private:
	//! 保存的后处理屏幕大小
	int m_iScreenSize[2];
	//! 后处理管理器可以访问
	friend class CPostProcessManager;
};

#endif