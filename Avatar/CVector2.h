//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CVECTOR2_H_
#define _CVECTOR2_H_
#include "AvatarConfig.h"

/**
* @brief 二维向量类
*/
class AVATAR_EXPORT CVector2 {
public:
	//! 默认构造函数
	CVector2();
	//! 复制构造函数
	CVector2(const CVector2& vector);
	//! 使用单一值赋值
	CVector2(float val);
	//! 使用数组构造向量
	CVector2(const float vector[2]);
	//! 使用xy构造向量
	CVector2(float x, float y);
	//! 使用xy构造向量
	CVector2(int x, int y);

	//! 设置向量值
	void SetValue(const CVector2& vector);
	//! 设置向量值
	void SetValue(const float vector[2]);
	//! 设置向量值
	void SetValue(float x, float y);

	//! 转换为单位向量
	CVector2& Normalize();

	//! 加指定值
	CVector2& Add(const CVector2& vector);
	//! 加指定值
	CVector2& Add(const float vector[2]);
	//! 加指定值
	CVector2& Add(float x, float y);
	//! 减指定值
	CVector2& Sub(const CVector2& vector);
	//! 减指定值
	CVector2& Sub(const float vector[2]);
	//! 减指定值
	CVector2& Sub(float x, float y);
	//! 缩放指定值
	CVector2& Scale(float scale);
	//! 缩放指定值
	CVector2& Scale(float sx, float sy);

	//! 取得向量的长度
	float Length() const;
	//! 向量点乘
	float DotProduct(const CVector2& vector) const;
	//! 向量叉乘
	float CrossProduct(const CVector2& vector) const;
	//! 线性插值
	CVector2 Lerp(const CVector2& vector, float t) const;
	//! 弧形插值
	CVector2 Slerp(const CVector2& vector, float t) const;
	//! 求反射向量
	CVector2 Reflect(const CVector2& normal) const;

public:
	//! 下标运算符 []
	float& operator [] (int index);
	//! 重载运算符 ==
	bool operator == (const CVector2& vector) const;
	//! 重载运算符 !=
	bool operator != (const CVector2& vector) const;

	//! 加法运算
	CVector2 operator + (const CVector2& vector) const;
	//! 减法运算
	CVector2 operator - (const CVector2& vector) const;
	//! 乘因子运算
	CVector2 operator * (float factor) const;
	//! 除因子运算
	CVector2 operator / (float factor) const;

	//! 重载运算符 +=
	CVector2& operator += (const CVector2& vector);
	//! 重载运算符 -=
	CVector2& operator -= (const CVector2& vector);
	//! 重载运算符 *=
	CVector2& operator *= (float factor);
	//! 重载运算符 /=
	CVector2& operator /= (float factor);

public:
	/**
	* 向量存储方式
	* [x, y]
	*/
	float m_fValue[2];
	//! 零向量常量
	const static CVector2 Zero;
	//! 单位X向量常量
	const static CVector2 X;
	//! 单位Y向量常量
	const static CVector2 Y;
};

#endif