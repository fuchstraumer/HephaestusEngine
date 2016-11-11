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

static const double FBM_FREQ = 0.0002; static const int FBM_OCTAVES = 5;
static const float FBM_LACUN = 2.0f; static const float FBM_GAIN = 1.5f;
static const double BILLOW_FREQ = 0.0018; static const int BILLOW_OCTAVES = 4;
static const float BILLOW_LACUN = 2.50f; static const float BILLOW_GAIN = 1.20f;
static const double RIDGED_FREQ = 0.00008; static const int RIDGED_OCTAVES = 6;
static const float RIDGED_LACUN = 2.50f; static const float RIDGED_GAIN = 2.20f;
static double SWISS_FREQ = 0.008; static double SWISS_OCTAVES = 7; // This is a slow terrain function because of high octaves
static float SWISS_LACUNARITY = 3.0f; static float SWISS_GAIN = 1.5f;

class TerrainGenerator {
public:
	// Instantiates a terrain generator, setting the seed and building the hash table
	TerrainGenerator(int seed) {
		this->Seed = seed;
		r_gen.seed(this->Seed);
		buildHash();
	}
	int Seed;
	std::mt19937 r_gen;
	// Returns evaluation of base perlin function at x,y,z
	double point(int x, int y, int z);
	
	// Fractal-Brownian-Motion perlin noise based terrain gen.
	double PerlinFBM(int x, int y, int z, double frequency, int octaves, float lacunarity, float gain);
	//double SimplexFBM(int x, int y, int z, double frequency, int octaves, float lacunarity, float gain);
	double SimplexFBM(int x, int y, double freq = FBM_FREQ, int octaves = FBM_OCTAVES, float lac = FBM_LACUN, float gain = FBM_GAIN);
	
	// Billowy perlin generator, takes the absolute value of the perlin gen and generally works at low gain and low freq
	double PerlinBillow(int x, int y, int z, double frequency, int octaves, float lacunarity, float gain);
	//double SimplexBillow(int x, int y, int z, double frequency, int octaves, float lacunarity, float gain);
	double SimplexBillow(int x, int y, double freq = BILLOW_FREQ, int octaves = BILLOW_OCTAVES, float lac = BILLOW_LACUN, float gain = BILLOW_GAIN);

	// Specialized instance of Billow gen with set parameters
	double RollingHills(int x, int y, int z);

	
	// Generates ridges and other bizarre patterns. Currently broken.
	double PerlinRidged(int x, int y, int z, double freq, int octaves, float lac, float gain);
	//double SimplexRidged(int x, int y, int z, double freq, int octaves, float lac, float gain);;
	double SimplexRidged(int x, int y, double freq = RIDGED_FREQ, int octaves = RIDGED_OCTAVES, float lac = RIDGED_LACUN, float gain = RIDGED_GAIN);

	// "Swiss" noise using derivatives to simulate erosion and create better mountains
	double SimplexSwiss(int x, int y, double freq = SWISS_FREQ, int octaves = SWISS_OCTAVES, float lac = SWISS_LACUNARITY, float gain = SWISS_GAIN);

	// Jordan noise
	auto SimplexJordan(int x, int y, double freq, int octaves, float lac, float gain);

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

	double sGrad(int hash, double x, double y, double z) {
		int h = hash & 15;     // Convert low 4 bits of hash code into 12 simple
		double u = h<8 ? x : y; // gradient directions, and compute dot product.
		double v = h<4 ? y : h == 12 || h == 14 ? x : z; // Fix repeats at h = 12 to 15
		return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
	}

	// Simplex noise gens
	// return the derivatives at x,y if non-null pointers dx,dy are supplied
	double simplex(double x, double y, double* dx, double* dy);

	double simplex(double x, double y, double z);
};

#endif