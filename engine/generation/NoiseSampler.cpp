#include "stdafx.h"
#include "NoiseSampler.h"

noise::NoiseSampler::NoiseSampler(const glm::vec3& _origin, const double & min, const double & max) : origin(_origin) {}

noise::NoiseSampler::NoiseSampler(NoiseSampler && other) noexcept : noiseGenerator(std::move(other.noiseGenerator)), Min(std::move(other.Min)), Max(std::move(other.Max)), VerticalScale(std::move(other.VerticalScale)), origin(std::move(other.origin)),
	Frequency(std::move(other.Frequency)), Octaves(std::move(other.Octaves)), Lacunarity(std::move(other.Lacunarity)), Persistence(std::move(other.Persistence)) {}

noise::NoiseSampler & noise::NoiseSampler::operator=(NoiseSampler && other) noexcept {
	noiseGenerator = std::move(other.noiseGenerator);
	Min = std::move(other.Min);
	Max = std::move(other.Max);
	Frequency = std::move(other.Frequency);
	Octaves = std::move(other.Octaves);
	Lacunarity = std::move(other.Lacunarity);
	Persistence = std::move(other.Persistence);
	VerticalScale = std::move(other.VerticalScale);
	origin = std::move(other.origin);
	return *this;
}

double noise::NoiseSampler::rawSample(const double & x, const double & z) const {
	return static_cast<double>(noiseGenerator.Sample(x, z));
}

double noise::NoiseSampler::rawSample(const double & x, const double & y, const double & z) const {
	return static_cast<double>(noiseGenerator.Sample(x, y, z));
}

float noise::NoiseSampler::Sample(const glm::vec3 & block_pos) const {
	
	double x = static_cast<double>(block_pos.x) * Frequency;
	x += origin.x;
	double y = static_cast<double>(block_pos.y) * Frequency;
	y += origin.y;
	double z = static_cast<double>(block_pos.z) * Frequency;
	z += origin.z;
	return std::clamp(rawSample(x, y, z), Min, Max);

}

float noise::NoiseSampler::SampleFBM(const glm::vec3 & block_pos) const {
	
	double x = static_cast<double>(block_pos.x) * Frequency;
	x += origin.x;
	double y = static_cast<double>(block_pos.y) * Frequency;
	y += origin.y;
	double z = static_cast<double>(block_pos.z) * Frequency;
	z += origin.z;
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
	x += origin.x;
	double y = static_cast<double>(block_pos.y) * Frequency;
	y += origin.y;
	double z = static_cast<double>(block_pos.z) * Frequency;
	z += origin.z;
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
	x += origin.x;
	double y = static_cast<double>(block_pos.y) * Frequency;
	y += origin.y;
	double z = static_cast<double>(block_pos.z) * Frequency;
	z += origin.z;
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
	
	double x = static_cast<double>(block_pos.x);
	x += origin.x;
	double z = static_cast<double>(block_pos.y);
	z += origin.z;
	return std::clamp(rawSample(x, z) * VerticalScale, Min, Max);

}

float noise::NoiseSampler::SampleFBM(const glm::vec2 & block_pos) const {

	double x = static_cast<double>(block_pos.x) * Frequency;
	x += origin.x;
	double z = static_cast<double>(block_pos.y) * Frequency;
	z += origin.z;
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
	x += origin.x;
	double z = static_cast<double>(block_pos.y) * Frequency;
	z += origin.z;
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
	
	double x = (static_cast<double>(block_pos.x) + origin.x) * Frequency;
	double z = (static_cast<double>(block_pos.y) + origin.z) * Frequency;
	double amplitude = 1.0, sum = 0.0;

	for (size_t i = 0; i < Octaves; ++i) {
		sum += std::abs(rawSample(x, z)) * amplitude;
		amplitude *= Persistence;
		x *= Lacunarity;
		z *= Lacunarity;
	}

	return std::clamp(sum * VerticalScale, Min, Max);
}
