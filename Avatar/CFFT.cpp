//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CFFT.h"
#include <cmath>

/**
* FFT �任
* @param data �������ݼ��任���
* @param count ���ݳ���
* @return �ɹ����� true
*/
bool CFFT::Forward(complex<float>* data, int count) {
	if (!data || count < 1 || count & (count - 1)) {
		return false;
	}
	Rearrange(data, count);
	Perform(data, count, false);
	return true;
}

/**
* FFT �任
* @param input ��������
* @param output �任���
* @param count ���ݳ���
* @return �ɹ����� true
*/
bool CFFT::Forward(const complex<float>* input, complex<float>* output, int count) {
	if (!input || !output || count < 1 || count & (count - 1)) {
		return false;
	}
	Rearrange(input, output, count);
	Perform(output, count, false);
	return true;
}

/**
* FFT ��任
* @param data �������ݼ��任���
* @param count ���ݳ���
* @param scale �Ƿ�Խ����һ��
* @return �ɹ����� true
*/
bool CFFT::Inverse(complex<float>* data, int count, bool scale) {
	if (!data || count < 1 || count & (count - 1)) {
		return false;
	}
	Rearrange(data, count);
	Perform(data, count, true);
	if (scale) {
		const float factor = 1.0f / float(count);
		for (int i = 0; i < count; i++) {
			data[i] *= factor;
		}
	}
	return true;
}

/**
* FFT ��任
* @param input ��������
* @param output �任���
* @param count ���ݳ���
* @param scale �Ƿ�Խ����һ��
* @return �ɹ����� true
*/
bool CFFT::Inverse(const complex<float>* input, complex<float>* output, int count, bool scale) {
	if (!input || !output || count < 1 || count & (count - 1)) {
		return false;
	}
	Rearrange(input, output, count);
	Perform(output, count, true);
	if (scale) {
		const float factor = 1.0f / float(count);
		for (int i = 0; i < count; i++) {
			output[i] *= factor;
		}
	}
	return true;
}

/**
* ��ȡƵ����Ƶ
* @param data Ƶ������
* @param count ���ݳ���
* @param sampleRate ����Ƶ��
* @return ��Ƶ��ֵ
*/
float CFFT::DominantFrequency(const complex<float>* data, int count, float sampleRate) {
	count >>= 1;
	int maxFreq = 0;
	float maxNorm = 0.0f;
	for (int i = 1; i < count; i++) {
		float norm = std::norm(data[i]);
		if (norm > maxNorm) {
			maxNorm = norm;
			maxFreq = i;
		}
	}
	return 0.5f * sampleRate * maxFreq / float(count);
}

/**
* ������������
*/
void CFFT::Rearrange(complex<float>* data, int count) {
	int target = 0;
	for (int i = 0; i < count; i++) {
		if (target > i) {
			const complex<float> temp(data[target]);
			data[target] = data[i];
			data[i] = temp;
		}
		int mask = count;
		while (target & (mask >>= 1)) {
			target &= ~mask;
		}
		target |= mask;
	}
}

/**
* ������������
*/
void CFFT::Rearrange(const complex<float>* input, complex<float>* output, int count) {
	int target = 0;
	for (int i = 0; i < count; i++) {
		output[target] = input[i];
		int mask = count;
		while (target & (mask >>= 1)) {
			target &= ~mask;
		}
		target |= mask;
	}
}

/**
* ���ٸ���Ҷ�任ʵ��
*/
void CFFT::Perform(complex<float>* data, int count, bool inverse) {
	const float PI = inverse ? 3.14159f : -3.14159f;
	for (int step = 1; step < count; step <<= 1) {
		const int jump = step << 1;
		const float delta = PI / float(step);
		const float sine = sinf(delta * 0.5f);
		const complex<float> multiplier(-2.0f * sine * sine, sin(delta));
		complex<float> factor(1.0f);
		for (int group = 0; group < step; group++) {
			for (int pair = group; pair < count; pair += jump) {
				const int match = pair + step;
				const complex<float> product(factor * data[match]);
				data[match] = data[pair] - product;
				data[pair] += product;
			}
			factor = multiplier * factor + factor;
		}
	}
}