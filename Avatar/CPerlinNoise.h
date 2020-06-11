//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CPERLINNOISE_H_
#define _CPERLINNOISE_H_
#include <cmath>

/**
* @brief Perlin 噪声发生器
*/
class CPerlinNoise {
public:
	/**
	* 获取点 X,Y 的 Perlin 噪声值
	*/
	float GetValue(float x, float y) {
		const int octaves = 3;
		const float persistence = 0.5f;
		float total = 0.0f;
		for (int i = 0; i < octaves; i++) {
			float frequency = pow(2.0f, i);
			float amplitude = pow(persistence, i);
			total += InterpolatedNoise(x * frequency, y * frequency) * amplitude;
		}
		return total;
	}

private:
	/**
	* 生成噪声
	*/
	inline float Noise(int x, int y) {
		x = x % 25;
		y = y % 25;
		int n = x + y * 57;
		n = (n << 13) ^ n;
		return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f); 
	}
	/**
	* 光滑噪声
	*/
	inline float SmoothNoise(int x, int y) {
		float corners = (Noise(x-1, y-1) + Noise(x+1, y-1) + Noise(x-1, y+1) + Noise(x+1, y+1)) / 16.0f;
		float sides = (Noise(x-1, y) + Noise(x+1, y) + Noise(x, y-1) + Noise(x, y+1)) / 8.0f;
		float center = Noise(x, y) / 4.0f;
		return (corners + sides + center);
	}
	/**
	* 余弦插值函数
	*/
	inline float CosineInterpolate(float a, float b, float x) {
		float f = (1.0f - cosf(x * 3.1415927f)) * 0.5f;
		return a * (1.0f - f) + b * f;
	}
	/**
	* 生成插值噪声
	*/
	inline float InterpolatedNoise(float x, float y) {
		int ix = static_cast<int>(x < 0 ? x - 1 : x);
		int iy = static_cast<int>(y < 0 ? y - 1 : y);
		float fx = x - ix;
		float fy = y - iy;

		float v1 = SmoothNoise(ix, iy);
		float v2 = SmoothNoise(ix + 1, iy);
		float v3 = SmoothNoise(ix, iy + 1);
		float v4 = SmoothNoise(ix + 1, iy + 1);

		float i1 = CosineInterpolate(v1, v2, fx);
		float i2 = CosineInterpolate(v3, v4, fx);
		return CosineInterpolate(i1, i2, fy);
	}
};

#endif