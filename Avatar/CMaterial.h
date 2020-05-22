//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CMATERIAL_H_
#define _CMATERIAL_H_
#include "AvatarConfig.h"
#include "CTexture.h"
#include "CShader.h"
#include <string>
#include <vector>
using std::string;
using std::vector;

/**
* @brief ������
*
* �����������ɫ��������֧�� 8 ��ͨ������
*/
class AVATAR_EXPORT CMaterial {
public:
	//! Ĭ�Ϲ��캯��
	CMaterial();
	//! Ĭ����������
	~CMaterial();

	//! ���Ʋ���
	void CopyFrom(const CMaterial* material);

	//! ʹ�ò���
	void UseMaterial();

	//! ���ò�����ͼ
	void SetTexture(CTexture* texture, int index = 0);
	//! ���ò�����ͼ
	void SetTexture(const string& file, int index = 0);
	//! ���ò�����ͼ
	void SetTexture(const string& name, const string files[6], int index = 0);
	//! ���ò�����ͼ
	void SetTexture(const string& name, int width, int height, int channel, const void* data, int index = 0);

	//! ���ò�����ɫ��
	void SetShader(CShader* shader);
	//! ���ò�����ɫ��
	void SetShader(const string& name);

	//! ���� Uniform ֵ
	void PassUniform(const string& name, bool value);
	//! ���� Uniform ֵ
	void PassUniform(const string& name, int value);
	//! ���� Uniform ֵ
	void PassUniform(const string& name, float value);
	//! ���� Uniform ֵ
	void PassUniform(const string& name, const CVector2& value);
	//! ���� Uniform ֵ
	void PassUniform(const string& name, const CVector3& value);
	//! ���� Uniform ֵ
	void PassUniform(const string& name, const CMatrix4& value);
	//! ���� Uniform ֵ
	void PassUniform(const string& name, const float value[], int size);

	//! ������Ⱦģʽ
	void SetRenderMode(bool cullFace, bool useDepth, bool addColor);

	//! ��ȡ��������
	inline string& GetName() { return m_strName; }
	//! ��ȡ�������
	inline int GetTextureCount() const { return m_iTextureCount; }
	//! ��ȡ����
	inline CTexture* GetTexture(int index = 0) const { return m_pTexture[index]; }
	//! ��ȡ��ɫ��
	inline CShader* GetShader() const { return m_pShader; }

public:
	//! �⻬�Բ���
	float m_fRoughness;
	//! �����Բ���
	float m_fMetalness;
	//! ������ɫ
	float m_fColor[4];

private:
	//! Uniform ����ͳһ����
	typedef struct _SUniformValue {
		int location;
		CShader::UniformType type;
		union {
			bool bValue;
			int iValue;
			float fValue[16];
		} value;
	} SUniformValue;

	//! �����޳�
	bool m_bCullFace;
	//! ��Ȳ���
	bool m_bUseDepth;
	//! ��ɫ���
	bool m_bAddColor;

	//! ��������
	string m_strName;
	//! �������
	int m_iTextureCount;
	//! ��������
	CTexture* m_pTexture[8];
	//! ��ɫ��
	CShader* m_pShader;
	//! �����ݵ� Uniform �б�
	vector<SUniformValue> m_vecUniforms;

	//! ����Ϊ CMesh ����Ԫ��
	friend class CMesh;
};

#endif