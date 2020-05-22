//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* ���캯��
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
* ��������
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
* �����Ƿ���Ч
*/
bool CTexture::IsValid() const {
	return m_bIsValid;
}

/**
* �����Ƿ�Ϊ��������ͼ
*/
bool CTexture::IsCubeMap() const {
	return m_iTextureType == GL_TEXTURE_CUBE_MAP;
}

/**
* �Ƿ���Ⱦ����
*/
bool CTexture::IsRenderTarget() const {
	return m_iFrameBuffer != 0;
}

/**
* �Ƿ񸡵�����
*/
bool CTexture::IsFloatType() const {
	return m_iPixelFormat == GL_R32F;
}

/**
* ��ȡ����ͨ��
*/
int CTexture::GetChannel() const {
	return m_iChannel;
}

/**
* ��ȡ������
*/
int CTexture::GetWidth() const {
	return m_iWidth;
}

/**
* ��ȡ����߶�
*/
int CTexture::GetHeight() const {
	return m_iHeight;
}

/**
* ��ȡ�������ظ�ʽ
*/
int CTexture::GetPixelFormat() const {
	return m_iPixelFormat;
}

/**
* ��ȡ����ID
*/
unsigned int CTexture::GetTextureId() const {
	return m_iTexture;
}

/**
* ��ȡ������֡����
*/
unsigned int CTexture::GetFramebuffer() const {
	return m_iFrameBuffer;
}

/**
* ��ȡ����ͼƬ·��
*/
const string& CTexture::GetFilePath() const {
	return m_strFilePath;
}

/**
* ��������߽�ģʽΪ�ظ�
*/
void CTexture::SetWrapModeRepeat(bool s, bool t) const {
	if (m_bIsValid) {
		glBindTexture(m_iTextureType, m_iTexture);
		if (s) glTexParameteri(m_iTextureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
		if (t) glTexParameteri(m_iTextureType, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
}

/**
* ��������߽�ģʽΪ��Ե������䣬�������곬��1��ʹ�ñ߽��������
*/
void CTexture::SetWrapModeClampToEdge(bool s, bool t) const {
	if (m_bIsValid) {
		glBindTexture(m_iTextureType, m_iTexture);
		if (s) glTexParameteri(m_iTextureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		if (t) glTexParameteri(m_iTextureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
}

/**
* ��������߽�ģʽΪ�����ظ�
*/
void CTexture::SetWrapModeMirroredRepeat(bool s, bool t) const {
	if (m_bIsValid) {
		glBindTexture(m_iTextureType, m_iTexture);
		if (s) glTexParameteri(m_iTextureType, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		if (t) glTexParameteri(m_iTextureType, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	}
}

/**
* ��ʼӳ���������ݣ������������ݻ�����ָ��
*/
void* CTexture::MapBuffer(bool clear) {
	if (!m_bIsValid || m_iTextureType == GL_TEXTURE_CUBE_MAP) return 0;
	int bufferSize = m_iWidth * m_iHeight * m_iChannel;
	// ���� PBO
	if (!m_iPixelBuffer) {
		glGenBuffers(1, &m_iPixelBuffer);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_iPixelBuffer);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, bufferSize, 0, GL_STREAM_DRAW);
	}
	// ��������
	glBindTexture(GL_TEXTURE_2D, m_iTexture);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_iPixelBuffer);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_iWidth, m_iHeight, m_iPixelFormat, GL_UNSIGNED_BYTE, 0);
	// ӳ�������ڴ�
	void* pBuffer = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, bufferSize, GL_MAP_WRITE_BIT);
	if (clear) memset(pBuffer, 0, bufferSize);
	return pBuffer;
}

/**
* ����ӳ����������
*/
void CTexture::UnmapBuffer() {
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

/**
* ʹ������
*/
void CTexture::UseTexture() {
	if (m_pManager->m_pCurrentTexture != this) {
		glBindTexture(m_iTextureType, m_iTexture);
		m_pManager->m_pCurrentTexture = this;
	}
}

/**
* ʹ����������Ԫ�Ӹ�����ʹ��
*/
void CTexture::UseTexture(int unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(m_iTextureType, m_iTexture);
	m_pManager->m_pCurrentTexture = this;
}

/**
* ��������
*/
CTexture* CTexture::AddReference() {
	m_iRefCount += 1;
	return this;
}