//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CCOLOR_H_
#define _CCOLOR_H_
#include "AvatarConfig.h"
#include <string>
using std::string;

/**
* @brief 颜色类
*/
class AVATAR_EXPORT CColor {
public:
	//! 默认构造函数
	CColor();
	//! 复制构造函数
	CColor(const CColor& color);
	//! 由数组形式的RGBA构造
	CColor(const float color[4]);
	//! 由四字节整数构造
	CColor(unsigned int color);
	//! 由RGB构造颜色
	CColor(float r, float g, float b);
	//! 由RGBA构造颜色
	CColor(float r, float g, float b, float a);
	//! 由RGB字节构造颜色
	CColor(int r, int g, int b);
	//! 由RGBA字节构造颜色
	CColor(int r, int g, int b, int a);
	//! 由字符串构造
	CColor(const string& color);

	//! 设置颜色值
	void SetValue(const CColor& color);
	//! 设置颜色值
	void SetValue(const float color[4]);
	//! 设置颜色值
	void SetValue(float r, float g, float b);
	//! 设置颜色值
	void SetValue(float r, float g, float b, float a);

	//! 获取颜色值
	void GetValue(float* color) const;
	//! 获取颜色值
	void GetValue(unsigned char* color) const;
	//! 填充缓冲区
	void FillBuffer(float* buffer, int size, int stride) const;
	//! 填充缓冲区
	void FillBuffer(unsigned char* buffer, int size, int stride) const;

	//! 颜色相加
	CColor& Add(const CColor& color, float scale);
	//! 颜色相加
	CColor& Add(const float color[4], float scale);

	//! 范围限制在 0.0 至 1.0 之间
	CColor& Clamp();
	//! 反转颜色
	CColor& Invert();
	//! 从 HSLA 转换
	CColor& FromHsla(float h, float s, float l, float a);
	//! 从名字设置颜色
	CColor& FromName(const string& name);

	//! 返回颜色代码
	string ToString();

public:
	//! 下标运算符 []
	float& operator [] (int index);
	//! 重载运算符 ==
	bool operator == (const CColor& color) const;
	//! 重载运算符 !=
	bool operator != (const CColor& color) const;
	//! 加法运算
	CColor operator + (const CColor& color) const;
	//! 减法运算
	CColor operator - (const CColor& color) const;
	//! 乘法运算
	CColor operator * (const CColor& color) const;

	//! 重载运算符 +=
	CColor operator += (const CColor& color);
	//! 重载运算符 -=
	CColor operator -= (const CColor& color);
	//! 重载运算符 *=
	CColor operator *= (const CColor& color);

public:
	/**
	* 颜色存储方式
	* [R, G, B, A]
	*/
	float m_fValue[4];
	//! 透明常量
	const static CColor Transparent;
	//! 白色常量
	const static CColor White;
	//! 灰色常量
	const static CColor Gray;
	//! 黑色常量
	const static CColor Black;
	//! 红色常量
	const static CColor Red;
	//! 绿色常量
	const static CColor Green;
	//! 蓝色常量
	const static CColor Blue;
	//! 黄色常量
	const static CColor Yellow;
	//! 紫色常量
	const static CColor Purple;
};

#endif