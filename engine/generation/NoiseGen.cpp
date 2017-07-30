#include "stdafx.h"
#include "NoiseGen.h"

namespace noise {

	NoiseCfg NoiseGenerator::NoiseConfig = NoiseCfg();

	NoiseGenerator::NoiseGenerator() {
		noiseGen = std::make_unique<FastNoise>(NoiseConfig.Seed);
		noiseGen->SetFrequency(NoiseConfig.Frequency);
		noiseGen->SetFractalGain(NoiseConfig.Persistence);
		noiseGen->SetFractalLacunarity(NoiseConfig.Lacunarity);
		noiseGen->SetFractalOctaves(NoiseConfig.Octaves);
		noiseGen->SetFractalType(NoiseConfig.FractalType);
		noiseGen->SetNoiseType(NoiseConfig.NoiseType);
	}

	NoiseGenerator::NoiseGenerator(NoiseGenerator && other) noexcept : noiseGen(std::move(other.noiseGen)) {}

	NoiseGenerator & NoiseGenerator::operator=(NoiseGenerator && other) noexcept {
		noiseGen = std::move(other.noiseGen);
		return *this;
	}

	float NoiseGenerator::Sample(const glm::vec3 & pos) const {
		return noiseGen->GetNoise(pos.x, pos.y, pos.z);
	}

	float NoiseGenerator::Sample(const size_t & x, const size_t & y, const size_t & z) const {
		return noiseGen->GetNoise(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
	}

	float NoiseGenerator::Sample(const double & x, const double & z) const {
		return noiseGen->GetNoise(x, z);
	}

}