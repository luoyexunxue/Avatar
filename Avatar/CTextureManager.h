//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CTEXTUREMANAGER_H_
#define _CTEXTUREMANAGER_H_
#include "AvatarConfig.h"
#include "CTexture.h"
#include "CShader.h"
#include "CFileManager.h"
#include "CRectangle.h"
#include "CVertex.h"
#include "CColor.h"
#include <string>
#include <vector>
#include <map>
using std::string;
using std::vector;
using std::map;

/**
* @brief 纹理资源管理器
*/
class AVATAR_EXPORT CTextureManager {
public:
	//! 获取管理器实例
	static CTextureManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CTextureManager();
		return m_pInstance;
	}

	//! 实例销毁，释放所有纹理资源
	void Destroy();

	//! 从文件创建纹理
	CTexture* Create(const string& file);
	//! 从文件创建立方体纹理
	CTexture* Create(const string& name, const string files[6]);
	//! 从内存创建纹理
	CTexture* Create(const string& name, int width, int height, int channel, const void* data, bool mipmap);
	//! 创建渲染纹理
	CTexture* Create(const string& name, int width, int height, bool floating, bool useDepth, bool mipmap);
	//! 创建立方体渲染纹理
	CTexture* Create(const string& name, int size, bool floating, bool useDepth, bool mipmap);
	//! 创建程序纹理
	CTexture* Create(const string& name, int width, int height, const string& shader);

	//! 释放指定的纹理资源
	void Drop(CTexture* texture);
	//! 更新所有程序纹理
	bool Update();
	//! 更新纹理图片
	bool Update(CTexture* texture, const string& file);
	//! 更新纹理数据
	bool Update(CTexture* texture, const unsigned char* data);
	//! 更新局部纹理数据
	bool Update(CTexture* texture, const unsigned char* data, const CRectangle& rect);
	//! 更新局部纹理颜色
	bool Update(CTexture* texture, const CColor& color, const CRectangle& rect);
	//! 通过着色器更新
	bool Update(CTexture* texture, const string& input, const string& shader, int level);
	//! 更新纹理大小
	void Resize(CTexture* texture, int width, int height);
	//! 读取纹理数据至RGBA数组
	void ReadData(CTexture* texture, int face, int level, void* buffer);

	//! 获取指定名称的纹理
	CTexture* GetTexture(const string& name);
	//! 获取当前使用的纹理
	CTexture* GetCurrentTexture();
	//! 获取管理的所有纹理列表
	void GetTextureList(vector<CTexture*>& textureList);

public:
	//! 读取图片
	static CFileManager::CImageFile* ReadImage(const string& file, bool resize);
	//! 旋转图片
	static void RotateImage(CFileManager::CImageFile* image, int angle);
	//! 镜像图片
	static void MirrorImage(CFileManager::CImageFile* image, bool horizontal);
	//! 缩放图片
	static void ScaleImage(CFileManager::CImageFile* image, int width, int height);

private:
	CTextureManager();
	~CTextureManager();

	//! 加载预定义纹理
	void LoadDefaultTexture();

private:
	//! 程序纹理定义
	typedef struct _SProceduralTexture {
		CTexture* texture;
		CShader* shader;
		bool update;
		bool dynamic;
	} SProceduralTexture;

private:
	//! 空渲染纹理
	unsigned int m_iDummyFrameBuffer;
	//! 当前纹理
	CTexture* m_pCurrentTexture;
	//! 纹理集合
	map<string, CTexture*> m_mapTexture;
	//! 渲染程序纹理使用的顶点
	vector<CVertex> m_vecProceduralVertices;
	//! 程序纹理集合
	map<string, SProceduralTexture> m_mapProceduralTexture;
	//! 实例
	static CTextureManager* m_pInstance;
	//! 设置友元类
	friend class CTexture;
};

#endif