//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CTEXTURE_H_
#define _CTEXTURE_H_
#include "AvatarConfig.h"
#include <string>
using std::string;

/**
* @brief ������
*/
class AVATAR_EXPORT CTexture {
public:
	//! �����Ƿ���Ч
	bool IsValid() const;
	//! �����Ƿ�Ϊ��������ͼ
	bool IsCubeMap() const;
	//! �Ƿ���Ⱦ����
	bool IsRenderTarget() const;
	//! �Ƿ񸡵�����
	bool IsFloatType() const;
	//! ��ȡ����ͨ��
	int GetChannel() const;
	//! ��ȡ������
	int GetWidth() const;
	//! ��ȡ����߶�
	int GetHeight() const;
	//! ��ȡ�������ظ�ʽ
	int GetPixelFormat() const;
	//! ��ȡ����ID
	unsigned int GetTextureId() const;
	//! ��ȡ������֡����
	unsigned int GetFramebuffer() const;
	//! ��ȡ����ͼƬ·��
	const string& GetFilePath() const;

	//! ��������ģʽΪ�ظ�
	void SetWrapModeRepeat(bool s, bool t) const;
	//! ��������ģʽΪ���Ƶ��߽�
	void SetWrapModeClampToEdge(bool s, bool t) const;
	//! ��������ģʽΪ�����ظ�
	void SetWrapModeMirroredRepeat(bool s, bool t) const;

	//! ��ʼӳ����������
	void* MapBuffer(bool clear);
	//! ����ӳ����������
	void UnmapBuffer();

	//! ʹ������
	void UseTexture();
	//! ʹ��������ָ������Ԫ
	void UseTexture(int unit);
	//! ��������
	CTexture* AddReference();

protected:
	//! �ܱ������캯����ֻ��ͨ�� CTextureManager ����
	CTexture(class CTextureManager* manager);
	//! �ܱ�����������
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