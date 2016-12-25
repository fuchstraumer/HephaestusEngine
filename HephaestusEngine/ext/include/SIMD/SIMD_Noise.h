#ifndef SIMD_NOISE_H
#define SIMD_NOISE_H
#include "SIMD.h"
#include "SIMD_Math.h"
#include "SIMD_Constants.h"
#include <random>
#include <functional>
namespace simd {


// Following are noise functions for SSE3/4.1 intrinsics.

#ifdef SIMD_LEVEL_SSE3
#include "SIMD_SSE.h"

	// "Ease curve" for noise values.
	vec4 quintic_interp(vec4 const& input);
	
	ivec4 hash(ivec4 const &seed, ivec4 const& x, ivec4 const& y, ivec4 const& z);

	vec4 gradientcoord(ivec4 const &seed, ivec4 const &xi, ivec4 const &yi, ivec4 const &zi, vec4 const &x, vec4 const& y, vec4 const &z);
	
	vec4 simplex(ivec4 const &seed, vec4 const &x, vec4 const &y, vec4 const &z);

	float FBM(ivec4 const &seed, vec4 const &xi, vec4 const &yi, vec4 const &zi, float frequency, int octaves, float lacunarity, float gain);

	vec4 RidgedMulti(ivec4 const &seed, vec4 const &xi, vec4 const &yi, vec4 const &zi, float frequency, int octaves, float lacunariy, float gain);

#endif // SIMD_LEVEL_SSE3


#ifdef SIMD_LEVEL_AVX2
#include "SIMD_AVX.h"
	// Following are noise functions for AVX2 instructions.
	// Operate on vec8's

	static __forceinline ivec8 hash(ivec8 const& seed, ivec8 const &x, ivec8 const &y, ivec8 const &z);

	static ivec8 gradient(ivec8 const& seed, ivec8 const &xi, ivec8 const &yi, ivec8 const &zi, ivec8 const &x, ivec8 const &y, ivec8 const &z);

	static vec8 simplex(ivec8 const& seed, vec8 const &x, vec8 const &y, vec8 const &z);

}

#endif // SIMD_LEVEL_AVX2
/*

	SIMD Noise map/storage class useable by any/all types of instructions.

*/

#endif // !SIMD_TERRAIN_H
