//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CTexture.h"
#include "CTextureManager.h"
#include "AvatarConfig.h"
#include <cstring>

#ifdef AVATAR_WINDOWS
#include "thirdparty/glew/glew.h"
#include <GL/gl.h>
#endif
#ifdef AVATAR_LINUX
#include "thirdparty/glew/glew.h"
#include <GL/gl.h>
#endif
#ifdef AVATAR_ANDROID
#include <GLES3/gl3.h>
#endif

/**
* 构造函数
*/
CTexture::CTexture(CTextureManager* manager) {
	m_pManager = manager;
	m_bIsValid = false;
	m_iTexture = 0;
	m_iChannel = 0;
	m_iWidth = 0;
	m_iHeight = 0;
	m_iTextureType = 0;
	m_iPixelFormat = 0;
	m_iFrameBuffer = 0;
	m_iRenderBuffer = 0;
	m_iPixelBuffer = 0;
	m_iRefCount = 1;
}

/**
* 析构函数
*/
CTexture::~CTexture() {
	if (m_bIsValid) glDeleteTextures(1, &m_iTexture);
	if (m_iFrameBuffer) {
		GLint current;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &current);
		if (current == (GLint)m_iFrameBuffer) glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &m_iFrameBuffer);
	}
	if (m_iRenderBuffer) glDeleteRenderbuffers(1, &m_iRenderBuffer);
	if (m_iPixelBuffer) glDeleteBuffers(1, &m_iPixelBuffer);
}

/**
* 纹理是否有效
*/
bool CTexture::IsValid() const {
	return m_bIsValid;
}

/**
* 纹理是否为立方体贴图
*/
bool CTexture::IsCubeMap() const {
	return m_iTextureType == GL_TEXTURE_CUBE_MAP;
}

/**
* 是否渲染纹理
*/
bool CTexture::IsRenderTarget() const {
	return m_iFrameBuffer != 0;
}

/**
* 是否浮点纹理
*/
bool CTexture::IsFloatType() const {
	return m_iPixelFormat == GL_R32F;
}

/**
* 获取纹理通道
*/
int CTexture::GetChannel() const {
	return m_iChannel;
}

/**
* 获取纹理宽度
*/
int CTexture::GetWidth() const {
	return m_iWidth;
}

/**
* 获取纹理高度
*/
int CTexture::GetHeight() const {
	return m_iHeight;
}

/**
* 获取纹理像素格式
*/
int CTexture::GetPixelFormat() const {
	return m_iPixelFormat;
}

/**
* 获取纹理ID
*/
unsigned int CTexture::GetTextureId() const {
	return m_iTexture;
}

/**
* 获取关联的帧缓存
*/
unsigned int CTexture::GetFramebuffer() const {
	return m_iFrameBuffer;
}

/**
* 获取纹理图片路径
*/
const string& CTexture::GetFilePath() const {
	return m_strFilePath;
}

/**
* 设置纹理边界模式为重复
*/
void CTexture::SetWrapModeRepeat(bool s, bool t) const {
	if (m_bIsValid) {
		glBindTexture(m_iTextureType, m_iTexture);
		if (s) glTexParameteri(m_iTextureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
		if (t) glTexParameteri(m_iTextureType, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
}

/**
* 设置纹理边界模式为边缘拉伸填充，纹理坐标超出1的使用边界像素填充
*/
void CTexture::SetWrapModeClampToEdge(bool s, bool t) const {
	if (m_bIsValid) {
		glBindTexture(m_iTextureType, m_iTexture);
		if (s) glTexParameteri(m_iTextureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		if (t) glTexParameteri(m_iTextureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
}

/**
* 设置纹理边界模式为镜像重复
*/
void CTexture::SetWrapModeMirroredRepeat(bool s, bool t) const {
	if (m_bIsValid) {
		glBindTexture(m_iTextureType, m_iTexture);
		if (s) glTexParameteri(m_iTextureType, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		if (t) glTexParameteri(m_iTextureType, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	}
}

/**
* 开始映射纹理数据，返回纹理数据缓冲区指针
*/
void* CTexture::MapBuffer(bool clear) {
	if (!m_bIsValid || m_iTextureType == GL_TEXTURE_CUBE_MAP) return 0;
	int bufferSize = m_iWidth * m_iHeight * m_iChannel;
	// 创建 PBO
	if (!m_iPixelBuffer) {
		glGenBuffers(1, &m_iPixelBuffer);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_iPixelBuffer);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, bufferSize, 0, GL_STREAM_DRAW);
	}
	// 更新纹理
	glBindTexture(GL_TEXTURE_2D, m_iTexture);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_iPixelBuffer);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_iWidth, m_iHeight, m_iPixelFormat, GL_UNSIGNED_BYTE, 0);
	// 映射纹理内存
	void* pBuffer = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, bufferSize, GL_MAP_WRITE_BIT);
	if (clear) memset(pBuffer, 0, bufferSize);
	return pBuffer;
}

/**
* 结束映射纹理数据
*/
void CTexture::UnmapBuffer() {
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

/**
* 使用纹理
*/
void CTexture::UseTexture() {
	if (m_pManager->m_pCurrentTexture != this) {
		glBindTexture(m_iTextureType, m_iTexture);
		m_pManager->m_pCurrentTexture = this;
	}
}

/**
* 使用纹理，纹理单元从高至低使用
*/
void CTexture::UseTexture(int unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(m_iTextureType, m_iTexture);
	m_pManager->m_pCurrentTexture = this;
}

/**
* 增加引用
*/
CTexture* CTexture::AddReference() {
	m_iRefCount += 1;
	return this;
}