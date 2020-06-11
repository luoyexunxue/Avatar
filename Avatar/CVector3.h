//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CVECTOR3_H_
#define _CVECTOR3_H_
#include "AvatarConfig.h"

/**
* @brief 三维向量类
*/
class AVATAR_EXPORT CVector3 {
public:
	//! 默认构造函数
	CVector3();
	//! 复制构造函数
	CVector3(const CVector3& vector);
	//! 使用单一值赋值
	CVector3(float val);
	//! 使用数组构造向量
	CVector3(const float vector[3]);
	//! 使用xyz构造向量
	CVector3(float x, float y, float z);
	//! 使用数组和w构造向量
	CVector3(const float vector[3], float w);
	//! 使用xyzw构造向量
	CVector3(float x, float y, float z, float w);

	//! 设置向量值
	void SetValue(const CVector3& vector);
	//! 设置向量值
	void SetValue(const float vector[3]);
	//! 设置向量值
	void SetValue(float x, float y, float z);
	//! 设置向量值
	void SetValue(const float vector[3], float w);
	//! 设置向量值
	void SetValue(float x, float y, float z, float w);

	//! 转换为单位向量
	CVector3& Normalize();
	//! 转换为倒数
	CVector3& Inverse();

	//! 加指定值
	CVector3& Add(const CVector3& vector);
	//! 加指定值
	CVector3& Add(const float vector[3]);
	//! 加指定值
	CVector3& Add(float x, float y, float z);
	//! 减指定值
	CVector3& Sub(const CVector3& vector);
	//! 减指定值
	CVector3& Sub(const float vector[3]);
	//! 减指定值
	CVector3& Sub(float x, float y, float z);
	//! 缩放指定值
	CVector3& Scale(float scale);
	//! 缩放指定值
	CVector3& Scale(float sx, float sy, float sz);

	//! 取得向量的长度
	float Length() const;
	//! 向量点乘
	float DotProduct(const CVector3& vector) const;
	//! 向量叉乘
	CVector3 CrossProduct(const CVector3& vector) const;
	//! 线性插值
	CVector3 Lerp(const CVector3& vector, float t) const;
	//! 球形插值
	CVector3 Slerp(const CVector3& vector, float t) const;
	//! 求反射向量
	CVector3 Reflect(const CVector3& normal) const;
	//! 生成切向量
	CVector3 Tangent() const;

public:
	//! 下标运算符 []
	float& operator [] (int index);
	//! 重载运算符 ==
	bool operator == (const CVector3& vector) const;
	//! 重载运算符 !=
	bool operator != (const CVector3& vector) const;

	//! 加法运算
	CVector3 operator + (const CVector3& vector) const;
	//! 减法运算
	CVector3 operator - (const CVector3& vector) const;
	//! 乘法运算
	CVector3 operator * (const CVector3& vector) const;
	//! 乘因子运算
	CVector3 operator * (float factor) const;
	//! 除因子运算
	CVector3 operator / (float factor) const;

	//! 重载运算符 +=
	CVector3& operator += (const CVector3& vector);
	//! 重载运算符 -=
	CVector3& operator -= (const CVector3& vector);
	//! 重载运算符 *=
	CVector3& operator *= (float factor);
	//! 重载运算符 /=
	CVector3& operator /= (float factor);

public:
	/**
	* 向量存储方式
	* [x, y, z, w]
	*/
	float m_fValue[4];
	//! 零向量常量
	const static CVector3 Zero;
	//! 单位一向量
	const static CVector3 One;
	//! 单位X向量常量
	const static CVector3 X;
	//! 单位Y向量常量
	const static CVector3 Y;
	//! 单位Z向量常量
	const static CVector3 Z;
};

#endif