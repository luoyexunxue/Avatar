//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* @brief ������Դ������
*/
class AVATAR_EXPORT CTextureManager {
public:
	//! ��ȡ������ʵ��
	static CTextureManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CTextureManager();
		return m_pInstance;
	}

	//! ʵ�����٣��ͷ�����������Դ
	void Destroy();

	//! ���ļ���������
	CTexture* Create(const string& file);
	//! ���ļ���������������
	CTexture* Create(const string& name, const string files[6]);
	//! ���ڴ洴������
	CTexture* Create(const string& name, int width, int height, int channel, const void* data, bool mipmap);
	//! ������Ⱦ����
	CTexture* Create(const string& name, int width, int height, bool floating, bool useDepth, bool mipmap);
	//! ������������Ⱦ����
	CTexture* Create(const string& name, int size, bool floating, bool useDepth, bool mipmap);
	//! ������������
	CTexture* Create(const string& name, int width, int height, const string& shader);

	//! �ͷ�ָ����������Դ
	void Drop(CTexture* texture);
	//! �������г�������
	bool Update();
	//! ��������ͼƬ
	bool Update(CTexture* texture, const string& file);
	//! ������������
	bool Update(CTexture* texture, const unsigned char* data);
	//! ���¾ֲ���������
	bool Update(CTexture* texture, const unsigned char* data, const CRectangle& rect);
	//! ���¾ֲ�������ɫ
	bool Update(CTexture* texture, const CColor& color, const CRectangle& rect);
	//! ͨ����ɫ������
	bool Update(CTexture* texture, const string& input, const string& shader, int level);
	//! ���������С
	void Resize(CTexture* texture, int width, int height);
	//! ��ȡ����������RGBA����
	void ReadData(CTexture* texture, int face, int level, void* buffer);

	//! ��ȡָ�����Ƶ�����
	CTexture* GetTexture(const string& name);
	//! ��ȡ��ǰʹ�õ�����
	CTexture* GetCurrentTexture();
	//! ��ȡ��������������б�
	void GetTextureList(vector<CTexture*>& textureList);

public:
	//! ��ȡͼƬ
	static CFileManager::CImageFile* ReadImage(const string& file, bool resize);
	//! ��תͼƬ
	static void RotateImage(CFileManager::CImageFile* image, int angle);
	//! ����ͼƬ
	static void MirrorImage(CFileManager::CImageFile* image, bool horizontal);
	//! ����ͼƬ
	static void ScaleImage(CFileManager::CImageFile* image, int width, int height);

private:
	CTextureManager();
	~CTextureManager();

	//! ����Ԥ��������
	void LoadDefaultTexture();

private:
	//! ����������
	typedef struct _SProceduralTexture {
		CTexture* texture;
		CShader* shader;
		bool update;
		bool dynamic;
	} SProceduralTexture;

private:
	//! ����Ⱦ����
	unsigned int m_iDummyFrameBuffer;
	//! ��ǰ����
	CTexture* m_pCurrentTexture;
	//! ������
	map<string, CTexture*> m_mapTexture;
	//! ��Ⱦ��������ʹ�õĶ���
	vector<CVertex> m_vecProceduralVertices;
	//! ����������
	map<string, SProceduralTexture> m_mapProceduralTexture;
	//! ʵ��
	static CTextureManager* m_pInstance;
	//! ������Ԫ��
	friend class CTexture;
};

#endif