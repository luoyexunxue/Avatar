//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CFONTMANAGER_H_
#define _CFONTMANAGER_H_
#include "AvatarConfig.h"
#include <string>
#include <vector>
#include <map>
using std::string;
using std::wstring;
using std::vector;
using std::map;
typedef struct FT_LibraryRec_* FT_Library;
typedef struct FT_FaceRec_* FT_Face;

/**
* @brief ���������
*/
class AVATAR_EXPORT CFontManager {
public:
	//! ��ȡ������ʵ��
	static CFontManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CFontManager();
		return m_pInstance;
	}
	//! ʵ������
	void Destroy();

public:
	//! ֧�ֵ����ֶ��뷽ʽ
	enum Alignment {
		BOTTOMLEFT, BOTTOMCENTER, BOTTOMRIGHT,
		MIDDLELEFT, MIDDLECENTER, MIDDLERIGHT,
		TOPLEFT, TOPCENTER, TOPRIGHT
	};
	//! ��Ⱦ�ı�ͼƬ
	class CTextImage {
	public:
		int width;
		int height;
		int textWidth;
		int textHeight;
		unsigned char* data;
		CTextImage(): width(0), height(0), textWidth(0), textHeight(0), data(0) {}
		CTextImage(int w, int h): width(w), height(h), textWidth(0), textHeight(0) {
			data = new unsigned char[w * h];
		}
		~CTextImage() { if (data) delete[] data; }
	};

public:
	//! ���������
	bool Load(const string& file, const string& name);
	//! ��������Ѽ��������
	void Clear();
	//! ���û����С
	void SetCacheSize(int size);

	//! ʹ�������
	bool UseFont(const string& name);
	//! ���������С
	void SetSize(int size);
	//! ��������ͼƬ��С
	void TextSize(const wchar_t* text, int* width, int* height);
	//! ���ƻҶ�ͼ
	void DrawText(const wchar_t* text, CTextImage* image, Alignment align, bool stretch);

	//! ��ȡ���������������б�
	void GetFontList(vector<string>& fontList);

private:
	CFontManager();
	~CFontManager();

	//! �����ַ�����
	void SetAlignment(CTextImage* src, CTextImage* dst, Alignment align, bool stretch);
	//! ������Ⱦ���
	bool CachePut(const wchar_t* text, CTextImage* image);
	//! ȡ������ͼƬ
	bool CacheGet(const wchar_t* text, CTextImage*& image);
	//! ���LRU����
	void CacheClear();

private:
	//! ���嶨��
	typedef struct _SFontFace {
		FT_Face face;
		//! �����ļ�ָ��
		void* data;
	} SFontFace;
	//! LRU ��ֵ����
	typedef struct _SCacheKey {
		wstring text;
		FT_Face face;
		int size;
		//! map�ȽϺ���
		bool operator < (const _SCacheKey& other) const {
			if (text != other.text) return text < other.text;
			if (face != other.face) return face < other.face;
			if (size != other.size) return size < other.size;
			return false;
		}
	} SCacheKey;
	//! LRU ���浥Ԫ
	typedef struct _SCacheEntry {
		SCacheKey key;
		CTextImage* image;
		// LRU˫����
		_SCacheEntry* prev;
		_SCacheEntry* next;
	} SCacheEntry;

private:
	//! FreeType ��
	FT_Library m_pFreeTypeLib;
	//! ��ǰ����
	FT_Face m_pCurrentFace;
	//! ��ǰ�����С
	int m_iCurrentSize;
	//! ���弯��
	map<string, SFontFace> m_mapFontFace;

	//! �趨�Ļ����С
	int m_iCacheSize;
	//! LRU����
	map<SCacheKey, SCacheEntry*> m_mapLRUCache;
	//! LRU��������ͷָ��
	SCacheEntry* m_pLRUCacheHead;
	//! LRU��������βָ��
	SCacheEntry* m_pLRUCacheTail;

	//! ʵ��
	static CFontManager* m_pInstance;
};

#endif