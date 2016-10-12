#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include <math.h>
#include <random>
#include <algorithm>
using namespace std;

#ifndef TERRAIN_GEN_H
#define TERRAIN_GEN_H


/*
	Class: Terrain Generator
	Primary terrain generation system for this library/engine.
	Sources:
	http://www.decarpentier.nl/scape-procedural-basics
	http://www.decarpentier.nl/scape-procedural-extensions
	http://www.iquilezles.org/www/articles/morenoise/morenoise.htm
	http://stackoverflow.com/questions/4297024/3d-perlin-noise-analytical-derivative
	https://github.com/Auburns/FastNoise/blob/master/FastNoise.cpp 
*/

// Finds the dot product of x,y,z and the gradient vector "hash". 
// Source: http://riven8192.blogspot.com/2010/08/calculate-perlinnoise-twice-as-fast.html //
static double grad(int hash, double x, double y, double z)
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
static float lerp(double a, double b, double x) {
	return a + x * (b - a);
}

class Terrain_Generator {
public:
	// Instantiates a terrain generator, setting the seed and building the hash table
	Terrain_Generator(int seed) {
		this->seed = seed;
		buildHash(this->seed);
	}
	int seed;

	// Returns evaluation of base perlin function at x,y,z
	float point(int x, int y, int z);

	// Fractal-Brownian-Motion perlin noise based terrain gen.
	float FBM(int x, int y, int z, float frequency, int octaves, float lacunarity, float gain);

	// Billowy perlin generator, takes the absolute value of the perlin gen and generally works at low gain and low freq
	float Billow(int x, int y, int z, float frequency, int octaves, float lacunarity, float gain);

	// Specialized instance of Billow gen with set parameters
	float RollingHills(int x, int y, int z);

	// Generates ridges and other bizarre patterns. Currently broken.
	float Ridged(int x, int y, int z, float freq, int octaves, float lac, float gain);

	// Basic example of octave summing with perlin noise in 3D. Deprecated.
	float octPerlin(float x, float y, float z, int octaves, float lacun);
private:

	// Hash table containing values to be used for gradient vectors
	unsigned char hashTable[512];

	// Builds the hash table containing our 0-255 values used for gradient vectors
	// This approach is psuedo-random (on purpose) and faster than other methods
	void buildHash(int s) {
		for (int c = 0; c < 255; ++c) {
			this->hashTable[c] = c;
			this->hashTable[c + 256] = c;
		}
		random_device r; ranlux48 gen(r());
		gen.seed(s);
		shuffle(this->hashTable, this->hashTable + 512, gen);
	}

	// Main noise function: terrain is specialized usage of this.
	float perlin(float x, float y, float z) {
		glm::vec3 p(x, y, z);
		// Calculate integer coords and fractional component for weighting
		// We have to normalize to be within the unit cube defined as part of the perlin function
		glm::highp_ivec3 p0; p0.x = floorf(p.x);
		p0.y = floorf(p.y); p0.z = floorf(p.z);
		glm::highp_vec3 p1(p0.x + 1, p0.y + 1, p0.z + 1);

		// Get ease curve values ease(u,v,w)
		glm::highp_vec3 w;
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
		float x00, x10, x01, x11, y00, y01;
		x00 = lerp(grad(aaa, p0.x, p0.y, p0.z), grad(baa, p1.x, p0.y, p0.z), w.x);
		x10 = lerp(grad(aba, p0.x, p1.y, p0.z), grad(bba, p1.x, p1.y, p0.z), w.x);
		x01 = lerp(grad(aab, p0.x, p0.y, p1.z), grad(bab, p1.x, p0.y, p1.z), w.x);
		x11 = lerp(grad(abb, p0.x, p1.y, p1.z), grad(bbb, p1.x, p1.y, p1.z), w.x);
		// Interpolate (bilinear)
		y00 = lerp(x00, x10, w.y);
		y01 = lerp(x01, x11, w.y);
		// Final step, trilinear interpolation (narrowing subrectangles)
		return (lerp(y00, y01, w.z) + 1) / 2;
	}
	// Used to finely add detail to various noise functions, like simulating erosion or making dunes
	float perlinDeriv(float x, float y, float z) {
		glm::vec3 p(x, y, z);
		// Calculate integer coords and fraction
		glm::highp_ivec3 p0; p0.x = floorf(p.x);
		p0.y = floorf(p.y); p0.z = floorf(p.z);
		glm::highp_vec3 p1(p0.x + 1, p0.y + 1, p0.z + 1);

		// Get ease curve values ease(u,v,w)
		glm::highp_vec3 w;
		w.x = fade(p.x - p0.x);
		w.y = fade(p.y - p0.y);
		w.z = fade(p.z - p0.z);
		// Get derivative ease curve values
		glm::highp_vec3 dw;
		dw.x = fadeDeriv(p.x - p0.x);
		dw.y = fadeDeriv(p.y - p0.y);
		dw.z = fadeDeriv(p.z - p0.z);

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
		// Get gradient vectors and interpolate using ease curve found earlier
		float x00, x10, x01, x11, y00, y01;
		x00 = lerp(grad(aaa, p0.x, p0.y, p0.z), grad(baa, p1.x, p0.y, p0.z), w.x);
		x10 = lerp(grad(aba, p0.x, p1.y, p0.z), grad(bba, p1.x, p1.y, p0.z), w.x);
		x01 = lerp(grad(aab, p0.x, p0.y, p1.z), grad(bab, p1.x, p0.y, p1.z), w.x);
		x11 = lerp(grad(abb, p0.x, p1.y, p1.z), grad(bbb, p1.x, p1.y, p1.z), w.x);
		//float k0, k1, k2, k3, k4, k5, k6, k7;
		//k0 = grad(baa,p1.x,p0.y,p0.z) - grad(aaa, p1.x, p0.y, p0.z);
		//k1 = grad(aba,p - grad(baa, p1.x, p0.y, p0.z);
		//k2 = dot001 - dot000
		// Interpolate (bilinear)
		y00 = lerp(x00, x10, w.y);
		y01 = lerp(x01, x11, w.y);
		// Final step, trilinear interpolation (narrowing subrectangles)
		return (lerp(y00, y01, w.z) + 1) / 2;
		
	}

};

// This function is just an interface to the private perlin member function.
inline float Terrain_Generator::point(int x, int y, int z) {
	float temp = perlin(x, y, z);
	if (temp > CHUNK_SIZE_Z)
		temp = CHUNK_SIZE_Z - 1;
	if (temp <= 1)
		temp = 0;
	return temp;
}

// Parameters:
// - Frequency is the frequency of the noise sampling (multiplier on input coords
// - An octave is an iteration through the noise function, but at smaller levels. It adds finer, grainier detail.
// - "lac" or Lacunarity controls how long an octave persists after the previous octave. Higher levels = more details. Too high is not good.
// - Gain controls the overall gain of the function.
// - Amplitude is set within the function body, in line with what generates the best terrain. The rest of the parameters are also set similarly.

inline float Terrain_Generator::FBM(int x, int y, int z, float freq = 0.005, int octaves = 4, float lac = 2.0, float gain = 0.5) {
	float sum = 0;
	float amplitude = 1.0;
	for (int i = 0; i < octaves; ++i) {
		float n = perlin(x*freq, y*freq, z*freq);
		sum += n*amplitude;
		freq *= lac;
		amplitude *= gain;
	}
	float temp = (sum / amplitude) + 1;
	//std::cerr << temp << std::endl;
	temp = sqrt(temp*temp);
	if (temp >= CHUNK_SIZE_Z - 4)
		temp = CHUNK_SIZE_Z - 4;
	if (temp <= 1)
		temp = 1;
	return temp;
}

inline float Terrain_Generator::Billow(int x, int y, int z, float freq = 0.01, int octaves = 3, float lac = 1.7, float gain = 0.6) {
	float sum = 0;
	float amplitude = 1.0;
	for (int i = 0; i < octaves; ++i) {
		float n = abs(perlin(x*freq, y*freq, z*freq));
		sum += n*amplitude;
		freq *= lac;
		amplitude *= gain;
	}
	float temp = (sum / amplitude) + 1;
	if (temp >= CHUNK_SIZE_Z - 4)
		temp = CHUNK_SIZE_Z - 4;
	if (temp <= 1)
		temp = 1;
	return temp;
}

inline float Terrain_Generator::RollingHills(int x, int y, int z) {
	return Billow(x, y, z,0.009,3,1.6,0.6);
}

inline float Terrain_Generator::Ridged(int x, int y, int z, float freq = 0.02, int octaves = 3, float lac = 2.5, float gain = 0.8) {
	float n = 1.0f - abs(perlin(x*freq, y*freq, z*freq));
	float temp = n * 1.5;
	if (temp >= CHUNK_SIZE_Z - 4)
		temp = CHUNK_SIZE_Z - 4;
	if (temp <= 1)
		temp = 1;
	return temp;
}

// Deprecated
inline float Terrain_Generator::octPerlin(float x, float y, float z, int octaves = 3, float lacun = 1.0) {
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

#endif