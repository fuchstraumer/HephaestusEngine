#include "stdafx.h"
#include "NoiseSampler.h"

double noise::NoiseSampler::rawSample(const double & x, const double & z) const {
	return static_cast<double>(noiseGenerator.sdnoise2(x, z, nullptr, nullptr));
}

double noise::NoiseSampler::rawSample(const double & x, const double & y, const double & z) const {
	return static_cast<double>(noiseGenerator.sdnoise3(x, y, z, nullptr));
}

noise::NoiseSampler::NoiseSampler(const double & freq, const size_t & octaves, const double & min, const double & max) : Frequency(freq), Octaves(octaves), Min(min), Max(max) {}

float noise::NoiseSampler::Sample(const glm::vec3 & block_pos) const {
	
	double x = static_cast<double>(block_pos.x) * Frequency;
	double y = static_cast<double>(block_pos.y) * Frequency;
	double z = static_cast<double>(block_pos.z) * Frequency;
	return std::clamp(rawSample(x, y, z), Min, Max);

}

float noise::NoiseSampler::SampleFBM(const glm::vec3 & block_pos) const {
	
	double x = static_cast<double>(block_pos.x) * Frequency;
	double y = static_cast<double>(block_pos.y) * Frequency;
	double z = static_cast<double>(block_pos.z) * Frequency;
	double amplitude = 1.0, sum = 0.0;

	for (size_t i = 0; i < Octaves; ++i) {
		sum += rawSample(x, y, z) * amplitude;
		amplitude *= Persistence;
		x *= Lacunarity;
		y *= Lacunarity;
		z *= Lacunarity;
	}

	return std::clamp(sum, Min, Max);
}

float noise::NoiseSampler::SampleRidged(const glm::vec3 & block_pos) const {
	
	double x = static_cast<double>(block_pos.x) * Frequency;
	double y = static_cast<double>(block_pos.y) * Frequency;
	double z = static_cast<double>(block_pos.z) * Frequency;
	double amplitude = 1.0, sum = 0.0;

	for (size_t i = 0; i < Octaves; ++i) {
		sum += (1.0 - std::abs(rawSample(x, y, z))) * amplitude;
		amplitude *= Persistence;
		x *= Lacunarity;
		y *= Lacunarity;
		z *= Lacunarity;
	}

	return std::clamp(sum, Min, Max);

}

float noise::NoiseSampler::SampleBillow(const glm::vec3 & block_pos) const {
	
	double x = static_cast<double>(block_pos.x) * Frequency;
	double y = static_cast<double>(block_pos.y) * Frequency;
	double z = static_cast<double>(block_pos.z) * Frequency;
	double amplitude = 1.0, sum = 0.0;

	for (size_t i = 0; i < Octaves; ++i) {
		sum += std::abs(rawSample(x, y, z)) * amplitude;
		amplitude *= Persistence;
		x *= Lacunarity;
		y *= Lacunarity;
		z *= Lacunarity;
	}

	return std::clamp(sum, Min, Max);
}

float noise::NoiseSampler::Sample(const glm::vec2 & block_pos) const {
	
	double x = static_cast<double>(block_pos.x) * Frequency;
	double z = static_cast<double>(block_pos.y) * Frequency;
	return std::clamp(rawSample(x, z) * VerticalScale, Min, Max);

}

float noise::NoiseSampler::SampleFBM(const glm::vec2 & block_pos) const {

	double x = static_cast<double>(block_pos.x + 1) * Frequency;
	double z = static_cast<double>(block_pos.y + 1) * Frequency;
	double amplitude = 1.0, sum = 0.0;

	for (size_t i = 0; i < Octaves; ++i) {
		sum += rawSample(x, z) * amplitude;
		amplitude *= Persistence;
		x *= Lacunarity;
		z *= Lacunarity;
	}

	return std::clamp(sum * VerticalScale, Min, Max);
}

float noise::NoiseSampler::SampleRidged(const glm::vec2 & block_pos) const {

	double x = static_cast<double>(block_pos.x) * Frequency;
	double z = static_cast<double>(block_pos.y) * Frequency;
	double amplitude = 1.0, sum = 0.0;

	for (size_t i = 0; i < Octaves; ++i) {
		sum += (1.0 - std::abs(rawSample(x, z))) * amplitude;
		amplitude *= Persistence;
		x *= Lacunarity;
		z *= Lacunarity;
	}

	return std::clamp(sum * VerticalScale, Min, Max);
}

float noise::NoiseSampler::SampleBillow(const glm::vec2 & block_pos) const {
	
	double x = static_cast<double>(block_pos.x) * Frequency;
	double z = static_cast<double>(block_pos.y) * Frequency;
	double amplitude = 1.0, sum = 0.0;

	for (size_t i = 0; i < Octaves; ++i) {
		sum += std::abs(rawSample(x, z)) * amplitude;
		amplitude *= Persistence;
		x *= Lacunarity;
		z *= Lacunarity;
	}

	return std::clamp(sum * VerticalScale, Min, Max);
}
