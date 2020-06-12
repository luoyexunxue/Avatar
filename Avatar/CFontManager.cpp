//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
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
* 构造函数
*/
CFontManager::CFontManager() {
	m_pFreeTypeLib = 0;
	m_pCurrentFace = 0;
	m_iCurrentSize = 16;
	// 默认缓存大小为100
	m_iCacheSize = 100;
	m_pLRUCacheHead = new SCacheEntry();
	m_pLRUCacheTail = new SCacheEntry();
	m_pLRUCacheHead->prev = 0;
	m_pLRUCacheHead->next = m_pLRUCacheTail;
	m_pLRUCacheTail->next = 0;
	m_pLRUCacheTail->prev = m_pLRUCacheHead;
	// 初始化 FreeType 库
	FT_Init_FreeType(&m_pFreeTypeLib);
}

/**
* 析构函数
*/
CFontManager::~CFontManager() {
	m_pInstance = 0;
}

/**
* 单例实例
*/
CFontManager* CFontManager::m_pInstance = 0;

/**
* 销毁字体库管理器
*/
void CFontManager::Destroy() {
	Clear();
	FT_Done_FreeType(m_pFreeTypeLib);
	delete m_pLRUCacheHead;
	delete m_pLRUCacheTail;
	delete this;
}

/**
* 加载字体库
* @param file 字体文件名
* @param name 字体库名称
* @return 加载成功返回 true
*/
bool CFontManager::Load(const string& file, const string& name) {
	// 是否已经存在
	if (UseFont(name)) return true;

	// 读取文件
	CFileManager::CBinaryFile* pFile = new CFileManager::CBinaryFile();
	if (!CEngine::GetFileManager()->ReadFile(file, pFile)) {
		CLog::Error("Read font file '%s' error", file.c_str());
		delete pFile;
		return false;
	}
	// 读取字体库
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
	// 默认字体大小
	FT_Set_Char_Size(face, m_iCurrentSize << 6, 0, 96, 0);
	// 加入到字体集
	SFontFace font;
	font.face = face;
	font.data = pFile;
	m_mapFontFace.insert(std::pair<string, SFontFace>(name, font));
	m_pCurrentFace = face;
	return true;
}

/**
* 清空所有已加载字体库
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
* 设置缓存大小，缓存可以减少字体绘制时间
* @param size LRU 缓存大小
*/
void CFontManager::SetCacheSize(int size) {
	m_iCacheSize = size;
	// 需要删除的元素个数
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
* 使用字体库
* @param name 字体库名称
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
* 设置字体大小
* @param size 字体大小
*/
void CFontManager::SetSize(int size){
	if (m_pCurrentFace && m_iCurrentSize != size) {
		m_iCurrentSize = size;
		FT_Set_Char_Size(m_pCurrentFace, size << 6, 0, 96, 0);
	}
}

/**
* 计算所需图片大小
* @param text UNICODE 文本
* @param width 文本占用的像素宽度
* @param height 文本占用的像素高度
*/
void CFontManager::TextSize(const wchar_t* text, int* width, int* height) {
	size_t count = wcslen(text);
	if (!m_pCurrentFace || count == 0) {
		if (width) *width = 0;
		if (height) *height = 0;
		return;
	}
	// 字符串占用宽高
	int maxWidth = 0;
	int maxHeight = 0;
	int rowWidth = 0;
	int rowHeight = static_cast<int>(m_pCurrentFace->size->metrics.height >> 6);
	for (size_t i = 0; i < count; i++) {
		// 处理换行符
		if (text[i] == (wchar_t)0x0A) {
			maxWidth = std::max(maxWidth, rowWidth);
			maxHeight += rowHeight;
			rowWidth = 0;
			continue;
		}
		// 记录字符串占用宽度
		if (FT_Load_Char(m_pCurrentFace, text[i], FT_LOAD_RENDER)) continue;
		rowWidth += static_cast<int>(m_pCurrentFace->glyph->advance.x >> 6);
	}
	if (width) *width = std::max(maxWidth, rowWidth);
	if (height) *height = maxHeight + rowHeight;
}

/**
* 绘制灰度图
* @param text UNICODE 文本
* @param image 指定绘制的图片
* @param align 对齐方式
* @param stretch 拉伸到图片大小并保持高宽比
*/
void CFontManager::DrawText(const wchar_t* text, CTextImage* image, Alignment align, bool stretch) {
	// 文本所占大小
	TextSize(text, &image->textWidth, &image->textHeight);
	if (!image->data) {
		image->width = image->textWidth;
		image->height = image->textHeight;
		image->data = new unsigned char[image->width * image->height];
	}
	// 检查字体和字符串
	size_t count = wcslen(text);
	if (!m_pCurrentFace || count == 0) {
		memset(image->data, 0, image->width * image->height);
		return;
	}
	// 优先读取缓存
	CTextImage* canvas = 0;
	if (CacheGet(text, canvas)) {
		SetAlignment(canvas, image, align, stretch);
		return;
	}
	canvas = new CTextImage(image->textWidth, image->textHeight);
	canvas->textWidth = image->textWidth;
	canvas->textHeight = image->textHeight;
	memset(canvas->data, 0, canvas->width * canvas->height);
	// 字符串占用宽高
	int maxWidth = 0;
	int maxHeight = 0;
	int rowWidth = 0;
	int rowHeight = static_cast<int>(m_pCurrentFace->size->metrics.height >> 6);
	for (size_t i = 0; i < count; i++) {
		// 处理换行符
		if (text[i] == (wchar_t)0x0A) {
			maxWidth = std::max(maxWidth, rowWidth);
			maxHeight += rowHeight;
			rowWidth = 0;
			continue;
		}
		// 渲染单个字符
		if (FT_Load_Char(m_pCurrentFace, text[i], FT_LOAD_RENDER)) continue;
		// 将渲染的字符复制到 canvas->data 缓冲区
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
	// 将图片缓存
	CachePut(text, canvas);
	SetAlignment(canvas, image, align, stretch);
}

/**
* 获取管理的所有字体库列表
* @param fontList 输出字体库列表
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
* 设置字符对齐
* @param src 源文本灰度图
* @param dst 目标文本灰度图
* @param align 对齐方式
* @param stretch 拉伸到图片大小
*/
void CFontManager::SetAlignment(CTextImage* src, CTextImage* dst, Alignment align, bool stretch) {
	if (stretch) {
		// 是否垂直方向拉伸到图片大小（保持宽高比）
		bool stretch_vertical = dst->textWidth * dst->height < dst->textHeight * dst->width;
		const int target_width = stretch_vertical ? dst->height * dst->textWidth / dst->textHeight : dst->width;
		const int target_height = stretch_vertical ? dst->height : dst->width * dst->textHeight / dst->textWidth;
		// 使用双线性插值缩放图片
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
				// 四个点权重
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
	// 默认左上角
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
	// 复制位图块，主要是对一块位图进行复制 (dx, dy)
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
	// 删除用来缩放创建的图片
	if (stretch) delete src;
}

/**
* 缓存渲染结果
* @param text UNICODE 文本
* @param image 文本灰度图
* @return 若缓存成功则返回true
*/
bool CFontManager::CachePut(const wchar_t* text, CTextImage* image) {
	SCacheKey key;
	key.text = text;
	key.face = m_pCurrentFace;
	key.size = m_iCurrentSize;
	SCacheEntry* entry = 0;
	// 检查是否已经存在
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
	// 将元素移动到头部
	entry->next = m_pLRUCacheHead->next;
	entry->prev = m_pLRUCacheHead;
	m_pLRUCacheHead->next = entry;
	entry->next->prev = entry;
	return true;
}

/**
* 取出缓存图片
* @param text UNICODE 文本
* @param image 文本灰度图
* @return 若读取成功则返回true
*/
bool CFontManager::CacheGet(const wchar_t* text, CTextImage*& image) {
	SCacheKey key;
	key.text = text;
	key.face = m_pCurrentFace;
	key.size = m_iCurrentSize;
	// 查找缓存
	map<SCacheKey, SCacheEntry*>::iterator iter = m_mapLRUCache.find(key);
	if (iter != m_mapLRUCache.end()) {
		SCacheEntry* entry = iter->second;
		image = entry->image;
		// 更新LRU缓存链表
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
* 清空LRU缓存
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