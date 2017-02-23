#pragma once
#ifndef TERRAIN_COMMON_H
#define TERRAIN_COMMON_H

/*
	
	Terrain Common:

	Used for various constants and methods common to the terrain generation
	system.

*/

/*
		
	Common includes for terrain generation

*/

#include "glm/glm.hpp"
#include <math.h>
#include <random>
#include <algorithm>
#include "common\Constants.h"

// Begin terrain namespace
namespace terrain {

	/*

	Default values for the fractal noise generators that build on the simplex
	baseline generator

	*/

	// Default values for FBM, constexpr = compile-time constant.
	constexpr double FBM_FREQ = 0.0002;
	constexpr int FBM_OCTAVES = 5;
	constexpr float FBM_LACUN = 2.0f;
	constexpr float FBM_GAIN = 1.5f;

	// Default values for Billow
	constexpr double BILLOW_FREQ = 0.0018;
	constexpr int BILLOW_OCTAVES = 4;
	constexpr float BILLOW_LACUN = 2.50f;
	constexpr float BILLOW_GAIN = 1.20f;

	// Default values for Ridged
	constexpr double RIDGED_FREQ = 0.00008;
	constexpr int RIDGED_OCTAVES = 6;
	constexpr float RIDGED_LACUN = 2.50f;
	constexpr float RIDGED_GAIN = 2.20f;

	// Default values for Swiss noise.
	constexpr double SWISS_FREQ = 0.008;
	constexpr double SWISS_OCTAVES = 7;
	constexpr float SWISS_LACUNARITY = 3.0f;
	constexpr float SWISS_GAIN = 1.5f;

	// Default values for Jordan noise.
	constexpr double JORDAN_FREQ = 0.008;
	constexpr int JORDAN_OCTAVES = 7;
	constexpr float JORDAN_LACUNARITY = 3.0f;
	constexpr float JORDAN_GAIN = 1.50f;
	// Variables specific to jordan.
	constexpr float warp0 = 0.4f;
	constexpr float warp1 = 0.35f;
	constexpr float damp0 = 1.0f;
	constexpr float damp1 = 0.8f;

	/*

		Methods used in various generators

	*/

	// Perlin noise easing curve
	static double fade(double f) {
		return f*f*f*(f*(f * 6 - 15) + 10);
	}
	// Derivative of the perlin noise easing curve
	static double fadeDeriv(double f) {
		return f*f*(f*(30 * f - 60) + 30);
	}

	// Basic linear interp
	static double lerp(double a, double b, double x) {
		return a + x * (b - a);
	}

	/*
		Enums
	*/

	// What are the types of noise we can use?
	enum class NoiseType {
		FBM,
		BILLOW,
		RIDGED_MULTI,
		DECARPIENTIER_SWISS,
		JORDAN,
		iQ,
	};

	// TODO: Could really use a voronoi generator.

	// What will the generator be used for?
	enum class GeneratorUse {
		HEIGHT,
		BIOME,
		TEMPERATURE,
		HUMIDITY,
	};

	/*
		The generator is used to make certain procedural features, beyond
		the usual terrain features. These are enumerated here.
	*/

	enum class TerrainFeatures {
		DUNGEON,
		MINE,
		TEMPLE,
		CAVE,
		RAVINE,
	};

}
#endif // !TERRAIN_COMMON_H
