//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CColor.h"
#include <cstdlib>
#include <algorithm>

/**
* 静态常量
*/
const CColor CColor::Transparent(0.0f, 0.0f, 0.0f, 0.0f);
const CColor CColor::White(1.0f, 1.0f, 1.0f, 1.0f);
const CColor CColor::Gray(0.5f, 0.5f, 0.5f, 1.0f);
const CColor CColor::Black(0.0f, 0.0f, 0.0f, 1.0f);
const CColor CColor::Red(1.0f, 0.0f, 0.0f, 1.0f);
const CColor CColor::Green(0.0f, 1.0f, 0.0f, 1.0f);
const CColor CColor::Blue(0.0f, 0.0f, 1.0f, 1.0f);
const CColor CColor::Yellow(1.0f, 1.0f, 0.0f, 1.0f);
const CColor CColor::Purple(1.0f, 0.0f, 1.0f, 1.0f);

/**
* 默认构造函数
*/
CColor::CColor() {
	m_fValue[0] = 1.0f;
	m_fValue[1] = 1.0f;
	m_fValue[2] = 1.0f;
	m_fValue[3] = 1.0f;
}

/**
* 复制构造函数
*/
CColor::CColor(const CColor& color) {
	m_fValue[0] = color.m_fValue[0];
	m_fValue[1] = color.m_fValue[1];
	m_fValue[2] = color.m_fValue[2];
	m_fValue[3] = color.m_fValue[3];
}

/**
* 由数组形式的RGBA构造
*/
CColor::CColor(const float color[4]) {
	m_fValue[0] = color[0];
	m_fValue[1] = color[1];
	m_fValue[2] = color[2];
	m_fValue[3] = color[3];
}

/**
* 由四字节整数构造
*/
CColor::CColor(unsigned int color) {
	const float scale = 1.0f / 255.0f;
	m_fValue[0] = ((color >> 24) & 0xFF) * scale;
	m_fValue[1] = ((color >> 16) & 0xFF) * scale;
	m_fValue[2] = ((color >> 8) & 0xFF) * scale;
	m_fValue[3] = (color & 0xFF) * scale;
}

/**
* 由RGB构造颜色
*/
CColor::CColor(float r, float g, float b) {
	m_fValue[0] = r;
	m_fValue[1] = g;
	m_fValue[2] = b;
	m_fValue[3] = 1.0f;
}

/**
* 由RGBA构造颜色
*/
CColor::CColor(float r, float g, float b, float a) {
	m_fValue[0] = r;
	m_fValue[1] = g;
	m_fValue[2] = b;
	m_fValue[3] = a;
}

/**
* 由RGB字节构造颜色
*/
CColor::CColor(int r, int g, int b) {
	const float scale = 1.0f / 255.0f;
	m_fValue[0] = r * scale;
	m_fValue[1] = g * scale;
	m_fValue[2] = b * scale;
	m_fValue[3] = 1.0f;
}

/**
* 由RGBA字节构造颜色
*/
CColor::CColor(int r, int g, int b, int a) {
	const float scale = 1.0f / 255.0f;
	m_fValue[0] = r * scale;
	m_fValue[1] = g * scale;
	m_fValue[2] = b * scale;
	m_fValue[3] = a * scale;
}

/**
* 由字符串构造
*/
CColor::CColor(const string& color) {
	if (!color.empty() && (color.at(0) == '#' || color.find("0x") == 0)) {
		const float scale_byte = 1.0f / 255.0f;
		const float scale_half = 1.0f / 15.0f;
		const string hex = color.at(0) == '#' ? "0x" + color.substr(1) : color;
		unsigned long var = strtoul(hex.c_str(), 0, 0);
		if (hex.length() == 10) {
			m_fValue[0] = ((var >> 24) & 0xFF) * scale_byte;
			m_fValue[1] = ((var >> 16) & 0xFF) * scale_byte;
			m_fValue[2] = ((var >> 8) & 0xFF) * scale_byte;
			m_fValue[3] = (var & 0xFF) * scale_byte;
		} else if (hex.length() == 8) {
			m_fValue[0] = ((var >> 16) & 0xFF) * scale_byte;
			m_fValue[1] = ((var >> 8) & 0xFF) * scale_byte;
			m_fValue[2] = (var & 0xFF) * scale_byte;
			m_fValue[3] = 1.0f;
		} else if (hex.length() == 6) {
			m_fValue[0] = ((var >> 12) & 0x0F) * scale_half;
			m_fValue[1] = ((var >> 8) & 0x0F) * scale_half;
			m_fValue[2] = ((var >> 4) & 0x0F) * scale_half;
			m_fValue[3] = (var & 0x0F) * scale_half;
		} else if (hex.length() == 5) {
			m_fValue[0] = ((var >> 8) & 0x0F) * scale_half;
			m_fValue[1] = ((var >> 4) & 0x0F) * scale_half;
			m_fValue[2] = (var & 0x0F) * scale_half;
			m_fValue[3] = 1.0f;
		} else SetValue(CColor::White);
	}
	else if (color == "transparent") SetValue(CColor::Transparent);
	else if (color == "white") SetValue(CColor::White);
	else if (color == "gray") SetValue(CColor::Gray);
	else if (color == "black") SetValue(CColor::Black);
	else if (color == "red") SetValue(CColor::Red);
	else if (color == "green") SetValue(CColor::Green);
	else if (color == "blue") SetValue(CColor::Blue);
	else if (color == "yellow") SetValue(CColor::Yellow);
	else if (color == "purple") SetValue(CColor::Purple);
	else SetValue(CColor::White);
}

/**
* 设置颜色值
*/
void CColor::SetValue(const CColor& color) {
	m_fValue[0] = color.m_fValue[0];
	m_fValue[1] = color.m_fValue[1];
	m_fValue[2] = color.m_fValue[2];
	m_fValue[3] = color.m_fValue[3];
}

/**
* 设置颜色值
*/
void CColor::SetValue(const float color[4]) {
	m_fValue[0] = color[0];
	m_fValue[1] = color[1];
	m_fValue[2] = color[2];
	m_fValue[3] = color[3];
}

/**
* 设置颜色值
*/
void CColor::SetValue(float r, float g, float b) {
	m_fValue[0] = r;
	m_fValue[1] = g;
	m_fValue[2] = b;
	m_fValue[3] = 1.0f;
}

/**
* 设置颜色值
*/
void CColor::SetValue(float r, float g, float b, float a) {
	m_fValue[0] = r;
	m_fValue[1] = g;
	m_fValue[2] = b;
	m_fValue[3] = a;
}

/**
* 获取颜色值
*/
void CColor::GetValue(float* color) const {
	color[0] = m_fValue[0];
	color[1] = m_fValue[1];
	color[2] = m_fValue[2];
	color[3] = m_fValue[3];
}

/**
* 获取颜色值
*/
void CColor::GetValue(unsigned char* color) const {
	color[0] = (unsigned char)(m_fValue[0] * 255.0f);
	color[1] = (unsigned char)(m_fValue[1] * 255.0f);
	color[2] = (unsigned char)(m_fValue[2] * 255.0f);
	color[3] = (unsigned char)(m_fValue[3] * 255.0f);
}

/**
* 填充缓冲区
*/
void CColor::FillBuffer(float* buffer, int size, int stride) const {
	int channel = std::min(stride, 4);
	while (size >= stride) {
		for (int i = 0; i < channel; i++) buffer[i] = m_fValue[i];
		size -= stride;
		buffer += stride;
	}
}

/**
* 填充缓冲区
*/
void CColor::FillBuffer(unsigned char* buffer, int size, int stride) const {
	unsigned char color[4];
	color[0] = (unsigned char)(m_fValue[0] * 255.0f);
	color[1] = (unsigned char)(m_fValue[1] * 255.0f);
	color[2] = (unsigned char)(m_fValue[2] * 255.0f);
	color[3] = (unsigned char)(m_fValue[3] * 255.0f);
	int channel = std::min(stride, 4);
	while (size >= stride) {
		for (int i = 0; i < channel; i++) buffer[i] = color[i];
		size -= stride;
		buffer += stride;
	}
}

/**
* 颜色相加，相当于加上 color * scale
*/
CColor& CColor::Add(const CColor& color, float scale) {
	m_fValue[0] += color.m_fValue[0] * scale;
	m_fValue[1] += color.m_fValue[1] * scale;
	m_fValue[2] += color.m_fValue[2] * scale;
	m_fValue[3] += color.m_fValue[3] * scale;
	return *this;
}

/**
* 颜色相加，相当于加上 color * scale
*/
CColor& CColor::Add(const float color[4], float scale) {
	m_fValue[0] += color[0] * scale;
	m_fValue[1] += color[1] * scale;
	m_fValue[2] += color[2] * scale;
	m_fValue[3] += color[3] * scale;
	return *this;
}

/**
* 范围限制在 0.0 至 1.0 之间
*/
CColor& CColor::Clamp() {
	if (m_fValue[0] < 0.0f) m_fValue[0] = 0.0f;
	else if (m_fValue[0] > 1.0f) m_fValue[0] = 1.0f;
	if (m_fValue[1] < 0.0f) m_fValue[1] = 0.0f;
	else if (m_fValue[1] > 1.0f) m_fValue[1] = 1.0f;
	if (m_fValue[2] < 0.0f) m_fValue[2] = 0.0f;
	else if (m_fValue[2] > 1.0f) m_fValue[2] = 1.0f;
	if (m_fValue[3] < 0.0f) m_fValue[3] = 0.0f;
	else if (m_fValue[3] > 1.0f) m_fValue[3] = 1.0f;
	return *this;
}

/**
* 颜色反转
*/
CColor& CColor::Invert() {
	m_fValue[0] = 1.0f - m_fValue[0];
	m_fValue[1] = 1.0f - m_fValue[1];
	m_fValue[2] = 1.0f - m_fValue[2];
	return *this;
}

/**
* 从 HSLA 颜色空间转换
* @param h 色相 hue, 范围 0-360
* @param s 饱和度 saturation, 范围 0-100
* @param l 亮度 lightness, 范围 0-100
* @param a 不透明度 alpha, 范围 0-1
*/
CColor& CColor::FromHsla(float h, float s, float l, float a) {
	h *= 0.002777f;
	s *= 0.01f;
	l *= 0.01f;
	if (s == 0.0f) {
		m_fValue[0] = l;
		m_fValue[1] = l;
		m_fValue[2] = l;
	} else {
		float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
		float p = 2.0f * l - q;
		float t1 = h + 1.0f / 3.0f;
		float t2 = h;
		float t3 = h - 1.0f / 3.0f;
		if (t1 > 1.0f) t1 -= 1.0f;
		if (t1 < 1.0f / 6.0f) m_fValue[0] = p + (q - p) * 6.0f * t1;
		else if (t1 < 1.0f / 2.0f) m_fValue[0] = q;
		else if (t1 < 2.0f / 3.0f) m_fValue[0] = p + (q - p) * (2.0f / 3.0f - t1) * 6.0f;
		else m_fValue[0] = p;
		if (t2 < 1.0f / 6.0f) m_fValue[1] = p + (q - p) * 6.0f * t2;
		else if (t2 < 1.0f / 2.0f) m_fValue[1] = q;
		else if (t2 < 2.0f / 3.0f) m_fValue[1] = p + (q - p) * (2.0f / 3.0f - t2) * 6.0f;
		else m_fValue[1] = p;
		if (t3 < 0.0f) t3 += 1.0f;
		if (t3 < 1.0f / 6.0f) m_fValue[2] = p + (q - p) * 6.0f * t3;
		else if (t3 < 1.0f / 2.0f) m_fValue[2] = q;
		else if (t3 < 2.0f / 3.0f) m_fValue[2] = p + (q - p) * (2.0f / 3.0f - t3) * 6.0f;
		else m_fValue[2] = p;
	}
	m_fValue[3] = a;
	return *this;
}

/**
* 从名称获取颜色值
*/
CColor& CColor::FromName(const string& color) {
	if (color == "transparent") this->SetValue(CColor::Transparent);
	else if (color == "white") this->SetValue(CColor::White);
	else if (color == "gray") this->SetValue(CColor::Gray);
	else if (color == "black") this->SetValue(CColor::Black);
	else if (color == "red") this->SetValue(CColor::Red);
	else if (color == "green")this->SetValue(CColor::Green);
	else if (color == "blue") this->SetValue(CColor::Blue);
	else if (color == "yellow") this->SetValue(CColor::Yellow);
	else if (color == "purple") this->SetValue(CColor::Purple);
	else this->SetValue(CColor::White);
	return *this;
}

/**
* 返回颜色代码
*/
string CColor::ToString() {
	char buffer[10];
	unsigned int r = (unsigned int)(m_fValue[0] * 255.0f);
	unsigned int g = (unsigned int)(m_fValue[1] * 255.0f);
	unsigned int b = (unsigned int)(m_fValue[2] * 255.0f);
	unsigned int a = (unsigned int)(m_fValue[3] * 255.0f);
	sprintf(buffer, "#%02X%02X%02X%02X", r, g, b, a);
	return buffer;
}

/**
* 重载运算符 []
*/
float& CColor::operator [] (int index) {
	return m_fValue[index];
}

/**
* 重载运算符 ==
*/
bool CColor::operator == (const CColor& color) const {
	if (m_fValue[0] == color.m_fValue[0] &&
		m_fValue[1] == color.m_fValue[1] &&
		m_fValue[2] == color.m_fValue[2] &&
		m_fValue[3] == color.m_fValue[3]) {
		return true;
	}
	return false;
}

/**
* 重载运算符 ==
*/
bool CColor::operator != (const CColor& color) const {
	return !(*this == color);
}

/**
* 加法运算
*/
CColor CColor::operator + (const CColor& color) const {
	float vTemp[4];
	vTemp[0] = std::min(m_fValue[0] + color.m_fValue[0], 1.0f);
	vTemp[1] = std::min(m_fValue[1] + color.m_fValue[1], 1.0f);
	vTemp[2] = std::min(m_fValue[2] + color.m_fValue[2], 1.0f);
	vTemp[3] = std::min(m_fValue[3] + color.m_fValue[3], 1.0f);
	return CColor(vTemp);
}

/**
* 减法运算
*/
CColor CColor::operator - (const CColor& color) const {
	float vTemp[4];
	vTemp[0] = std::max(m_fValue[0] - color.m_fValue[0], 0.0f);
	vTemp[1] = std::max(m_fValue[1] - color.m_fValue[1], 0.0f);
	vTemp[2] = std::max(m_fValue[2] - color.m_fValue[2], 0.0f);
	vTemp[3] = std::max(m_fValue[3] - color.m_fValue[3], 0.0f);
	return CColor(vTemp);
}

/**
* 乘法运算
*/
CColor CColor::operator * (const CColor& color) const {
	float vTemp[4];
	vTemp[0] = m_fValue[0] * color.m_fValue[0];
	vTemp[1] = m_fValue[1] * color.m_fValue[1];
	vTemp[2] = m_fValue[2] * color.m_fValue[2];
	vTemp[3] = m_fValue[3] * color.m_fValue[3];
	return CColor(vTemp);
}

/**
* 重载运算符 +=
*/
CColor CColor::operator += (const CColor& color) {
	m_fValue[0] = std::min(m_fValue[0] + color.m_fValue[0], 1.0f);
	m_fValue[1] = std::min(m_fValue[1] + color.m_fValue[1], 1.0f);
	m_fValue[2] = std::min(m_fValue[2] + color.m_fValue[2], 1.0f);
	m_fValue[3] = std::min(m_fValue[3] + color.m_fValue[3], 1.0f);
	return *this;
}

/**
* 重载运算符 -=
*/
CColor CColor::operator -= (const CColor& color) {
	m_fValue[0] = std::max(m_fValue[0] - color.m_fValue[0], 0.0f);
	m_fValue[1] = std::max(m_fValue[1] - color.m_fValue[1], 0.0f);
	m_fValue[2] = std::max(m_fValue[2] - color.m_fValue[2], 0.0f);
	m_fValue[3] = std::max(m_fValue[3] - color.m_fValue[3], 0.0f);
	return *this;
}

/**
* 重载运算符 *=
*/
CColor CColor::operator *= (const CColor& color) {
	m_fValue[0] *= color.m_fValue[0];
	m_fValue[1] *= color.m_fValue[1];
	m_fValue[2] *= color.m_fValue[2];
	m_fValue[3] *= color.m_fValue[3];
	return *this;
}