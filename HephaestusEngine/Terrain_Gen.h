#ifndef TERRAIN_GEN_H
#define TERRAIN_GEN_H

#include "stdafx.h"
#include "glm/glm.hpp"
#include <Noise\anl_noise.h>
#include <Imaging\anl_imaging.h>
using namespace anl;
using namespace std;
/*
Idea:
1. First pass. Generate base terrain rest sits on. Boolean array. If true, block is stone.
2. Cave generator and ore placement. Boolean array for caves. If true, do something like remove stone or keep stone depending on output.
3. Ore placement - not boolean array. Array of ints clamped to a range. Weighted for higher values at lower z. Lower ints represent low-tier ores. Higher ints represent
higher tiers.
4. Terrain differentiation. For now, just apply a layer of grass and dirt over stone base. more later.
*/

CArray3Dd generate_terrain(int x_range, int y_range, int z_range, glm::vec2 chunk_pos)
{

	float x0, x1, z0, z1;
	x0 = chunk_pos.x - 1; x1 = chunk_pos.x + 1;
	z0 = chunk_pos.y - 1; z1 = chunk_pos.y + 1;

	SMappingRanges range(x0, x1, -2, 2, z0, z1);
	static CArray3Dd terrain(x_range, y_range, z_range);

	// Base gradient defining range of terrain
	CImplicitGradient ground_gradient; ground_gradient.setGradient(0.0, 0.0, 0.0, 1.0);
	// Lowland generator
	CImplicitFractal lowland_shape_fractal(BILLOW, GRADIENT, QUINTIC, 2, 0.25, false);
	CImplicitAutoCorrect lowland_autocorrect; lowland_autocorrect.setSource(&lowland_shape_fractal); lowland_autocorrect.setRange(0, 1);
	CImplicitScaleOffset lowland_scale; lowland_scale.setScale(0.125); lowland_scale.setOffset(-0.45);
	CImplicitScaleDomain lowland_y_scale; lowland_y_scale.setYScale(0.0);
	CImplicitTranslateDomain lowland_terrain; lowland_terrain.setSource(&ground_gradient); lowland_terrain.setYAxisSource(&lowland_y_scale);

	// Highland terrain
	CImplicitFractal highland_shape_fractal(FBM, GRADIENT, QUINTIC, 4, 2, false);
	CImplicitAutoCorrect highland_autocorrect; highland_autocorrect.setSource(&highland_shape_fractal); highland_autocorrect.setRange(-1, 1);
	CImplicitScaleOffset highland_scale; highland_scale.setScale(0.25); highland_scale.setOffset(0.0); highland_scale.setSource(&highland_autocorrect);
	CImplicitScaleDomain highland_y_scale; highland_y_scale.setYScale(0.0); highland_y_scale.setSource(&highland_scale);
	CImplicitTranslateDomain highland_terrain; highland_terrain.setSource(&ground_gradient); highland_terrain.setYAxisSource(&highland_y_scale);

	// Mountain terrain
	CImplicitFractal mountain_shape_fractal(RIDGEDMULTI, GRADIENT, QUINTIC, 4, 1, false);
	CImplicitAutoCorrect mountain_autocorrect; mountain_autocorrect.setSource(&mountain_shape_fractal); mountain_autocorrect.setRange(-1, 1);
	CImplicitScaleOffset mountain_scale; mountain_scale.setScale(0.45); mountain_scale.setOffset(0.15);
	CImplicitScaleDomain mountain_y_scale; mountain_y_scale.setSource(&mountain_scale); mountain_y_scale.setYScale(0.25);
	CImplicitTranslateDomain mountain_terrain; mountain_terrain.setSource(&ground_gradient); mountain_terrain.setYAxisSource(&mountain_y_scale);

	// Terrain type selection functions + cache function
	CImplicitFractal terrain_type_fractal(FBM,GRADIENT,QUINTIC,3,0.2,false); 
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
	CImplicitMath cave_attenuate_bias(BIAS,&highland_lowland_cache,0.45); // This is set well. This changes how sharply the caves widen under the first layer of terrain
	CImplicitFractal cave_shape_1(RIDGEDMULTI, GRADIENT, QUINTIC, 1, 4, false); CImplicitFractal cave_shape_2(RIDGEDMULTI, GRADIENT, QUINTIC, 1, 4, false);
	CImplicitCombiner cave_shape_attenuate(MULT); cave_shape_attenuate.setSource(0, &cave_shape_1); 
	cave_shape_attenuate.setSource(1, &cave_attenuate_bias); cave_shape_attenuate.setSource(2, &cave_shape_2);
	CImplicitFractal cave_perturb_fractal(FBM, GRADIENT, QUINTIC, 5, 3, false); CImplicitScaleOffset cave_perturb_scale;
	cave_perturb_scale.setScale(0.5); cave_perturb_scale.setSource(&cave_perturb_fractal); 
	CImplicitTranslateDomain cave_perturb; cave_perturb.setXAxisSource(&cave_perturb_scale); cave_perturb.setSource(&cave_shape_attenuate);
	CImplicitSelect cave_select; cave_select.setControlSource(&cave_perturb); cave_select.setLowSource(1.0); cave_select.setHighSource(0.0); cave_select.setThreshold(0.48);
	cave_select.setFalloff(0.0);
	// Final output
	CImplicitCombiner final_output(MULT); final_output.setSource(0, &cave_select); final_output.setSource(1, &ground_select);

	map3D(SEAMLESS_NONE, terrain, final_output, range);

	return terrain;

}

#endif