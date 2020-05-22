//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPOSTPROCESS_H_
#define _CPOSTPROCESS_H_
#include "CTexture.h"
#include "CShader.h"
#include "CMesh.h"

/**
* @brief ���������
*/
class CPostProcess {
public:
	//! Ĭ�Ϲ��캯��
	CPostProcess();
	//! Ĭ������������
	virtual ~CPostProcess() {}

	//! ��ʼ���������
	virtual bool Init(int width, int height) = 0;
	//! ��Ⱦ�����С�ı�
	virtual void Resize(int width, int height) = 0;
	//! ���ٺ������
	virtual void Destroy() = 0;
	//! Ӧ�õ�ǰ����
	virtual void Apply(CTexture* target, CMesh* mesh);

protected:
	//! ��ȡ������ɫ������
	const char* GetVertexShader();

protected:
	//! ������ɫ��
	CShader* m_pPostProcessShader;
	//! ������Ⱦ����
	CTexture* m_pRenderTexture;

private:
	//! ����ĺ�����Ļ��С
	int m_iSavedScreenSize[2];
	//! ������������Է���
	friend class CPostProcessManager;
};

#endif