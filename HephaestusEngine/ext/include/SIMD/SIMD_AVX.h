#pragma once
#ifndef SIMD_AVX_H
#define SIMD_AVX_H

#include "SIMD.h"
#ifdef SIMD_LEVEL_AVX2
#include <immintrin.h>
#include <cstdlib>
namespace simd {
	class ivec8;
	class dvec4;

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
		__forceinline vec8& operator+=(vec8 const &other) {
			this->Data = _mm256_add_ps(this->Data, other.Data);
			return *this;
		}
		__forceinline vec8& operator-=(vec8 const &other) {
			this->Data = _mm256_sub_ps(this->Data, other.Data);
			return *this;
		}
		__forceinline vec8& operator*=(vec8 const &other) {
			this->Data = _mm256_mul_ps(this->Data, other.Data);
			return *this;
		}
		__forceinline vec8& operator/=(vec8 const &other) {
			this->Data = _mm256_div_ps(this->Data, other.Data);
			return *this;
		}
		// Unary logic operators
		__forceinline vec8& operator&=(vec8 const &a) {
			this->Data = _mm256_and_ps(this->Data, a.Data);
			return *this;
		}
		__forceinline vec8& operator|=(vec8 const &a) {
			this->Data = _mm256_or_ps(this->Data, a.Data);
			return *this;
		}

		// Increment/Decrement operators
		__forceinline vec8& operator++() {
			vec8 one(1.0f);
			*this += one;
			return *this;
		}
		__forceinline vec8& operator--() {
			vec8 one(1.0f);
			*this -= one;
			return *this;
		}

		// Binary operators
		__forceinline vec8 operator+(vec8 const &other) const {
			return vec8(_mm256_add_ps(this->Data, other.Data));
		}
		__forceinline vec8 operator-(vec8 const &other) const {
			return vec8(_mm256_sub_ps(this->Data, other.Data));
		}
		__forceinline vec8 operator*(vec8 const &other) const {
			return vec8(_mm256_mul_ps(this->Data, other.Data));
		}
		__forceinline vec8 operator/(vec8 const &other) const {
			return vec8(_mm256_div_ps(this->Data, other.Data));
		}

		// Binary logic operators
		__forceinline vec8 operator&(vec8 const &other) const {
			return vec8(_mm256_and_ps(this->Data, other.Data));
		}
		__forceinline vec8 operator|(vec8 const &other) const {
			return vec8(_mm256_or_ps(this->Data, other.Data));
		}

		// Binary comparison operators
		__forceinline vec8 operator>(vec8 const &other) const {
			return vec8(_mm256_cmp_ps(this->Data, other.Data, _CMP_GT_OQ));
		}
		__forceinline vec8 operator>=(vec8 const &other) const {
			return vec8(_mm256_cmp_ps(this->Data, other.Data, _CMP_GE_OQ));
		}
		__forceinline vec8 operator<(vec8 const &other) const {
			return vec8(_mm256_cmp_ps(this->Data, other.Data, _CMP_LT_OQ));
		}
		__forceinline vec8 operator<=(vec8 const &other) const {
			return vec8(_mm256_cmp_ps(this->Data, other.Data, _CMP_LE_OQ));
		}

		// Store

		// Load

		

		// Math operators
		__forceinline vec8 floor(const vec8 &in) {
			return vec8(_mm256_floor_ps(in.Data));
		}

		__forceinline static vec8 and_not(const vec8& v0, const vec8& v1) {
			return vec8(_mm256_andnot_ps(v0.Data, v1.Data));
		}

		__forceinline static vec8 xor (const vec8& v0, const vec8& v1) {
			return vec8(_mm256_xor_ps(v0.Data, v1.Data));
		}
	};

	class ivec8 : public SIMDv<__m256i, 32> {
	public:
		// Constructors
		ivec8() : SIMDv() {
			this->Data = _mm256_setzero_si256();
		}

		ivec8(__m256i data) {
			this->Data = data;
		}

		ivec8(__m256 data) {
			this->Data = _mm256_cvttps_epi32(data);
		}

		ivec8(float a) {
			this->Data = _mm256_set1_epi32(a);
		}

		ivec8(float a, float b, float c = 0.0f, float d = 0.0f, float e = 0.0f, float f = 0.0f, float g = 0.0f, float h = 0.0f) {
			this->Data = _mm256_set_epi32(h, g, f, e, d, c, b, a);
		}

		ivec8(ivec8 const &other) {
			this->Data = other.Data;
		}

		// Conversion
		__forceinline static vec8 ConvertToFloat(ivec8 const &a) {
			return vec8(_mm256_cvtepi32_ps(a.Data));
		}
		__forceinline static vec8 CastToFloat(ivec8 const& a) {
			return vec8(_mm256_castsi256_ps(a.Data));
		}
		// Operators

		// Unary arithmetic operators
		__forceinline ivec8& operator+=(ivec8 const &other) {
			this->Data = _mm256_add_epi32(this->Data, other.Data);
			return *this;
		}
		__forceinline ivec8& operator-=(ivec8 const &other) {
			this->Data = _mm256_sub_epi32(this->Data, other.Data);
			return *this;
		}
		__forceinline ivec8 operator*=(ivec8 const &other) {
			this->Data = _mm256_mul_epi32(this->Data, other.Data);
			return *this;
		}

		// Unary logic operators
		__forceinline ivec8 const operator~() const {

		}
		__forceinline ivec8& operator&=(ivec8 const& a) {
			this->Data = _mm256_and_si256(this->Data, a.Data);
			return *this;
		}
		__forceinline ivec8& operator|=(ivec8 const& a) {
			this->Data = _mm256_or_si256(this->Data, a.Data);
			return *this;
		}

		// Increment and decrement operators
		__forceinline ivec8& operator++() {
			ivec8 one(1);
			this->Data = _mm256_add_epi32(this->Data, one.Data);
			return *this;
		}
		__forceinline ivec8& operator--() {
			ivec8 one(1);
			this->Data = _mm256_sub_epi32(this->Data, one.Data);
			return *this;
		}

		// Binary arithmetic operators
		__forceinline ivec8 const operator+(ivec8 const &a) const {
			return ivec8(_mm256_add_epi32(this->Data, a.Data));
		}
		__forceinline ivec8 const operator-(ivec8 const &a) const {
			return ivec8(_mm256_sub_epi32(this->Data, a.Data));
		}
		__forceinline ivec8 const operator*(ivec8 const &a) const {
			return ivec8(_mm256_mul_epi32(this->Data, a.Data));
		}


		// No division operand

		// Binary logic operators
		__forceinline ivec8 const operator&(ivec8 const& a) const {
			return ivec8(_mm256_and_si256(this->Data, a.Data));
		}
		__forceinline ivec8 const operator|(ivec8 const& a) const {
			return ivec8(_mm256_or_si256(this->Data, a.Data));
		}

		// Binary operands for bitshifting
		__forceinline ivec8 const operator>>(int const& count) const {
			return ivec8(_mm256_slli_epi32(this->Data, count));
		}
		__forceinline ivec8 const operator<<(int const& count) const {
			return ivec8(_mm256_srli_epi32(this->Data, count));
		}
		// Binary comparison operators
		__forceinline ivec8 operator>(ivec8 const& other) const {
			return ivec8(_mm256_cmpgt_epi32(this->Data, other.Data));
		}
		__forceinline ivec8 operator<(ivec8 const& other) const {
			return ivec8(_mm256_cmpgt_epi32(other.Data, this->Data));
		}
		__forceinline ivec8 operator==(ivec8 const& other) const {
			return ivec8(_mm256_cmpeq_epi32(this->Data, other.Data));
		}
		// Other operators/functions
		__forceinline static ivec8 and_not(ivec8 const& v0, ivec8 const& v1) {
			return ivec8(_mm256_andnot_si256(v0.Data, v1.Data));
		}
		__forceinline static ivec8 xor (ivec8 const& v0, ivec8 const& v1) {
			return ivec8(_mm256_xor_si256(v0.Data, v1.Data));
		}
		__forceinline static ivec8 not(ivec8 const& v0) {
			return ivec8(_mm256_xor_si256(v0.Data, ivec8(0xffffffff).Data));
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
