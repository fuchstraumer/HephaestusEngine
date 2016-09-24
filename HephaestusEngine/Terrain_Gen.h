#ifndef TERRAIN_GEN_H
#define TERRAIN_GEN_H

#include "stdafx.h"
#include "glm/glm.hpp"
#include <Noise\anl_noise.h>
#include <Imaging\anl_imaging.h>
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

class Terrain_Generator {
public:
	void build_generator()
	{
		// Base gradient defining range of terrain
		CImplicitGradient ground_gradient; ground_gradient.setGradient(0.0, 0.0, 0.0, 1.0);
		// Lowland generator
		CImplicitFractal lowland_shape_fractal(BILLOW, this->Lowland_Basis_Type, this->Terrain_Fractal_Interpolation, this->LowlandOctaves, this->LowlandFrequency, false);
		CImplicitAutoCorrect lowland_autocorrect; lowland_autocorrect.setSource(&lowland_shape_fractal); lowland_autocorrect.setRange(0, 1);
		CImplicitScaleOffset lowland_scale; lowland_scale.setScale(this->LowlandScale); lowland_scale.setOffset(-0.45);
		CImplicitScaleDomain lowland_y_scale; lowland_y_scale.setYScale(0.0);
		CImplicitTranslateDomain lowland_terrain; lowland_terrain.setSource(&ground_gradient); lowland_terrain.setYAxisSource(&lowland_y_scale);

		// Highland terrain
		CImplicitFractal highland_shape_fractal(FBM, this->Highland_Basis_Type, this->Terrain_Fractal_Interpolation, this->HighlandOctaves, this->HighlandFrequency, false);
		CImplicitAutoCorrect highland_autocorrect; highland_autocorrect.setSource(&highland_shape_fractal); highland_autocorrect.setRange(-1, 1);
		CImplicitScaleOffset highland_scale; highland_scale.setScale(this->HighlandScale); highland_scale.setOffset(0.0); highland_scale.setSource(&highland_autocorrect);
		CImplicitScaleDomain highland_y_scale; highland_y_scale.setYScale(0.0); highland_y_scale.setSource(&highland_scale);
		CImplicitTranslateDomain highland_terrain; highland_terrain.setSource(&ground_gradient); highland_terrain.setYAxisSource(&highland_y_scale);

		// Mountain terrain
		CImplicitFractal mountain_shape_fractal(RIDGEDMULTI, this->Mountain_Basis_Type, this->Terrain_Fractal_Interpolation, this->MountainOctaves, this->MountainFrequency, false);
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
		ground_select.setThreshold(0.5);

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
		CImplicitCombiner final_out(MULT); final_out.setSource(0, &cave_select); final_out.setSource(1, &ground_select);

		this->final_output = final_out;
	}
	// Generate chunk of 3D terrain data, with size specified by the first three parameters. Centered at "chunk_pos", given as XZ coordinates.
	CArray3Dd Terrain_Chunk(int x_range, int y_range, int z_range, glm::vec2 chunk_pos = glm::vec2(0, 0)) {
		CArray3Dd terrain(x_range,y_range,z_range);
		map3D(SEAMLESS_NONE, terrain, this->final_output, SMappingRanges());
		return terrain;
	}
	// Writes an image to "filename", centered at "range". I recommend keeping y_range half of x_range, otherwise things get warped.
	CArray2Dd Terrain_Img(int x_range = 1024, int y_range = 512, SMappingRanges range = SMappingRanges(-2,2,-1,1), string filename = "img.png") {
		CArray2Dd img(x_range, y_range);
		map2DNoZ(SEAMLESS_NONE, img, this->final_output, range);
		saveDoubleArray(filename, &img);
	}

	CImplicitCombiner final_output;

	/// Terrain Generation parameters and class attributes follow
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
	double LowlandScale = 0.125;
	double HighlandScale = 0.250;
	double MountainScale = 0.450;

	// This effectively controls the complexity of the primary noise output used to make this terrain type.
	int LowlandOctaves = 2;
	int HighlandOctaves = 4;
	int MountainOctaves = 4;
	int TunnelOctaves = 1; // Octaves for the simple noise used to generate tunnels. Values > 1 may cause strange tunnel behavior.
	int CaveGrainOctaves = 5; // Octaves for the noise used to add grain and detail to the tunnels - makes them more than wormy cylinders.

	// This could best be seen as adjusting the density of terrain features: higher frequency means more of the same noise gets packed into the same area as a lower setting.
	// Scale makes all features "bigger", this just makes "more" of everything.
	double LowlandFrequency = 0.250;
	double HighlandFrequency = 2.0;
	double MountainFrequency = 1.0;
	double CaveFrequency = 4.0;

	// This parameter controls how sharply the caves "widen" into broader cave systems from their start as small tunnels near the surface.
	// Adjust this too sharply can cause bizarre results. It is recommended to just leave it as-is.
	double CaveBias = 0.450;

	
};


#endif