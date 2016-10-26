#ifndef TERRAIN_GEN_H
#define TERRAIN_GEN_H
#include "glm/glm.hpp"
#include <math.h>
#include <random>
#include <algorithm>
#include "../stdafx.h"
/*
	Class: Terrain Generator
	Primary terrain generation system for this library/engine.
	Sources:
	http://www.decarpentier.nl/scape-procedural-basics
	http://www.decarpentier.nl/scape-procedural-extensions
	http://www.iquilezles.org/www/articles/morenoise/morenoise.htm
	http://stackoverflow.com/questions/4297024/3d-perlin-noise-analytical-derivative
	https://github.com/Auburns/FastNoise/blob/master/FastNoise.cpp 
	http://webstaff.itn.liu.se/~stegu/simplexnoise/DSOnoises.html
*/

// Perlin normaliziation value
static double PERLIN_NORM = 0.75;
// Perlin noise easing curve
static double fade(double f) {
	return f*f*f*(f*(f * 6 - 15) + 10);
}
// Derivative of the perlin noise easing curve
static double fadeDeriv(double f) {
	return f*f*(f*(30 * f - 60) + 30);
}
// Find the point XYZ using known values V000-V111. The numbers after each name "v" represent points (x,y,z) on the unit cube.
// The full 8 point set builds a full unit cube. This function can be scaled or rotated freely. Useful for decreasing sampling of noise functions.
// Remember that linear interpolation is not always pretty - apply some sort of ease curve as possible.
static float triLerp(float x, float y, float z, float V000, float V100, float V010, float V001, float V101, float V011, float V110, float V111) {
	float s1 = V000 * (1 - x) * (1 - y) * (1 - z);
	float s2 = V100 * x * (1 - y) * (1 - z);
	float s3 = V010 * (1 - x) * y * (1 - z);
	float s4 = V001 * (1 - x) * (1 - y) * z;
	float s5 = V101 * x * (1 - y) * z;
	float s6 = V011 * (1 - x) * y * z;
	float s7 = V110 * x * y * (1 - z);
	float s8 = V111 * x * y * z;
	return (s1 + s2 + s3 + s4 + s5 + s6 + s7 + s8);
}
// Basic linear interp
static double lerp(double a, double b, double x) {
	return a + x * (b - a);
}

class TerrainGenerator {
public:
	// Instantiates a terrain generator, setting the seed and building the hash table
	TerrainGenerator(int seed) {
		this->Seed = seed;
		r_gen.seed(this->Seed);
		buildHash();
	}
	int Seed;
	std::ranlux48 r_gen;
	// Returns evaluation of base perlin function at x,y,z
	double point(int x, int y, int z);

	// Fractal-Brownian-Motion perlin noise based terrain gen.
	double PerlinFBM(int x, int y, int z, double frequency, int octaves, float lacunarity, float gain);
	inline double SimplexFBM(int x, int y, int z, double frequency, int octaves, float lacunarity, float gain);
	inline double SimplexFBM(int x, int y, double frequency, int octaves, float lacunarity, float gain);

	// Billowy perlin generator, takes the absolute value of the perlin gen and generally works at low gain and low freq
	double PerlinBillow(int x, int y, int z, double frequency, int octaves, float lacunarity, float gain);
	double SimplexBillow(int x, int y, int z, double frequency, int octaves, float lacunarity, float gain);
	double SimplexBillow(int x, int y, double frequency, int octaves, float lacunarity, float gain);

	// Specialized instance of Billow gen with set parameters
	double RollingHills(int x, int y, int z);

	// Generates ridges and other bizarre patterns. Currently broken.
	double PerlinRidged(int x, int y, int z, double freq, int octaves, float lac, float gain);
	double SimplexRidged(int x, int y, int z, double freq, int octaves, float lac, float gain);
	double SimplexRidged(int x, int y, double freq, int octaves, float lac, float gain);
	
	// "Swiss" noise using derivatives to simulate erosion and create better mountains
	inline double SimplexSwiss(int x, int y, double freq, int octaves, float lac, float gain);

	// Jordan noise
	inline auto SimplexJordan(int x, int y, double freq, int octaves, float lac, float gain);

	// ridged optimized for nice caves
	double SimplexCaves(int x, int y);
	// Basic example of octave summing with perlin noise in 3D. Deprecated.
	double octPerlin(float x, float y, float z, int octaves, float lacun);
private:
	// Value to use for normalization (1 / sqrt(norm). Approxiamately 0.75 for 3D perlin
	
	// Hash table containing values to be used for gradient vectors
	// Leave as unsigned char - getting this to fit in the cache is optimal
	unsigned char hashTable[512];

	// Builds the hash table containing our 0-255 values used for gradient vectors
	// This approach is psuedo-random (on purpose) and faster than other methods
	void buildHash() {
		for (int c = 0; c < 255; ++c) {
			this->hashTable[c] = c;
			this->hashTable[c + 256] = c;
		}
		shuffle(this->hashTable, this->hashTable + 512, this->r_gen);
	}
	// Faster flooring function than std::floor()
	inline int fastfloor(double x) {
		return x>0 ? (int)x : (int)x - 1;
	}
	// Finds the dot product of x,y,z and the gradient vector "hash". 
	// Source: http://riven8192.blogspot.com/2010/08/calculate-perlinnoise-twice-as-fast.html //
	double grad(int hash, double x, double y, double z)
	{
		switch (hash & 0xF)
		{
		case 0x0: return  x + y;
		case 0x1: return -x + y;
		case 0x2: return  x - y;
		case 0x3: return -x - y;
		case 0x4: return  x + z;
		case 0x5: return -x + z;
		case 0x6: return  x - z;
		case 0x7: return -x - z;
		case 0x8: return  y + z;
		case 0x9: return -y + z;
		case 0xA: return  y - z;
		case 0xB: return -y - z;
		case 0xC: return  y + x;
		case 0xD: return -y + z;
		case 0xE: return  y - x;
		case 0xF: return -y - z;
		default: return 0; // never happens
		}
	}

	// Main noise function: terrain is specialized usage of this.
	double perlin(double x, double y, double z) {
		glm::dvec3 p(x, y, z);
		// Calculate integer coords and fractional component for weighting
		// We have to normalize to be within the unit cube defined as part of the perlin function
		glm::ivec3 p0; p0.x = fastfloor(p.x);
		p0.y = fastfloor(p.y); p0.z = fastfloor(p.z);
		glm::dvec3 p1(p0.x + 1, p0.y + 1, p0.z + 1);

		// Get ease curve values ease(u,v,w)
		glm::dvec3 w;
		w.x = fade(p.x - p0.x);
		w.y = fade(p.y - p0.y);
		w.z = fade(p.z - p0.z);

		// Noise hash function, grabbing the values at the 8 points of our unit cube
		int aaa, aba, aab, abb, baa, bba, bab, bbb;
		aaa = hashTable[hashTable[hashTable[p0.x] + p0.y] + p0.z];
		aba = hashTable[hashTable[hashTable[p0.x] + p0.y + 1] + p0.z];
		aab = hashTable[hashTable[hashTable[p0.x] + p0.y] + p0.z + 1];
		abb = hashTable[hashTable[hashTable[p0.x] + p0.y + 1] + p0.z + 1];
		baa = hashTable[hashTable[hashTable[p0.x + 1] + p0.y] + p0.z];
		bba = hashTable[hashTable[hashTable[p0.x + 1] + p0.y + 1] + p0.z];
		bab = hashTable[hashTable[hashTable[p0.x + 1] + p0.y] + p0.z + 1];
		bbb = hashTable[hashTable[hashTable[p0.x + 1] + p0.y + 1] + p0.z + 1];
		// Get gradient vectors
		double x00, x10, x01, x11, y00, y01;
		x00 = lerp(grad(aaa, p0.x, p0.y, p0.z), grad(baa, p1.x, p0.y, p0.z), w.x);
		x10 = lerp(grad(aba, p0.x, p1.y, p0.z), grad(bba, p1.x, p1.y, p0.z), w.x);
		x01 = lerp(grad(aab, p0.x, p0.y, p1.z), grad(bab, p1.x, p0.y, p1.z), w.x);
		x11 = lerp(grad(abb, p0.x, p1.y, p1.z), grad(bbb, p1.x, p1.y, p1.z), w.x);
		// Interpolate (bilinear)
		y00 = lerp(x00, x10, w.y);
		y01 = lerp(x01, x11, w.y);
		// Final step, trilinear interpolation (narrowing subrectangles)
		return (lerp(y00, y01, w.z) + 1) * (1/sqrt(PERLIN_NORM));
	}


	// Function for finding gradient of simplex noise
	double sGrad(int hash, double x, double y) {
		int h = hash & 7;      // Convert low 3 bits of hash code
		double u = h<4 ? x : y;  // into 8 simple gradient directions,
		double v = h<4 ? y : x;  // and compute the dot product with (x,y).
		return ((h & 1) ? -u : u) + ((h & 2) ? -2.0*v : 2.0*v);
	}

	inline double sGrad(int hash, double x, double y, double z) {
		int h = hash & 15;     // Convert low 4 bits of hash code into 12 simple
		double u = h<8 ? x : y; // gradient directions, and compute dot product.
		double v = h<4 ? y : h == 12 || h == 14 ? x : z; // Fix repeats at h = 12 to 15
		return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
	}

	// Simplex noise gens
	// return the derivatives at x,y if non-null pointers dx,dy are supplied
	double simplex(double x, double y, double* dx, double* dy) {
		#define F2 0.366025403 // F2 = 0.5*(sqrt(3.0)-1.0)
		#define G2 0.211324865 // G2 = (3.0-Math.sqrt(3.0))/6.0

		double n0, n1, n2; // Noise contributions from the three corners
		double temp0, temp1, temp2;

						   // Skew the input space to determine which simplex cell we're in
		double s = (x + y)*F2; // Hairy factor for 2D
		double xs = x + s;
		double ys = y + s;
		int i = fastfloor(xs);
		int j = fastfloor(ys);

		double t = (double)(i + j)*G2;
		double X0 = i - t; // Unskew the cell origin back to (x,y) space
		double Y0 = j - t;
		double x0 = x - X0; // The x,y distances from the cell origin
		double y0 = y - Y0;

		// For the 2D case, the simplex shape is an equilateral triangle.
		// Determine which simplex we are in.
		int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
		if (x0>y0) { i1 = 1; j1 = 0; } // lower triangle, XY order: (0,0)->(1,0)->(1,1)
		else { i1 = 0; j1 = 1; }      // upper triangle, YX order: (0,0)->(0,1)->(1,1)

									  // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
									  // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
									  // c = (3-sqrt(3))/6

		double x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
		double y1 = y0 - j1 + G2;
		double x2 = x0 - 1.0 + 2.0 * G2; // Offsets for last corner in (x,y) unskewed coords
		double y2 = y0 - 1.0 + 2.0 * G2;

		// Wrap the integer indices at 256, to avoid indexing perm[] out of bounds
		int ii = i & 0xff;
		int jj = j & 0xff;

		// Calculate the contribution from the three corners
		double t0 = 0.5 - x0*x0 - y0*y0;
		if (t0 < 0.0) n0 = 0.0;
		else {
			t0 *= t0;
			n0 = t0 * t0 * sGrad(hashTable[ii + hashTable[jj]], x0, y0);
		}

		double t1 = 0.5 - x1*x1 - y1*y1;
		if (t1 < 0.0) n1 = 0.0;
		else {
			t1 *= t1;
			n1 = t1 * t1 * sGrad(hashTable[ii + i1 + hashTable[jj + j1]], x1, y1);
		}

		double t2 = 0.5 - x2*x2 - y2*y2;
		if (t2 < 0.0) n2 = 0.0;
		else {
			t2 *= t2;
			n2 = t2 * t2 * sGrad(hashTable[ii + 1 + hashTable[jj + 1]], x2, y2);
		}

		if ((nullptr != dx) && (nullptr != dy))
		{
			/*  A straight, unoptimised calculation would be like:
			*    *dnoise_dx = -8.0f * t0 * t0 * x0 * ( gx0 * x0 + gy0 * y0 ) + t0^4 * gx0;
			*    *dnoise_dy = -8.0f * t0 * t0 * y0 * ( gx0 * x0 + gy0 * y0 ) + t0^4 * gy0;
			*    *dnoise_dx += -8.0f * t1 * t1 * x1 * ( gx1 * x1 + gy1 * y1 ) + t1^4 * gx1;
			*    *dnoise_dy += -8.0f * t1 * t1 * y1 * ( gx1 * x1 + gy1 * y1 ) + t1^4 * gy1;
			*    *dnoise_dx += -8.0f * t2 * t2 * x2 * ( gx2 * x2 + gy2 * y2 ) + t2^4 * gx2;
			*    *dnoise_dy += -8.0f * t2 * t2 * y2 * ( gx2 * x2 + gy2 * y2 ) + (t2^4) * gy2;
			*/
			double t40 = t0*t0;
			double t41 = t1*t1;
			double t42 = t2*t2;
			temp0 = t0 * t0 * (x0* x0 + y0 * y0);
			*dx = temp0 * x0;
			*dy = temp0 * y0;
			temp1 = t1 * t1 * (x1 * x1 + y1 * y1);
			*dx += temp1 * x1;
			*dy += temp1 * y1;
			temp2 = t2 * t2 * (x2* x2 + y2 * y2);
			*dx += temp2 * x2;
			*dy += temp2 * y2;
			*dx *= -8.0f;
			*dy *= -8.0f;
			*dx += t40 * x0 + t41 * x1 + t42 * x2;
			*dy += t40 * y0 + t41 * y1 + t42 * y2;
			*dx *= 40.0f; /* Scale derivative to match the noise scaling */
			*dy *= 40.0f;
		}
		// Add contributions from each corner to get the final noise value.
		// The result is scaled to return values in the interval [-1,1].
		return 40.0 * (n0 + n1 + n2); // TODO: The scale factor is preliminary!

	}

	double simplex(double x, double y, double z) {

		// Simple skewing factors for the 3D case
		#define F3 0.333333333
		#define G3 0.166666667

		double n0, n1, n2, n3; // Noise contributions from the four corners

							   // Skew the input space to determine which simplex cell we're in
		double s = (x + y + z)*F3; // Very nice and simple skew factor for 3D
		double xs = x + s;
		double ys = y + s;
		double zs = z + s;
		int i = fastfloor(xs);
		int j = fastfloor(ys);
		int k = fastfloor(zs);

		double t = (double)(i + j + k)*G3;
		double X0 = i - t; // Unskew the cell origin back to (x,y,z) space
		double Y0 = j - t;
		double Z0 = k - t;
		double x0 = x - X0; // The x,y,z distances from the cell origin
		double y0 = y - Y0;
		double z0 = z - Z0;

		// For the 3D case, the simplex shape is a slightly irregular tetrahedron.
		// Determine which simplex we are in.
		int i1, j1, k1; // Offsets for second corner of simplex in (i,j,k) coords
		int i2, j2, k2; // Offsets for third corner of simplex in (i,j,k) coords

						/* This code would benefit from a backport from the GLSL version! */
		if (x0 >= y0) {
			if (y0 >= z0)
			{
				i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0;
			} // X Y Z order
			else if (x0 >= z0) { i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1; } // X Z Y order
			else { i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1; } // Z X Y order
		}
		else { // x0<y0
			if (y0<z0) { i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1; } // Z Y X order
			else if (x0<z0) { i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1; } // Y Z X order
			else { i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0; } // Y X Z order
		}

		// A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
		// a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
		// a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
		// c = 1/6.

		double x1 = x0 - i1 + G3; // Offsets for second corner in (x,y,z) coords
		double y1 = y0 - j1 + G3;
		double z1 = z0 - k1 + G3;
		double x2 = x0 - i2 + 2.0*G3; // Offsets for third corner in (x,y,z) coords
		double y2 = y0 - j2 + 2.0*G3;
		double z2 = z0 - k2 + 2.0*G3;
		double x3 = x0 - 1.0 + 3.0*G3; // Offsets for last corner in (x,y,z) coords
		double y3 = y0 - 1.0 + 3.0*G3;
		double z3 = z0 - 1.0 + 3.0*G3;

		// Wrap the integer indices at 256, to avoid indexing perm[] out of bounds
		int ii = i & 0xff;
		int jj = j & 0xff;
		int kk = k & 0xff;

		// Calculate the contribution from the four corners
		double t0 = 0.6 - x0*x0 - y0*y0 - z0*z0;
		if (t0 < 0.0) n0 = 0.0;
		else {
			t0 *= t0;
			n0 = t0 * t0 * sGrad(hashTable[ii + hashTable[jj + hashTable[kk]]], x0, y0, z0);
		}

		double t1 = 0.6 - x1*x1 - y1*y1 - z1*z1;
		if (t1 < 0.0) n1 = 0.0;
		else {
			t1 *= t1;
			n1 = t1 * t1 * sGrad(hashTable[ii + i1 + hashTable[jj + j1 + hashTable[kk + k1]]], x1, y1, z1);
		}

		double t2 = 0.6 - x2*x2 - y2*y2 - z2*z2;
		if (t2 < 0.0) n2 = 0.0;
		else {
			t2 *= t2;
			n2 = t2 * t2 * sGrad(hashTable[ii + i2 + hashTable[jj + j2 + hashTable[kk + k2]]], x2, y2, z2);
		}

		double t3 = 0.6 - x3*x3 - y3*y3 - z3*z3;
		if (t3<0.0) n3 = 0.0;
		else {
			t3 *= t3;
			n3 = t3 * t3 * sGrad(hashTable[ii + 1 + hashTable[jj + 1 + hashTable[kk + 1]]], x3, y3, z3);
		}

		// Add contributions from each corner to get the final noise value.
		// The result is scaled to stay just inside [-1,1]
		return 32.0 * (n0 + n1 + n2 + n3); // TODO: The scale factor is preliminary!
	}
};

// This function is just an interface to the private perlin member function.
inline double TerrainGenerator::point(int x, int y, int z) {
	double temp = perlin(x, y, z);
	if (temp > CHUNK_SIZE_Z)
		temp = CHUNK_SIZE_Z - 1;
	if (temp <= 1)
		temp = 0;
	return temp;
}
// PERLIN NOISE FUNCTIONS FOLLOW
// Parameters:
// - Frequency is the frequency of the noise sampling (multiplier on input coords
// - An octave is an iteration through the noise function, but at smaller levels. It adds finer, grainier detail.
// - "lac" or Lacunarity controls how long an octave persists after the previous octave. Higher levels = more details. Too high is not good.
// - Gain controls the overall gain of the function.
// - Amplitude is set within the function body, in line with what generates the best terrain. The rest of the parameters are also set similarly.

inline double TerrainGenerator::PerlinFBM(int x, int y, int z, double freq = 0.003, int octaves = 4, float lac = 2.0, float gain = 0.6) {
	double sum = 0;
	float amplitude = 1.0;
	glm::dvec3 f; f.x = x * freq;
	f.y = y * freq; f.z = z * freq;
	for (int i = 0; i < octaves; ++i) {
		double n = perlin(x*freq, y*freq, z*freq);
		sum += n*amplitude;
		freq *= lac;
		amplitude *= gain;
	}
	double temp = (sum / amplitude) + 1;
	//std::cerr << temp << std::endl;
	temp = sqrt(temp*temp);
	if (temp >= CHUNK_SIZE_Z - 4)
		temp = CHUNK_SIZE_Z - 4;
	if (temp <= 1)
		temp = 1;
	return temp;
}

inline double TerrainGenerator::PerlinBillow(int x, int y, int z, double freq = 0.01, int octaves = 3, float lac = 1.7, float gain = 0.6) {
	double sum = 0;
	float amplitude = 1.0;
	for (int i = 0; i < octaves; ++i) {
		double n = abs(perlin(x*freq, y*freq, z*freq));
		sum += n*amplitude;
		freq *= lac;
		amplitude *= gain;
	}
	double temp = (sum / amplitude) + 1;
	if (temp >= CHUNK_SIZE_Z - 4)
		temp = CHUNK_SIZE_Z - 4;
	if (temp <= 1)
		temp = 1;
	return temp;
}

inline double TerrainGenerator::RollingHills(int x, int y, int z) {
	return PerlinBillow(x, y, z,0.009,3,1.6f,0.6f);
}

inline double TerrainGenerator::PerlinRidged(int x, int y, int z, double freq = 0.02, int octaves = 3, float lac = 2.5, float gain = 0.8) {
	double n = 1.0f - abs(perlin(x*freq, y*freq, z*freq));
	double temp = n * 1.5;
	if (temp >= CHUNK_SIZE_Z - 4)
		temp = CHUNK_SIZE_Z - 4;
	if (temp <= 1)
		temp = 1;
	return temp;
}

// Deprecated
inline double TerrainGenerator::octPerlin(float x, float y, float z, int octaves = 3, float lacun = 1.0) {
	double total = 0;
	double frequency = 1;
	double amplitude = 1;
	double maxValue = 0;  // Used for normalizing result to 0.0 - 1.0
	for (int i = 0; i<octaves; i++) {
		total += perlin(x * frequency, y * frequency, z * frequency) * amplitude;

		maxValue += amplitude;

		amplitude *= lacun;
		frequency *= 2;
	}
	return total / maxValue;
}

// SIMPLEX NOISE FUNCTIONS FOLLOW
// SOURCE FROM http://staffwww.itn.liu.se/~stegu/aqsis/aqsis-newnoise/
static double FBM_FREQ = 0.0002; static int FBM_OCTAVES = 5;
static float FBM_LACUN = 2.0; static float FBM_GAIN = 1.5;

inline double TerrainGenerator::SimplexFBM(int x, int y, int z, double freq = FBM_FREQ, int octaves = FBM_OCTAVES, float lac = FBM_LACUN, float gain = FBM_GAIN) {
	double sum = 0;
	float amplitude = 1.0;
	glm::dvec3 f; f.x = x * freq;
	f.y = y * freq; f.z = z * freq;
	for (int i = 0; i < octaves; ++i) {
		double n = simplex(x*freq, y*freq, z*freq);
		sum += n*amplitude;
		freq *= lac;
		amplitude *= gain;
	}
	double temp = (sum / amplitude) + 1;
	//std::cerr << temp << std::endl;
	temp = 10 * sqrt(temp*temp);
	if (temp >= CHUNK_SIZE_Z - 4)
		temp = CHUNK_SIZE_Z - 4;
	if (temp <= 1)
		temp = 1;
	return temp;
}

inline double TerrainGenerator::SimplexFBM(int x, int y, double freq = FBM_FREQ, int octaves = FBM_OCTAVES, float lac = FBM_LACUN, float gain = FBM_GAIN) {
	double sum = 0;
	float amplitude = 1.0;
	glm::dvec2 f; f.x = x * freq;
	f.y = y * freq;
	for (int i = 0; i < octaves; ++i) {
		double n = simplex(x*freq, y*freq, nullptr, nullptr);
		sum += n*amplitude;
		freq *= lac;
		amplitude *= gain;
	}
	double temp = (sum / amplitude) + 2;
	//std::cerr << temp << std::endl;
	temp = 32 * temp;
	if (temp >= CHUNK_SIZE_Z - 4)
		temp = CHUNK_SIZE_Z - 4;
	if (temp <= 1)
		temp = 1;
	return temp;
}

static double BILLOW_FREQ = 0.0018; static int BILLOW_OCTAVES = 4;
static float BILLOW_LACUN = 2.50f; static float BILLOW_GAIN = 1.20f;

inline double TerrainGenerator::SimplexBillow(int x, int y, int z, double freq = BILLOW_FREQ, int octaves = BILLOW_OCTAVES, float lac = BILLOW_LACUN, float gain = BILLOW_GAIN) {
	double sum = 0;
	float amplitude = 1.0;
	for (int i = 0; i < octaves; ++i) {
		double n = abs(simplex(x*freq, y*freq, z*freq));
		sum += n*amplitude;
		freq *= lac;
		amplitude *= gain;
	}
	double temp = (sum / amplitude) + 1;
	if (temp >= CHUNK_SIZE_Z - 4)
		temp = CHUNK_SIZE_Z - 4;
	if (temp <= 1)
		temp = 1;
	return temp;
}

inline double TerrainGenerator::SimplexBillow(int x, int y, double freq = BILLOW_FREQ, int octaves = BILLOW_OCTAVES, float lac = BILLOW_LACUN, float gain = BILLOW_GAIN) {
	double sum = 0;
	float amplitude = 1.0;
	glm::dvec2 f;
	f.x = x * freq; f.y = y * freq;
	for (int i = 0; i < octaves; ++i) {
		double n = abs(simplex(f.x, f.y, nullptr, nullptr));
		sum += n*amplitude;
		freq *= lac;
		amplitude *= gain;
	}
	double temp = (sum / amplitude) + 1;
	//std::cerr << temp << std::endl;
	temp = 32 * temp;
	if (temp >= CHUNK_SIZE_Z - 4)
		temp = CHUNK_SIZE_Z - 4;
	if (temp <= 1)
		temp = 1;
	return temp;
}

static double RIDGED_FREQ = 0.00008; static int RIDGED_OCTAVES = 6;
static float RIDGED_LACUN = 2.50f; static float RIDGED_GAIN = 2.20f;

inline double TerrainGenerator::SimplexRidged(int x, int y, int z, double freq = RIDGED_FREQ, int octaves = RIDGED_OCTAVES, 
	float lac = RIDGED_LACUN, float gain = RIDGED_GAIN) {
	double sum = 0;
	float amplitude = 1.0;
	glm::dvec3 f; f.x = x * freq;
	f.y = y * freq; f.z = z * freq;
	for (int i = 0; i < octaves; ++i) {
		double n = 1.0f - abs(simplex(x*freq, y*freq, z*freq));
		sum += n*amplitude;
		freq *= lac;
		amplitude *= gain;
	}
	double temp = (sum / amplitude) + 1;
	temp = 64 * temp;
	if (temp >= CHUNK_SIZE_Z - 4)
		temp = CHUNK_SIZE_Z - 4;
	if (temp <= 1)
		temp = 1;
	return temp;
}

inline double TerrainGenerator::SimplexRidged(int x, int y, double freq = RIDGED_FREQ, int octaves = RIDGED_OCTAVES,
	float lac = RIDGED_LACUN, float gain = RIDGED_GAIN) {
	double sum = 0;
	float amplitude = 1.0f;
	glm::dvec2 f; 
	f.x = x * freq;
	f.y = y * freq;
	for (int i = 0; i < octaves; ++i) {
		double n = 1.0f - abs(simplex(x*freq, y*freq, nullptr, nullptr));
		sum += n*amplitude;
		freq *= lac;
		amplitude *= gain;
	}
	double temp = (sum / amplitude);
	temp = 64 * temp;
	if (temp >= CHUNK_SIZE_Z - 4)
		temp = CHUNK_SIZE_Z - 4;
	if (temp <= 1)
		temp = 1;
	return temp;
}

inline double TerrainGenerator::SimplexCaves(int x, int y) {
	return SimplexRidged(x,y, 0.0001,5,2.5,1);
}

// The following functions come from:
// http://www.decarpentier.nl/scape-procedural-extensions
static double SWISS_FREQ = 0.008; static double SWISS_OCTAVES = 7; // This is a slow terrain function because of high octaves
static float SWISS_LACUNARITY = 3.0f; static float SWISS_GAIN = 1.5f;

inline double TerrainGenerator::SimplexSwiss(int x, int y, double freq = SWISS_FREQ, int octaves = SWISS_OCTAVES,
	float lac = SWISS_LACUNARITY, float gain = SWISS_GAIN) {
	glm::dvec2 f; f.x = x * freq; f.y = y * freq;
	auto sum = 0.0; auto amplitude = 1.0;
	glm::dvec2 derivSum(0.0, 0.0);
	auto warp = 0.15;
	for (int i = 0; i < octaves; ++i) {
		double *dx = new double;
		double *dy = new double;
		auto n = 1.0 - abs(simplex(f.x + warp*derivSum.x, f.y + warp*derivSum.y, dx, dy));
		sum += amplitude * n;
		derivSum += glm::dvec2(*dx*amplitude*(-n), *dy*amplitude*(-n));
		freq *= lac;
		amplitude *= (gain * glm::clamp(sum, 0.0, 1.0));
		delete dx, dy;
	}
	auto temp = sum / amplitude;
	temp = temp * 16;
	if (temp >= CHUNK_SIZE_Z - 4) {
		temp = CHUNK_SIZE_Z - 4;
	}
	if (temp <= 1) {
		temp = 1;
	}
	return temp;
}


// Jordan-style terrain
static auto JORDAN_FREQ = 0.008; static auto JORDAN_OCTAVES = 7; // This is a slow terrain function because of high octaves
static auto JORDAN_LACUNARITY = 3.0; static auto JORDAN_GAIN = 1.5;
static auto warp0 = 0.4, warp1 = 0.35;
static auto damp0 = 1.0, damp1 = 0.8;

auto TerrainGenerator::SimplexJordan(int x, int y, double freq, int octaves, float lac, float gain) {
	double* dx1 = new double; double* dy1 = new double;
	glm::dvec2 f; f.x = x * freq; f.y = y * freq;
	auto n = simplex(f.x, f.y, dx1, dy1);
	glm::dvec3 nSq = glm::dvec3(n*n, *dx1*n, *dy1*n);
	glm::dvec2 dWarp = glm::dvec2(*dx1*warp0, *dy1*warp0);
	glm::dvec2 dDamp = glm::dvec2(*dx1*damp0, *dy1*warp1);
}
#endif