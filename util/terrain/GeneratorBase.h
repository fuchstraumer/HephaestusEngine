#ifndef GENERATOR_BASE_H
#define GENERATOR_BASE_H

#include "TerrainCommon.h"

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

namespace terrain {

	class GeneratorBase {
	public:

		// Instantiates a terrain generator, setting the seed and building the hash table
		GeneratorBase(int seed) {
			this->Seed = seed;
			r_gen.seed(this->Seed);
			buildHash();
		}

		// Used to seed the random generator (below)
		int Seed;

		// Mersenne-twister used to shuffle the hash table on initialization of this object.
		std::mt19937 r_gen;

		//double SimplexFBM(int x, int y, int z, double frequency, int octaves, float lacunarity, float gain);
		double SimplexFBM(int x, int y, double freq = FBM_FREQ, int octaves = FBM_OCTAVES, float lac = FBM_LACUN, float gain = FBM_GAIN);

		//double SimplexBillow(int x, int y, int z, double frequency, int octaves, float lacunarity, float gain);
		double SimplexBillow(int x, int y, double freq = BILLOW_FREQ, int octaves = BILLOW_OCTAVES, float lac = BILLOW_LACUN, float gain = BILLOW_GAIN);

		//double SimplexRidged(int x, int y, int z, double freq, int octaves, float lac, float gain);;
		double SimplexRidged(int x, int y, double freq = RIDGED_FREQ, int octaves = RIDGED_OCTAVES, float lac = RIDGED_LACUN, float gain = RIDGED_GAIN);

		// "Swiss" noise using derivatives to simulate erosion and create better mountains
		double SimplexSwiss(int x, int y, double freq = SWISS_FREQ, int octaves = SWISS_OCTAVES, float lac = SWISS_LACUNARITY, float gain = SWISS_GAIN);

		// Jordan noise - not implemented yet.
		double SimplexJordan(int x, int y, double freq, int octaves, float lac, float gain);

	protected:

		// Hash table containing values to be used for gradient vectors
		// Leave as unsigned char - getting this to fit in the cache is optimal
		unsigned char hashTable[512];

		// Builds the hash table containing our 0-255 values used for gradient vectors
		// This approach is psuedo-random (on purpose) and faster than other methods
		// This is relatively expensive, so it should only be called once.
		void buildHash() {
			for (int c = 0; c < 255; ++c) {
				this->hashTable[c] = c;
				this->hashTable[c + 256] = c;
			}
			shuffle(this->hashTable, this->hashTable + 512, this->r_gen);
		}

		// Faster flooring function than std::floor()
		inline int fastfloor(double x) {
			return x > 0 ? (int)x : (int)x - 1;
		}

		// Finds the dot product of x,y,z and the gradient vector "hash". 
		// Source: http://riven8192.blogspot.com/2010/08/calculate-perlinnoise-twice-as-fast.html //
		// Forcing inline to increase speed (I hope).
		__forceinline double grad(int hash, double x, double y, double z);

		// Function for finding gradient of simplex noise in 2D
		__forceinline double sGrad(int hash, double x, double y);

		// Function for finding gradient of simplex noise in 3D
		__forceinline double sGrad(int hash, double x, double y, double z);

		// Simplex noise gens
		// return the derivatives at x,y if non-null pointers dx,dy are supplied
		double simplex(double x, double y, double* dx, double* dy);

		// Returns value of noise at given point xyz
		double simplex(double x, double y, double z);

	};

}
#endif