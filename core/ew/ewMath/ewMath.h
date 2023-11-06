/*
	Author: Eric Winebrenner
*/

#pragma once
#include <random>
#include <math.h>

#include "vec2.h"
#include "vec3.h"
#include "mat4.h"

namespace ew {
	constexpr float PI = 3.14159265359f;
	constexpr float TAU = 6.283185307179586f;
	constexpr float DEG2RAD = (PI / 180.0f);
	constexpr float RAD2DEG = (180.0f / PI);
	inline float Radians(float degrees) {
		return degrees * DEG2RAD;
	}
	inline float Degrees(float radians) {
		return radians * RAD2DEG;
	}
	inline float RandomRange(float min, float max) {
		float t = (float)rand() / RAND_MAX;
		return min + (max - min) * t;
	}
	inline float Clamp(float x, float min, float max) {
		return std::fminf(std::fmaxf(x, min), max);
	}
}
