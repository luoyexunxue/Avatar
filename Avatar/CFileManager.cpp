//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CFileManager.h"
#include "AvatarConfig.h"
#include "CLog.h"
#include "CZipReader.h"
#include "CUrlConnection.h"
#include <cstring>
#include <fstream>
#include <vector>
#include <cstdint>
#ifdef AVATAR_WINDOWS
#include <Windows.h>
#include "thirdparty/libjpeg/jpeglib.h"
#pragma comment(lib, "thirdparty/zlib/zlib.lib")
#pragma comment(lib, "thirdparty/libpng/libpng16.lib")
#pragma comment(lib, "thirdparty/libjpeg/jpeg.lib")
#else
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include "thirdparty/libjpeg/jpeglib_linux.h"
#endif
#define MINIMP3_IMPLEMENTATION
#include "thirdparty/libpng/png.h"
#include "thirdparty/minimp3/minimp3_ex.h"
using std::vector;
using std::ifstream;
using std::ofstream;

/**
* ���캯��
*/
CFileManager::CFileManager() {
	m_pCurrentDirectory = new SDirectory();
	m_pCurrentDirectory->path = "";
	m_pCurrentDirectory->isCompressed = false;
	m_pCurrentDirectory->isRemoteUrl = false;
	m_pUrlConnection = new CUrlConnection();
	m_pUrlConnection->SetTimeout(600);
	m_pZipReader = 0;
}

/**
* ��������
*/
CFileManager::~CFileManager() {
	m_pInstance = 0;
}

/**
* ����ʵ��
*/
CFileManager* CFileManager::m_pInstance = 0;

/**
* �����ļ�������
*/
void CFileManager::Destroy() {
	delete m_pCurrentDirectory;
	delete m_pUrlConnection;
	if (m_pZipReader) delete m_pZipReader;
	delete this;
}

/**
* ��ȡ����·��
* @return ��ִ�г���·�������� '/'
*/
string CFileManager::GetAppDirectory() {
#ifdef AVATAR_WINDOWS
	char strPath[MAX_PATH];
	if (GetModuleFileNameA(NULL, strPath, MAX_PATH)) {
		return GetDirectory(strPath);
	}
	return "";
#else
	char strPath[255];
	if (getcwd(strPath, 255)) {
		return string(strPath).append("/");
	}
	return "";
#endif
}

/**
* ��ȡ�ļ���չ��
* @param path ·����
* @return ��չ���������� '.'
*/
string CFileManager::GetExtension(const string& path) {
	size_t dot = path.find_last_of('.');
	size_t slash = path.find_last_of("/\\");
	if (dot != string::npos && (slash == string::npos || slash < dot)) {
		return path.substr(dot + 1);
	} else {
		return "";
	}
}

/**
* ��ȡ�ļ�����Ŀ¼
* @param path ·����
* @return �ļ�Ŀ¼������ '/'
*/
string CFileManager::GetDirectory(const string& path) {
	size_t slash = path.find_last_of("/\\");
	if (slash != string::npos) {
		return path.substr(0, slash + 1);
	} else {
		return "";
	}
}


/**
* ��ȡ�ļ�����
* @param path ·����
* @param withExt �Ƿ������չ��
* @return �ļ�����
*/
string CFileManager::GetFileName(const string& path, bool withExt) {
	string name = path;
	size_t slash = path.find_last_of("/\\");
	if (slash != string::npos) {
		name.assign(path.substr(slash + 1));
	}
	if (!withExt) {
		size_t dot = name.find_last_of('.');
		if (dot != string::npos) {
			name.assign(name.substr(0, dot));
		}
	}
	return name;
}

/**
* ����Ƿ���ȫ·��
* @param path ·����
* @return ��� path ��ʾ����·���򷵻� true
*/
bool CFileManager::IsFullPath(const string& path) {
	if (path.empty()) return false;
	if (path.find("http://") == 0) return true;
	if (path.find("https://") == 0) return true;
	if (path.find("ftp://") == 0) return true;
	if (path.find("ftps://") == 0) return true;
#ifdef AVATAR_WINDOWS
	if (path.find(':') == 1) return true;
	return false;
#else
	if (path.find('/') == 0) return true;
	return false;
#endif
}

/**
* ������ԴĿ¼
* @param directory ��ǰ��ԴĿ¼�������趨Ϊ ZIP �ļ��� URL ��ַ
* @note ���趨Ϊ ZIP �ļ�ʱ��GetDataDirectory ���� ZIP �ļ�����Ŀ¼������������ý�ѹ����(���� C:\\media.zip?password)
*/
void CFileManager::SetDataDirectory(const string& directory) {
	m_pCurrentDirectory->path = "";
	m_pCurrentDirectory->isCompressed = false;
	m_pCurrentDirectory->isRemoteUrl = false;
	// directory Ϊ URL ·��
	if (directory.find("http://") == 0 ||
		directory.find("https://") == 0 ||
		directory.find("ftp://") == 0 ||
		directory.find("ftps://") == 0) {
		m_pCurrentDirectory->path = directory;
		m_pCurrentDirectory->isRemoteUrl = true;
		if (directory.at(directory.length() - 1) != '/') {
			m_pCurrentDirectory->path += "/";
		}
	}
	// directory Ϊ ZIP �ļ�
	else if (directory.find(".zip") != string::npos) {
		size_t index = directory.rfind('?');
		string filepath = index != string::npos? directory.substr(0, index): directory;
		string password = index != string::npos? directory.substr(index + 1): "";
		if (!FileExists(filepath)) CLog::Error("Data directory not exist");
		if (m_pZipReader) delete m_pZipReader;
		m_pZipReader = new CZipReader(filepath, password);
		m_pCurrentDirectory->path = GetDirectory(filepath);
		m_pCurrentDirectory->isCompressed = true;
		for (size_t i = 0; i < m_pCurrentDirectory->path.size(); i++) {
			if (m_pCurrentDirectory->path[i] == '\\') m_pCurrentDirectory->path[i] = '/';
		}
	}
	// directory Ϊ����Ŀ¼
	else {
		if (!DirectoryExists(directory)) CLog::Error("Data directory not exist");
		m_pCurrentDirectory->path = directory;
		for (size_t i = 0; i < m_pCurrentDirectory->path.size(); i++) {
			if (m_pCurrentDirectory->path[i] == '\\') m_pCurrentDirectory->path[i] = '/';
		}
		if (m_pCurrentDirectory->path.back() != '/') {
			m_pCurrentDirectory->path += "/";
		}
	}
}

/**
* ��ȡ��ԴĿ¼
* @return ��ǰ��ԴĿ¼
*/
string CFileManager::GetDataDirectory() {
	return m_pCurrentDirectory->path;
}

/**
* �ж�Ŀ¼�Ƿ����
* @param directory ��Ҫ����Ŀ¼��
* @return ��ָ����Ŀ¼�����򷵻� true
*/
bool CFileManager::DirectoryExists(const string& directory) {
	if (directory.empty() || m_pCurrentDirectory->isRemoteUrl) return true;
#ifdef AVATAR_WINDOWS
	if (IsFullPath(directory)) {
		return GetFileAttributesA(directory.c_str()) == FILE_ATTRIBUTE_DIRECTORY;
	} else {
		return GetFileAttributesA((m_pCurrentDirectory->path + directory).c_str()) == FILE_ATTRIBUTE_DIRECTORY;
	}
#else
	string unifyDir = directory;
	if (directory.at(directory.length() - 1) != '/') unifyDir += "/";
	if (IsFullPath(unifyDir)) {
		return access(unifyDir.c_str(), F_OK) == 0;
	} else {
		return access((m_pCurrentDirectory->path + unifyDir).c_str(), F_OK) == 0;
	}
#endif
}

/**
* ����Ŀ¼
* @param directory ��Ҫ������Ŀ¼��
* @return �����ɹ����� true
*/
bool CFileManager::DirectoryCreate(const string& directory) {
	string fullpath = directory;
	if (!IsFullPath(directory)) {
		fullpath = m_pCurrentDirectory->path + directory;
	}
#ifdef AVATAR_WINDOWS
	DWORD ret = GetFileAttributesA(fullpath.c_str());
	if (!(ret & FILE_ATTRIBUTE_DIRECTORY) || ret == INVALID_FILE_ATTRIBUTES) {
		return CreateDirectoryA(fullpath.c_str(), NULL) != 0;
	}
	return true;
#else
	if (directory.at(directory.length() - 1) != '/') fullpath += "/";
	if (access(fullpath.c_str(), F_OK) != 0) {
		if (errno == ENOTDIR) return false;
		return mkdir(fullpath.c_str(), 0777) == 0;
	}
	return true;
#endif
}

/**
* �ж��ļ��Ƿ����
* @param filename ��Ҫ�����ļ�
* @return �������򷵻� true
*/
bool CFileManager::FileExists(const string& filename) {
#ifdef AVATAR_WINDOWS
	DWORD ret = INVALID_FILE_ATTRIBUTES;
	if (IsFullPath(filename)) ret = GetFileAttributesA(filename.c_str());
	else ret = GetFileAttributesA((m_pCurrentDirectory->path + filename).c_str());
	return ret != INVALID_FILE_ATTRIBUTES && !(ret & FILE_ATTRIBUTE_DIRECTORY);
#else
	int ret = -1;
	if (IsFullPath(filename)) ret = access(filename.c_str(), F_OK);
	else ret = access((m_pCurrentDirectory->path + filename).c_str(), F_OK);
	return ret == 0;
#endif
}

/**
* ��ȡ�ļ���С
* @param filename ��Ҫ�����ļ�
* @return �ļ���С
*/
unsigned int CFileManager::FileSize(const string& filename) {
	unsigned int size = 0;
	// ��ȫ·��
	bool isFullPath = IsFullPath(filename);
	string fullpath = filename;
	if (!isFullPath && !m_pCurrentDirectory->isCompressed) {
		fullpath = m_pCurrentDirectory->path + filename;
	}
	// URL Դ
	if (fullpath.find("http://") == 0 ||
		fullpath.find("https://") == 0 ||
		fullpath.find("ftp://") == 0 ||
		fullpath.find("ftps://") == 0) {
		vector<unsigned char> buffer;
		if (m_pUrlConnection->Request(fullpath, buffer) != 200) return 0;
		size = buffer.size();
	}
	// ZIP Դ
	else if (!isFullPath && m_pCurrentDirectory->isCompressed && m_pZipReader->Open(fullpath)) {
		size = m_pZipReader->Size();
		m_pZipReader->Close();
	}
	// ����Դ
	else {
		if (!isFullPath) fullpath = m_pCurrentDirectory->path + filename;
		ifstream fin(fullpath, std::ios::in | std::ios::ate | std::ios::binary);
		if (!fin.is_open()) return 0;
		size = (unsigned int)fin.tellg();
		fin.close();
	}
	return size;
}

/**
* ��ȡָ��Ŀ¼�������ļ�
* @param directory ��Ҫ�г��ļ���Ŀ¼
* @param file �ļ��б�
*/
void CFileManager::GetFileList(const string& directory, vector<string>& file) {
	string fullpath = directory;
	if (!IsFullPath(directory)) fullpath = m_pCurrentDirectory->path + directory;
	if (fullpath.back() == '\\' || fullpath.back() == '/') fullpath.append("*");
	else fullpath.append("/*");
	file.clear();
	// ��ȡ�ļ��б�
#ifdef AVATAR_WINDOWS
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = FindFirstFileA(fullpath.c_str(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) return;
	while (true) {
		if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) file.push_back(FindFileData.cFileName);
		if (!FindNextFileA(hFind, &FindFileData)) break;
	}
	FindClose(hFind);
#else
	DIR* dir = opendir(fullpath.c_str());
	if (!dir) return;
	struct dirent* pdirent;
	while (!(pdirent = readdir(dir))) {
		if (pdirent->d_type == DT_REG) file.push_back(pdirent->d_name);
	}
	closedir(dir);
#endif
}

/**
* ��ȡ�ļ�
* @param filename ��ȡ���ļ���
* @param file �ļ�����ָ��
* @return ��ȡ�ɹ����� true
*/
bool CFileManager::ReadFile(const string& filename, CFile* file) {
	unsigned int size = 0;
	unsigned char* data = 0;

	// ����ļ�ԭ����
	if (file->contents) {
		delete[] file->contents;
		file->contents = 0;
		file->size = 0;
	}
	// ��ȫ·��
	bool isFullPath = IsFullPath(filename);
	string fullpath = filename;
	if (!isFullPath && !m_pCurrentDirectory->isCompressed) {
		fullpath = m_pCurrentDirectory->path + filename;
	}
	// URL Դ
	if (fullpath.find("http://") == 0 ||
		fullpath.find("https://") == 0 ||
		fullpath.find("ftp://") == 0 ||
		fullpath.find("ftps://") == 0) {
		vector<unsigned char> buffer;
		if (m_pUrlConnection->Request(fullpath, buffer) != 200) {
			CLog::Warn("Could not read file from %s", fullpath.c_str());
			return false;
		}
		size = buffer.size();
		data = new unsigned char[size];
		memcpy(data, &buffer[0], size);
	}
	// ZIP Դ
	else if (!isFullPath && m_pCurrentDirectory->isCompressed && m_pZipReader->Open(fullpath)) {
		size = m_pZipReader->Size();
		data = new unsigned char[size];
		m_pZipReader->Read(data, size);
		m_pZipReader->Close();
	}
	// ����Դ
	else {
		if (!isFullPath) fullpath = m_pCurrentDirectory->path + filename;
		ifstream fin(fullpath, std::ios::in | std::ios::ate | std::ios::binary);
		if (!fin.is_open()) {
			CLog::Warn("Could not read file from %s", fullpath.c_str());
			return false;
		}
		size = (unsigned int)fin.tellg();
		data = new unsigned char[size];
		fin.seekg(0, std::ios::beg);
		fin.read((char*)data, size);
		fin.close();
	}
	// ���ݲ�ͬ���ļ����ͽ��н���
	bool ret = false;
	switch (file->type) {
	case BIN: ret = ParseBinFile(file, data, size); break;
	case TXT: ret = ParseTxtFile(file, data, size); break;
	case BMP: ret = ParseBmpFile(file, data, size); break;
	case TGA: ret = ParseTgaFile(file, data, size); break;
	case PNG: ret = ParsePngFile(file, data, size); break;
	case JPG: ret = ParseJpgFile(file, data, size); break;
	case WAV: ret = ParseWavFile(file, data, size); break;
	case MP3: ret = ParseMp3File(file, data, size); break;
	}
	delete[] data;
	return ret;
}

/**
* �ӻ�������ȡ�ļ�
* @param buffer ��ȡ�Ļ�����ָ��
* @param size ��ȡ�Ļ�������С
* @param file �ļ�����ָ��
* @return ��ȡ�ɹ����� true
*/
bool CFileManager::ReadFile(unsigned char* buffer, unsigned int size, CFile* file) {
	// ����ļ�ԭ����
	if (file->contents) {
		delete[] file->contents;
		file->contents = 0;
		file->size = 0;
	}
	// ���ݲ�ͬ���ļ����ͽ��н���
	bool ret = false;
	switch (file->type) {
	case BIN: ret = ParseBinFile(file, buffer, size); break;
	case TXT: ret = ParseTxtFile(file, buffer, size); break;
	case BMP: ret = ParseBmpFile(file, buffer, size); break;
	case TGA: ret = ParseTgaFile(file, buffer, size); break;
	case PNG: ret = ParsePngFile(file, buffer, size); break;
	case JPG: ret = ParseJpgFile(file, buffer, size); break;
	case WAV: ret = ParseWavFile(file, buffer, size); break;
	case MP3: ret = ParseMp3File(file, buffer, size); break;
	}
	return ret;
}

/**
* д���ļ�
* @param file �ļ�����ָ��
* @param filename д����ļ���
* @return д���ֽ���
*/
int CFileManager::WriteFile(CFile* file, const string& filename) {
	unsigned int size = 0;
	unsigned char* data = 0;

	// ����ļ������Ƿ�Ϊ��
	if (!file->contents) {
		return 0;
	}
	// ���ݲ�ͬ���ļ����ͽ������л�
	bool ret = false;
	switch (file->type) {
	case BIN: ret = SerializeBinFile(file, &data, &size); break;
	case TXT: ret = SerializeTxtFile(file, &data, &size); break;
	case BMP: ret = SerializeBmpFile(file, &data, &size); break;
	case TGA: ret = SerializeTgaFile(file, &data, &size); break;
	case PNG: ret = SerializePngFile(file, &data, &size); break;
	case JPG: ret = SerializeJpgFile(file, &data, &size); break;
	case WAV: ret = SerializeWavFile(file, &data, &size); break;
	case MP3: ret = SerializeMp3File(file, &data, &size); break;
	}
	if (!ret) return 0;
	// ��������·��
	string fullpath = filename;
	if (!IsFullPath(filename) && !m_pCurrentDirectory->isRemoteUrl) {
		fullpath = m_pCurrentDirectory->path + filename;
	}
	// д���ļ�����
	ofstream fout(fullpath, std::ios::out | std::ios::binary | std::ios::trunc);
	if (fout.is_open()) {
		fout.write((char*)data, size);
		fout.close();
		delete[] data;
		return size;
	} else CLog::Warn("Could not write file to %s", fullpath.c_str());
	delete[] data;
	return 0;
}

/**
* д���ļ���������
* @param file �ļ�����ָ��
* @param buffer д��Ļ�����ָ��
* @param size д��Ļ�������С
* @return д���ֽ���
*/
int CFileManager::WriteFile(CFile* file, unsigned char* buffer, unsigned int size) {
	unsigned int length = 0;
	unsigned char* data = 0;

	// ����ļ������Ƿ�Ϊ��
	if (!file->contents) {
		return 0;
	}
	// ���ݲ�ͬ���ļ����ͽ������л�
	bool ret = false;
	switch (file->type) {
	case BIN: ret = SerializeBinFile(file, &data, &length); break;
	case TXT: ret = SerializeTxtFile(file, &data, &length); break;
	case BMP: ret = SerializeBmpFile(file, &data, &length); break;
	case TGA: ret = SerializeTgaFile(file, &data, &length); break;
	case PNG: ret = SerializePngFile(file, &data, &length); break;
	case JPG: ret = SerializeJpgFile(file, &data, &length); break;
	case WAV: ret = SerializeWavFile(file, &data, &length); break;
	case MP3: ret = SerializeMp3File(file, &data, &length); break;
	}
	if (!ret) return 0;
	if (length > size) length = size;
	// д���ļ�����
	memcpy(buffer, data, length);
	delete[] data;
	return length;
}

/**
* ���� BIN �ļ�
* @param file �ļ�����ָ��
* @param data ����������
* @param size ���ݴ�С
* @return �ɹ��򷵻� true
*/
bool CFileManager::ParseBinFile(CFile* file, unsigned char* data, unsigned int size) {
	file->size = size;
	file->contents = new unsigned char[size];
	memcpy(file->contents, data, size);
	return true;
}

/**
* ���� TXT �ļ�
* @param file �ļ�����ָ��
* @param data �ı�����
* @param size ���ݴ�С
* @return �ɹ��򷵻� true
*/
bool CFileManager::ParseTxtFile(CFile* file, unsigned char* data, unsigned int size) {
	file->size = size + 1;
	file->contents = new unsigned char[size + 1];
	file->contents[size] = 0;
	memcpy(file->contents, data, size);
	return true;
}

/**
* ���� BMP �ļ�
* @param file �ļ�����ָ��
* @param data BMP ͼƬ����
* @param size ���ݴ�С
* @return �ɹ��򷵻� true
*/
bool CFileManager::ParseBmpFile(CFile* file, unsigned char* data, unsigned int size) {
	// ������ݳ���
	if (size <= 0x36) return false;
	uint32_t offBits = *(uint32_t*)(data + 0x0A);
	uint32_t width = *(uint32_t*)(data + 0x12);
	uint32_t height = *(uint32_t*)(data + 0x16);
	uint16_t bitCount = *(uint16_t*)(data + 0x1C);
	uint32_t compression = *(uint32_t*)(data + 0x1E);
	uint32_t clrUsed = *(uint32_t*)(data + 0x2E);

	// ��֧��ѹ����ʽ
	if (compression) return false;
	CImageFile* pImage = static_cast<CImageFile*>(file);
	// ֧�ִ���ɫ���256ɫλͼ
	if (bitCount == 8 && clrUsed > 0) pImage->channels = 3;
	else if (bitCount == 8) pImage->channels = 1;
	else if (bitCount == 24) pImage->channels = 3;
	else if (bitCount == 32) pImage->channels = 4;
	else return false;

	int byteCount = bitCount >> 3;
	pImage->width = width;
	pImage->height = height;
	pImage->size = pImage->height * pImage->width * pImage->channels;

	int position = offBits;
	int rowExtra = 3 - (width * byteCount - 1) % 4;
	int dataSize = height * (width * byteCount + rowExtra);
	if (static_cast<int>(size) - position < dataSize) return false;

	// ��ȡ����
	pImage->contents = new unsigned char[pImage->size];
	if (clrUsed == 0) {
		for (int h = 0; h < pImage->height; h++) {
			for (int w = 0; w < pImage->width; w++) {
				// ע�� BMP �ǵ��õ�
				int srcIndex = pImage->channels * (pImage->width * h + w) + rowExtra * h + position;
				int dstIndex = pImage->channels * (pImage->width * (pImage->height - h - 1) + w);
				memcpy(pImage->contents + dstIndex, data + srcIndex, pImage->channels);
				// BGR ת��Ϊ RGB
				if (pImage->channels > 2) {
					unsigned char temp = pImage->contents[dstIndex];
					pImage->contents[dstIndex] = pImage->contents[dstIndex + 2];
					pImage->contents[dstIndex + 2] = temp;
				}
			}
		}
	} else {
		// ��ɫ������
		for (int h = 0; h < pImage->height; h++) {
			for (int w = 0; w < pImage->width; w++) {
				int srcIndex = data[pImage->width * h + w + rowExtra * h + position] * 4 + 0x36;
				int dstIndex = pImage->channels * (pImage->width * (pImage->height - h - 1) + w);
				pImage->contents[dstIndex + 0] = data[srcIndex + 2];
				pImage->contents[dstIndex + 1] = data[srcIndex + 1];
				pImage->contents[dstIndex + 2] = data[srcIndex + 0];
			}
		}
	}
	return true;
}

/**
* ���� TGA �ļ�
* @param file �ļ�����ָ��
* @param data TGA ͼƬ����
* @param size ���ݴ�С
* @return �ɹ��򷵻� true
*/
bool CFileManager::ParseTgaFile(CFile* file, unsigned char* data, unsigned int size) {
	// ֧�ֵ� TGA �ļ�ͷ
	const unsigned char tag_header[10] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0 };
	const unsigned char tag_header_rle[10] = { 0, 0, 10, 0, 0, 0, 0, 0, 0, 0 };
	if (size < 18) return false;
	if (memcmp(tag_header, data, 10) && memcmp(tag_header_rle, data, 10)) return false;

	bool compressed = (data[2] == 0x0A);
	bool upsidedown = ((data[17] & 0x20) == 0x00);
	const unsigned char* header = data + 12;
	const unsigned char* content = data + 18;
	CImageFile* pImage = static_cast<CImageFile*>(file);
	pImage->width = header[1] * 256 + header[0];
	pImage->height = header[3] * 256 + header[2];
	if (header[2] == data[10] && header[3] == data[11]) upsidedown = !upsidedown;

	// ֻ֧�� 24 λ 32 λͼ
	if (header[4] == 24) pImage->channels = 3;
	else if (header[4] == 32) pImage->channels = 4;
	else return false;

	// ������������
	pImage->size = pImage->width * pImage->height * pImage->channels;
	if (!compressed && size - 18 < pImage->size) return false;
	pImage->contents = new unsigned char[pImage->size];

	// RLE ѹ������
	if (compressed) {
		unsigned int srcIndex = 0;
		unsigned int dstIndex = 0;
		while (dstIndex < pImage->size) {
			bool repeat = (content[srcIndex] & 0x80) != 0x00;
			int pixelCount = (content[srcIndex] & 0x7F) + 1;
			srcIndex += 1;
			if (repeat) {
				unsigned char pixel[4];
				memcpy(pixel, content + srcIndex, pImage->channels);
				srcIndex += pImage->channels;
				for (int i = 0; i < pixelCount; i++) {
					memcpy(pImage->contents + dstIndex, pixel, pImage->channels);
					dstIndex += pImage->channels;
				}
			} else {
				int byteCount = pImage->channels * pixelCount;
				memcpy(pImage->contents + dstIndex, content + srcIndex, byteCount);
				srcIndex += byteCount;
				dstIndex += byteCount;
			}
		}
	} else {
		memcpy(pImage->contents, content, pImage->size);
	}
	// BGR->RGB ��ɫת��
	for (int h = 0; h < pImage->height; h++) {
		for (int w = 0; w < pImage->width; w++) {
			int dstIndex = pImage->channels * (pImage->width * h + w);
			unsigned char temp = pImage->contents[dstIndex];
			pImage->contents[dstIndex] = pImage->contents[dstIndex + 2];
			pImage->contents[dstIndex + 2] = temp;
		}
	}
	// ԭ���Ƿ������½�
	if (upsidedown) {
		const int halfHeight = pImage->height >> 1;
		const int lineSize = pImage->width * pImage->channels;
		unsigned char* line = new unsigned char[lineSize];
		for (int y = 0; y < halfHeight; y++) {
			const int index1 = lineSize * y;
			const int index2 = lineSize * (pImage->height - 1 - y);
			memcpy(line, pImage->contents + index1, lineSize);
			memcpy(pImage->contents + index1, pImage->contents + index2, lineSize);
			memcpy(pImage->contents + index2, line, lineSize);
		}
		delete[] line;
	}
	return true;
}

/**
* PNG ����ȡ�ص�����
*/
void pngStreamRead(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
	struct SParam {
		unsigned char* ptr;
		unsigned int size;
	};
	SParam* ptr = (SParam*)png_get_io_ptr(png_ptr);
	if (ptr != NULL && ptr->size >= byteCountToRead) {
		memcpy(outBytes, ptr->ptr, byteCountToRead);
		ptr->ptr  += byteCountToRead;
		ptr->size -= byteCountToRead;
	}
	else if(ptr != NULL) {
		memcpy(outBytes, ptr->ptr, byteCountToRead);
		ptr->ptr += ptr->size;
		ptr->size = 0;
	}
}

/**
* ���� PNG �ļ�
* @param file �ļ�����ָ��
* @param data PNG ͼƬ����
* @param size ���ݴ�С
* @return �ɹ��򷵻� true
*/
bool CFileManager::ParsePngFile(CFile* file, unsigned char* data, unsigned int size) {
	png_uint_32 i, width, height, rowbytes;
	int bit_depth, color_type, colorChannels;
	png_bytep* row_pointers;

	if (!png_check_sig(data, 8)) return false;
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) return false;
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		return false;
	}

	struct SParam {
		unsigned char* ptr;
		unsigned int size;
	} streamPtr;

	streamPtr.ptr  = data;
	streamPtr.size = size;
	streamPtr.ptr += 8;
	streamPtr.size -= 8;
	png_set_sig_bytes(png_ptr, 8);
	png_set_read_fn(png_ptr, &streamPtr, pngStreamRead);

	png_infop end_ptr = png_create_info_struct(png_ptr);
	if (!end_ptr) {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
		return false;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_ptr);
		return false;
	}

	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);
	if (bit_depth != 8) {
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_ptr);
		return false;
	}
	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_expand(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) png_set_gray_to_rgb(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_expand(png_ptr);

	png_read_update_info(png_ptr, info_ptr);
	rowbytes = png_get_rowbytes(png_ptr, info_ptr);
	colorChannels = static_cast<int>(png_get_channels(png_ptr, info_ptr));

	CImageFile* pImage = static_cast<CImageFile*>(file);
	pImage->channels = colorChannels;
	pImage->height = height;
	pImage->width = width;
	pImage->size = rowbytes * height;
	pImage->contents = new unsigned char[pImage->size];
	row_pointers = new png_bytep[height];

	for (i = 0; i < height; i++) {
		row_pointers[i] = pImage->contents + i * rowbytes;
	}
	png_read_image(png_ptr, row_pointers);
	png_read_end(png_ptr, NULL);
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_ptr);
	delete[] row_pointers;
	return true;
}

/**
* ���� JPG �ļ�
* @param file �ļ�����ָ��
* @param data JPG ͼƬ����
* @param size ���ݴ�С
* @return �ɹ��򷵻� true
*/
bool CFileManager::ParseJpgFile(CFile* file, unsigned char* data, unsigned int size) {
	struct jpeg_decompress_struct info;
	struct jpeg_error_mgr err;

	info.err = jpeg_std_error(&err);
	jpeg_create_decompress(&info);
	jpeg_mem_src(&info, data, size);
	jpeg_read_header(&info, TRUE);

	CImageFile* pImage = static_cast<CImageFile*>(file);
	pImage->width = info.image_width;
	pImage->height = info.image_height;
	pImage->channels = info.num_components;
	pImage->size = info.image_width * info.image_height * info.num_components;
	pImage->contents = new unsigned char[pImage->size];

	jpeg_start_decompress(&info);

	unsigned char* p = pImage->contents;
	while (info.output_scanline < info.output_height) {
		int numlines = jpeg_read_scanlines(&info, &p, 1);
		p += numlines * info.output_width * info.num_components;
	}

	jpeg_finish_decompress(&info);
	jpeg_destroy_decompress(&info);
	return true;
}

/**
* ���� WAV �ļ�
* @param file �ļ�����ָ��
* @param data WAV ��Ƶ����
* @param size ���ݴ�С
* @return �ɹ��򷵻� true
*/
bool CFileManager::ParseWavFile(CFile* file, unsigned char* data, unsigned int size) {
	// ���θ�ʽ�ļ�ͷ
	struct FmtHeader {
		int8_t fmtId[4];
		uint32_t fmtSize;
		uint16_t wavFormat;
		uint16_t channel;
		uint32_t sampleRate;
		uint32_t byteRate;
		uint16_t blockAlign;
		uint16_t bitsPerSample;
	} fmtHeader;

	// ������ݳ���
	if (size <= 0x24) return false;

	fmtHeader.fmtId[0] = *(int8_t*)(data + 0x0C);
	fmtHeader.fmtId[1] = *(int8_t*)(data + 0x0D);
	fmtHeader.fmtId[2] = *(int8_t*)(data + 0x0E);
	fmtHeader.fmtId[3] = *(int8_t*)(data + 0x0F);
	fmtHeader.fmtSize = *(uint32_t*)(data + 0x10);
	fmtHeader.wavFormat = *(uint16_t*)(data + 0x14);
	fmtHeader.channel = *(uint16_t*)(data + 0x16);
	fmtHeader.sampleRate = *(uint32_t*)(data + 0x18);
	fmtHeader.byteRate = *(uint32_t*)(data + 0x1C);
	fmtHeader.blockAlign = *(uint16_t*)(data + 0x20);
	fmtHeader.bitsPerSample = *(uint16_t*)(data + 0x22);

	// �Ƿ�Ϊ��Ч WAV �ļ�
	if (fmtHeader.fmtId[0] != 'f' || fmtHeader.fmtId[1] != 'm' || fmtHeader.fmtId[2] != 't') {
		return false;
	}
	// ��֧������ PCM ����
	if (fmtHeader.wavFormat != 1) {
		return false;
	}
	unsigned int ulBufferSize = *(uint32_t*)(data + 0x28);
	unsigned char* pWaveData = data + 0x2C;
	if (size < ulBufferSize + 0x2C) return false;

	CAudioFile* pAudio = static_cast<CAudioFile*>(file);
	pAudio->channels = fmtHeader.channel;
	pAudio->sampleRate = fmtHeader.sampleRate;
	pAudio->sampleBits = fmtHeader.bitsPerSample;
	pAudio->size = ulBufferSize;
	pAudio->contents = new unsigned char[pAudio->size];
	memcpy(pAudio->contents, pWaveData, pAudio->size);
	return true;
}

/**
* ���� MP3 �ļ�
* @param file �ļ�����ָ��
* @param data MP3 ��Ƶ����
* @param size ���ݴ�С
* @return �ɹ��򷵻� true
*/
bool CFileManager::ParseMp3File(CFile* file, unsigned char* data, unsigned int size) {
	// ��ʼ�� minimp3 ��
	mp3dec_t mp3d;
	mp3dec_file_info_t info;
	mp3dec_init(&mp3d);
	mp3dec_load_buf(&mp3d, data, size, &info, 0, 0);
	// ������Ƶ����
	CAudioFile* pAudio = static_cast<CAudioFile*>(file);
	pAudio->channels = info.channels;
	pAudio->sampleRate = info.hz;
	pAudio->sampleBits = sizeof(mp3d_sample_t) * 8;
	pAudio->size = info.samples * sizeof(mp3d_sample_t);
	pAudio->contents = new unsigned char[pAudio->size];
	memcpy(pAudio->contents, info.buffer, pAudio->size);
	free(info.buffer);
	return true;
}

/**
* ���л�Ϊ BIN ��ʽ
* @param file �ļ�����ָ��
* @param data ������л�����
* @param size ������ݴ�С
* @return �ɹ��򷵻� true
*/
bool CFileManager::SerializeBinFile(CFile* file, unsigned char** data, unsigned int* size){
	*size = file->size;
	*data = new unsigned char[file->size];
	memcpy(*data, file->contents, file->size);
	return true;
}

/**
* ���л�Ϊ TXT ��ʽ
* @param file �ļ�����ָ��
* @param data ������л�����
* @param size ������ݴ�С
* @return �ɹ��򷵻� true
*/
bool CFileManager::SerializeTxtFile(CFile* file, unsigned char** data, unsigned int* size) {
	unsigned int txtLength = file->size;
	while (file->contents[txtLength - 1] == 0) txtLength -= 1;
	*size = txtLength;
	*data = new unsigned char[txtLength];
	memcpy(*data, file->contents, txtLength);
	return true;
}

/**
* ���л� Pixel ����Ϊ BMP ��ʽ
* @param file �ļ�����ָ��
* @param data ������л�����
* @param size ������ݴ�С
* @return �ɹ��򷵻� true
*/
bool CFileManager::SerializeBmpFile(CFile* file, unsigned char** data, unsigned int* size) {
	CImageFile* pImage = static_cast<CImageFile*>(file);
	int rowExtra = 3 - (pImage->width * pImage->channels - 1) % 4;
	unsigned int fileSize = pImage->size + 54 + pImage->height * rowExtra;
	unsigned char* pData = new unsigned char[fileSize];
	memset(pData, 0x00, 54);
	pData[0] = 'B';
	pData[1] = 'M';
	pData[2] = (unsigned char)(fileSize & 0xFF);
	pData[3] = (unsigned char)((fileSize >> 8) & 0xFF);
	pData[4] = (unsigned char)((fileSize >> 16) & 0xFF);
	pData[5] = (unsigned char)((fileSize >> 24) & 0xFF);
	pData[10] = 0x36;
	pData[14] = 0x28;
	pData[18] = (unsigned char)(pImage->width & 0xFF);
	pData[19] = (unsigned char)((pImage->width >> 8) & 0xFF);
	pData[20] = (unsigned char)((pImage->width >> 16) & 0xFF);
	pData[21] = (unsigned char)((pImage->width >> 24) & 0xFF);
	pData[22] = (unsigned char)(pImage->height & 0xFF);
	pData[23] = (unsigned char)((pImage->height >> 8) & 0xFF);
	pData[24] = (unsigned char)((pImage->height >> 16) & 0xFF);
	pData[25] = (unsigned char)((pImage->height >> 24) & 0xFF);
	pData[26] = 0x01;
	pData[28] = (unsigned char)(pImage->channels * 8);
	pData[34] = (unsigned char)(pImage->size & 0xFF);
	pData[35] = (unsigned char)((pImage->size >> 8) & 0xFF);
	pData[36] = (unsigned char)((pImage->size >> 16) & 0xFF);
	pData[37] = (unsigned char)((pImage->size >> 24) & 0xFF);
	// ���� BMP ����
	for (int h = 0; h < pImage->height; h++) {
		for (int w = 0; w < pImage->width; w++) {
			int srcIndex = pImage->channels * (pImage->width * (pImage->height - h - 1) + w);
			int dstIndex = pImage->channels * (pImage->width * h + w) + rowExtra * h + 54;
			memcpy(pData + dstIndex, pImage->contents + srcIndex, pImage->channels);
			// RGB ת��Ϊ BGR
			if (pImage->channels > 2) {
				unsigned char temp = pData[dstIndex];
				pData[dstIndex] = pData[dstIndex + 2];
				pData[dstIndex + 2] = temp;
			}
		}
	}
	*data = pData;
	*size = fileSize;
	return true;
}

/**
* ���л� Pixel ����Ϊ TGA ��ʽ
* @param file �ļ�����ָ��
* @param data ������л�����
* @param size ������ݴ�С
* @return �ɹ��򷵻� true
*/
bool CFileManager::SerializeTgaFile(CFile* file, unsigned char** data, unsigned int* size) {
	CImageFile* pImage = static_cast<CImageFile*>(file);
	unsigned char* pData = new unsigned char[pImage->size + 18];
	// �ļ�ͷ
	const unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	memcpy(pData, TGAheader, 12);
	pData[12] = (unsigned char)(pImage->width & 0xFF);
	pData[13] = (unsigned char)((pImage->width >> 8) & 0xFF);
	pData[14] = (unsigned char)(pImage->height & 0xFF);
	pData[15] = (unsigned char)((pImage->height >> 8) & 0xFF);
	pData[16] = (unsigned char)(pImage->channels * 8);
	pData[17] = 0x20;
	// ������������
	int srcIndex = 0;
	int dstIndex = 18;
	for (int h = 0; h < pImage->height; h++) {
		for (int w = 0; w < pImage->width; w++) {
			memcpy(pData + dstIndex, pImage->contents + srcIndex, pImage->channels);
			// RGB ת��Ϊ BGR
			if (pImage->channels > 2) {
				unsigned char temp = pData[dstIndex];
				pData[dstIndex] = pData[dstIndex + 2];
				pData[dstIndex + 2] = temp;
			}
			srcIndex += pImage->channels;
			dstIndex += pImage->channels;
		}
	}
	*data = pData;
	*size = pImage->size + 18;
	return true;
}

/**
* PNG ��д��ص�����
*/
void pngStreamWrite(png_structp png_ptr, png_bytep data, png_size_t length) {
	vector<unsigned char>& vec = *(vector<unsigned char>*)png_get_io_ptr(png_ptr);
	vec.reserve(vec.size() + length);
	vec.insert(vec.end(), (unsigned char*)data, (unsigned char*)data + length);
}

/**
* PNG ��д��ص�����
*/
void pngStreamFlush(png_structp png_ptr){
}

/**
* ʹ�� libpng �� Pixel �������л�Ϊ PNG ��ʽ
* @param file �ļ�����ָ��
* @param data ������л�����
* @param size ������ݴ�С
* @return �ɹ��򷵻� true
*/
bool CFileManager::SerializePngFile(CFile* file, unsigned char** data, unsigned int* size) {
	vector<unsigned char> vecData;
	int colorType;

	CImageFile* pImage = static_cast<CImageFile*>(file);
	if (pImage->channels == 1) colorType = PNG_COLOR_TYPE_GRAY;
	else if (pImage->channels == 2) colorType = PNG_COLOR_TYPE_GA;
	else if (pImage->channels == 3) colorType = PNG_COLOR_TYPE_RGB;
	else if (pImage->channels == 4) colorType = PNG_COLOR_TYPE_RGBA;
	else return false;

	vecData.reserve((size_t)((pImage->width * pImage->height * pImage->channels) * 0.2f));
	png_bytepp rows = new unsigned char*[pImage->height];
	for (int i = 0; i < pImage->height; i++) {
		rows[i] = pImage->contents + pImage->width * pImage->channels * i;
	}
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,	NULL, NULL);
	if (!png_ptr) {
		delete[] rows;
		return false;
	}
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr || setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		delete[] rows;
		return false;
	}

	png_set_write_fn(png_ptr, (void*)&vecData, pngStreamWrite, pngStreamFlush);
	png_set_IHDR(png_ptr, info_ptr, pImage->width, pImage->height, 8, colorType, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_set_rows(png_ptr, info_ptr, rows);
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
	setjmp(png_jmpbuf(png_ptr));
	png_destroy_write_struct(&png_ptr, &info_ptr);
	delete[] rows;

	*size = vecData.size();
	*data = new unsigned char[vecData.size()];
	memcpy(*data, &vecData[0], vecData.size());
	return true;
}

/**
* ʹ�� libjpeg �� Pixel �������л�Ϊ JPG ��ʽ
* @param file �ļ�����ָ��
* @param data ������л�����
* @param size ������ݴ�С
* @return �ɹ��򷵻� true
*/
bool CFileManager::SerializeJpgFile(CFile* file, unsigned char** data, unsigned int* size) {
	struct jpeg_compress_struct info;
	struct jpeg_error_mgr err;

	info.err = jpeg_std_error(&err);
	jpeg_create_compress(&info);
	unsigned long dataSize = 0;
	// �˴��Զ��������ڴ�
	jpeg_mem_dest(&info, data, &dataSize);

	CImageFile* pImage = static_cast<CImageFile*>(file);
	info.image_width = pImage->width;
	info.image_height = pImage->height;
	info.input_components = pImage->channels;
	if (pImage->channels == 1) info.in_color_space = JCS_GRAYSCALE;
	else if (pImage->channels == 3) info.in_color_space = JCS_RGB;
	else if (pImage->channels == 4) info.in_color_space = JCS_UNKNOWN;
	else {
		jpeg_destroy_compress(&info);
		return false;
	}

	jpeg_set_defaults(&info);
	// ѹ������ 0 �� 100
	jpeg_set_quality(&info, 80, TRUE);
	jpeg_start_compress(&info, TRUE);

	unsigned char* p = pImage->contents;
	while (info.next_scanline < info.image_height) {
		int numlines = jpeg_write_scanlines(&info, &p, 1);
		p += numlines * pImage->width * pImage->channels;
	}
	jpeg_finish_compress(&info);
	jpeg_destroy_compress(&info);
	*size = dataSize;
	return true;
}

/**
* �� PCM ��Ƶ���л�Ϊ WAV ��ʽ
* @param file �ļ�����ָ��
* @param data ������л�����
* @param size ������ݴ�С
* @return �ɹ��򷵻� true
*/
bool CFileManager::SerializeWavFile(CFile* file, unsigned char** data, unsigned int* size) {
	unsigned int fileSize = file->size + 0x2C;
	unsigned char* pData = new unsigned char[fileSize];
	CAudioFile* pAudio = static_cast<CAudioFile*>(file);
	// WAV �ļ�ͷ
	memcpy(pData, "RIFF", 4);
	pData[4] = (unsigned char)((fileSize - 8) & 0xFF);
	pData[5] = (unsigned char)(((fileSize - 8) >> 8) & 0xFF);
	pData[6] = (unsigned char)(((fileSize - 8) >> 16) & 0xFF);
	pData[7] = (unsigned char)(((fileSize - 8) >> 24) & 0xFF);
	memcpy(pData + 0x08, "WAVE", 4);
	memcpy(pData + 0x0C, "fmt ", 4);
	const unsigned char fmtSize[] = { 0x10, 0x00, 0x00, 0x00 };
	memcpy(pData + 0x10, fmtSize, 4);
	pData[0x14] = 0x01;
	pData[0x15] = 0x00;
	pData[0x16] = (unsigned char)pAudio->channels;
	pData[0x17] = 0x00;
	pData[0x18] = (unsigned char)(pAudio->sampleRate & 0xFF);
	pData[0x19] = (unsigned char)((pAudio->sampleRate >> 8)  & 0xFF);
	pData[0x1A] = (unsigned char)((pAudio->sampleRate >> 16)  & 0xFF);
	pData[0x1B] = (unsigned char)((pAudio->sampleRate >> 24)  & 0xFF);
	unsigned int byteRate = (pAudio->sampleRate * pAudio->channels * pAudio->sampleBits) >> 3;
	pData[0x1C] = (unsigned char)(byteRate & 0xFF);
	pData[0x1D] = (unsigned char)((byteRate >> 8) & 0xFF);
	pData[0x1E] = (unsigned char)((byteRate >> 16) & 0xFF);
	pData[0x1F] = (unsigned char)((byteRate >> 24) & 0xFF);
	unsigned short blockAlign = (pAudio->channels * pAudio->sampleBits) >> 3;
	pData[0x20] = (unsigned char)(blockAlign & 0xFF);
	pData[0x21] = (unsigned char)((blockAlign >> 8) & 0xFF);
	pData[0x22] = (unsigned char)(pAudio->sampleBits & 0xFF);
	pData[0x23] = (unsigned char)((pAudio->sampleBits >> 8) & 0xFF);
	memcpy(pData + 0x24, "data", 4);
	pData[0x28] = (unsigned char)(file->size & 0xFF);
	pData[0x29] = (unsigned char)((file->size >> 8) & 0xFF);
	pData[0x2A] = (unsigned char)((file->size >> 16) & 0xFF);
	pData[0x2B] = (unsigned char)((file->size >> 24) & 0xFF);
	// WAV �ļ�����
	memcpy(pData+ 0x2C, file->contents, file->size);
	*size = fileSize;
	*data = pData;
	return true;
}

/**
* �� PCM ��Ƶ���л�Ϊ MP3 ��ʽ
* @param file �ļ�����ָ��
* @param data ������л�����
* @param size ������ݴ�С
* @return �ɹ��򷵻� true
*/
bool CFileManager::SerializeMp3File(CFile* file, unsigned char** data, unsigned int* size) {
	CLog::Warn("Can't serialize MP3 file, SerializeMp3File is not implenment");
	return false;
}