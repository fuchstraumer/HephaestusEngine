#ifndef TERRAIN_GEN_H
#define TERRAIN_GEN_H

#include "stdafx.h"
#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include <math.h>
#include <random>

#include "FastNoise.h"

using namespace std;
/*
	Class: Terrain Generator
	Primary terrain generation system for this library/engine. The "build_generator" function builds and links the terrain generator pipeline.
	If struggling for speed, try lowering the octave count on any noise generators. This is the primary performance culprit. Switching to cubic
	or even linear interpolation for the noise generators may also increase speed, but at a significant expense with regards to quality.

	Caves are currently a big bugged and/or odd. Widen too steeply too swiftly into chasms you could drop a building into.

	Use Terrain_Chunk to get 3D terrain data. Use Terrain_Img to get a vague idea of what the terrain at a given location will look. 
	The height is currently restrained to effectively run from -1 to 1. X is endless though. With the default range values, keep the x_range of 
	Terrain_Img exactly twice that of y_range. If you want to increase the x_range and not y_range, adjust the SMappingRange and increase the x values. 
	The y_range should stay fixed at -1 to 1 though, and jsut be sure to account for the proportions between the ranges set.

*/

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

typedef std::vector<glm::vec4> triLerpCube;
static float perlinPsuedoDeriv(glm::vec2 p, float seed) {
	p.x = floor(p.x); p.y = floor(p.y);
}
static float iq_Noise(float x, float y, float z, int octaves = 8, float frequency = 0.5f, float gain = 1.0f) {
	float sum = 0.5; float freq = frequency; float amplitude = gain; glm::vec2 dsum(0.0f, 0.0f);
	FastNoise iqNoise; iqNoise.SetNoiseType(FastNoise::SimplexFractal); iqNoise.SetFractalType(FastNoise::FBM);
	iqNoise.SetFractalOctaves(octaves);
	for (int i = 0; i < octaves; ++i) {
		glm::vec3 n;
	}
}


class Terrain_Generator {
public:
	FastNoise myNoise;
	float genTerrain(int x, int y){
		myNoise.SetNoiseType(FastNoise::ValueFractal);
		myNoise.SetFractalType(FastNoise::Billow);
		myNoise.SetFractalOctaves(3.0f); myNoise.SetFrequency(0.02f);
		myNoise.SetFractalLacunarity(0.3f); myNoise.SetFractalGain(1.5f);
		myNoise.SetPositionWarpAmp(5.0f);
		float temp = myNoise.GetNoise(x, y);
		temp = 64 * sqrt(temp*temp);
		if (temp > 64)
			temp = 64;
		if (temp < 0)
			temp = 1;
		return temp;
	}

	triLerpCube getNoiseCube(int x_center, int y_center,int z_center) {
		FastNoise cNoise; cNoise.SetNoiseType(FastNoise::SimplexFractal);
		cNoise.SetFractalType(FastNoise::FBM); cNoise.SetFractalOctaves(4); cNoise.SetFractalLacunarity(0.3f);
		cNoise.SetFractalGain(0.1f); x_center *= CHUNK_SIZE; y_center *= CHUNK_SIZE; z_center *= CHUNK_SIZE_Z;
		triLerpCube noiseCube = {
			glm::vec4(0+x_center,0+y_center,0+z_center,0), // V000
			glm::vec4(1+x_center,0+y_center,0+z_center,0), // V100
			glm::vec4(0+x_center,1+y_center,0+z_center,0), // V010
			glm::vec4(0+x_center,0+y_center,1+z_center,0), // V001
			glm::vec4(1+x_center,0+y_center,1+z_center,0), // V101
			glm::vec4(0+x_center,1+y_center,1+z_center,0), // V011
			glm::vec4(1+x_center,1+y_center,0+z_center,0), // V110
			glm::vec4(1+x_center,1+y_center,1+z_center,0), // V111
		};
		for (int i = 0; i < noiseCube.size(); ++i) {
			glm::vec4 vec; vec.x = noiseCube[i].x;
			vec.y = noiseCube[i].y; vec.z = noiseCube[i].z;
			noiseCube[i].w = cNoise.GetNoise(vec.x, vec.y, vec.z);
		}

		return noiseCube;
	}

	float genTerrain3D(int x, int y, int z) {
		FastNoise volNoise;
		volNoise.SetFractalType(FastNoise::Billow); 
		volNoise.SetNoiseType(FastNoise::SimplexFractal);

	}
	
	

};


#endif