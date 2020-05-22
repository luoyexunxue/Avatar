//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESSMANAGER_H_
#define _CPOSTPROCESSMANAGER_H_
#include "AvatarConfig.h"
#include "CPostProcess.h"
#include "CTexture.h"
#include "CMesh.h"
#include <string>
#include <vector>
#include <map>
#include <list>
using std::string;
using std::vector;
using std::map;
using std::list;

/**
* @brief 图像后处理管理器
*/
class AVATAR_EXPORT CPostProcessManager {
public:
	//! 获取管理器实例
	static CPostProcessManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CPostProcessManager();
		return m_pInstance;
	}
	//! 实例销毁
	void Destroy();

	//! 注册后处理
	bool Register(const string& name, CPostProcess* postProcess);
	//! 附加或取消指定的图像后处理
	bool Enable(const string& name, bool enable);
	//! 指定的后处理是否启用
	bool IsEnabled(const string& name);
	//! 获取已注册的后处理列表
	void GetPostProcessList(vector<string>& postList);

	//! 准备后处理
	bool PrepareFrame(CTexture* renderTarget);
	//! 进行后处理
	void ApplyFrame();
	//! 更新后处理图片大小
	void UpdateSize(int width, int height);

	//! 设置后处理参数 int
	void SetProcessParam(const string& name, const string& key, int value);
	//! 设置后处理参数 float
	void SetProcessParam(const string& name, const string& key, float value);
	//! 设置后处理参数 vec2
	void SetProcessParam(const string& name, const string& key, const CVector2& value);
	//! 设置后处理参数 vec3
	void SetProcessParam(const string& name, const string& key, const CVector3& value);
	//! 设置后处理参数 mat4
	void SetProcessParam(const string& name, const string& key, const CMatrix4& value);
	//! 设置后处理参数 float[]
	void SetProcessParam(const string& name, const string& key, const float value[], int size);

private:
	CPostProcessManager();
	~CPostProcessManager();

	//! 注册内置后处理
	bool RegisterDefault(const string& name);

private:
	//! 后处理屏幕大小
	int m_iScreenSize[2];
	//! 保存的源渲染目标
	CTexture* m_pSavedRenderTarget;
	//! 后处理列表
	map<string, CPostProcess*> m_mapPostProcess;
	//! 启用的后处理
	list<CPostProcess*> m_lstEnabledPostProcess;
	//! 用于后处理渲染的网格对象
	CMesh* m_pRenderMesh;

	//! 实例
	static CPostProcessManager* m_pInstance;
};

#endif