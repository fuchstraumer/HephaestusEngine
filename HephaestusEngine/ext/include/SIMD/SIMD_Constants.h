#pragma once
#ifndef SIMD_CONSTANTS_H
#define SIMD_CONSTANTS_H
#include "SIMD.h"
// This header defines a number of useful constants for each instruction set,
// from things like pi to common fractional numbers. These are easier to use
// than initializing a new vec4/vec8 each time one is needed.

namespace simd {
#ifdef SIMD_LEVEL_SSE3
	// Constants for SSE3 instruction set
	#define thirdconst vec4(0.3333333f)
	#define sixthconst vec4(0.1666667f)
	#define halfconst vec4(0.5f)
	#define oneVecF vec4(1.0f)
	#define oneVecI ivec4(1,1,1,1)
	// These constants are for use with the noise functions
	#define xPrime ivec4(1619,1619,1619,1619)
	#define yPrime ivec4(31337,31337,31337,31337)
	#define zPrime ivec4(6971,6971,6971,6971)
#endif // SIMD_LEVEL_SSE3

#ifdef SIMD_LEVEL_AVX2
	// Constants for AVX2 instruction set
#endif // SIMD_LEVEL_AVX2

}

#endif // !SIMD_CONSTANTS_H
