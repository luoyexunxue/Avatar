//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* @brief �ļ�������
*/
class AVATAR_EXPORT CFileManager {
public:
	//! ��ȡ������ʵ��
	static CFileManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CFileManager();
		return m_pInstance;
	}
	//! ʵ������
	void Destroy();

public:
	//! ��ȡ����·��������'/'
	static string GetAppDirectory();
	//! ��ȡ�ļ���չ����������'.'
	static string GetExtension(const string& path);
	//! ��ȡ�ļ�·��������'/'
	static string GetDirectory(const string& path);
	//! ��ȡ�ļ�����
	static string GetFileName(const string& path, bool withExt);
	//! ����Ƿ���ȫ·��
	static bool IsFullPath(const string& path);

	//! ������ԴĿ¼
	void SetDataDirectory(const string& directory);
	//! ��ȡ��ԴĿ¼
	string GetDataDirectory();
	//! ���Ŀ¼�Ƿ����
	bool DirectoryExists(const string& directory);
	//! ����Ŀ¼
	bool DirectoryCreate(const string& directory);
	//! ����ļ��Ƿ����
	bool FileExists(const string& filename);
	//! ��ȡ�ļ���С
	unsigned int FileSize(const string& filename);
	//! ��ȡ·���������ļ�
	void GetFileList(const string& directory, vector<string>& file);

public:
	//! ֧�ֵ��ļ�����
	enum FileType { BIN, TXT, BMP, TGA, PNG, JPG, WAV, MP3 };

	//! �ļ���
	class CFile {
	public:
		FileType type;
		unsigned int size;
		unsigned char* contents;

		CFile(FileType type): type(type), size(0), contents(0) {}
		CFile(unsigned int size, FileType type): type(type), size(size) { contents = new unsigned char[size]; }
		virtual ~CFile() { if (contents) delete[] contents; }
	};

	//! �������ļ�
	class CBinaryFile: public CFile {
	public:
		CBinaryFile(): CFile(BIN) {}
		CBinaryFile(unsigned int size): CFile(size, BIN) {}
	};

	//! �ı��ļ�
	class CTextFile: public CFile {
	public:
		CTextFile(): CFile(TXT) {}
		CTextFile(unsigned int size): CFile(size + 1, TXT) {}
	};

	//! ͼƬ�ļ�
	class CImageFile: public CFile {
	public:
		int channels;
		int width;
		int height;
		CImageFile(FileType type): CFile(type), channels(0), width(0), height(0) {}
		CImageFile(unsigned int size, FileType type): CFile(size, type), channels(0), width(0), height(0) {}
	};

	//! ��Ƶ�ļ�
	class CAudioFile: public CFile {
	public:
		int channels;
		int sampleRate;
		int sampleBits;
		CAudioFile(FileType type): CFile(type), channels(0), sampleRate(0), sampleBits(0) {}
		CAudioFile(unsigned int size, FileType type): CFile(size, type), channels(0), sampleRate(0), sampleBits(0) {}
	};

public:
	//! �ļ�������
	bool ReadFile(const string& filename, CFile* file);
	//! �ļ�������
	bool ReadFile(unsigned char* buffer, unsigned int size, CFile* file);
	//! �ļ�д����
	int WriteFile(CFile* file, const string& filename);
	//! �ļ�д����
	int WriteFile(CFile* file, unsigned char* buffer, unsigned int size);

private:
	CFileManager();
	~CFileManager();

	//! ���������ļ�����
	bool ParseBinFile(CFile* file, unsigned char* data, unsigned int size);
	bool ParseTxtFile(CFile* file, unsigned char* data, unsigned int size);
	bool ParseBmpFile(CFile* file, unsigned char* data, unsigned int size);
	bool ParseTgaFile(CFile* file, unsigned char* data, unsigned int size);
	bool ParsePngFile(CFile* file, unsigned char* data, unsigned int size);
	bool ParseJpgFile(CFile* file, unsigned char* data, unsigned int size);
	bool ParseWavFile(CFile* file, unsigned char* data, unsigned int size);
	bool ParseMp3File(CFile* file, unsigned char* data, unsigned int size);

	//! �����ĵ������л�
	bool SerializeBinFile(CFile* file, unsigned char** data, unsigned int* size);
	bool SerializeTxtFile(CFile* file, unsigned char** data, unsigned int* size);
	bool SerializeBmpFile(CFile* file, unsigned char** data, unsigned int* size);
	bool SerializeTgaFile(CFile* file, unsigned char** data, unsigned int* size);
	bool SerializePngFile(CFile* file, unsigned char** data, unsigned int* size);
	bool SerializeJpgFile(CFile* file, unsigned char** data, unsigned int* size);
	bool SerializeWavFile(CFile* file, unsigned char** data, unsigned int* size);
	bool SerializeMp3File(CFile* file, unsigned char** data, unsigned int* size);

private:
	//! ����Ŀ¼����
	typedef struct _SDirectory {
		string path;
		bool isCompressed;
		bool isRemoteUrl;
	} SDirectory;

private:
	//! ��ǰ�ļ�Ŀ¼
	SDirectory* m_pCurrentDirectory;
	//! URL ����ӿ�
	class CUrlConnection* m_pUrlConnection;
	//! ZIP ��ȡ�ӿ�
	class CZipReader* m_pZipReader;
	//! ʵ��
	static CFileManager* m_pInstance;
};

#endif