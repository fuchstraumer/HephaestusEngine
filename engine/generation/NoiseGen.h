#pragma once
#ifndef NOISE_GENERATOR_H
#define NOISE_GERNATOR_H

#include "stdafx.h"


namespace noise {

	enum class fractalType {
		FBM,
		BILLOW,
		RIDGED
	};

	enum class noiseType {
		VALUE,
		SIMPLEX,
	};

	struct NoiseCfg {
		float Frequency = 0.01f, Lacunarity = 1.6f, Persistence = 0.80f;
		size_t Octaves = 5, Seed = 192487;
		fractalType FractalType = fractalType::FBM;
		noiseType NoiseType = noiseType::VALUE;
	};

	class NoiseGenerator {

	public:

		NoiseGenerator();

		float Sample(const glm::vec3& pos) const;
		float Sample(const size_t& x, const size_t& y, const size_t& z) const;
		float Sample(const double& x, const double& z) const;

		size_t Seed;
		static NoiseCfg NoiseConfig;
	private:
		std::array<uint8_t, 512> perm;
		double simplex(const double& x, const double& y, glm::dvec2* deriv = nullptr) const;
		double valueNoise(const double& x, const double& y) const;
		double fbm(const double& x, const double& y) const;
	};


}

#endif //!NOISE_GENERATOR_H