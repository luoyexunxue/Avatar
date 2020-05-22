//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CFFT_H_
#define _CFFT_H_
#include "AvatarConfig.h"
#include <complex>
using std::complex;

/**
* @brief ���ٸ���Ҷ�任��
* @note FFT ���ɵ�Ƶ��ͼ��ΧΪ(0Hz ~ Fs/2)�����ҶԳƣ�Ƶ��ͼ�ֱ���Ϊ Fs/N.
*	ֱ���������� abs(Fn)/N [n=0]����ֱ���������� abs(Fn)/(N/2)����λΪ atan2(Fn.imag, Fn.real).
*	ע��Fs�ǲ���Ƶ�ʣ�N�ǲ���������Fn��Ƶ��ͼ�ϵ�n�ĸ���ֵ
*/
class AVATAR_EXPORT CFFT {
public:
	//! FFT �任
	static bool Forward(complex<float>* data, int count);
	//! FFT �任
	static bool Forward(const complex<float>* input, complex<float>* output, int count);
	//! FFT ��任
	static bool Inverse(complex<float>* data, int count, bool scale = true);
	//! FFT ��任
	static bool Inverse(const complex<float>* input, complex<float>* output, int count, bool scale = true);

	//! ��ȡƵ����Ƶ
	static float DominantFrequency(const complex<float>* data, int count, float sampleRate);

private:
	//! ������������
	static void Rearrange(complex<float>* data, int count);
	//! ������������
	static void Rearrange(const complex<float>* input, complex<float>* output, int count);
	//! FFT ����
	static void Perform(complex<float>* data, int count, bool inverse);
};

#endif