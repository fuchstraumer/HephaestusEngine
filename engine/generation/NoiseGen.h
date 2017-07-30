#pragma once
#ifndef NOISE_GENERATOR_H
#define NOISE_GERNATOR_H

#include "stdafx.h"

#include "util/FastNoise/FastNoise.h"

namespace noise {

	struct NoiseCfg {
		float Frequency = 0.5f, Lacunarity = 2.30f, Persistence = 0.80f;
		size_t Octaves = 12, Seed = 192487;
		FastNoise::NoiseType NoiseType = FastNoise::NoiseType::SimplexFractal;
		FastNoise::FractalType FractalType = FastNoise::FractalType::Billow;
	};

	class NoiseGenerator {
		NoiseGenerator(const NoiseGenerator&) = delete;
		NoiseGenerator& operator=(const NoiseGenerator&) = delete;
	public:

		NoiseGenerator();
		NoiseGenerator(NoiseGenerator&& other) noexcept;
		NoiseGenerator& operator=(NoiseGenerator&& other) noexcept;

		float Sample(const glm::vec3& pos) const;
		float Sample(const size_t& x, const size_t& y, const size_t& z) const;
		float Sample(const double& x, const double& z) const;

		size_t Seed;
		static NoiseCfg NoiseConfig;
	private:

		std::unique_ptr<FastNoise> noiseGen;

	};


}

#endif //!NOISE_GENERATOR_H