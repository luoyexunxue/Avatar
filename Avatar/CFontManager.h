//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
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
* @brief 字体管理器
*/
class AVATAR_EXPORT CFontManager {
public:
	//! 获取管理器实例
	static CFontManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CFontManager();
		return m_pInstance;
	}
	//! 实例销毁
	void Destroy();

public:
	//! 支持的文字对齐方式
	enum Alignment {
		BOTTOMLEFT, BOTTOMCENTER, BOTTOMRIGHT,
		MIDDLELEFT, MIDDLECENTER, MIDDLERIGHT,
		TOPLEFT, TOPCENTER, TOPRIGHT
	};
	//! 渲染文本图片
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
	//! 加载字体库
	bool Load(const string& file, const string& name);
	//! 清空所有已加载字体库
	void Clear();
	//! 设置缓存大小
	void SetCacheSize(int size);

	//! 使用字体库
	bool UseFont(const string& name);
	//! 设置字体大小
	void SetSize(int size);
	//! 计算所需图片大小
	void TextSize(const wchar_t* text, int* width, int* height);
	//! 绘制灰度图
	void DrawText(const wchar_t* text, CTextImage* image, Alignment align, bool stretch);

	//! 获取管理的所有字体库列表
	void GetFontList(vector<string>& fontList);

private:
	CFontManager();
	~CFontManager();

	//! 设置字符对齐
	void SetAlignment(CTextImage* src, CTextImage* dst, Alignment align, bool stretch);
	//! 缓存渲染结果
	bool CachePut(const wchar_t* text, CTextImage* image);
	//! 取出缓存图片
	bool CacheGet(const wchar_t* text, CTextImage*& image);
	//! 清空LRU缓存
	void CacheClear();

private:
	//! 字体定义
	typedef struct _SFontFace {
		FT_Face face;
		//! 字体文件指针
		void* data;
	} SFontFace;
	//! LRU 键值定义
	typedef struct _SCacheKey {
		wstring text;
		FT_Face face;
		int size;
		//! map比较函数
		bool operator < (const _SCacheKey& other) const {
			if (text != other.text) return text < other.text;
			if (face != other.face) return face < other.face;
			if (size != other.size) return size < other.size;
			return false;
		}
	} SCacheKey;
	//! LRU 缓存单元
	typedef struct _SCacheEntry {
		SCacheKey key;
		CTextImage* image;
		// LRU双链表
		_SCacheEntry* prev;
		_SCacheEntry* next;
	} SCacheEntry;

private:
	//! FreeType 库
	FT_Library m_pFreeTypeLib;
	//! 当前字体
	FT_Face m_pCurrentFace;
	//! 当前字体大小
	int m_iCurrentSize;
	//! 字体集合
	map<string, SFontFace> m_mapFontFace;

	//! 设定的缓存大小
	int m_iCacheSize;
	//! LRU缓存
	map<SCacheKey, SCacheEntry*> m_mapLRUCache;
	//! LRU缓存链表头指针
	SCacheEntry* m_pLRUCacheHead;
	//! LRU缓存链表尾指针
	SCacheEntry* m_pLRUCacheTail;

	//! 实例
	static CFontManager* m_pInstance;
};

#endif