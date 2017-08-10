#include "stdafx.h"
#include "NoiseGen.h"

namespace noise {

	NoiseCfg NoiseGenerator::NoiseConfig = NoiseCfg();

	NoiseGenerator::NoiseGenerator() {
		std::random_device rd;
		std::mt19937 rng(rd());
		rng.seed(NoiseConfig.Seed);
		std::accumulate(perm.begin(), perm.begin() + 256, 0);
		std::accumulate(perm.begin() + 256, perm.end(), 0);
		std::shuffle(perm.begin(), perm.end(), rng);
	}

	float NoiseGenerator::Sample(const double & x, const double & z) const {
		switch (NoiseConfig.FractalType) {
		case fractalType::FBM:
			return fbm(x, z);
		default:
			return 0.0f;
		}
	}

	static inline int fastfloor(double x) {
		return x>0 ? (int)x : (int)x - 1;
	}

	static inline double sGrad(const int& hash, const double& x, const double& y) {
		int h = hash & 7;      // Convert low 3 bits of hash code
		double u = h<4 ? x : y;  // into 8 simple gradient directions,
		double v = h<4 ? y : x;  // and compute the dot product with (x,y).
		return ((h & 1) ? -u : u) + ((h & 2) ? -2.0*v : 2.0*v);
	}

	double NoiseGenerator::simplex(const double & x, const double & y, glm::dvec2 * deriv) const {
		constexpr double F2 = 0.366025403; // F2 = 0.5*(sqrt(3.0)-1.0)
		constexpr double G2 = 0.211324865; // G2 = (3.0-Math.sqrt(3.0))/6.0
		
		double n0, n1, n2; // Noise contributions from the three corners
		double temp0, temp1, temp2;

		// Skew the input space to determine which simplex cell we're in
		double s = (x + y)*F2; // Hairy factor for 2D
		double xs = x + s;
		double ys = y + s;
		int i = fastfloor(xs);
		int j = fastfloor(ys);

		double t = (double)(i + j)*G2;
		double X0 = i - t; // Unskew the cell origin back to (x,y) space
		double Y0 = j - t;
		double x0 = x - X0; // The x,y distances from the cell origin
		double y0 = y - Y0;

		// For the 2D case, the simplex shape is an equilateral triangle.
		// Determine which simplex we are in.
		int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
		if (x0>y0) { i1 = 1; j1 = 0; } // lower triangle, XY order: (0,0)->(1,0)->(1,1)
		else { i1 = 0; j1 = 1; }      // upper triangle, YX order: (0,0)->(0,1)->(1,1)

									  // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
									  // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
									  // c = (3-sqrt(3))/6

		double x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
		double y1 = y0 - j1 + G2;
		double x2 = x0 - 1.0 + 2.0 * G2; // Offsets for last corner in (x,y) unskewed coords
		double y2 = y0 - 1.0 + 2.0 * G2;

		// Wrap the integer indices at 256, to avoid indexing perm[] out of bounds
		int ii = abs(i % 256);
		int jj = abs(j % 256);

		// Calculate the contribution from the three corners
		double t0 = 0.5 - x0*x0 - y0*y0;
		if (t0 < 0.0) n0 = 0.0;
		else {
			t0 *= t0;
			n0 = t0 * t0 * sGrad(perm[ii + perm[jj]], x0, y0);
		}

		double t1 = 0.5 - x1*x1 - y1*y1;
		if (t1 < 0.0) n1 = 0.0;
		else {
			t1 *= t1;
			n1 = t1 * t1 * sGrad(perm[ii + i1 + perm[jj + j1]], x1, y1);
		}

		double t2 = 0.5 - x2*x2 - y2*y2;
		if (t2 < 0.0) n2 = 0.0;
		else {
			t2 *= t2;
			n2 = t2 * t2 * sGrad(perm[ii + 1 + perm[jj + 1]], x2, y2);
		}

		if (deriv != nullptr)
		{
			/*  A straight, unoptimised calculation would be like:
			*    *dnoise_dx = -8.0f * t0 * t0 * x0 * ( gx0 * x0 + gy0 * y0 ) + t0^4 * gx0;
			*    *dnoise_dy = -8.0f * t0 * t0 * y0 * ( gx0 * x0 + gy0 * y0 ) + t0^4 * gy0;
			*    *dnoise_dx += -8.0f * t1 * t1 * x1 * ( gx1 * x1 + gy1 * y1 ) + t1^4 * gx1;
			*    *dnoise_dy += -8.0f * t1 * t1 * y1 * ( gx1 * x1 + gy1 * y1 ) + t1^4 * gy1;
			*    *dnoise_dx += -8.0f * t2 * t2 * x2 * ( gx2 * x2 + gy2 * y2 ) + t2^4 * gx2;
			*    *dnoise_dy += -8.0f * t2 * t2 * y2 * ( gx2 * x2 + gy2 * y2 ) + (t2^4) * gy2;
			*/
			double t40 = t0*t0;
			double t41 = t1*t1;
			double t42 = t2*t2;
			temp0 = t0 * t0 * (x0* x0 + y0 * y0);
			deriv->x = temp0 * x0;
			deriv->y = temp0 * y0;
			temp1 = t1 * t1 * (x1 * x1 + y1 * y1);
			deriv->x += temp1 * x1;
			deriv->y += temp1 * y1;
			temp2 = t2 * t2 * (x2* x2 + y2 * y2);
			deriv->x += temp2 * x2;
			deriv->y += temp2 * y2;
			deriv->x *= -8.0f;
			deriv->y *= -8.0f;
			deriv->x += t40 * x0 + t41 * x1 + t42 * x2;
			deriv->y += t40 * y0 + t41 * y1 + t42 * y2;
			deriv->x *= 40.0f; /* Scale derivative to match the noise scaling */
			deriv->y *= 40.0f;
		}
		// Add contributions from each corner to get the final noise value.
		// The result is scaled to return values in the interval [-1,1].
		return 40.0 * (n0 + n1 + n2); // TODO: The scale factor is preliminary!
	}

	static inline double lerp(const double& a, const double& b, const double& z) {
		double mu2 = (1.0 - std::cos(z * 3.14)) / 2.0;
		return (a * (1.0 - mu2) + b * mu2);
	}

	double NoiseGenerator::valueNoise(const double & x, const double & z) const {
		auto noise_1 = [&](const int& _n) { 
			int n = _n + NoiseConfig.Seed;
			n = (n << 13) ^ n;
			int nn = (n * (n*n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
			return (1.0 - (static_cast<double>(nn) / 1073741824.0));
		};

		double fx = fastfloor(x);
		double fz = fastfloor(z);


		double s, t, u, v;
		s = t = u = v = 0.0;

		s = noise_1(fx + fz * 57);
		t = noise_1(fx + 1 + (fz * 57));
		u = noise_1(fx + ((fz + 1) * 57));
		v = noise_1((fx + 1) + ((fz + 1) * 57));

		double rec1 = lerp(s, t, x - fx);
		double rec2 = lerp(u, v, x - fx);
		return lerp(rec1, rec2, z - fz);
	}

	double NoiseGenerator::fbm(const double & x, const double & y) const {
		double sum = 0;
		double amplitude = 1.0;
		glm::dvec2 f; 
		f.x = x * NoiseConfig.Frequency;
		f.y = y * NoiseConfig.Frequency;
		for (size_t i = 0; i < NoiseConfig.Octaves; ++i) {
			double n = 0.0;
			switch (NoiseConfig.NoiseType) {
			case noiseType::VALUE:
				n = valueNoise(f.x, f.y);
				break;
			case noiseType::SIMPLEX:
				n = simplex(f.x, f.y);
				break;
			}
			sum += n*amplitude;
			f *= NoiseConfig.Lacunarity;
			amplitude *= NoiseConfig.Persistence;
		}
		return sum;
	}


}