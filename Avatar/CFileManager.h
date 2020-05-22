//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CFILEMANAGER_H_
#define _CFILEMANAGER_H_
#include "AvatarConfig.h"
#include <string>
#include <vector>
using std::string;
using std::vector;

/**
* @brief 文件管理器
*/
class AVATAR_EXPORT CFileManager {
public:
	//! 获取管理器实例
	static CFileManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CFileManager();
		return m_pInstance;
	}
	//! 实例销毁
	void Destroy();

public:
	//! 获取程序路径，包含'/'
	static string GetAppDirectory();
	//! 获取文件扩展名，不包含'.'
	static string GetExtension(const string& path);
	//! 获取文件路径，包含'/'
	static string GetDirectory(const string& path);
	//! 获取文件名称
	static string GetFileName(const string& path, bool withExt);
	//! 检查是否是全路径
	static bool IsFullPath(const string& path);

	//! 设置资源目录
	void SetDataDirectory(const string& directory);
	//! 获取资源目录
	string GetDataDirectory();
	//! 检查目录是否存在
	bool DirectoryExists(const string& directory);
	//! 创建目录
	bool DirectoryCreate(const string& directory);
	//! 检查文件是否存在
	bool FileExists(const string& filename);
	//! 获取文件大小
	unsigned int FileSize(const string& filename);
	//! 获取路径下所有文件
	void GetFileList(const string& directory, vector<string>& file);

public:
	//! 支持的文件类型
	enum FileType { BIN, TXT, BMP, TGA, PNG, JPG, WAV, MP3 };

	//! 文件类
	class CFile {
	public:
		FileType type;
		unsigned int size;
		unsigned char* contents;

		CFile(FileType type): type(type), size(0), contents(0) {}
		CFile(unsigned int size, FileType type): type(type), size(size) { contents = new unsigned char[size]; }
		virtual ~CFile() { if (contents) delete[] contents; }
	};

	//! 二进制文件
	class CBinaryFile: public CFile {
	public:
		CBinaryFile(): CFile(BIN) {}
		CBinaryFile(unsigned int size): CFile(size, BIN) {}
	};

	//! 文本文件
	class CTextFile: public CFile {
	public:
		CTextFile(): CFile(TXT) {}
		CTextFile(unsigned int size): CFile(size + 1, TXT) {}
	};

	//! 图片文件
	class CImageFile: public CFile {
	public:
		int channels;
		int width;
		int height;
		CImageFile(FileType type): CFile(type), channels(0), width(0), height(0) {}
		CImageFile(unsigned int size, FileType type): CFile(size, type), channels(0), width(0), height(0) {}
	};

	//! 音频文件
	class CAudioFile: public CFile {
	public:
		int channels;
		int sampleRate;
		int sampleBits;
		CAudioFile(FileType type): CFile(type), channels(0), sampleRate(0), sampleBits(0) {}
		CAudioFile(unsigned int size, FileType type): CFile(size, type), channels(0), sampleRate(0), sampleBits(0) {}
	};

public:
	//! 文件读操作
	bool ReadFile(const string& filename, CFile* file);
	//! 文件读操作
	bool ReadFile(unsigned char* buffer, unsigned int size, CFile* file);
	//! 文件写操作
	int WriteFile(CFile* file, const string& filename);
	//! 文件写操作
	int WriteFile(CFile* file, unsigned char* buffer, unsigned int size);

private:
	CFileManager();
	~CFileManager();

	//! 解析各种文件方法
	bool ParseBinFile(CFile* file, unsigned char* data, unsigned int size);
	bool ParseTxtFile(CFile* file, unsigned char* data, unsigned int size);
	bool ParseBmpFile(CFile* file, unsigned char* data, unsigned int size);
	bool ParseTgaFile(CFile* file, unsigned char* data, unsigned int size);
	bool ParsePngFile(CFile* file, unsigned char* data, unsigned int size);
	bool ParseJpgFile(CFile* file, unsigned char* data, unsigned int size);
	bool ParseWavFile(CFile* file, unsigned char* data, unsigned int size);
	bool ParseMp3File(CFile* file, unsigned char* data, unsigned int size);

	//! 各种文档的序列化
	bool SerializeBinFile(CFile* file, unsigned char** data, unsigned int* size);
	bool SerializeTxtFile(CFile* file, unsigned char** data, unsigned int* size);
	bool SerializeBmpFile(CFile* file, unsigned char** data, unsigned int* size);
	bool SerializeTgaFile(CFile* file, unsigned char** data, unsigned int* size);
	bool SerializePngFile(CFile* file, unsigned char** data, unsigned int* size);
	bool SerializeJpgFile(CFile* file, unsigned char** data, unsigned int* size);
	bool SerializeWavFile(CFile* file, unsigned char** data, unsigned int* size);
	bool SerializeMp3File(CFile* file, unsigned char** data, unsigned int* size);

private:
	//! 数据目录定义
	typedef struct _SDirectory {
		string path;
		bool isCompressed;
		bool isRemoteUrl;
	} SDirectory;

private:
	//! 当前文件目录
	SDirectory* m_pCurrentDirectory;
	//! URL 请求接口
	class CUrlConnection* m_pUrlConnection;
	//! ZIP 读取接口
	class CZipReader* m_pZipReader;
	//! 实例
	static CFileManager* m_pInstance;
};

#endif