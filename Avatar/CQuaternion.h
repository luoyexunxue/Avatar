//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CQUATERNION_H_
#define _CQUATERNION_H_
#include "AvatarConfig.h"
#include "CMatrix4.h"
#include "CVector3.h"

/**
* @brief 四元数类
*/
class AVATAR_EXPORT CQuaternion {
public:
	//! 默认构造函数
	CQuaternion();
	//! 复制构造函数
	CQuaternion(const CQuaternion& quaternion);
	//! 使用数组构造四元数
	CQuaternion(const float quaternion[4]);
	//! 使用xyzw构造四元数
	CQuaternion(float x, float y, float z, float w);

	//! 设置四元数值
	void SetValue(const CQuaternion& quaternion);
	//! 设置四元数值
	void SetValue(float x, float y, float z, float w);

	//! 单位化
	CQuaternion& Normalize();
	//! 共轭
	CQuaternion& Conjugate();
	//! 取逆
	CQuaternion& Inverse();

	//! 由欧拉角转为四元数
	CQuaternion& FromEulerAngles(float x, float y, float z);
	//! 由轴角转为四元数
	CQuaternion& FromAxisAngle(const CVector3& axis, float angle);
	//! 由向量转为四元数
	CQuaternion& FromVector(const CVector3& from, const CVector3& to);
	//! 由矩阵转为四元数
	CQuaternion& FromMatrix(const CMatrix4& matrix);

	//! 转换为欧拉角表示
	void ToEulerAngles(float* x, float* y, float* z) const;
	//! 转换为轴角表示
	void ToAxisAngle(CVector3& axis, float* angle) const;
	//! 转换为矩阵表示
	CMatrix4 ToMatrix() const;

	//! 球形插值
	CQuaternion Slerp(const CQuaternion& quaternion, float t) const;
	//! 最短路径球形插值
	CQuaternion SlerpShortest(const CQuaternion& quaternion, float t) const;

public:
	//! 下标运算符 []
	float& operator [] (int index);
	//! 运算符重载 ==
	bool operator == (const CQuaternion& quaternion) const;
	//! 运算符重载 !=
	bool operator != (const CQuaternion& quaternion) const;

	//! 加法运算
	CQuaternion operator + (const CQuaternion& quaternion) const;
	//! 减法运算
	CQuaternion operator - (const CQuaternion& quaternion) const;
	//! 乘法运算
	CQuaternion operator * (const CQuaternion& quaternion) const;
	//! 比例运算
	CQuaternion operator * (float scale) const;
	//! 乘向量运算
	CVector3 operator * (const CVector3& vector) const;

	//! 运算符重载 +=
	CQuaternion& operator += (const CQuaternion& quaternion);
	//! 运算符重载 -=
	CQuaternion& operator -= (const CQuaternion& quaternion);
	//! 运算符重载 *=
	CQuaternion& operator *= (const CQuaternion& quaternion);
	//! 运算符重载 *=
	CQuaternion& operator *= (float scale);

public:
	/**
	* 四元数存储方式
	* [x, y, z, w]
	*/
	float m_fValue[4];
};

#endif