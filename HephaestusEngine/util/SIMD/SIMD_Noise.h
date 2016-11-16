#ifndef SIMD_NOISE_H
#define SIMD_NOISE_H
#include "SIMD_SSE.h"
#include "SIMD_Math.h"
#include "SIMD_Constants.h"
#include <random>
namespace simd {


// Following are noise functions for SSE3/4.1 intrinsics.

#ifdef SIMD_LEVEL_SSE3

	

	// "Ease curve" for noise values.
	static vec4 quintic_interp(vec4 const& input) {

	}
	
	static ivec4 hash(ivec4 const &seed, ivec4 const& x, ivec4 const& y, ivec4 const& z) {
		ivec4 hashtmp = seed;
		hashtmp += (x * xPrime);
		hashtmp += (y * yPrime);
		hashtmp += (z * zPrime);

		hashtmp = ((hashtmp * hashtmp) * ivec4(60493)) * hashtmp;
		hashtmp = ivec4::xor((hashtmp >> 13), hashtmp);
		return hashtmp;
	}

	static vec4 gradientcoord(ivec4 const &seed, ivec4 const &xi, ivec4 const &yi, ivec4 const &zi, vec4 const &x, vec4 const& y, vec4 const &z) {
		ivec4 hashresult = ((hash(seed, xi, yi, zi))) & ivec4(15);
		vec4 u = CastToFloat((hashresult < ivec4(8)));
		u = blendv(y, x, u);

		vec4 v = CastToFloat(hashresult < ivec4(4));
		vec4 h12o14 = CastToFloat((hashresult == ivec4(12)) | (hashresult == ivec4(14)));
		h12o14 = blendv(z, x, h12o14);
		v = blendv(h12o14, y, v);

		vec4 h1 = CastToFloat((hashresult & ivec4(1)) << 31);
		vec4 h2 = CastToFloat((hashresult & ivec4(2)) << 30);
		
		return (vec4::xor(u, h1) + vec4::xor(v, h2));
	}
	
	static vec4 simplex(ivec4 const &seed, vec4 const &x, vec4 const &y, vec4 const &z) {
		vec4 f; f = thirdconst * ((x + y) + z);
		vec4 x0 = floor(x + f);
		vec4 y0 = floor(y + f);
		vec4 z0 = floor(z + f);

		ivec4 i = ConvertToInt(x);
		ivec4 j = ConvertToInt(y);
		ivec4 k = ConvertToInt(z);

		vec4 g = thirdconst * ConvertToFloat((i + j) + k);
		x0 = x - (x0 - g);
		y0 = y - (y0 - g);
		z0 = z - (z0 - g);

		ivec4 x0_ge_y0 = CastToInt(x0 >= y0);
		ivec4 y0_ge_z0 = CastToInt(y0 >= z0);
		ivec4 x0_ge_z0 = CastToInt(x0 >= z0);

		ivec4 i1 = oneVecI & (x0_ge_y0 & x0_ge_z0);
		ivec4 j1 = oneVecI & ivec4::andnot(x0_ge_z0, y0_ge_z0);
		ivec4 k1 = oneVecI & ivec4::andnot(x0_ge_z0, ~y0_ge_z0);

		ivec4 i2 = oneVecI & (x0_ge_y0 | x0_ge_z0);
		ivec4 j2 = oneVecI & (~x0_ge_y0 | y0_ge_z0);
		ivec4 k2 = oneVecI & (x0_ge_y0 & y0_ge_z0);

		vec4 x1 = (x0 - ConvertToFloat(i1)) + thirdconst;
		vec4 y1 = (y0 - ConvertToFloat(j1)) + thirdconst;
		vec4 z1 = (z0 - ConvertToFloat(k1)) + thirdconst;
		vec4 x2 = (x0 - ConvertToFloat(i2)) + halfconst;
		vec4 y2 = (y0 - ConvertToFloat(j2)) + halfconst;
		vec4 z2 = (z0 - ConvertToFloat(k2)) + halfconst;
		vec4 x3 = (x0 - oneVecF) + halfconst;
		vec4 y3 = (y0 - oneVecF) + halfconst;
		vec4 z3 = (z0 - oneVecF) + halfconst;

		vec4 t0 = ((vec4(0.6f) - (x0 * x0)) - (y0 * y0)) - (z0 * z0);
		vec4 t1 = ((vec4(0.6f) - (x1 * x1)) - (y1 * y1)) - (z1 * z1);
		vec4 t2 = ((vec4(0.6f) - (x2 * x2)) - (y2 * y2)) - (z2 * z2);
		vec4 t3 = ((vec4(0.6f) - (x3 * x3)) - (y3 * y3)) - (z3 * z3);

		vec4 n0 = t0 >= vec4(0.0f);
		vec4 n1 = t1 >= vec4(0.0f);
		vec4 n2 = t2 >= vec4(0.0f);
		vec4 n3 = t3 >= vec4(0.0f);

		t0 = t0 * t0;
		t1 = t1 * t1;
		t2 = t2 * t2;
		t3 = t3 * t3;

		n0 = n0 & ((t0 * t0) * gradientcoord(seed, i, j, k, x0, y0, z0));
		n1 = n1 & ((t1 * t1) * gradientcoord(seed, i + i1, j + j1, k + k1, x1, y1, z1));
		n2 = n2 & ((t2 * t2) * gradientcoord(seed, i + i2, j + j2, k + k2, x2, y2, z2));
		n3 = n2 & ((t3 * t3) * gradientcoord(seed, i + oneVecI, j + oneVecI, k + oneVecI, x3, y3, z3));

		return (vec4(32.0f, 32.0f, 32.0f, 32.0f) * (n0 + n1 + n2 + n3));
	}

	static vec8 simplex(ivec4 const &seed, vec8 const &xi, vec8 const& yi, vec8 const &zi) {

	}

	static vec4 FBM(ivec4 const &seed, vec4 const &xi, vec4 const &yi, vec4 const &zi, float frequency, int octaves, float lacunarity, float gain) {
		vec4 amplitude(1.0f);
		vec4 x, y, z; ivec4 seedv = seed;
		x = xi * vec4(frequency);
		y = yi * vec4(frequency);
		z = zi * vec4(frequency);
		vec4 max = oneVecF;
		vec4 n = simplex(seed, x, y, z);
		for (int i = 0; i < octaves; ++i) {
			// Multiply initial coords by frequency to scale them to right domain
			x = xi * vec4(frequency);
			y = yi * vec4(frequency);
			z = zi * vec4(frequency);
			seedv = seedv + oneVecI;
			// Get simplex value
			n = simplex(seed, x, y, z);
			// Total simplex value is current * amplitude
			n = n + n * amplitude;
			// Scale the frequency by the lacunarity since this is octaved noise
			frequency *= lacunarity;
			// Gain changes over octaves as well.
			amplitude *= vec4(gain);
			max = max + amplitude;
		}
		n = (n / max);
		return n;
	}

	static vec4 RidgedMulti(ivec4 const &seed, vec4 const &xi, vec4 const &yi, vec4 const &zi, float frequency, int octaves, float lacunariy, float gain) {
		vec4 sum(0.0f);
		vec4 amplitude(1.0f);
		for (int i = 0; i < octaves; ++i) {
		}
	}

	static std::vector<vec4> ParallelNoise(unsigned int num) {
		std::vector<vec4> results; std::mt19937 r;
		std::uniform_int_distribution<> distr(-40, 40);
		results.resize(num);
#pragma loop(hint_parallel(4))
		for (unsigned int i = 0; i < num; ++i) {
			vec4 x = vec4((float)i + (float)distr(r), (float)i + (float)distr(r), (float)i + distr(r), (float)i + (float)distr(r));
			vec4 y = vec4((float)i + (float)distr(r), (float)i + (float)distr(r), (float)i + distr(r), (float)i + (float)distr(r));
			vec4 z = vec4((float)i + (float)distr(r), (float)i + (float)distr(r), (float)i + distr(r), (float)i + (float)distr(r));
			vec4 res = FBM(ivec4(2342, 42111, 362, 2132), x, y, z, 0.01f, 6, 2.5f, 1.0f);
			vec4 store; res.Store(store.Data);
			results[i] = (store);
		}
		return results;
	}

#endif // SIMD_LEVEL_SSE3


#ifdef SIMD_LEVEL_AVX2
	// Following are noise functions for AVX2 instructions.


#endif // SIMD_LEVEL_AVX2

}


#endif // !SIMD_TERRAIN_H
