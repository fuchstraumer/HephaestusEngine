#pragma once
#ifndef NOISE_SAMPLER_H
#define NOISE_SAMPLER_H

#include "stdafx.h"
#include "sdnoise1234.h"

namespace noise {

	struct NoiseSampler {

		NoiseSampler(const double& freq = 0.05f, const size_t& octaves = 12, const double& min = 0.0f, const double& max = 128.0f);

		float Sample(const glm::vec3& block_pos) const;
		float SampleFBM(const glm::vec3& block_pos) const;
		float SampleRidged(const glm::vec3& block_pos) const;
		float SampleBillow(const glm::vec3& block_pos) const;

		float Sample(const glm::vec2& block_pos) const;
		float SampleFBM(const glm::vec2& block_pos) const;
		float SampleRidged(const glm::vec2& block_pos) const;
		float SampleBillow(const glm::vec2 & block_pos) const;

		double Frequency;
		size_t Octaves;
		double Min, Max;
		double VerticalScale = 1.0;
		double Lacunarity = 2.20, Persistence = 0.70;

	private:
		NoiseGen noiseGenerator;
		double rawSample(const double& x, const double& z) const;
		double rawSample(const double& x, const double& y, const double& z) const;
	};

}

#endif // !NOISE_SAMPLER_H
