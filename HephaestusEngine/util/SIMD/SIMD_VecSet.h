#pragma once
#ifndef SIMD_VEC_SET_H
#define SIMD_VEC_SET_H
#include <vector>
#include <array>
#include "SIMD.h"
#ifdef SIMD_LEVEL_SSE3
#include "SIMD_SSE.h"

namespace simd {
	// Build a vector set ranging from x0:x1, y0:y1, z0:z1, with step size h
	std::array<std::vector<vec4>,3> vec4Set(float x0, float x1, float y0, float y1, float z0, float z1, float h) {
		std::array<std::vector<vec4>, 3> result;
		std::vector<vec4> xresult; xresult.reserve(floorf(x1 - x0 / h));
		std::vector<vec4> yresult; yresult.reserve(floorf(y1 - y0 / h));
		std::vector<vec4> zresult; zresult.reserve(floorf(z1 - z0 / h));
		for (float i = x0; i < x1; i += h*4) {
					vec4 v(i, i+h, i+(h*2), i+(h*3));
					xresult.push_back(v);
		}
		result[0] = std::move(xresult);
		for (float i = y0; i < y1; i += h * 4) {
			vec4 v(i, i + h, i + h * 2, i + h * 3);
			yresult.push_back(v);
		}
		result[1] = std::move(yresult);
		for (float i = z0; i < z1; i += h * 4) {
			vec4 v(i, i + h, i + h * 2, i + h * 3);
			zresult.push_back(v);
		}
		result[2] = std::move(zresult);
		return result;
	}
}
#endif // SIMD_LEVEL_SSE3



#endif // !SIMD_VEC_SET_H
