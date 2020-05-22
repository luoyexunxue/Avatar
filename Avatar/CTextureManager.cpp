//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CTextureManager.h"
#include "AvatarConfig.h"
#include "AvatarLogo.h"
#include "CEngine.h"
#include "CTimer.h"
#include "CStringUtil.h"
#include "CLog.h"
#include <cstring>
#include <algorithm>

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
CTextureManager::CTextureManager() {
	m_iDummyFrameBuffer = 0;
	m_pCurrentTexture = 0;
	m_vecProceduralVertices.push_back(CVertex(-1.0f, -1.0f, 0.0f, 0.0f, 1.0f));
	m_vecProceduralVertices.push_back(CVertex(1.0f, -1.0f, 0.0f, 1.0f, 1.0f));
	m_vecProceduralVertices.push_back(CVertex(-1.0f, 1.0f, 0.0f, 0.0f, 0.0f));
	m_vecProceduralVertices.push_back(CVertex(1.0f, 1.0f, 0.0f, 1.0f, 0.0f));
	LoadDefaultTexture();
}

/**
* ��������
*/
CTextureManager::~CTextureManager() {
	m_pInstance = 0;
}

/**
* ����ʵ��
*/
CTextureManager* CTextureManager::m_pInstance = 0;

/**
* �������������
*/
void CTextureManager::Destroy() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (m_iDummyFrameBuffer) glDeleteFramebuffers(1, &m_iDummyFrameBuffer);
	map<string, CTexture*>::iterator iter = m_mapTexture.begin();
	while (iter != m_mapTexture.end()) {
		delete iter->second;
		++iter;
	}
	delete this;
}

/**
* ���ļ���������
* @param file ͼƬ�ļ�·��
* @return ����������
*/
CTexture* CTextureManager::Create(const string& file) {
	// ����Ƿ��Ѿ�������ͬ��������Դ
	map<string, CTexture*>::iterator iter = m_mapTexture.find(file);
	if (iter != m_mapTexture.end()) {
		iter->second->m_iRefCount++;
		return iter->second;
	}
	CTexture* pTexture = new CTexture(this);
	pTexture->m_iTextureType = GL_TEXTURE_2D;
	pTexture->m_strFilePath = file;
	m_mapTexture.insert(std::pair<string, CTexture*>(file, pTexture));

	// ��ȡͼƬ�ļ�
	CFileManager::CImageFile* pFile = ReadImage(file, true);
	if (!pFile) return pTexture;
	// ������Ϣ
	pTexture->m_iChannel = pFile->channels;
	pTexture->m_iWidth = pFile->width;
	pTexture->m_iHeight = pFile->height;
	// ֧��1,2,3,4ͨ����λ���ͼƬ
	if (pTexture->m_iChannel >= 1 && pTexture->m_iChannel <= 4) {
		glGenTextures(1, &pTexture->m_iTexture);
		glBindTexture(GL_TEXTURE_2D, pTexture->m_iTexture);
		GLint pixelFormat;
		if (pTexture->m_iChannel == 1) pixelFormat = GL_LUMINANCE;
		else if (pTexture->m_iChannel == 2) pixelFormat = GL_LUMINANCE_ALPHA;
		else if (pTexture->m_iChannel == 3) pixelFormat = GL_RGB;
		else pixelFormat = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, pTexture->m_iWidth, pTexture->m_iHeight, 0, pixelFormat, GL_UNSIGNED_BYTE, pFile->contents);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		pTexture->m_bIsValid = true;
		pTexture->m_iPixelFormat = pixelFormat;
	}
	delete pFile;
	return pTexture;
}

/**
* ���ļ���������������
* @param name �������ƣ���Ϊ��
* @param files R-L-F-B-U-DͼƬ�ļ�·��
* @return ����������
*/
CTexture* CTextureManager::Create(const string& name, const string files[6]) {
	string file = name;
	if (file.empty()) {
		file.append(files[0]).append("_").append(files[1]).append("_");
		file.append(files[2]).append("_").append(files[3]).append("_");
		file.append(files[4]).append("_").append(files[5]);
	}
	map<string, CTexture*>::iterator iter = m_mapTexture.find(file);
	if (iter != m_mapTexture.end()) {
		iter->second->m_iRefCount++;
		return iter->second;
	}
	CTexture* pTexture = new CTexture(this);
	pTexture->m_iTextureType = GL_TEXTURE_CUBE_MAP;
	pTexture->m_strFilePath = file;
	m_mapTexture.insert(std::pair<string, CTexture*>(file, pTexture));
	// ��������ϵ(Z-UP)�� OpenGL ����ϵ(Y-UP)��һ�£���Ҫ��תͼƬ
	static const int angle[6] = { 270, 90, 180, 0, 0, 180 };
	CFileManager::CImageFile* pFile[6];
	// ��ȡͼƬ�ļ�
	for (int i = 0; i < 6; i++) {
		pFile[i] = ReadImage(files[i], true);
		if (!pFile[i] || pFile[i]->channels < 1 || pFile[i]->channels > 4) {
			while (i--) delete pFile[i];
			return pTexture;
		}
		RotateImage(pFile[i], angle[i]);
	}
	pTexture->m_iChannel = pFile[5]->channels;
	pTexture->m_iWidth = pFile[5]->width;
	pTexture->m_iHeight = pFile[5]->height;
	pTexture->m_bIsValid = true;
	glGenTextures(1, &pTexture->m_iTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, pTexture->m_iTexture);
	GLint pixelFormat;
	for (int i = 0; i < 6; i++) {
		if (pFile[i]->channels == 1) pixelFormat = GL_LUMINANCE;
		else if (pFile[i]->channels == 2) pixelFormat = GL_LUMINANCE_ALPHA;
		else if (pFile[i]->channels == 3) pixelFormat = GL_RGB;
		else pixelFormat = GL_RGBA;
		GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
		glTexImage2D(face, 0, pixelFormat, pFile[i]->width, pFile[i]->height, 0, pixelFormat, GL_UNSIGNED_BYTE, pFile[i]->contents);
		delete pFile[i];
	}
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	// ��ֹ�����Ӿ��ɼ����ѷ�
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	pTexture->m_iPixelFormat = pixelFormat;
	return pTexture;
}

/**
* ���ڴ洴������
* @param name �������ƣ���Ϊ��
* @param width ���
* @param height �߶�
* @param channel ͨ����
* @param data ����ָ��
* @param mipmap �Ƿ�����MIPMAP
* @return ����������
*/
CTexture* CTextureManager::Create(const string& name, int width, int height, int channel, const void* data, bool mipmap) {
	// �ڴ��������ƣ���δָ�������򰴹�������
	string file = name;
	if (file.empty()) {
		file.append("mem_");
		file.append(std::to_string(width)).append("_");
		file.append(std::to_string(height)).append("_");
		file.append(std::to_string(channel)).append("_");
		file.append(CStringUtil::Guid());
	}
	// ����Ƿ��Ѿ�������ͬ��������Դ
	map<string, CTexture*>::iterator iter = m_mapTexture.find(file);
	if (iter != m_mapTexture.end()) {
		iter->second->m_iRefCount++;
		return iter->second;
	}
	CTexture* pTexture = new CTexture(this);
	pTexture->m_iTextureType = GL_TEXTURE_2D;
	pTexture->m_strFilePath = file;
	pTexture->m_iChannel = channel;
	pTexture->m_iWidth = width;
	pTexture->m_iHeight = height;
	m_mapTexture.insert(std::pair<string, CTexture*>(file, pTexture));
	// ���� OpenGL �������֧���ĸ�ͨ��
	if (channel >= 1 && channel <= 4) {
		glGenTextures(1, &pTexture->m_iTexture);
		glBindTexture(GL_TEXTURE_2D, pTexture->m_iTexture);
		GLint pixelFormat;
		if (channel == 1) pixelFormat = GL_LUMINANCE;
		else if (channel == 2) pixelFormat = GL_LUMINANCE_ALPHA;
		else if (channel == 3) pixelFormat = GL_RGB;
		else pixelFormat = GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, width, height, 0, pixelFormat, GL_UNSIGNED_BYTE, data);
		if (mipmap && !(width & (width - 1)) && !(height & (height - 1))) {
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		} else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		pTexture->m_bIsValid = true;
		pTexture->m_iPixelFormat = pixelFormat;
	}
	return pTexture;
}

/**
* ������Ⱦ����
* @param name ��������
* @param width ������
* @param height ����߶�
* @param floating �Ƿ񸡵�����
* @param useDepth �Ƿ���Ҫ�����Ϣ
* @param mipmap �Ƿ�����MIPMAP
* @return ����������
*/
CTexture* CTextureManager::Create(const string& name, int width, int height, bool floating, bool useDepth, bool mipmap) {
	// ����Ƿ��Ѿ�������ͬ��������Դ
	map<string, CTexture*>::iterator iter = m_mapTexture.find(name);
	if (iter != m_mapTexture.end()) {
		iter->second->m_iRefCount++;
		return iter->second;
	}
	CTexture* pTexture = new CTexture(this);
	pTexture->m_iTextureType = GL_TEXTURE_2D;
	pTexture->m_strFilePath = name;
	pTexture->m_iChannel = 4;
	pTexture->m_iWidth = width;
	pTexture->m_iHeight = height;
	pTexture->m_iPixelFormat = floating ? GL_R32F : GL_RGBA;
	m_mapTexture.insert(std::pair<string, CTexture*>(name, pTexture));
	// ֡����
	GLint current;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &current);
	glGenFramebuffers(1, &pTexture->m_iFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, pTexture->m_iFrameBuffer);
	// ��Ⱦ����
	glGenTextures(1, &pTexture->m_iTexture);
	glBindTexture(GL_TEXTURE_2D, pTexture->m_iTexture);
	if (floating) glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, 0);
	else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	if (mipmap) glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pTexture->m_iTexture, 0);
	// �����Ҫ�����Ϣ���򴴽���Ⱦ����
	if (useDepth) {
		glGenRenderbuffers(1, &pTexture->m_iRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, pTexture->m_iRenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pTexture->m_iRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, current);
	pTexture->m_bIsValid = true;
	return pTexture;
}

/**
* ������������Ⱦ����
* @param name ��������
* @param size �����С
* @param floating �Ƿ񸡵�����
* @param useDepth �Ƿ���Ҫ�����Ϣ
* @param mipmap �Ƿ�����MIPMAP
* @return ����������
*/
CTexture* CTextureManager::Create(const string& name, int size, bool floating, bool useDepth, bool mipmap) {
	// ����Ƿ��Ѿ�������ͬ��������Դ
	map<string, CTexture*>::iterator iter = m_mapTexture.find(name);
	if (iter != m_mapTexture.end()) {
		iter->second->m_iRefCount++;
		return iter->second;
	}
	CTexture* pTexture = new CTexture(this);
	pTexture->m_iTextureType = GL_TEXTURE_CUBE_MAP;
	pTexture->m_strFilePath = name;
	pTexture->m_iChannel = 4;
	pTexture->m_iWidth = size;
	pTexture->m_iHeight = size;
	pTexture->m_iPixelFormat = floating ? GL_R32F : GL_RGBA;
	m_mapTexture.insert(std::pair<string, CTexture*>(name, pTexture));
	// ��������������
	glGenTextures(1, &pTexture->m_iTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, pTexture->m_iTexture);
	// ֡�������Ⱦ����
	GLint current;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &current);
	glGenFramebuffers(1, &pTexture->m_iFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, pTexture->m_iFrameBuffer);
	if (useDepth) {
		glGenRenderbuffers(1, &pTexture->m_iRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, pTexture->m_iRenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pTexture->m_iRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
	for (int i = 0; i < 6; i++) {
		GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
		if (floating) glTexImage2D(face, 0, GL_R32F, size, size, 0, GL_RED, GL_FLOAT, 0);
		else glTexImage2D(face, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	}
	if (mipmap) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, pTexture->m_iTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, current);
	pTexture->m_bIsValid = true;
	return pTexture;
}

/**
* ������������
* @param name ��������
* @param width ������
* @param height ����߶�
* @param shader ������ɫ���ַ���
* @return ����������
* @note shader �пɰ��� uElapsedTime �� uResolution ��ȡʱ��ͷֱ���
*/
CTexture* CTextureManager::Create(const string& name, int width, int height, const string& shader) {
	// ������ɫ��
	const char* vertShader = "\
		in vec4 aPosition;\
		in vec4 aNormal;\
		in vec2 aTexCoord;\
		in vec4 aColor;\
		out vec2 vTexCoord;\
		void main()\
		{\
			vTexCoord = aTexCoord;\
			gl_Position = aPosition;\
		}";
	CTexture* pTexture = Create(name, width, height, false, false, false);
	if (m_mapProceduralTexture.count(name) == 0) {
		// ��������������ɫ��
		string shaderName = name + "_textureshader";
		SProceduralTexture proceduralTexture;
		proceduralTexture.texture = pTexture;
		proceduralTexture.texture->SetWrapModeClampToEdge(true, true);
		proceduralTexture.shader = CEngine::GetShaderManager()->Create(shaderName, vertShader, shader.c_str());
		proceduralTexture.shader->SetUniform("uElapsedTime", 0.0f);
		proceduralTexture.shader->SetUniform("uResolution", CVector2(width, height));
		proceduralTexture.update = true;
		proceduralTexture.dynamic = proceduralTexture.shader->IsUniform("uElapsedTime");
		m_mapProceduralTexture.insert(std::pair<string, SProceduralTexture>(name, proceduralTexture));
	}
	return pTexture;
}

/**
* �ͷ�ָ����������Դ
* @param texture ��Ҫ���ٵ�����
*/
void CTextureManager::Drop(CTexture* texture) {
	if (!texture) return;
	// ���ü�����һ
	texture->m_iRefCount--;
	if (texture->m_iRefCount <= 0) {
		m_mapTexture.erase(texture->m_strFilePath);
		// ɾ�����������Ӧ����ɫ��
		map<string, SProceduralTexture>::iterator iter = m_mapProceduralTexture.find(texture->m_strFilePath);
		if (iter != m_mapProceduralTexture.end()) {
			CEngine::GetShaderManager()->Drop(iter->second.shader);
			m_mapProceduralTexture.erase(iter);
		}
		delete texture;
	}
}

/**
* ���³�������
* @attention ���ô˷������޸ĵ�ǰ��ȾĿ�꣬��Ҫ�ȱ�����ȾĿ��
*/
bool CTextureManager::Update() {
	if (m_mapProceduralTexture.empty()) return false;
	// ���λ������г�������
	CGraphicsManager* pGraphicsMgr = CEngine::GetGraphicsManager();
	float proceduralTime = CTimer::Reset("__texturemanager__", false);
	map<string, SProceduralTexture>::iterator iter = m_mapProceduralTexture.begin();
	while (iter != m_mapProceduralTexture.end()) {
		if (iter->second.update) {
			iter->second.update = iter->second.dynamic;
			pGraphicsMgr->SetRenderTarget(iter->second.texture, 0, true, true, false);
			iter->second.shader->UseShader();
			iter->second.shader->SetUniform("uElapsedTime", proceduralTime);
			pGraphicsMgr->DrawTriangles(&m_vecProceduralVertices[0], 4, true);
		}
		++iter;
	}
	return true;
}

/**
* ��������ͼƬ
* @param texture ��Ҫ���µ�����
* @param file ��ͼƬ��ַ
*/
bool CTextureManager::Update(CTexture* texture, const string& file) {
	if (!texture || texture->IsCubeMap() || texture->IsRenderTarget()) return false;
	// ��ȡͼƬ�ļ�
	CFileManager::CImageFile* pFile = ReadImage(file, true);
	if (!pFile) return false;
	if (pFile->channels < 1 || pFile->channels > 4) {
		delete pFile;
		return false;
	}
	// ������Ϣ
	texture->m_iChannel = pFile->channels;
	texture->m_iWidth = pFile->width;
	texture->m_iHeight = pFile->height;
	if (!texture->m_bIsValid) {
		glGenTextures(1, &texture->m_iTexture);
		glBindTexture(GL_TEXTURE_2D, texture->m_iTexture);
		texture->m_bIsValid = true;
	} else glBindTexture(GL_TEXTURE_2D, texture->m_iTexture);
	GLint pixelFormat;
	if (texture->m_iChannel == 1) pixelFormat = GL_LUMINANCE;
	else if (texture->m_iChannel == 2) pixelFormat = GL_LUMINANCE_ALPHA;
	else if (texture->m_iChannel == 3) pixelFormat = GL_RGB;
	else pixelFormat = GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, texture->m_iWidth, texture->m_iHeight, 0, pixelFormat, GL_UNSIGNED_BYTE, pFile->contents);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	texture->m_iPixelFormat = pixelFormat;
	delete pFile;
	return true;
}

/**
* ������������
* @param texture ��Ҫ���µ�����
* @param data ����������
*/
bool CTextureManager::Update(CTexture* texture, const unsigned char* data) {
	if (!texture || !texture->m_bIsValid || texture->IsCubeMap() || texture->IsRenderTarget()) return false;
	int w = texture->m_iWidth;
	int h = texture->m_iHeight;
	glBindTexture(GL_TEXTURE_2D, texture->m_iTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, texture->m_iPixelFormat, GL_UNSIGNED_BYTE, data);
	return true;
}

/**
* ����ָ��������������
* @param texture ��Ҫ���µ�����
* @param data �ֲ���������
* @param rect ��������
*/
bool CTextureManager::Update(CTexture* texture, const unsigned char* data, const CRectangle& rect) {
	if (!texture || !texture->m_bIsValid || texture->IsCubeMap() || texture->IsRenderTarget()) return false;
	int x = rect.GetLeft();
	int y = rect.GetTop();
	int w = std::min(rect.GetWidth(), texture->m_iWidth);
	int h = std::min(rect.GetHeight(), texture->m_iHeight);
	glBindTexture(GL_TEXTURE_2D, texture->m_iTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, texture->m_iPixelFormat, GL_UNSIGNED_BYTE, data);
	return true;
}

/**
* ���¾ֲ�������ɫ
* @param texture ��Ҫ���µ�����
* @param color ���º����ɫ
* @param rect ��������Ϊ�ձ�ʾ������������
*/
bool CTextureManager::Update(CTexture* texture, const CColor& color, const CRectangle& rect) {
	if (!texture || !texture->m_bIsValid || texture->IsCubeMap() || texture->IsRenderTarget()) return false;
	int x = rect.GetLeft();
	int y = rect.GetTop();
	int w = (rect.IsEmpty() || rect.GetWidth() > texture->m_iWidth) ? texture->m_iWidth : rect.GetWidth();
	int h = (rect.IsEmpty() || rect.GetHeight() > texture->m_iHeight) ? texture->m_iHeight : rect.GetHeight();
	int size = w * h * texture->m_iChannel;
	unsigned char* data = new unsigned char[size];
	color.FillBuffer(data, size, texture->m_iChannel);
	glBindTexture(GL_TEXTURE_2D, texture->m_iTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, texture->m_iPixelFormat, GL_UNSIGNED_BYTE, data);
	delete[] data;
	return true;
}

/**
* ͨ����ɫ������
* @param texture ��Ҫ���µ�����
* @param input �õ���������������
* @param shader ��ɫ������
* @param level �������� mipmap �ȼ�
*/
bool CTextureManager::Update(CTexture* texture, const string& input, const string& shader, int level) {
	if (!texture || !texture->m_bIsValid || !texture->IsRenderTarget()) return false;
	CGraphicsManager* pGraphicsMgr = CEngine::GetGraphicsManager();
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	CShader* pShader = pShaderMgr->GetShader(shader);
	if (!pShader) return false;
	CTexture* renderTarget = pGraphicsMgr->GetRenderTarget();
	if (texture != renderTarget) pGraphicsMgr->SetRenderTarget(texture, 0, true, false, false);
	pShader->UseShader();
	map<string, CTexture*>::iterator iter = m_mapTexture.find(input);
	if (iter != m_mapTexture.end()) iter->second->UseTexture();
	if (texture->IsCubeMap()) {
		CMatrix4 viewMats[6];
		viewMats[0].LookAt(CVector3(), CVector3(1.0f, 0.0f, 0.0f), CVector3(0.0f, -1.0f, 0.0f));
		viewMats[1].LookAt(CVector3(), CVector3(-1.0f, 0.0f, 0.0f), CVector3(0.0f, -1.0f, 0.0f));
		viewMats[2].LookAt(CVector3(), CVector3(0.0f, 1.0f, 0.0f), CVector3(0.0f, 0.0f, 1.0f));
		viewMats[3].LookAt(CVector3(), CVector3(0.0f, -1.0f, 0.0f), CVector3(0.0f, 0.0f, -1.0f));
		viewMats[4].LookAt(CVector3(), CVector3(0.0f, 0.0f, 1.0f), CVector3(0.0f, -1.0f, 0.0f));
		viewMats[5].LookAt(CVector3(), CVector3(0.0f, 0.0f, -1.0f), CVector3(0.0f, -1.0f, 0.0f));
		pShader->SetUniform("uProjMatrix", CMatrix4().Perspective(90.0f, 1.0f, 0.1f, 10.0f));
		for (int i = 0; i < 6; i++) {
			pShader->SetUniform("uViewMatrix", viewMats[i]);
			GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, face, texture->m_iTexture, level);
			glViewport(0, 0, texture->m_iWidth >> level, texture->m_iHeight >> level);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			pGraphicsMgr->DrawCube(CColor::White);
		}
	} else {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->m_iTexture, level);
		glViewport(0, 0, texture->m_iWidth >> level, texture->m_iHeight >> level);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pGraphicsMgr->DrawQuadrilateral(CColor::White, false);
	}
	if (texture != renderTarget) pGraphicsMgr->SetRenderTarget(renderTarget, 0, false, false, false);
	return true;
}

/**
* ���������С
* @param texture ��Ҫ���µ�����
* @param width ������
* @param height ����߶�
*/
void CTextureManager::Resize(CTexture* texture, int width, int height) {
	if (!texture || !texture->m_bIsValid || texture->IsCubeMap()) return;
	if (texture->m_iWidth == width && texture->m_iHeight == height) return;
	texture->m_iWidth = width;
	texture->m_iHeight = height;
	GLint pixelFormat = texture->m_iPixelFormat;
	GLenum format = pixelFormat == GL_R32F ? GL_RED : pixelFormat;
	GLenum type = pixelFormat == GL_R32F ? GL_FLOAT : GL_UNSIGNED_BYTE;
	glBindTexture(GL_TEXTURE_2D, texture->m_iTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, width, height, 0, format, type, 0);
	// ��Ⱦ��������
	if (texture->m_iRenderBuffer) {
		glBindRenderbuffer(GL_RENDERBUFFER, texture->m_iRenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	}
	// �������ػ���
	if (texture->m_iPixelBuffer) {
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texture->m_iPixelBuffer);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, texture->m_iChannel * width * height, 0, GL_STREAM_DRAW);
	}
	// ���³�������� Uniform ����
	map<string, SProceduralTexture>::iterator iter = m_mapProceduralTexture.find(texture->m_strFilePath);
	if (iter != m_mapProceduralTexture.end()) {
		iter->second.shader->UseShader();
		iter->second.shader->SetUniform("uResolution", CVector2(width, height));
		iter->second.update = true;
	}
}

/**
* ��ȡ����������RGBA����
* @param texture ��Ҫ��ȡ������
* @param face ��� texture Ϊ CubeMap ��Ӧ����ָ������������ĳ������±�
* @param level MIPMAP����Ĭ��Ӧ��0
* @param buffer ���ݻ�����
* @attention ��� texture �Ǹ��������� buffer ӦΪ float ���飬������ unsigned char ����
* @see CTexture::IsFloatType()
*/
void CTextureManager::ReadData(CTexture* texture, int face, int level, void* buffer) {
	GLint current;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &current);
	if (!m_iDummyFrameBuffer) {
		glGenFramebuffers(1, &m_iDummyFrameBuffer);
	}
	int width = texture->m_iWidth >> level;
	int height = texture->m_iHeight >> level;
	face = texture->m_iTextureType == GL_TEXTURE_CUBE_MAP ? std::abs(face) : -1;
	glBindFramebuffer(GL_FRAMEBUFFER, m_iDummyFrameBuffer);
	if (face < 0) glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->m_iTexture, level);
	else glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, texture->m_iTexture, level);
	glReadPixels(0, 0, width, height, GL_RGBA, texture->m_iPixelFormat == GL_R32F ? GL_FLOAT : GL_UNSIGNED_BYTE, buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, current);
	if (m_pCurrentTexture) {
		glBindTexture(m_pCurrentTexture->m_iTextureType, m_pCurrentTexture->m_iTexture);
	}
}

/**
* ��ȡָ�����Ƶ�����
* @param name ��������
* @return ����Ϊ name ������
*/
CTexture* CTextureManager::GetTexture(const string& name) {
	map<string, CTexture*>::iterator iter = m_mapTexture.find(name);
	if (iter != m_mapTexture.end()) {
		return iter->second;
	}
	return 0;
}

/**
* ��ȡ��ǰʹ�õ�����
* @return ��ǰ����
*/
CTexture* CTextureManager::GetCurrentTexture() {
	return m_pCurrentTexture;
}

/**
* ��ȡ��������������б�
* @param textureList ��������б�
*/
void CTextureManager::GetTextureList(vector<CTexture*>& textureList) {
	textureList.resize(m_mapTexture.size());
	map<string, CTexture*>::iterator iter = m_mapTexture.begin();
	int index = 0;
	while (iter != m_mapTexture.end()) {
		textureList[index++] = iter->second;
		++iter;
	}
}

/**
* ��ȡͼƬ�ļ�
* @param file �ļ�·��
* @param resize �Ƿ�ͼƬ��С����Ϊ2���ݴ�
* @return ͼƬ����ָ��
*/
CFileManager::CImageFile* CTextureManager::ReadImage(const string& file, bool resize) {
	// Ŀǰ֧�� BMP��TGA��JPG��PNG ����ͼƬ��ʽ
	CFileManager::CImageFile* pFile = 0;
	string ext = CStringUtil::UpperCase(CFileManager::GetExtension(file));
	if (ext == "BMP") pFile = new CFileManager::CImageFile(CFileManager::BMP);
	else if (ext == "TGA") pFile = new CFileManager::CImageFile(CFileManager::TGA);
	else if (ext == "PNG") pFile = new CFileManager::CImageFile(CFileManager::PNG);
	else if (ext == "JPG" || ext == "JPEG") pFile = new CFileManager::CImageFile(CFileManager::JPG);
	else {
		CLog::Error("Image file type is not supported '%s'", file.c_str());
		return 0;
	}
	// ��ȡ�ļ�
	if (!CEngine::GetFileManager()->ReadFile(file, pFile)) {
		CLog::Error("Read image file error '%s'", file.c_str());
		delete pFile;
		return 0;
	}
	// �жϸ߿��Ƿ��� 2 ����
	if (resize) {
		int flag_width = pFile->width & (pFile->width - 1);
		int flag_height = pFile->height & (pFile->height - 1);
		if (flag_width != 0 || flag_height != 0) {
			CLog::Warn("Image width or height is not power of 2 '%s'", file.c_str());
			int width = 1;
			int height = 1;
			do { width <<= 1; } while (width <= pFile->width);
			do { height <<= 1; } while (height <= pFile->height);
			ScaleImage(pFile, width >> 1, height >> 1);
		}
	}
	return pFile;
}

/**
* ˳ʱ����תͼƬ
* @param image ͼƬ����
* @param angle ��ת�Ƕ�
* @attention ��ת�Ƕ�ֻ��Ϊ 0,90,180,270 �ĸ�ֵ������֮һ
*/
void CTextureManager::RotateImage(CFileManager::CImageFile* image, int angle) {
	if (angle == 0 || (angle != 90 && angle != 180 && angle != 270)) return;
	int tWidth = image->width;
	int tHeight = image->height;
	int tStride = image->channels;
	unsigned char* tData = new unsigned char[tStride * tWidth * tHeight];
	// ����ת�Ƕȷֱ�������ؽ���
	if (angle == 90) {
		for (int y = 0; y < tHeight; y++) {
			for (int x = 0; x < tWidth; x++) {
				int srcIndex = y * tWidth + x;
				int dstIndex = (x + 1) * tHeight - 1 - y;
				memcpy(tData + dstIndex * tStride, image->contents + srcIndex * tStride, tStride);
			}
		}
	} else if (angle == 180) {
		for (int y = 0; y < tHeight; y++) {
			for (int x = 0; x < tWidth; x++) {
				int srcIndex = y * tWidth + x;
				int dstIndex = (tHeight - y) * tWidth - 1 - x;
				memcpy(tData + dstIndex * tStride, image->contents + srcIndex * tStride, tStride);
			}
		}
	} else if (angle == 270) {
		for (int y = 0; y < tHeight; y++) {
			for (int x = 0; x < tWidth; x++) {
				int srcIndex = y * tWidth + x;
				int dstIndex = (tWidth - 1 - x) * tHeight + y;
				memcpy(tData + dstIndex * tStride, image->contents + srcIndex * tStride, tStride);
			}
		}
	}
	image->width = angle == 180? tWidth: tHeight;
	image->height = angle == 180? tHeight: tWidth;
	memcpy(image->contents, tData, tStride * tWidth * tHeight);
	delete[] tData;
}

/**
* ����ͼƬ
* @param image ͼƬ����
* @param horizontal �Ƿ�ˮƽ����
* @note horizontal=true ��ˮƽ�����񣬷���ֱ������
*/
void CTextureManager::MirrorImage(CFileManager::CImageFile* image, bool horizontal) {
	if (horizontal) {
		int half = image->width >> 1;
		for (int y = 0; y < image->height; y++) {
			for (int x = 0; x < half; x++) {
				int index1 = (y * image->width + x) * image->channels;
				int index2 = (y * image->width + image->width - 1 - x) * image->channels;
				for (int n = 0; n < image->channels; n++) {
					unsigned char temp = image->contents[index1 + n];
					image->contents[index1 + n] = image->contents[index2 + n];
					image->contents[index2 + n] = temp;
				}
			}
		}
	} else {
		int half = image->height >> 1;
		for (int y = 0; y < half; y++) {
			for (int x = 0; x < image->width; x++) {
				int index1 = (y * image->width + x) * image->channels;
				int index2 = ((image->height - 1 - y) * image->width + x) * image->channels;
				for (int n = 0; n < image->channels; n++) {
					unsigned char temp = image->contents[index1 + n];
					image->contents[index1 + n] = image->contents[index2 + n];
					image->contents[index2 + n] = temp;
				}
			}
		}
	}
}

/**
* ����ͼƬ
* @param image ͼƬ����
* @param width Ŀ����
* @param height Ŀ��߶�
*/
void CTextureManager::ScaleImage(CFileManager::CImageFile* image, int width, int height) {
	image->size = width * height * image->channels;
	unsigned char* tData = new unsigned char[image->size];
	const float fw = static_cast<float>(image->width - 1) / static_cast<float>(width - 1);
	const float fh = static_cast<float>(image->height - 1) / static_cast<float>(height - 1);
	// ʹ��˫���Բ�ֵ����ͼƬ
	for (int i = 0; i < height; i++) {
		float sy = fh * i;
		int y1 = static_cast<int>(sy);
		int y2 = y1 + 1 == image->height? y1: y1 + 1;
		float v = sy - y1;
		for (int j = 0; j < width; j++) {
			float sx = fw * j;
			int x1 = static_cast<int>(sx);
			int x2 = x1 + 1 == image->width? x1: x1 + 1;
			float u = sx - x1;
			// �ĸ���Ȩ��
			float s1 = (1.0f - u) * (1.0f - v);
			float s2 = (1.0f - u) * v;
			float s3 = u * (1.0f - v);
			float s4 = u * v;
			for (int n = 0; n < image->channels; n++) {
				float x1y1 = s1 * image->contents[(y1 * image->width + x1) * image->channels + n];
				float x1y2 = s2 * image->contents[(y2 * image->width + x1) * image->channels + n];
				float x2y1 = s3 * image->contents[(y1 * image->width + x2) * image->channels + n];
				float x2y2 = s4 * image->contents[(y2 * image->width + x2) * image->channels + n];
				tData[(i * width + j) * image->channels + n] = (unsigned char)(x1y1 + x1y2 + x2y1 + x2y2);
			}
		}
	}
	delete[] image->contents;
	image->contents = tData;
	image->width = width;
	image->height = height;
}

/**
* ����Ԥ��������
*/
void CTextureManager::LoadDefaultTexture() {
	// Ԥ������ɫ
	const char* name[] = { "", "transparent", "white", "black", "red", "green", "blue", "yellow", "purple" };
	const unsigned char color[] = {
		0x80, 0x80, 0x80, 0xFF,
		0xFF, 0xFF, 0xFF, 0x00,
		0xFF, 0xFF, 0xFF, 0xFF,
		0x00, 0x00, 0x00, 0xFF,
		0xFF, 0x00, 0x00, 0xFF,
		0x00, 0xFF, 0x00, 0xFF,
		0x00, 0x00, 0xFF, 0xFF,
		0xFF, 0xFF, 0x00, 0xFF,
		0xFF, 0x00, 0xFF, 0xFF
	};
	for (size_t i = 0; i < sizeof(name) / sizeof(char*); i++) {
		CTexture* pTexture = new CTexture(this);
		pTexture->m_iTextureType = GL_TEXTURE_2D;
		pTexture->m_strFilePath = name[i];
		pTexture->m_iChannel = 4;
		pTexture->m_iWidth = 1;
		pTexture->m_iHeight = 1;
		pTexture->m_iPixelFormat = GL_RGBA;
		pTexture->m_bIsValid = true;
		glGenTextures(1, &pTexture->m_iTexture);
		glBindTexture(GL_TEXTURE_2D, pTexture->m_iTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color[i * 4]);
		m_mapTexture.insert(std::pair<string, CTexture*>(pTexture->m_strFilePath, pTexture));
	}
	// ���� logo ����
	CTexture* pTexture = new CTexture(this);
	pTexture->m_iTextureType = GL_TEXTURE_2D;
	pTexture->m_strFilePath = "logo";
	pTexture->m_iChannel = 4;
	pTexture->m_iWidth = 64;
	pTexture->m_iHeight = 64;
	pTexture->m_iPixelFormat = GL_RGBA;
	pTexture->m_bIsValid = true;
	glGenTextures(1, &pTexture->m_iTexture);
	glBindTexture(GL_TEXTURE_2D, pTexture->m_iTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, LOGO_IMAGE_DATA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_mapTexture.insert(std::pair<string, CTexture*>(pTexture->m_strFilePath, pTexture));
}