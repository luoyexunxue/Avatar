//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CFontManager.h"
#include "AvatarConfig.h"
#include "CEngine.h"
#include "CLog.h"
#include "thirdparty/freetype/ft2build.h"
#include "thirdparty/freetype/freetype.h"
#include <algorithm>

#ifdef AVATAR_WINDOWS
#pragma comment(lib, "thirdparty/freetype/freetype253.lib")
#endif

/**
* ���캯��
*/
CFontManager::CFontManager() {
	m_pFreeTypeLib = 0;
	m_pCurrentFace = 0;
	m_iCurrentSize = 16;
	// Ĭ�ϻ����СΪ100
	m_iCacheSize = 100;
	m_pLRUCacheHead = new SCacheEntry();
	m_pLRUCacheTail = new SCacheEntry();
	m_pLRUCacheHead->prev = 0;
	m_pLRUCacheHead->next = m_pLRUCacheTail;
	m_pLRUCacheTail->next = 0;
	m_pLRUCacheTail->prev = m_pLRUCacheHead;
	// ��ʼ�� FreeType ��
	FT_Init_FreeType(&m_pFreeTypeLib);
}

/**
* ��������
*/
CFontManager::~CFontManager() {
	m_pInstance = 0;
}

/**
* ����ʵ��
*/
CFontManager* CFontManager::m_pInstance = 0;

/**
* ��������������
*/
void CFontManager::Destroy() {
	Clear();
	FT_Done_FreeType(m_pFreeTypeLib);
	delete m_pLRUCacheHead;
	delete m_pLRUCacheTail;
	delete this;
}

/**
* ���������
* @param file �����ļ���
* @param name ���������
* @return ���سɹ����� true
*/
bool CFontManager::Load(const string& file, const string& name) {
	// �Ƿ��Ѿ�����
	if (UseFont(name)) return true;

	// ��ȡ�ļ�
	CFileManager::CBinaryFile* pFile = new CFileManager::CBinaryFile();
	if (!CEngine::GetFileManager()->ReadFile(file, pFile)) {
		CLog::Error("Read font file '%s' error", file.c_str());
		delete pFile;
		return false;
	}
	// ��ȡ�����
	FT_Face face;
	FT_Error err = FT_New_Memory_Face(m_pFreeTypeLib, pFile->contents, pFile->size, 0, &face);
	if (err == FT_Err_Unknown_File_Format) {
		CLog::Error("Could not open unknown font file format");
		delete pFile;
		return false;
	} else if(err) {
		CLog::Error("Could not open font file %d", err);
		delete pFile;
		return false;
	}
	// Ĭ�������С
	FT_Set_Char_Size(face, m_iCurrentSize << 6, 0, 96, 0);
	// ���뵽���弯
	SFontFace font;
	font.face = face;
	font.data = pFile;
	m_mapFontFace.insert(std::pair<string, SFontFace>(name, font));
	m_pCurrentFace = face;
	return true;
}

/**
* ��������Ѽ��������
*/
void CFontManager::Clear() {
	map<string, SFontFace>::iterator iter = m_mapFontFace.begin();
	while (iter != m_mapFontFace.end()) {
		FT_Done_Face(iter->second.face);
		delete static_cast<CFileManager::CBinaryFile*>(iter->second.data);
		++iter;
	}
	m_mapFontFace.clear();
	m_pCurrentFace = 0;
	CacheClear();
}

/**
* ���û����С��������Լ����������ʱ��
* @param size LRU �����С
*/
void CFontManager::SetCacheSize(int size) {
	m_iCacheSize = size;
	// ��Ҫɾ����Ԫ�ظ���
	int count = static_cast<int>(m_mapLRUCache.size()) - size;
	if (count > 0) {
		SCacheEntry* entry = m_pLRUCacheTail->prev;
		while (count--) {
			m_mapLRUCache.erase(entry->key);
			delete entry->image;
			entry = entry->prev;
			delete entry->next;
		}
		entry->next = m_pLRUCacheTail;
		m_pLRUCacheTail->prev = entry;
	}
}

/**
* ʹ�������
* @param name ���������
* @see CFontManager::Load
*/
bool CFontManager::UseFont(const string& name) {
	map<string, SFontFace>::iterator iter = m_mapFontFace.find(name);
	if(iter != m_mapFontFace.end()) {
		m_pCurrentFace = iter->second.face;
		return true;
	}
	return false;
}

/**
* ���������С
* @param size �����С
*/
void CFontManager::SetSize(int size){
	if (m_pCurrentFace && m_iCurrentSize != size) {
		m_iCurrentSize = size;
		FT_Set_Char_Size(m_pCurrentFace, size << 6, 0, 96, 0);
	}
}

/**
* ��������ͼƬ��С
* @param text UNICODE �ı�
* @param width �ı�ռ�õ����ؿ��
* @param height �ı�ռ�õ����ظ߶�
*/
void CFontManager::TextSize(const wchar_t* text, int* width, int* height) {
	size_t count = wcslen(text);
	if (!m_pCurrentFace || count == 0) {
		if (width) *width = 0;
		if (height) *height = 0;
		return;
	}
	// �ַ���ռ�ÿ��
	int maxWidth = 0;
	int maxHeight = 0;
	int rowWidth = 0;
	int rowHeight = static_cast<int>(m_pCurrentFace->size->metrics.height >> 6);
	for (size_t i = 0; i < count; i++) {
		// �����з�
		if (text[i] == (wchar_t)0x0A) {
			maxWidth = std::max(maxWidth, rowWidth);
			maxHeight += rowHeight;
			rowWidth = 0;
			continue;
		}
		// ��¼�ַ���ռ�ÿ��
		if (FT_Load_Char(m_pCurrentFace, text[i], FT_LOAD_RENDER)) continue;
		rowWidth += static_cast<int>(m_pCurrentFace->glyph->advance.x >> 6);
	}
	if (width) *width = std::max(maxWidth, rowWidth);
	if (height) *height = maxHeight + rowHeight;
}

/**
* ���ƻҶ�ͼ
* @param text UNICODE �ı�
* @param image ָ�����Ƶ�ͼƬ
* @param align ���뷽ʽ
* @param stretch ���쵽ͼƬ��С�����ָ߿��
*/
void CFontManager::DrawText(const wchar_t* text, CTextImage* image, Alignment align, bool stretch) {
	// �ı���ռ��С
	TextSize(text, &image->textWidth, &image->textHeight);
	if (!image->data) {
		image->width = image->textWidth;
		image->height = image->textHeight;
		image->data = new unsigned char[image->width * image->height];
	}
	// ���������ַ���
	size_t count = wcslen(text);
	if (!m_pCurrentFace || count == 0) {
		memset(image->data, 0, image->width * image->height);
		return;
	}
	// ���ȶ�ȡ����
	CTextImage* canvas = 0;
	if (CacheGet(text, canvas)) {
		SetAlignment(canvas, image, align, stretch);
		return;
	}
	canvas = new CTextImage(image->textWidth, image->textHeight);
	canvas->textWidth = image->textWidth;
	canvas->textHeight = image->textHeight;
	memset(canvas->data, 0, canvas->width * canvas->height);
	// �ַ���ռ�ÿ��
	int maxWidth = 0;
	int maxHeight = 0;
	int rowWidth = 0;
	int rowHeight = static_cast<int>(m_pCurrentFace->size->metrics.height >> 6);
	for (size_t i = 0; i < count; i++) {
		// �����з�
		if (text[i] == (wchar_t)0x0A) {
			maxWidth = std::max(maxWidth, rowWidth);
			maxHeight += rowHeight;
			rowWidth = 0;
			continue;
		}
		// ��Ⱦ�����ַ�
		if (FT_Load_Char(m_pCurrentFace, text[i], FT_LOAD_RENDER)) continue;
		// ����Ⱦ���ַ����Ƶ� canvas->data ������
		FT_GlyphSlot g = m_pCurrentFace->glyph;
		int xPos = rowWidth + g->bitmap_left;
		int yPos = maxHeight + static_cast<int>(m_pCurrentFace->size->metrics.ascender >> 6) - g->bitmap_top;
		rowWidth += static_cast<int>(g->advance.x >> 6);
		for (int row = 0; row < g->bitmap.rows; row++) {
			int iy = row + yPos;
			if (iy < 0) continue;
			if (iy >= canvas->height) break;
			int idx_src = row * g->bitmap.pitch;
			int idx_dst = iy * canvas->width + xPos;
			int line_width = std::min(g->bitmap.width, canvas->width - xPos);
			memcpy(canvas->data + idx_dst, g->bitmap.buffer + idx_src, line_width);
		}
	}
	// ��ͼƬ����
	CachePut(text, canvas);
	SetAlignment(canvas, image, align, stretch);
}

/**
* ��ȡ���������������б�
* @param fontList ���������б�
*/
void CFontManager::GetFontList(vector<string>& fontList) {
	fontList.resize(m_mapFontFace.size());
	map<string, SFontFace>::iterator iter = m_mapFontFace.begin();
	int index = 0;
	while (iter != m_mapFontFace.end()) {
		fontList[index++] = iter->first;
		++iter;
	}
}

/**
* �����ַ�����
* @param src Դ�ı��Ҷ�ͼ
* @param dst Ŀ���ı��Ҷ�ͼ
* @param align ���뷽ʽ
* @param stretch ���쵽ͼƬ��С
*/
void CFontManager::SetAlignment(CTextImage* src, CTextImage* dst, Alignment align, bool stretch) {
	if (stretch) {
		// �Ƿ�ֱ�������쵽ͼƬ��С�����ֿ�߱ȣ�
		bool stretch_vertical = dst->textWidth * dst->height < dst->textHeight * dst->width;
		const int target_width = stretch_vertical ? dst->height * dst->textWidth / dst->textHeight : dst->width;
		const int target_height = stretch_vertical ? dst->height : dst->width * dst->textHeight / dst->textWidth;
		// ʹ��˫���Բ�ֵ����ͼƬ
		CTextImage* temp = new CTextImage(target_width, target_height);
		const float fw = static_cast<float>(src->width - 1) / static_cast<float>(target_width - 1);
		const float fh = static_cast<float>(src->height - 1) / static_cast<float>(target_height - 1);
		for (int i = 0; i < target_height; i++) {
			float sy = fh * i;
			int y1 = static_cast<int>(sy);
			int y2 = y1 + 1 == src->height ? y1 : y1 + 1;
			float v = sy - y1;
			for (int j = 0; j < target_width; j++) {
				float sx = fw * j;
				int x1 = static_cast<int>(sx);
				int x2 = x1 + 1 == src->width ? x1 : x1 + 1;
				float u = sx - x1;
				// �ĸ���Ȩ��
				float s1 = (1.0f - u) * (1.0f - v);
				float s2 = (1.0f - u) * v;
				float s3 = u * (1.0f - v);
				float s4 = u * v;
				float x1y1 = s1 * src->data[y1 * src->width + x1];
				float x1y2 = s2 * src->data[y2 * src->width + x1];
				float x2y1 = s3 * src->data[y1 * src->width + x2];
				float x2y2 = s4 * src->data[y2 * src->width + x2];
				temp->data[i * target_width + j] = (unsigned char)(x1y1 + x1y2 + x2y1 + x2y2);
			}
		}
		src = temp;
		dst->textWidth = src->width;
		dst->textHeight = src->height;
	}
	int dx = dst->width - dst->textWidth;
	int dy = dst->height - dst->textHeight;
	// Ĭ�����Ͻ�
	switch (align) {
	case TOPLEFT: { dx = 0; dy = 0; } break;
	case TOPCENTER: { dx /= 2; dy = 0; } break;
	case TOPRIGHT: { dy = 0; } break;
	case MIDDLELEFT: { dx = 0; dy /= 2; } break;
	case MIDDLECENTER: { dx /= 2; dy /= 2; } break;
	case MIDDLERIGHT: { dy /= 2; } break;
	case BOTTOMLEFT: { dx = 0; } break;
	case BOTTOMCENTER: { dx /= 2; } break;
	case BOTTOMRIGHT: break;
	}
	// ����λͼ�飬��Ҫ�Ƕ�һ��λͼ���и��� (dx, dy)
	memset(dst->data, 0, dst->width * dst->height);
	int imgW = std::min(dst->width, dst->textWidth);
	int imgH = std::min(dst->height, dst->textHeight);
	for (int y = 0; y < imgH; y++) {
		for (int x = 0; x < imgW; x++) {
			int srcx = dx > 0 ? x : x - dx;
			int srcy = dy > 0 ? y : y - dy;
			int dstx = srcx + dx;
			int dsty = srcy + dy;
			dst->data[dst->width * dsty + dstx] = src->data[src->width * srcy + srcx];
		}
	}
	// ɾ���������Ŵ�����ͼƬ
	if (stretch) delete src;
}

/**
* ������Ⱦ���
* @param text UNICODE �ı�
* @param image �ı��Ҷ�ͼ
* @return ������ɹ��򷵻�true
*/
bool CFontManager::CachePut(const wchar_t* text, CTextImage* image) {
	SCacheKey key;
	key.text = text;
	key.face = m_pCurrentFace;
	key.size = m_iCurrentSize;
	SCacheEntry* entry = 0;
	// ����Ƿ��Ѿ�����
	map<SCacheKey, SCacheEntry*>::iterator iter = m_mapLRUCache.find(key);
	if (iter != m_mapLRUCache.end()) {
		entry = iter->second;
		entry->prev->next = entry->next;
		entry->next->prev = entry->prev;
	} else {
		if (static_cast<int>(m_mapLRUCache.size()) < m_iCacheSize) {
			entry = new SCacheEntry;
		} else {
			entry = m_pLRUCacheTail->prev;
			entry->prev->next = entry->next;
			entry->next->prev = entry->prev;
			m_mapLRUCache.erase(entry->key);
			delete entry->image;
		}
		entry->key = key;
		entry->image = image;
		m_mapLRUCache.insert(std::pair<SCacheKey, SCacheEntry*>(key, entry));
	}
	// ��Ԫ���ƶ���ͷ��
	entry->next = m_pLRUCacheHead->next;
	entry->prev = m_pLRUCacheHead;
	m_pLRUCacheHead->next = entry;
	entry->next->prev = entry;
	return true;
}

/**
* ȡ������ͼƬ
* @param text UNICODE �ı�
* @param image �ı��Ҷ�ͼ
* @return ����ȡ�ɹ��򷵻�true
*/
bool CFontManager::CacheGet(const wchar_t* text, CTextImage*& image) {
	SCacheKey key;
	key.text = text;
	key.face = m_pCurrentFace;
	key.size = m_iCurrentSize;
	// ���һ���
	map<SCacheKey, SCacheEntry*>::iterator iter = m_mapLRUCache.find(key);
	if (iter != m_mapLRUCache.end()) {
		SCacheEntry* entry = iter->second;
		image = entry->image;
		// ����LRU��������
		entry->prev->next = entry->next;
		entry->next->prev = entry->prev;
		entry->next = m_pLRUCacheHead->next;
		entry->prev = m_pLRUCacheHead;
		m_pLRUCacheHead->next = entry;
		entry->next->prev = entry;
		return true;
	}
	return false;
}

/**
* ���LRU����
*/
void CFontManager::CacheClear() {
	SCacheEntry* entry = m_pLRUCacheHead->next;
	while (entry->next) {
		delete entry->image;
		entry = entry->next;
		delete entry->prev;
	}
	m_pLRUCacheHead->next = m_pLRUCacheTail;
	m_pLRUCacheTail->prev = m_pLRUCacheHead;
	m_mapLRUCache.clear();
}