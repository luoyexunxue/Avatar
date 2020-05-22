//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CTEXTURE_H_
#define _CTEXTURE_H_
#include "AvatarConfig.h"
#include <string>
using std::string;

/**
* @brief 纹理类
*/
class AVATAR_EXPORT CTexture {
public:
	//! 纹理是否有效
	bool IsValid() const;
	//! 纹理是否为立方体贴图
	bool IsCubeMap() const;
	//! 是否渲染纹理
	bool IsRenderTarget() const;
	//! 是否浮点纹理
	bool IsFloatType() const;
	//! 获取纹理通道
	int GetChannel() const;
	//! 获取纹理宽度
	int GetWidth() const;
	//! 获取纹理高度
	int GetHeight() const;
	//! 获取纹理像素格式
	int GetPixelFormat() const;
	//! 获取纹理ID
	unsigned int GetTextureId() const;
	//! 获取关联的帧缓存
	unsigned int GetFramebuffer() const;
	//! 获取纹理图片路径
	const string& GetFilePath() const;

	//! 设置纹理模式为重复
	void SetWrapModeRepeat(bool s, bool t) const;
	//! 设置纹理模式为限制到边界
	void SetWrapModeClampToEdge(bool s, bool t) const;
	//! 设置纹理模式为镜像重复
	void SetWrapModeMirroredRepeat(bool s, bool t) const;

	//! 开始映射纹理数据
	void* MapBuffer(bool clear);
	//! 结束映射纹理数据
	void UnmapBuffer();

	//! 使用纹理
	void UseTexture();
	//! 使用纹理至指定纹理单元
	void UseTexture(int unit);
	//! 增加引用
	CTexture* AddReference();

protected:
	//! 受保护构造函数，只能通过 CTextureManager 创建
	CTexture(class CTextureManager* manager);
	//! 受保护析构函数
	~CTexture();

private:
	bool m_bIsValid;
	string m_strFilePath;
	unsigned int m_iTexture;

	int m_iChannel;
	int m_iWidth;
	int m_iHeight;
	int m_iPixelFormat;

	unsigned int m_iTextureType;
	unsigned int m_iFrameBuffer;
	unsigned int m_iRenderBuffer;
	unsigned int m_iPixelBuffer;

	int m_iRefCount;
	class CTextureManager* m_pManager;
	friend class CTextureManager;
};

#endif