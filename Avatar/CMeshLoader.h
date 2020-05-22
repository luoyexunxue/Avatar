//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CMESHLOADER_H_
#define _CMESHLOADER_H_
#include "AvatarConfig.h"
#include "CMeshData.h"
#include <map>
using std::map;

/**
* @brief ģ�ͼ�����
*/
class AVATAR_EXPORT CMeshLoader {
public:
	//! ע��ģ�ͼ�����
	static bool Register(const string& type, CMeshLoader* loader);
	//! ������ע��ļ�����
	static void Destroy();
	//! ��������ģ��
	static CMeshData* Load(const string& filename, bool cache);
	//! ��������ģ��
	static bool Save(const string& filename, CMeshData* meshData);
	//! �Ƴ��ѻ����ģ��
	static void Remove(CMeshData* meshData);

protected:
	//! Ĭ�Ϲ��캯��
	CMeshLoader() {}
	//! ����������
	virtual ~CMeshLoader() {}
	//! ����ģ���ļ�
	virtual CMeshData* LoadFile(const string& filename, const string& type) = 0;

private:
	//! ���� AVT �ļ�
	static CMeshData* LoadAvatar(const string& filename);
	//! ���� AVT �ļ�
	static bool SaveAvatar(const string& filename, CMeshData* meshData);
	//! ע�����ü�����
	static void RegisterLoader();
	//! ģ�ͼ������б�
	static map<string, CMeshLoader*> m_mapMeshLoader;
	//! �ѻ��������ģ��
	static map<string, CMeshData*> m_mapMeshDataCache;
	//! �ѻ��������ģ������
	static map<CMeshData*, int> m_mapCacheRefCount;
};

#endif