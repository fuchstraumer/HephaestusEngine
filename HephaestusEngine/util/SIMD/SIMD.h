#pragma once
#ifndef SIMD_H
#define SIMD_H
template<class T, size_t N>
class __declspec(align(N)) SIMDv {
public:
	T Data;
};
#if defined(_WIN64)
// 64-bit CPU's should all support SSE3 instruction set
#define SIMD_LEVEL_SSE3
#endif

// Comment this line out if your CPU doesn't support SSE4.1 instructions.
#define SIMD_LEVEL_SSE41
// Currently only one SSE41 instruction - floor for a float-vec

// Set correct aligned memory allocation function based on OS/Compiler
#if defined(_WIN32)
template <typename T>
__inline T* aligned_malloc(std::size_t alignment) {
	return static_cast<T*>(_aligned_malloc(sizeof(T), alignment));
};
#endif

// Untested, but these "should" be correct.
#if defined(unix)
template<typename T>
__inline T* aligned_malloc(std::size_t alignment) {
	return posix_memalign(sizeof(T),alignment);
}
#endif
#define SIMD_LEVEL_AVX2
// AVX implementation not functional. AVX is very sparse
// features-wise, AVX2 has actual features
#ifdef SIMD_LEVEL_AVX2
#include "SIMD_AVX.h"
#endif // SIMD_LEVEL_AVX
#ifdef SIMD_LEVEL_SSE3
#include "SIMD_SSE.h"
#endif


#endif // !SIMD_H
