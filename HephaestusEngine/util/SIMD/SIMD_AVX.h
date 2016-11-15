#pragma once
#ifndef SIMD_AVX_H
#define SIMD_AVX_H

#include "SIMD.h"
#ifdef SIMD_LEVEL_AVX2
#include <immintrin.h>
#include <cstdlib>
namespace simd {
	class vec8 : public SIMDv<__m256, 32> {
	public:
		// Constructors
		vec8() : SIMDv() {
			this->Data = _mm256_setzero_ps();
		}

		vec8(__m256 data) {
			this->Data = data;
		}

		vec8(float a) {
			this->Data = _mm256_set1_ps(a);
		}

		vec8(float a, float b, float c = 0.0f, float d = 0.0f, float e = 0.0f, float f = 0.0f, float g = 0.0f, float h = 0.0f) {
			this->Data = _mm256_set_ps(h, g, f, e, d, c, b, a);
		}

		vec8(vec8 const &other) {
			this->Data = other.Data;
		}
		// Basic operators
		// Copy/equals
		__inline vec8& operator=(vec8 const &other) {
			this->Data = other.Data;
			return *this;
		}
		// Unary operators
		__inline vec8& operator+=(vec8 const &other) {
			this->Data = _mm256_add_ps(this->Data, other.Data);
			return *this;
		}
		__inline vec8& operator-=(vec8 const &other) {
			this->Data = _mm256_sub_ps(this->Data, other.Data);
			return *this;
		}
		__inline vec8& operator*=(vec8 const &other) {
			this->Data = _mm256_mul_ps(this->Data, other.Data);
			return *this;
		}
		__inline vec8& operator/=(vec8 const &other) {
			this->Data = _mm256_div_ps(this->Data, other.Data);
		}
		// Binary operators
		__inline vec8 operator+(vec8 const &other) const {
			return vec8(_mm256_add_ps(this->Data, other.Data));
		}
		__inline vec8 operator-(vec8 const &other) const {
			return vec8(_mm256_sub_ps(this->Data, other.Data));
		}
		__inline vec8 operator*(vec8 const &other) const {
			return vec8(_mm256_mul_ps(this->Data, other.Data));
		}
	};

	class dvec4 : public SIMDv<__m256d, 32>{
	public:
		dvec4() {
			this->Data = _mm256_setzero_pd();
		}
		dvec4(double a) {
			this->Data = _mm256_set1_pd(a);
		}
		dvec4(double a, double b, double c, double d) {
			// Order in memory is effectively reversed.
			this->Data = _mm256_set_pd(d, c, b, a);
		}
	};
}
#endif // SIMD_LEVEL_AVX


#endif // !SIMD_AVX_H
