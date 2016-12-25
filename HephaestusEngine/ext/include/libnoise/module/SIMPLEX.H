#pragma once
#ifndef NOISE_MODULE_SIMPLEX_H

#define NOISE_MODULE_SIMPLEX_H

#include "modulebase.h"
#include <random>
#include <algorithm>
namespace noise {

	namespace module{

		/// Default frequency for the noise::module::Perlin noise module.
		const double DEFAULT_SIMPLEX_FREQUENCY = 0.9;

		/// Default lacunarity for the noise::module::Perlin noise module.
		const double DEFAULT_SIMPLEX_LACUNARITY = 2.0;

		/// Default number of octaves for the noise::module::Perlin noise module.
		const int DEFAULT_SIMPLEX_OCTAVE_COUNT = 4;

		/// Default persistence value for the noise::module::Perlin noise module.
		const double DEFAULT_SIMPLEX_PERSISTENCE = 0.5;

		/// Default noise quality for the noise::module::Perlin noise module.
		const noise::NoiseQuality DEFAULT_SIMPLEX_QUALITY = QUALITY_STD;

		/// Default noise seed for the noise::module::Perlin noise module.
		const int DEFAULT_SIMPLEX_SEED = 0;

		/// Maximum number of octaves for the noise::module::Perlin noise module.
		const int SIMPLEX_MAX_OCTAVE = 30;


		class Simplex : public Module {
		public:
			
			Simplex();

			/// Returns the frequency of the first octave.
			///
			/// @returns The frequency of the first octave.
			double GetFrequency() const
			{
				return m_frequency;
			}

			/// Returns the lacunarity of the Perlin noise.
			///
			/// @returns The lacunarity of the Perlin noise.
			/// 
			/// The lacunarity is the frequency multiplier between successive
			/// octaves.
			double GetLacunarity() const
			{
				return m_lacunarity;
			}

			/// Returns the quality of the Perlin noise.
			///
			/// @returns The quality of the Perlin noise.
			///
			/// See noise::NoiseQuality for definitions of the various
			/// coherent-noise qualities.
			noise::NoiseQuality GetNoiseQuality() const
			{
				return m_noiseQuality;
			}

			/// Returns the number of octaves that generate the Perlin noise.
			///
			/// @returns The number of octaves that generate the Perlin noise.
			///
			/// The number of octaves controls the amount of detail in the Perlin
			/// noise.
			int GetOctaveCount() const
			{
				return m_octaveCount;
			}

			/// Returns the persistence value of the Perlin noise.
			///
			/// @returns The persistence value of the Perlin noise.
			///
			/// The persistence value controls the roughness of the Perlin noise.
			double GetPersistence() const
			{
				return m_persistence;
			}

			/// Returns the seed value used by the Perlin-noise function.
			///
			/// @returns The seed value.
			int GetSeed() const
			{
				return m_seed;
			}

			virtual int GetSourceModuleCount() const
			{
				return 0;
			}

			virtual double GetValue(double x, double y, double z) const;

			/// Sets the frequency of the first octave.
			///
			/// @param frequency The frequency of the first octave.
			void SetFrequency(double frequency)
			{
				m_frequency = frequency;
			}

			/// Sets the lacunarity of the Perlin noise.
			///
			/// @param lacunarity The lacunarity of the Perlin noise.
			/// 
			/// The lacunarity is the frequency multiplier between successive
			/// octaves.
			///
			/// For best results, set the lacunarity to a number between 1.5 and
			/// 3.5.
			void SetLacunarity(double lacunarity)
			{
				m_lacunarity = lacunarity;
			}

			/// Sets the quality of the Perlin noise.
			///
			/// @param noiseQuality The quality of the Perlin noise.
			///
			/// See noise::NoiseQuality for definitions of the various
			/// coherent-noise qualities.
			void SetNoiseQuality(noise::NoiseQuality noiseQuality)
			{
				m_noiseQuality = noiseQuality;
			}

			/// Sets the number of octaves that generate the Perlin noise.
			///
			/// @param octaveCount The number of octaves that generate the Perlin
			/// noise.
			///
			/// @pre The number of octaves ranges from 1 to
			/// noise::module::PERLIN_MAX_OCTAVE.
			///
			/// @throw noise::ExceptionInvalidParam An invalid parameter was
			/// specified; see the preconditions for more information.
			///
			/// The number of octaves controls the amount of detail in the Perlin
			/// noise.
			///
			/// The larger the number of octaves, the more time required to
			/// calculate the Perlin-noise value.
			void SetOctaveCount(int octaveCount)
			{
				if (octaveCount < 1 || octaveCount > SIMPLEX_MAX_OCTAVE) {
					throw noise::ExceptionInvalidParam();
				}
				m_octaveCount = octaveCount;
			}

			/// Sets the persistence value of the Perlin noise.
			///
			/// @param persistence The persistence value of the Perlin noise.
			///
			/// The persistence value controls the roughness of the Perlin noise.
			///
			/// For best results, set the persistence to a number between 0.0 and
			/// 1.0.
			void SetPersistence(double persistence)
			{
				m_persistence = persistence;
			}

			/// Sets the seed value used by the Perlin-noise function.
			///
			/// @param seed The seed value.
			void SetSeed(int seed)
			{
				m_seed = seed;
			}

		protected:

			/// Frequency of the first octave.
			double m_frequency;

			/// Frequency multiplier between successive octaves.
			double m_lacunarity;

			/// Quality of the Perlin noise.
			noise::NoiseQuality m_noiseQuality;

			/// Total number of octaves that generate the Perlin noise.
			int m_octaveCount;

			/// Persistence of the Perlin noise.
			double m_persistence;

			/// Seed value used by the Perlin-noise function.
			int m_seed;

			// Hash table containing values to be used for gradient vectors
			// Leave as unsigned char - getting this to fit in the cache is optimal
			unsigned char hash[512];

			// Build the hash table for this generator
			void buildHash();

			float simplex_base(float x, float y, float z) const;

			// Random gen
			std::mt19937 RGen;
		};
	}


}

#endif // SIMPLEX_H