#ifndef TERRAIN_GEN_H
#define TERRAIN_GEN_H

#include "stdafx.h"
#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include <math.h>
#include <FastNoiseSIMD.h>

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

struct triLerpCube {
	std::vector<glm::vec4> cube_verts;
};

class Terrain_Generator {
public:
	triLerpCube triLerpCube0;
	double*** Generate() {
		FastNoiseSIMD* myNoise = FastNoiseSIMD::NewFastNoiseSIMD();
		float* noiseSet = FastNoiseSIMD::GetEmptySet(CHUNK_SIZE, CHUNK_SIZE_Y, CHUNK_SIZE);
		myNoise->SetFractalOctaves(5);
		myNoise->SetFractalType(FastNoiseSIMD::Billow);
		//myNoise->SetAxisScales(1.0, 0.0, 1.0);
		myNoise->FillSimplexFractalSet(noiseSet,0,0,0,CHUNK_SIZE,CHUNK_SIZE_Y,CHUNK_SIZE);
		triLerpCube0.cube_verts = {
			glm::vec4(0,0,0,0), // V000
			glm::vec4(16,0,0,0), // V100
			glm::vec4(0,32,0,0), // V010
			glm::vec4(0,0,16,0), // V001
			glm::vec4(16,0,16,0), // V101
			glm::vec4(0,32,16,0), // V011
			glm::vec4(16,32,0,0), // V110
			glm::vec4(16,32,16,0), // V111
		};

		triLerpCube triLerpCube1;
		triLerpCube1.cube_verts = std::vector<glm::vec4>{
			glm::vec4(16,0,16,0), // V000
			glm::vec4(32,0,16,0), // V100
			glm::vec4(16,32,16,0), // V010
			glm::vec4(16,0,32,0), // V001
			glm::vec4(32,0,32,0), // V101
			glm::vec4(16,32,32,0), // V011
			glm::vec4(32,32,16,0), // V110
			glm::vec4(32,32,32,0), // V111
		};

		triLerpCube triLerpCube2;
		triLerpCube2.cube_verts = std::vector<glm::vec4>{
			glm::vec4(0,32,0,0), // V000
			glm::vec4(16,32,0,0), // V100
			glm::vec4(0,64,0,0), // V010
			glm::vec4(0,32,16,0), // V001
			glm::vec4(16,32,16,0), // V101
			glm::vec4(0,64,16,0), // V011
			glm::vec4(16,64,0,0), // V110
			glm::vec4(16,64,16,0), // V111
		};

		triLerpCube triLerpCube3;
		triLerpCube3.cube_verts = std::vector<glm::vec4>{
			glm::vec4(16,32,16,0), // V000
			glm::vec4(32,32,16,0), // V100
			glm::vec4(16,64,16,0), // V010
			glm::vec4(16,32,32,0), // V001
			glm::vec4(32,32,32,0), // V101
			glm::vec4(16,64,32,0), // V011
			glm::vec4(32,64,16,0), // V110
			glm::vec4(32,64,32,0), // V111
		};
		int index = 0;
		double*** terrain;
		terrain = new double**[CHUNK_SIZE];
		for (int i = 0; i < CHUNK_SIZE; ++i) {
			terrain[i] = new double*[CHUNK_SIZE_Y];
			for (int j = 0; j < CHUNK_SIZE_Y; ++j) {
				terrain[i][j] = new double[CHUNK_SIZE];
				for (int k = 0; k < CHUNK_SIZE; ++k) {
					std::cerr << noiseSet[index];
					terrain[i][j][k] = double(noiseSet[index++]);
					
				}
			}
		}
		return terrain;
	}
			
};
/*
terraintree3d=
{
{name="ground_gradient",               type="gradient",         x1=0, x2=0, y1=0, y2=1},

{name="lowland_shape_fractal",         type="fractal",          fractaltype=anl.BILLOW, basistype=anl.GRADIENT, interptype=anl.QUINTIC, octaves=2, frequency=0.25},
{name="lowland_autocorrect",           type="autocorrect",      source="lowland_shape_fractal", low=0, high=1},
{name="lowland_scale",                 type="scaleoffset",      source="lowland_autocorrect", scale=0.125, offset=-0.45},
{name="lowland_y_scale",               type="scaledomain",      source="lowland_scale", scaley=0},
{name="lowland_terrain",               type="translatedomain",  source="ground_gradient", ty="lowland_y_scale"},

{name="highland_shape_fractal",        type="fractal",          fractaltype=anl.FBM, basistype=anl.GRADIENT, interptype=anl.QUINTIC, octaves=4, frequency=2},
{name="highland_autocorrect",          type="autocorrect",      source="highland_shape_fractal", low=-1, high=1},
{name="highland_scale",                type="scaleoffset",      source="highland_autocorrect", scale=0.25, offset=0},
{name="highland_y_scale",              type="scaledomain",      source="highland_scale", scaley=0},
{name="highland_terrain",              type="translatedomain",  source="ground_gradient", ty="highland_y_scale"},

{name="mountain_shape_fractal",        type="fractal",          fractaltype=anl.RIDGEDMULTI, basistype=anl.GRADIENT, interptype=anl.QUINTIC, octaves=8, frequency=1},
{name="mountain_autocorrect",          type="autocorrect",      source="mountain_shape_fractal", low=-1, high=1},
{name="mountain_scale",                type="scaleoffset",      source="mountain_autocorrect", scale=0.45, offset=0.15},
{name="mountain_y_scale",              type="scaledomain",      source="mountain_scale", scaley=0.25},
{name="mountain_terrain",              type="translatedomain",  source="ground_gradient", ty="mountain_y_scale"},

{name="terrain_type_fractal",          type="fractal",          fractaltype=anl.FBM, basistype=anl.GRADIENT, interptype=anl.QUINTIC, octaves=3, frequency=0.125},
{name="terrain_autocorrect",           type="autocorrect",      source="terrain_type_fractal", low=0, high=1},
{name="terrain_type_y_scale",          type="scaledomain",      source="terrain_autocorrect", scaley=0},
{name="terrain_type_cache",            type="cache",            source="terrain_type_y_scale"},
{name="highland_mountain_select",      type="select",           low="highland_terrain", high="mountain_terrain", control="terrain_type_cache", threshold=0.55, falloff=0.2},
{name="highland_lowland_select",       type="select",           low="lowland_terrain", high="highland_mountain_select", control="terrain_type_cache", threshold=0.25, falloff=0.15},
{name="highland_lowland_select_cache", type="cache",            source="highland_lowland_select"},
{name="ground_select",                 type="select",           low=0, high=1, threshold=0.5, control="highland_lowland_select_cache"},

{name="cave_attenuate_bias",           type="bias",              source="highland_lowland_select_cache", bias=0.45},
{name="cave_shape1",                   type="fractal",           fractaltype=anl.RIDGEDMULTI, basistype=anl.GRADIENT, interptype=anl.QUINTIC, octaves=1, frequency=4},
{name="cave_shape2",                   type="fractal",           fractaltype=anl.RIDGEDMULTI, basistype=anl.GRADIENT, interptype=anl.QUINTIC, octaves=1, frequency=4},
{name="cave_shape_attenuate",         type="combiner",           operation=anl.MULT, source_0="cave_shape1", source_1="cave_attenuate_bias", source_2="cave_shape2"},
{name="cave_perturb_fractal",          type="fractal",           fractaltype=anl.FBM, basistype=anl.GRADIENT, interptype=anl.QUINTIC, octaves=6, frequency=3},
{name="cave_perturb_scale",            type="scaleoffset",       source="cave_perturb_fractal", scale=0.5, offset=0},
{name="cave_perturb",                  type="translatedomain",   source="cave_shape_attenuate", tx="cave_perturb_scale"},
{name="cave_select",                   type="select",            low=1, high=0, control="cave_perturb", threshold=0.48, falloff=0},

{name="ground_cave_multiply",          type="combiner",          operation=anl.MULT, source_0="cave_select", source_1="ground_select"}
}
*/

#endif