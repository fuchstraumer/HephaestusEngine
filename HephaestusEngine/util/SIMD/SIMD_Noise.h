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
		hashtmp = (x * xPrime) + hashtmp;
		hashtmp = (y * yPrime) + hashtmp;
		hashtmp = (z * zPrime) + hashtmp;

		hashtmp = ((hashtmp * hashtmp) * ivec4(60493)) * hashtmp;
		hashtmp = ivec4::xor((hashtmp >> ivec4(13)), hashtmp);
		return hashtmp;
	}

	static ivec4 hash(ivec4 const &seed, ivec4 const &x, ivec4 const &y) {
		ivec4 result = seed;
		result = (x * ivec4(1619)) + result;
		result = (y * ivec4(31337)) + result;

		result = (result * result) * ivec4(60493) * result;
		ivec4 shiftResult = result >> ivec4(13);
		result = ivec4:: xor (shiftResult, result);
		return result;
	}

	static vec4 gradientcoord(ivec4 const &seed, ivec4 const &xi, ivec4 const &yi, vec4 const &x, vec4 const &y) {
		ivec4 hashvec = hash(seed, xi, yi);
		hashvec = hashvec & ivec4(15);
		vec4 u = CastToFloat(hashvec < ivec4(8));
		u = blendv(y, x, u);

		vec4 h1 = CastToFloat((hashvec & ivec4(1)) << ivec4(31));

		vec4 result = vec4::xor(u, h1);
		return result;
	}

	static vec4 gradientcoord(ivec4 const &seed, ivec4 const &xi, ivec4 const &yi, ivec4 const &zi, vec4 const &x, vec4 const& y, vec4 const &z) {
		ivec4 hashresult = ((hash(seed, xi, yi, zi))) & ivec4(15);
		vec4 u = CastToFloat((hashresult < ivec4(8)));
		u = blendv(y, x, u);

		vec4 v = CastToFloat(hashresult < ivec4(4));
		vec4 h12o14 = CastToFloat((hashresult == ivec4(12)) | (hashresult == ivec4(14)));
		h12o14 = blendv(z, x, h12o14);
		v = blendv(h12o14, y, v);

		vec4 h1 = CastToFloat((hashresult & ivec4(1)) << ivec4(31));
		vec4 h2 = CastToFloat((hashresult & ivec4(2)) << ivec4(30));
		
		return (vec4::xor(u, h1) + vec4::xor(v, h2));
	}
	
	static vec4 simplex(ivec4 const &seed, vec4 const &x, vec4 const &y, vec4 const &z) {
		vec4 g3, f3; // Constants for skewing in 3D case
		f3 = vec4(0.33333333f); g3 = vec4(0.6666666f);
		// Skew vector for 3D case
		vec4 s; s = f3 * (x + y + z);
		vec4 xs, ys, zs;
		xs = x + s;
		ys = y + s;
		zs = z + s;

		vec4 i = floor(xs);
		vec4 j = floor(ys);
		vec4 k = floor(zs);

		vec4 t = (i + j + k) * g3;
		vec4 x0 = x - (i - t);
		vec4 y0 = y - (j - t);
		vec4 z0 = z - (k - t);

		ivec4 x0_ge_y0 = CastToInt(x0 >= y0);
		ivec4 y0_ge_z0 = CastToInt(y0 >= z0);
		ivec4 x0_ge_z0 = CastToInt(x0 >= z0);

		ivec4 i1 = ivec4(1.0) & (x0_ge_y0 & x0_ge_z0);
		ivec4 j1 = ivec4(1.0) & ivec4::andnot(x0_ge_z0, y0_ge_z0);
		ivec4 k1 = ivec4(1.0) & ivec4::andnot(x0_ge_z0, ~y0_ge_z0);

		ivec4 i2 = ivec4(1.0) & (x0_ge_y0 | x0_ge_z0);
		ivec4 j2 = ivec4(1.0) & (~x0_ge_y0 | y0_ge_z0);
		ivec4 k2 = ivec4(1.0) & (x0_ge_y0 & y0_ge_z0);

		vec4 x1 = (x0 - ConvertToFloat(i1)) + f3;
		vec4 y1 = (y0 - ConvertToFloat(j1)) + f3;
		vec4 z1 = (z0 - ConvertToFloat(k1)) + f3;
		vec4 x2 = (x0 - ConvertToFloat(i2)) + g3;
		vec4 y2 = (y0 - ConvertToFloat(j2)) + g3;
		vec4 z2 = (z0 - ConvertToFloat(k2)) + g3;
		vec4 x3 = (x0 - vec4(1.0f)) + g3;
		vec4 y3 = (y0 - vec4(1.0f)) + g3;
		vec4 z3 = (z0 - vec4(1.0f)) + g3;

		vec4 t0 = (vec4(0.5f) - (x0 * x0)) - (y0 * y0) - (z0 * z0);
		vec4 t1 = (vec4(0.5f) - (x1 * x1)) - (y1 * y1) - (z1 * z1);
		vec4 t2 = (vec4(0.5f) - (x2 * x2)) - (y2 * y2) - (z2 * z2);
		vec4 t3 = (vec4(0.5f) - (x3 * x3)) - (y3 * y3) - (z3 * z3);

		vec4 n0 = (t0 < vec4(0.0f));
		vec4 n1 = (t1 < vec4(0.0f));
		vec4 n2 = (t2 < vec4(0.0f));
		vec4 n3 = (t3 < vec4(0.0f));

		t0 = t0 * t0;
		t1 = t1 * t1;
		t2 = t2 * t2;
		t3 = t3 * t3;
		ivec4 iF, jF, kF;
		iF = ConvertToInt(i); jF = ConvertToInt(j); kF = ConvertToInt(k);
		n0 = n0 & ((t0 * t0) * gradientcoord(seed, iF, jF, kF, x0, y0, z0));
		n1 = n1 & ((t1 * t1) * gradientcoord(seed, iF + i1, jF + j1, kF + k1, x1, y1, z1));
		n2 = n2 & ((t2 * t2) * gradientcoord(seed, iF + i2, jF + j2, kF + k2, x2, y2, z2));
		n3 = n2 & ((t3 * t3) * gradientcoord(seed, iF + ivec4(1.0), jF + ivec4(1.0), kF + ivec4(1.0), x3, y3, z3));

		return (vec4(32.0f, 32.0f, 32.0f, 32.0f) * (n0 + n1 + n2 + n3));
	}

	static vec8 simplex(ivec4 const &seed, vec8 const &xi, vec8 const& yi, vec8 const &zi) {

	}

	static vec4 simplex2d(ivec4 const& seed, vec4 const &x, vec4 const &y, vec4* dx = nullptr, vec4 *dy = nullptr) {
		vec4 f2(0.366025403f);
		vec4 g2(0.211324865f);

		vec4 temp0, temp1, temp2;
		// Skew vector
		vec4 s = (x + y) * f2;
		// Skewed vectors in simplex space
		vec4 xs = x + s;
		vec4 ys = y + s;
		vec4 i = floor(xs);
		vec4 j = floor(ys);

		// Get reverse skew vector
		vec4 t = (i + j) * g2;
		vec4 x0 = i - t;
		vec4 y0 = j - t;
		vec4 X0 = x - x0;
		vec4 Y0 = y - y0;

		// Simplex in 2D is a triangle, find cell of simplex we're in
		ivec4 comp = CastToInt(x0 >= y0);
		ivec4 i1 = oneVecI & comp;
		ivec4 j1 = oneVecI & (~comp);

		vec4 x1 = x0 - ConvertToFloat(i1) + g2;
		vec4 y1 = y0 - ConvertToFloat(j1) + g2;
		vec4 x2 = x0 - oneVecF + (vec4(2.0f) * g2);
		vec4 y2 = y0 - oneVecF + (vec4(2.0f) * g2);

		// Get the actual gradient contributions
		vec4 t0, t1, t2;
		t0 = vec4(0.5f) - (x0 * x0) - (y0 * y0);
		t1 = vec4(0.5f) - (x1 * x1) - (y1 * y1);
		t2 = vec4(0.5f) - (x2 * x2) - (y2 * y2);
		// Conditional vector creation
		vec4 n0, n1, n2;
		n0 = (t0 < vec4(0.0f));
		n1 = (t1 < vec4(0.0f));
		n2 = (t2 < vec4(0.0f));

		t0 = (t0 * t0) * (t0 * t0);
		t1 = (t1 * t1) * (t1 * t1);
		t2 = (t2 * t2) * (t2 * t2);

		n0 = n0 & (t0 * gradientcoord(seed, ConvertToInt(i), ConvertToInt(j), x0, y0));
		n1 = n1 & (t0 * gradientcoord(seed, ConvertToInt(i) + i1, ConvertToInt(j) + j1, x1, y1));
		n2 = n2 & (t0 * gradientcoord(seed, ConvertToInt(i) + ivec4(1), ConvertToInt(j) + ivec4(1), x2, y2));
		vec4 result; 
		result = vec4(32.0f) * (n0 + n1 + n2);
		return result;
	}

	static vec4 FBM(ivec4 const &seed, vec4 const &xi, vec4 const &yi, float frequency, int octaves, float lacunarity, float initgain) {
		vec4 amplitude(1.0f); vec4 sum(1.0f);
		vec4 freq(frequency); vec4 lacun(lacunarity);
		vec4 gain(initgain);
		for (int i = 0; i < octaves; ++i) {
			vec4 x, y;
			x = xi * freq;
			y = yi * freq;
			vec4 res = simplex2d(seed, x, y);
			sum = sum + (res * amplitude);
			freq = freq * lacun;
			amplitude = amplitude * gain;
		}
		sum = (sum / amplitude);
		return sum;
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
