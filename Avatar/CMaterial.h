//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CMATERIAL_H_
#define _CMATERIAL_H_
#include "AvatarConfig.h"
#include "CTexture.h"
#include "CShader.h"
#include <string>
#include <vector>
#include <map>
using std::string;
using std::vector;
using std::map;

/**
* @brief 材质类
*
* 包含纹理和着色器，最多可支持 8 个通道纹理
*/
class AVATAR_EXPORT CMaterial {
public:
	//! 默认构造函数
	CMaterial();
	//! 默认析构函数
	~CMaterial();

	//! 复制材质
	void CopyFrom(const CMaterial* material);

	//! 使用材质
	void UseMaterial();

	//! 设置材质贴图
	void SetTexture(CTexture* texture, int index = 0);
	//! 设置材质贴图
	void SetTexture(const string& file, int index = 0);
	//! 设置材质贴图
	void SetTexture(const string& name, int width, int height, int channel, const void* data, bool mipmap, int index = 0);

	//! 缓冲加载纹理
	void SetTextureBuffered(const string& file, int index = 0);
	//! 缓冲加载纹理
	void SetTextureBuffered(const string& name, int width, int height, int channel, const void* data, bool mipmap, int index = 0);
	//! 设置缓冲纹理模式
	void SetTextureBufferedWrapMode(int s, int t, int index = 0);

	//! 设置材质着色器
	void SetShader(CShader* shader);
	//! 设置材质着色器
	void SetShader(const string& name);

	//! 传递 Uniform 值
	void PassUniform(const string& name, bool value);
	//! 传递 Uniform 值
	void PassUniform(const string& name, int value);
	//! 传递 Uniform 值
	void PassUniform(const string& name, float value);
	//! 传递 Uniform 值
	void PassUniform(const string& name, const CVector2& value);
	//! 传递 Uniform 值
	void PassUniform(const string& name, const CVector3& value);
	//! 传递 Uniform 值
	void PassUniform(const string& name, const CMatrix4& value);
	//! 传递 Uniform 值
	void PassUniform(const string& name, const float value[], int size);

	//! 设置渲染模式
	void SetRenderMode(bool cullFace, bool useDepth, bool addColor);

	//! 获取材质名称
	inline string& GetName() { return m_strName; }
	//! 获取纹理个数
	inline int GetTextureCount() const { return m_iTextureCount; }
	//! 获取纹理
	inline CTexture* GetTexture(int index = 0) const { return m_pTexture[index]; }
	//! 获取着色器
	inline CShader* GetShader() const { return m_pShader; }

public:
	//! 光滑性参数
	float m_fRoughness;
	//! 金属性参数
	float m_fMetalness;
	//! 材质颜色
	float m_fColor[4];

private:
	//! 生成已缓冲的纹理
	inline void CreateBufferedTexture();

	//! 缓冲纹理对象
	typedef struct _SBufferedTexture {
		string name;
		int width;
		int height;
		int channel;
		int wraps;
		int wrapt;
		bool mipmap;
		unsigned char* data;
		_SBufferedTexture() : wraps(0), wrapt(0), data(0) {}
	} SBufferedTexture;

	//! Uniform 变量统一定义
	typedef struct _SUniformValue {
		int location;
		CShader::UniformType type;
		union {
			bool bValue;
			int iValue;
			float fValue[16];
		};
	} SUniformValue;

private:
	//! 背面剔除
	bool m_bCullFace;
	//! 深度测试
	bool m_bUseDepth;
	//! 颜色相加
	bool m_bAddColor;

	//! 材质名称
	string m_strName;
	//! 纹理个数
	int m_iTextureCount;
	//! 纹理数组
	CTexture* m_pTexture[8];
	//! 着色器
	CShader* m_pShader;
	//! 待传递的 Uniform 列表
	vector<SUniformValue> m_vecUniforms;
	//! 待创建的纹理对象
	map<int, SBufferedTexture> m_mapBufferedTextures;

	//! 设置为 CMesh 的友元类
	friend class CMesh;
};

#endif