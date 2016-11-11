#pragma once
#ifndef SIMD_AVX_H
#define SIMD_AVX_H

#include "SIMD.h"
#ifdef SIMD_LEVEL_AVX
#include <immintrin.h>
#include <cstdlib>

using vf8Data = union {
	__m256 m;
	float v[8];
};

template<class T, std::size_t N>
class __declspec(align(N)) SIMDV {
public:
	SIMD8(){
		this->Data = (T*)_aligned_malloc(sizeof(T), sizeof(T) / N);
	}
	~SIMD8() {
		_aligned_free(this->Data);
	}
	T* Data;
};

class SIMD8fv : public SIMDV<__m256,32> {
public:
	// Initialize this data with all zeros
	SIMD8fv() : SIMDV() {
		*this->Data = _mm256_setzero_ps();
	}
	// Initialize this data type with eight values.
	SIMD8fv(float a, float b, float c, float d, float e, float f, float g, float h) : SIMDV() {
		*this->Data = _mm256_set_ps(a, b, c, d, e, f, g, h);
	}
	// Initialize a vector with all eight values set to f
	SIMD8fv(float f) : SIMDV() {
		*this->Data = _mm256_set1_ps(f);
	}
	// Store *data in the SIMD data of this class
	void Store(float* data) {
		_mm256_storeu_ps(data, *this->Data);
	}
	// Load into this object
	void Load(float* data) {
		*this->Data = _mm256_loadu_ps(data);
	}
	// Get specified element
	float* GetElement(int i) {
		this->Data->m256_f32[i % 8];
	}
	// Operators for SIMD types
	__inline SIMD8fv operator+(SIMD8fv& other) const {
		_mm256_add_ps(*this->Data, *other.Data);
	}
	__inline SIMD8fv operator-(SIMD8fv& other) const {
		_mm256_sub_ps(*this->Data, *other.Data);
	}
	__inline SIMD8fv operator*(SIMD8fv& other) const {
		_mm256_mul_ps(*this->Data, *other.Data);
	}
	__inline SIMD8fv operator/(SIMD8fv& other) const {
		_mm256_div_ps(*this->Data, *other.Data);
	}
	__inline SIMD8fv FMAdd(SIMD8fv& other0, SIMD8fv& other1) const {
		_mm256_fmadd_ps(*this->Data, *other0.Data, *other1.Data);
	}
};

class SIMD8dv : public SIMDV<__m256d, 64> {
public:
	// Construct with four values equal to a,b,c,d
	SIMD8dv(double a, double b, double c, double d) : SIMDV() {
		*this->Data = _mm256_set_pd(a, b, c, d);
	}
	// Construct with all four values equal to x
	SIMD8dv(double x) {
		*this->Data = _mm256_set1_pd(x);
	}
	// Get specified element
	double* GetElement(int i) {
		this->Data->m256d_f64[i % 8];
	}
	// Operators for SIMD types
	__inline SIMD8dv operator+(SIMD8dv& other) const {
		_mm256_add_pd(*this->Data, *other.Data);
	}
	__inline SIMD8dv operator-(SIMD8dv& other) const {
		_mm256_sub_pd(*this->Data, *other.Data);
	}
	__inline SIMD8dv operator*(SIMD8dv& other) const {
		_mm256_mul_pd(*this->Data, *other.Data);
	}
	__inline SIMD8dv operator/(SIMD8dv& other) const {
		_mm256_div_pd(*this->Data, *other.Data);
	}
	// Multiplies this by other0 and then adds other1 to the result.
	__inline SIMD8dv FMAdd(SIMD8dv& other0, SIMD8dv& other1) const {
		_mm256_fmadd_pd(*this->Data, *other0.Data, *other1.Data);
	}
};


class SIMD16uv : public SIMDV<__m256i, 16> {
public:
	// Construct with four values equal to a,b,c,d
	SIMD16uv(double a, double b, double c, double d) : SIMDV() {
		*this->Data = _mm256_set_epi16(a, b, c, d);
	}
	// Construct with all four values equal to x
	SIMD16uv(double x) {
		*this->Data = _mm256_set1_epi16(x);
	}
	// Get specified element
	int* GetElement(int i) {
		this->Data->m256i_u16[i % 16];
	}
	// Operators for SIMD types
	__inline SIMD16uv operator+(SIMD16uv& other) const {
		_mm256_adds_epu16(*this->Data, *other.Data);
	}
	__inline SIMD16uv operator-(SIMD16uv& other) const {
		_mm256_subs_epu16(*this->Data, *other.Data);
	}
};



#endif // SIMD_LEVEL_AVX


#endif // !SIMD_AVX_H
