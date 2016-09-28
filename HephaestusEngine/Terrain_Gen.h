#ifndef TERRAIN_GEN_H
#define TERRAIN_GEN_H

#include "stdafx.h"
#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include <Noise\anl_noise.h>
#include <Imaging\anl_imaging.h>
#include <math.h>
using namespace anl;
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
clock_t t5;

// Interp functions for making more volume of noise out of less actual generation calls
static float lerp(float x, float x1, float x2, float q00, float q01) {
	return ((x2 - x) / (x2 - x1)) * q00 + ((x - x1) / (x2 - x1)) * q01;
}

static float biLerp(float x, float y, float q11, float q12, float q21, float q22, float x1, float x2, float y1, float y2) {
	float r1 = lerp(x, x1, x2, q11, q21);
	float r2 = lerp(x, x1, x2, q12, q22);

	return lerp(y, y1, y2, r1, r2);
}

/*static float triLerp(float x, float y, float z, float q000, float q001, float q010, float q011, float q100, float q101, float q110, float q111, float x1, float x2, float y1, float y2, float z1, float z2) {
	// The eight q points are around our cube of interest
	float x00 = lerp(x, x1, x2, q000, q100);
	float x10 = lerp(x, x1, x2, q010, q110);
	float x01 = lerp(x, x1, x2, q001, q101);
	float x11 = lerp(x, x1, x2, q011, q111);
	float r0 = lerp(y, y1, y2, x00, x01);
	float r1 = lerp(y, y1, y2, x10, x11);

	return lerp(z, z1, z2, r0, r1);
}*/
/* Vxyz = V(0,0,0)*(1-x)*(1-y)*(1-z) +
		  V(1,0,0)*x*(1-y)*(1-z)     +
		  V(0,1,0)*(1-x)*y*(1-z)     +
		  V(0,0,1)*(1-x)*(1-y)*z     +
		  V(1,0,1)*x*(1-y)*z         +
		  V(0,1,1)*(1-x)*y*z         +
		  V(1,1,0)*x*y*(1-z)         +
		  V(1,1,1)*x*y*z
	Returns the value at x,y,z given the corner points
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
	std::vector<glm::vec4> cube_verts{
		glm::vec4(0,0,0,0), // V000
		glm::vec4(32,0,0,0), // V100
		glm::vec4(0,64,0,0), // V010
		glm::vec4(0,0,32,0), // V001
		glm::vec4(32,0,32,0), // V101
		glm::vec4(0,64,32,0), // V011
		glm::vec4(32,64,0,0), // V110
		glm::vec4(32,64,32,0), // V111
	};
};

class Terrain_Generator {
public:
	CArray3Dd generator(int x_range, int y_range, int z_range, glm::vec2 chunk_pos = glm::vec2(0, 0))
	{
		ANLFloatType x_min = (chunk_pos.x - 1); ANLFloatType x_max = (chunk_pos.x + 1);
		ANLFloatType z_min = (chunk_pos.y - 1); ANLFloatType z_max = (chunk_pos.y + 1);
		ANLFloatType y_min = 0; ANLFloatType y_max = 2;
		SMappingRanges(x_min, x_max, y_min, y_max, z_min, z_max);
		// Create boxes/cubes for trilerp
		triLerpCube cube; 
		// Base gradient defining range of terrain
		CImplicitGradient ground_gradient; ground_gradient.setGradient(0.0, 0.0, 0.0, 1.0);
		// Lowland generator
		CImplicitFractal lowland_shape_fractal(BILLOW, this->Lowland_Basis_Type, this->Terrain_Fractal_Interpolation, this->LowlandOctaves, this->LowlandFrequency, false);
		lowland_shape_fractal.setSeed(this->Lowland_Seed);
		CImplicitAutoCorrect lowland_autocorrect; lowland_autocorrect.setSource(&lowland_shape_fractal); lowland_autocorrect.setRange(0, 1);
		CImplicitScaleOffset lowland_scale; lowland_scale.setScale(this->LowlandScale); lowland_scale.setOffset(-0.45);
		CImplicitScaleDomain lowland_y_scale; lowland_y_scale.setYScale(0.0);
		CImplicitTranslateDomain lowland_terrain; lowland_terrain.setSource(&ground_gradient); lowland_terrain.setYAxisSource(&lowland_y_scale);

		// Highland terrain
		CImplicitFractal highland_shape_fractal(FBM, this->Highland_Basis_Type, this->Terrain_Fractal_Interpolation, this->HighlandOctaves, this->HighlandFrequency, false);
		highland_shape_fractal.setSeed(this->Highland_Seed);
		CImplicitAutoCorrect highland_autocorrect; highland_autocorrect.setSource(&highland_shape_fractal); highland_autocorrect.setRange(-1, 1);
		CImplicitScaleOffset highland_scale; highland_scale.setScale(this->HighlandScale); highland_scale.setOffset(0.0); highland_scale.setSource(&highland_autocorrect);
		CImplicitScaleDomain highland_y_scale; highland_y_scale.setYScale(0.0); highland_y_scale.setSource(&highland_scale);
		CImplicitTranslateDomain highland_terrain; highland_terrain.setSource(&ground_gradient); highland_terrain.setYAxisSource(&highland_y_scale);

		// Mountain terrain
		CImplicitFractal mountain_shape_fractal(RIDGEDMULTI, this->Mountain_Basis_Type, this->Terrain_Fractal_Interpolation, this->MountainOctaves, this->MountainFrequency, false);
		mountain_shape_fractal.setSeed(this->Mountain_Seed);
		CImplicitAutoCorrect mountain_autocorrect; mountain_autocorrect.setSource(&mountain_shape_fractal); mountain_autocorrect.setRange(-1, 1);
		CImplicitScaleOffset mountain_scale; mountain_scale.setScale(this->MountainScale); mountain_scale.setOffset(0.15);
		CImplicitScaleDomain mountain_y_scale; mountain_y_scale.setSource(&mountain_scale); mountain_y_scale.setYScale(0.25);
		CImplicitTranslateDomain mountain_terrain; mountain_terrain.setSource(&ground_gradient); mountain_terrain.setYAxisSource(&mountain_y_scale);

		// Terrain type selection functions + cache function
		CImplicitFractal terrain_type_fractal(FBM, GRADIENT, QUINTIC, 3, 0.2, false);
		//CImplicitFractal terrain_type_fractal(FBM, GRADIENT, QUINTIC, 3, 0.5, false);
		CImplicitAutoCorrect terrain_autocorrect; terrain_autocorrect.setSource(&terrain_type_fractal); terrain_autocorrect.setRange(0.0, 1.0);
		CImplicitCache terrain_type_cache; terrain_type_cache.setSource(&terrain_autocorrect);
		CImplicitSelect highland_mountain_select; highland_mountain_select.setLowSource(&highland_terrain); highland_mountain_select.setHighSource(&mountain_terrain);
		highland_mountain_select.setControlSource(&terrain_type_cache); highland_mountain_select.setThreshold(0.55); highland_mountain_select.setFalloff(0.2);
		CImplicitSelect highland_lowland_select; highland_lowland_select.setLowSource(&lowland_terrain); highland_lowland_select.setHighSource(&highland_mountain_select);
		highland_lowland_select.setControlSource(&terrain_type_cache); highland_lowland_select.setThreshold(0.25); highland_lowland_select.setFalloff(0.15);
		// Declare cache to cache output and decrease how often we have to generate noise.
		CImplicitCache highland_lowland_cache; highland_lowland_cache.setSource(&highland_lowland_select);
		CImplicitSelect ground_select; ground_select.setLowSource(0.0); ground_select.setHighSource(1.0); ground_select.setControlSource(&highland_lowland_cache);
		ground_select.setThreshold(0.5); ground_select.setFalloff(0.0);

		// Cave generation system
		CImplicitMath cave_attenuate_bias(BIAS, &highland_lowland_cache, this->CaveBias); // This is set well. This changes how sharply the caves widen under the first layer of terrain
		CImplicitFractal cave_shape_1(RIDGEDMULTI, this->Cave_Basis_Type, this->Cave_Fractal_Interpolation, this->TunnelOctaves, this->CaveFrequency, false); 
		CImplicitFractal cave_shape_2(RIDGEDMULTI, this->Cave_Basis_Type, this->Cave_Fractal_Interpolation, this->TunnelOctaves, this->CaveFrequency, false);
		CImplicitCombiner cave_shape_attenuate(MULT); cave_shape_attenuate.setSource(0, &cave_shape_1);
		cave_shape_attenuate.setSource(1, &cave_attenuate_bias); cave_shape_attenuate.setSource(2, &cave_shape_2);
		CImplicitFractal cave_perturb_fractal(FBM, GRADIENT, QUINTIC, this->CaveGrainOctaves, 3, false); CImplicitScaleOffset cave_perturb_scale;
		cave_perturb_scale.setScale(0.5); cave_perturb_scale.setSource(&cave_perturb_fractal);
		CImplicitTranslateDomain cave_perturb; cave_perturb.setXAxisSource(&cave_perturb_scale); cave_perturb.setSource(&cave_shape_attenuate);
		CImplicitSelect cave_select; cave_select.setControlSource(&cave_perturb); cave_select.setLowSource(1.0); cave_select.setHighSource(0.0); cave_select.setThreshold(0.48);
		cave_select.setFalloff(0.0);
		// Final output
		CImplicitCombiner final_out(AVG); final_out.setSource(0, &cave_select); final_out.setSource(1, &ground_select);
		std::vector<triLerpCube> cube_vec;
		int w = x_range; int h = y_range; int l = z_range;
		for (unsigned int i = 0; i < cube.cube_verts.size(); i++) {
			float dz = z_max - z_min; float dx = x_max - x_min; float dy = y_max - y_min;
			float x, y, z; 
			x = cube.cube_verts[i].x / w; z = (cube.cube_verts[i].z / l);
			y = cube.cube_verts[i].y / h; float nz = z_min + z*dz;
			float nx = x_min + x*dx; float ny = y_min + y*dy;
			cube.cube_verts[i].w = final_out.get(nx, ny, nz);
			std::cerr << " " << nx << " " << ny << " " << nz << " " << std::endl;
			std::cerr << ground_select.get(nx, ny, nz) << std::endl;
			std::cerr << cave_select.get(nx,ny,nz) << std::endl;
			std::cerr << cube.cube_verts[i].w << std::endl;
		}
		//z_max = 1; z_min = 0; SMappingRanges(x_min, x_max, y_min, y_max, z_min, z_max);
		t5 = clock();
		CArray3Dd img(x_range, y_range, z_range);
		map3D(SEAMLESS_NONE, img, final_out, SMappingRanges(x_min, x_max, y_min, y_max, z_min, z_max));
		t5 = clock() - t5;
		std::cerr << static_cast<float>(t5) / CLOCKS_PER_SEC;
		return img;
		
	}

	/// Terrain Generation parameters and class attributes follow

	// Noise gen seeds
	ANLFloatType Lowland_Seed = 6266;
	ANLFloatType Highland_Seed = 6266;
	ANLFloatType Mountain_Seed = 6266;
	ANLFloatType Cave_Seed = 6266;
	// Basis types drastically change how the noise will prevent itself. Changing this will result in completely undocumented behavior.
	// Simply, it's sorta highly recommended this remain unchanged.
	EBasisTypes Lowland_Basis_Type = GRADIENT;
	EBasisTypes Highland_Basis_Type = GRADIENT;
	EBasisTypes Mountain_Basis_Type = GRADIENT;
	EBasisTypes Cave_Basis_Type = GRADIENT;

	// Interpolation schemes affect how the output noise is smoothed from its base value. Quintic comes at no large speed penalty and works best. It is recommended that this remain unchanged.
	EInterpTypes Terrain_Fractal_Interpolation = QUINTIC;
	EInterpTypes Cave_Fractal_Interpolation = QUINTIC; // Modifies only the cave base generators

	// This uniformly controls the scale of the primary noise output used to make this terrain type.
	double LowlandScale = 0.250;
	double HighlandScale = 0.250;
	double MountainScale = 0.450;

	// This effectively controls the complexity of the primary noise output used to make this terrain type.
	int LowlandOctaves = 3;
	int HighlandOctaves = 3;
	int MountainOctaves = 5;
	int TunnelOctaves = 2; // Octaves for the simple noise used to generate tunnels. Values > 1 may cause strange tunnel behavior.
	int CaveGrainOctaves = 5; // Octaves for the noise used to add grain and detail to the tunnels - makes them more than wormy cylinders.

	// This could best be seen as adjusting the density of terrain features: higher frequency means more of the same noise gets packed into the same area as a lower setting.
	// Scale makes all features "bigger", this just makes "more" of everything.
	double LowlandFrequency = 0.350;
	double HighlandFrequency = 2.0;
	double MountainFrequency = 1.0;
	double CaveFrequency = 4.0;

	// This parameter controls how sharply the caves "widen" into broader cave systems from their start as small tunnels near the surface.
	// Adjust this too sharply can cause bizarre results. It is recommended to just leave it as-is.
	double CaveBias = 0.450;

	
};


#endif