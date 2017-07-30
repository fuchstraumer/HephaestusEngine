#pragma once
#ifndef NOISE_SAMPLER_H
#define NOISE_SAMPLER_H

#include "stdafx.h"
#include "NoiseGen.h"

namespace noise {

	struct NoiseSampler {

		NoiseSampler() = default;
		NoiseSampler(const glm::vec3& origin, const double& min = 0.0f, const double& max = 128.0f);
		NoiseSampler(NoiseSampler&& other) noexcept;
		
		NoiseSampler& operator=(NoiseSampler&& other) noexcept;

		float Sample(const glm::vec3& block_pos) const;
		float SampleFBM(const glm::vec3& block_pos) const;
		float SampleRidged(const glm::vec3& block_pos) const;
		float SampleBillow(const glm::vec3& block_pos) const;

		float Sample(const glm::vec2& block_pos) const;
		float SampleFBM(const glm::vec2& block_pos) const;
		float SampleRidged(const glm::vec2& block_pos) const;
		float SampleBillow(const glm::vec2 & block_pos) const;

		double Frequency = 0.10;
		size_t Octaves = 12;
		double Min = 0.0, Max = 127.0;
		double VerticalScale = 1.0;
		double Lacunarity = 2.20, Persistence = 0.70;

	private:
		glm::vec3 origin;
		NoiseSampler(const NoiseSampler&) = delete;
		NoiseSampler& operator=(const NoiseSampler&) = delete;
		NoiseGenerator noiseGenerator;
		double rawSample(const double& x, const double& z) const;
		double rawSample(const double& x, const double& y, const double& z) const;
	};

}

#endif // !NOISE_SAMPLER_H
