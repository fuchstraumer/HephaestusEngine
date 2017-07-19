#pragma once
#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#include "stdafx.h"
#include "NoiseSampler.h"

namespace terrain {

	class TerrainGenerator {
	public:

		TerrainGenerator();

	private:
		
		noise::NoiseSampler HeightBase, HeightScale, SoilDepth, GrassDepth,
			CaveStart, CaveEnd, CaveWalkVariance;
	};

}
#endif // !TERRAIN_GENERATOR_H
