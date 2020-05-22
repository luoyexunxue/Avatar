//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CRECTANGLE_H_
#define _CRECTANGLE_H_
#include "AvatarConfig.h"

/**
* @brief 矩形类
*/
class AVATAR_EXPORT CRectangle {
public:
	//! 默认构造函数
	CRectangle();
	//! 复制构造函数
	CRectangle(const CRectangle& rect);
	//! 由数组构造矩形
	CRectangle(const int rect[4]);
	//! 由左上角xy坐标和宽高构造矩形
	CRectangle(int x, int y, int w, int h);

	//! 设置矩形值
	void SetValue(const CRectangle& rect);
	//! 设置矩形值
	void SetValue(const int rect[4]);
	//! 设置矩形值
	void SetValue(int x, int y, int w, int h);

	//! 设置偏移
	CRectangle& Offset(int x, int y);
	//! 设置偏移
	CRectangle& Offset(int offset[2]);
	//! 设置偏移
	CRectangle& Offset(const CRectangle& rect);
	//! 设置缩放
	CRectangle& Scale(float scale, bool center);

	//! 设置区域为空
	void SetEmpty();
	//! 判断区域是否为空
	bool IsEmpty() const;
	//! 判断是否有交集
	bool IsIntersect(const CRectangle& rect) const;
	//! 判断是否包含点
	bool IsContain(int x, int y) const;
	//! 判断是否包含区域
	bool IsContain(const CRectangle& rect) const;

	//! 获取矩形左边界
	int GetLeft() const;
	//! 获取矩形上边界
	int GetTop() const;
	//! 获取矩形右边界
	int GetRight() const;
	//! 获取矩形下边界
	int GetBottom() const;
	//! 获取矩形宽度
	int GetWidth() const;
	//! 获取矩形高度
	int GetHeight() const;
	//! 获取矩形面积
	int GetArea() const;

	//! 与 rect 求并集
	CRectangle Union(const CRectangle& rect) const;
	//! 与 rect 求交集
	CRectangle Intersect(const CRectangle& rect) const;

public:
	//! 下标运算符 []
	int& operator [] (int index);
	//! 重载运算符 ==
	bool operator == (const CRectangle& rect) const;
	//! 重载运算符 !=
	bool operator != (const CRectangle& rect) const;
	//! 并集计算
	CRectangle& operator += (const CRectangle& rect);
	//! 交集计算
	CRectangle& operator *= (const CRectangle& rect);

public:
	/**
	* 矩形存储方式
	* [x, y, w, h]
	*/
	int m_iValue[4];
	//! 空矩形常量
	const static CRectangle Empty;
};

#endif