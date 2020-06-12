//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CFFT_H_
#define _CFFT_H_
#include "AvatarConfig.h"
#include <complex>
using std::complex;

/**
* @brief 快速傅里叶变换类
* @note FFT 生成的频谱图范围为(0Hz ~ Fs/2)且左右对称，频谱图分辨率为 Fs/N.
*	直流分量幅度 abs(Fn)/N [n=0]，非直流分量幅度 abs(Fn)/(N/2)，相位为 atan2(Fn.imag, Fn.real).
*	注：Fs是采样频率，N是采样点数，Fn是频谱图上点n的复数值
*/
class AVATAR_EXPORT CFFT {
public:
	//! FFT 变换
	static bool Forward(complex<float>* data, int count);
	//! FFT 变换
	static bool Forward(const complex<float>* input, complex<float>* output, int count);
	//! FFT 逆变换
	static bool Inverse(complex<float>* data, int count, bool scale = true);
	//! FFT 逆变换
	static bool Inverse(const complex<float>* input, complex<float>* output, int count, bool scale = true);

	//! 获取频谱主频
	static float DominantFrequency(const complex<float>* data, int count, float sampleRate);

private:
	//! 重新排列数组
	static void Rearrange(complex<float>* data, int count);
	//! 重新排列数组
	static void Rearrange(const complex<float>* input, complex<float>* output, int count);
	//! FFT 方法
	static void Perform(complex<float>* data, int count, bool inverse);
};

#endif