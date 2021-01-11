//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CMATRIX4_H_
#define _CMATRIX4_H_
#include "AvatarConfig.h"
#include "CVector3.h"
class CQuaternion;
class CPlane;

/**
* @brief 4x4矩阵类
*/
class AVATAR_EXPORT CMatrix4 {
public:
	//! 默认构造函数
	CMatrix4();
	//! 复制构造函数
	CMatrix4(const CMatrix4& matrix);
	//! 使用4x4的数组构造矩阵
	CMatrix4(const float matrix[16]);

	//! 设置矩阵值
	void SetValue(const CMatrix4& matrix);
	//! 设置矩阵值
	void SetValue(const float matrix[16]);

	//! 设置为单位矩阵
	CMatrix4& SetIdentity();
	//! 计算逆矩阵
	CMatrix4& Invert();
	//! 计算转置矩阵
	CMatrix4& Transpose();
	//! 计算斜对称矩阵
	CMatrix4& SkewSymmetric(const CVector3& vector);
	//! 计算被缩放的矩阵
	CMatrix4& SetScaled(const CVector3& scale);

	//! 缩放操作
	CMatrix4& Scale(const CVector3& scale);
	//! 缩放操作
	CMatrix4& Scale(float dx, float dy, float dz);

	//! 绕 X 轴旋转矩阵
	CMatrix4& RotateX(float angle);
	//! 绕 Y 轴旋转矩阵
	CMatrix4& RotateY(float angle);
	//! 绕 Z 轴旋转矩阵
	CMatrix4& RotateZ(float angle);

	//! 平移操作
	CMatrix4& Translate(const CVector3& pos);
	//! 平移操作
	CMatrix4& Translate(float dx, float dy, float dz);

	//! 生成变换矩阵(顺序 scale, rotation, translation)
	void MakeTransform(const CVector3& scale, const CMatrix4& rotation, const CVector3& translation);
	//! 生成变换矩阵(顺序 scale, rotation, translation)
	void MakeTransform(const CVector3& scale, const CQuaternion& rotation, const CVector3& translation);

	//! 计算近斜平面裁剪投影矩阵
	CMatrix4& ObliqueNearPlaneClipping(const CMatrix4& proj, const CPlane& clip);
	//! 计算视图矩阵
	CMatrix4& LookAt(const CVector3& pos, const CVector3& look, const CVector3& up);
	//! 计算正交投影矩阵
	CMatrix4& Orthographic(float width, float height, float zNear, float zFar);
	//! 计算透视投影矩阵
	CMatrix4& Perspective(float fov, float aspect, float zNear, float zFar);

public:
	//! 下标运算符 []
	float& operator [] (int index);
	//! 行列运算符 ()
	float& operator () (int row, int col);
	//! 重载运算符 ==
	bool operator == (const CMatrix4& matrix) const;
	//! 重载运算符 !=
	bool operator != (const CMatrix4& matrix) const;
	//! 加法运算
	CMatrix4 operator + (const CMatrix4& matrix) const;
	//! 减法运算
	CMatrix4 operator - (const CMatrix4& matrix) const;
	//! 乘法运算
	CMatrix4 operator * (const CMatrix4& matrix) const;
	//! 乘向量运算
	CVector3 operator * (const CVector3& vector) const;
	//! 乘四元数运算
	CQuaternion operator * (const CQuaternion& rotation) const;
	//! 乘因子运算
	CMatrix4 operator * (float factor) const;

	//! 重载运算符 +=
	CMatrix4& operator += (const CMatrix4& matrix);
	//! 重载运算符 -=
	CMatrix4& operator -= (const CMatrix4& matrix);
	//! 重载运算符 *=
	CMatrix4& operator *= (const CMatrix4& matrix);
	//! 重载运算符 *=
	CMatrix4& operator *= (float factor);

public:
	/**
	* 矩阵存储方式，该存储方式与 OpenGL 兼容
	* 0 4 8 12
	* 1 5 9 13
	* 2 6 10 14
	* 3 7 11 15
	*/
	float m_fValue[16];
	//! 单位矩阵常量
	const static CMatrix4 Identity;
};

#endif