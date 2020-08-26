//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
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

	//! 文件数据类
	class CFileData {
	public:
		FileType type;
		unsigned int size;
		unsigned char* contents;
		CFileData(FileType type) : type(type), size(0), contents(0) {}
		CFileData(FileType type, unsigned int size) : type(type), size(size) { contents = new unsigned char[size]; }
		virtual ~CFileData() { if (contents) delete[] contents; }
	};

	//! 二进制文件
	class CBinaryFile : public CFileData {
	public:
		CBinaryFile() : CFileData(BIN) {}
		CBinaryFile(unsigned int size) : CFileData(BIN, size) {}
	};

	//! 文本文件
	class CTextFile : public CFileData {
	public:
		CTextFile() : CFileData(TXT) {}
		CTextFile(unsigned int size) : CFileData(TXT, size + 1) {}
	};

	//! 图片文件
	class CImageFile : public CFileData {
	public:
		int channels;
		int width;
		int height;
		CImageFile(FileType type) : CFileData(type), channels(0), width(0), height(0) {}
		CImageFile(FileType type, unsigned int size) : CFileData(type, size), channels(0), width(0), height(0) {}
	};

	//! 音频文件
	class CAudioFile : public CFileData {
	public:
		int channels;
		int sampleRate;
		int sampleBits;
		CAudioFile(FileType type) : CFileData(type), channels(0), sampleRate(0), sampleBits(0) {}
		CAudioFile(FileType type, unsigned int size) : CFileData(type, size), channels(0), sampleRate(0), sampleBits(0) {}
	};

public:
	//! 文件读操作
	bool ReadFile(const string& filename, CFileData* file);
	//! 文件读操作
	bool ReadFile(unsigned char* buffer, unsigned int size, CFileData* file);
	//! 文件写操作
	int WriteFile(CFileData* file, const string& filename);
	//! 文件写操作
	int WriteFile(CFileData* file, unsigned char* buffer, unsigned int size);

private:
	CFileManager();
	~CFileManager();

	//! 解析各种文件方法
	bool ParseBinFile(unsigned char* data, unsigned int size, CFileData* file);
	bool ParseTxtFile(unsigned char* data, unsigned int size, CFileData* file);
	bool ParseBmpFile(unsigned char* data, unsigned int size, CFileData* file);
	bool ParseTgaFile(unsigned char* data, unsigned int size, CFileData* file);
	bool ParsePngFile(unsigned char* data, unsigned int size, CFileData* file);
	bool ParseJpgFile(unsigned char* data, unsigned int size, CFileData* file);
	bool ParseWavFile(unsigned char* data, unsigned int size, CFileData* file);
	bool ParseMp3File(unsigned char* data, unsigned int size, CFileData* file);

	//! 各种文档的序列化
	bool SerializeBinFile(CFileData* file, unsigned char** data, unsigned int* size);
	bool SerializeTxtFile(CFileData* file, unsigned char** data, unsigned int* size);
	bool SerializeBmpFile(CFileData* file, unsigned char** data, unsigned int* size);
	bool SerializeTgaFile(CFileData* file, unsigned char** data, unsigned int* size);
	bool SerializePngFile(CFileData* file, unsigned char** data, unsigned int* size);
	bool SerializeJpgFile(CFileData* file, unsigned char** data, unsigned int* size);
	bool SerializeWavFile(CFileData* file, unsigned char** data, unsigned int* size);
	bool SerializeMp3File(CFileData* file, unsigned char** data, unsigned int* size);

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