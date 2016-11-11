#include "TerrainGen.h"

double TerrainGenerator::simplex(double x, double y, double* dx, double* dy) {
#define F2 0.366025403 // F2 = 0.5*(sqrt(3.0)-1.0)
#define G2 0.211324865 // G2 = (3.0-Math.sqrt(3.0))/6.0

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
	int ii = i & 0xff;
	int jj = j & 0xff;

	// Calculate the contribution from the three corners
	double t0 = 0.5 - x0*x0 - y0*y0;
	if (t0 < 0.0) n0 = 0.0;
	else {
		t0 *= t0;
		n0 = t0 * t0 * sGrad(hashTable[ii + hashTable[jj]], x0, y0);
	}

	double t1 = 0.5 - x1*x1 - y1*y1;
	if (t1 < 0.0) n1 = 0.0;
	else {
		t1 *= t1;
		n1 = t1 * t1 * sGrad(hashTable[ii + i1 + hashTable[jj + j1]], x1, y1);
	}

	double t2 = 0.5 - x2*x2 - y2*y2;
	if (t2 < 0.0) n2 = 0.0;
	else {
		t2 *= t2;
		n2 = t2 * t2 * sGrad(hashTable[ii + 1 + hashTable[jj + 1]], x2, y2);
	}

	if ((nullptr != dx) && (nullptr != dy))
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
		*dx = temp0 * x0;
		*dy = temp0 * y0;
		temp1 = t1 * t1 * (x1 * x1 + y1 * y1);
		*dx += temp1 * x1;
		*dy += temp1 * y1;
		temp2 = t2 * t2 * (x2* x2 + y2 * y2);
		*dx += temp2 * x2;
		*dy += temp2 * y2;
		*dx *= -8.0f;
		*dy *= -8.0f;
		*dx += t40 * x0 + t41 * x1 + t42 * x2;
		*dy += t40 * y0 + t41 * y1 + t42 * y2;
		*dx *= 40.0f; /* Scale derivative to match the noise scaling */
		*dy *= 40.0f;
	}
	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1].
	return 40.0 * (n0 + n1 + n2); // TODO: The scale factor is preliminary!

}

double TerrainGenerator::simplex(double x, double y, double z) {
		// Simple skewing factors for the 3D case
#define F3 0.333333333
#define G3 0.166666667

		double n0, n1, n2, n3; // Noise contributions from the four corners

							   // Skew the input space to determine which simplex cell we're in
		double s = (x + y + z)*F3; // Very nice and simple skew factor for 3D
		double xs = x + s;
		double ys = y + s;
		double zs = z + s;
		int i = fastfloor(xs);
		int j = fastfloor(ys);
		int k = fastfloor(zs);

		double t = (double)(i + j + k)*G3;
		double X0 = i - t; // Unskew the cell origin back to (x,y,z) space
		double Y0 = j - t;
		double Z0 = k - t;
		double x0 = x - X0; // The x,y,z distances from the cell origin
		double y0 = y - Y0;
		double z0 = z - Z0;

		// For the 3D case, the simplex shape is a slightly irregular tetrahedron.
		// Determine which simplex we are in.
		int i1, j1, k1; // Offsets for second corner of simplex in (i,j,k) coords
		int i2, j2, k2; // Offsets for third corner of simplex in (i,j,k) coords

						/* This code would benefit from a backport from the GLSL version! */
		if (x0 >= y0) {
			if (y0 >= z0)
			{
				i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0;
			} // X Y Z order
			else if (x0 >= z0) { i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1; } // X Z Y order
			else { i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1; } // Z X Y order
		}
		else { // x0<y0
			if (y0<z0) { i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1; } // Z Y X order
			else if (x0<z0) { i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1; } // Y Z X order
			else { i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0; } // Y X Z order
		}

		// A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
		// a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
		// a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
		// c = 1/6.

		double x1 = x0 - i1 + G3; // Offsets for second corner in (x,y,z) coords
		double y1 = y0 - j1 + G3;
		double z1 = z0 - k1 + G3;
		double x2 = x0 - i2 + 2.0*G3; // Offsets for third corner in (x,y,z) coords
		double y2 = y0 - j2 + 2.0*G3;
		double z2 = z0 - k2 + 2.0*G3;
		double x3 = x0 - 1.0 + 3.0*G3; // Offsets for last corner in (x,y,z) coords
		double y3 = y0 - 1.0 + 3.0*G3;
		double z3 = z0 - 1.0 + 3.0*G3;

		// Wrap the integer indices at 256, to avoid indexing perm[] out of bounds
		int ii = i & 0xff;
		int jj = j & 0xff;
		int kk = k & 0xff;

		// Calculate the contribution from the four corners
		double t0 = 0.6 - x0*x0 - y0*y0 - z0*z0;
		if (t0 < 0.0) n0 = 0.0;
		else {
			t0 *= t0;
			n0 = t0 * t0 * sGrad(hashTable[ii + hashTable[jj + hashTable[kk]]], x0, y0, z0);
		}

		double t1 = 0.6 - x1*x1 - y1*y1 - z1*z1;
		if (t1 < 0.0) n1 = 0.0;
		else {
			t1 *= t1;
			n1 = t1 * t1 * sGrad(hashTable[ii + i1 + hashTable[jj + j1 + hashTable[kk + k1]]], x1, y1, z1);
		}

		double t2 = 0.6 - x2*x2 - y2*y2 - z2*z2;
		if (t2 < 0.0) n2 = 0.0;
		else {
			t2 *= t2;
			n2 = t2 * t2 * sGrad(hashTable[ii + i2 + hashTable[jj + j2 + hashTable[kk + k2]]], x2, y2, z2);
		}

		double t3 = 0.6 - x3*x3 - y3*y3 - z3*z3;
		if (t3<0.0) n3 = 0.0;
		else {
			t3 *= t3;
			n3 = t3 * t3 * sGrad(hashTable[ii + 1 + hashTable[jj + 1 + hashTable[kk + 1]]], x3, y3, z3);
		}

		// Add contributions from each corner to get the final noise value.
		// The result is scaled to stay just inside [-1,1]
		return 32.0 * (n0 + n1 + n2 + n3); // TODO: The scale factor is preliminary!
}

// This function is just an interface to the private perlin member function.
double TerrainGenerator::point(int x, int y, int z) {
	double temp = perlin(x, y, z);
	if (temp > CHUNK_SIZE_Z)
		temp = CHUNK_SIZE_Z - 1;
	if (temp <= 1)
		temp = 0;
	return temp;
}
// PERLIN NOISE FUNCTIONS FOLLOW
// Parameters:
// - Frequency is the frequency of the noise sampling (multiplier on input coords
// - An octave is an iteration through the noise function, but at smaller levels. It adds finer, grainier detail.
// - "lac" or Lacunarity controls how long an octave persists after the previous octave. Higher levels = more details. Too high is not good.
// - Gain controls the overall gain of the function.
// - Amplitude is set within the function body, in line with what generates the best terrain. The rest of the parameters are also set similarly.

double TerrainGenerator::PerlinFBM(int x, int y, int z, double freq = 0.003, int octaves = 4, float lac = 2.0, float gain = 0.6) {
	double sum = 0;
	float amplitude = 1.0;
	glm::dvec3 f; f.x = x * freq;
	f.y = y * freq; f.z = z * freq;
	for (int i = 0; i < octaves; ++i) {
		double n = perlin(x*freq, y*freq, z*freq);
		sum += n*amplitude;
		freq *= lac;
		amplitude *= gain;
	}
	double temp = (sum / amplitude) + 1;
	//std::cerr << temp << std::endl;
	temp = sqrt(temp*temp);
	if (temp >= CHUNK_SIZE_Z - 4)
		temp = CHUNK_SIZE_Z - 4;
	if (temp <= 1)
		temp = 1;
	return temp;
}

double TerrainGenerator::PerlinBillow(int x, int y, int z, double freq = 0.01, int octaves = 3, float lac = 1.7, float gain = 0.6) {
	double sum = 0;
	float amplitude = 1.0;
	for (int i = 0; i < octaves; ++i) {
		double n = abs(perlin(x*freq, y*freq, z*freq));
		sum += n*amplitude;
		freq *= lac;
		amplitude *= gain;
	}
	double temp = (sum / amplitude) + 1;
	if (temp >= CHUNK_SIZE_Z - 4)
		temp = CHUNK_SIZE_Z - 4;
	if (temp <= 1)
		temp = 1;
	return temp;
}

double TerrainGenerator::RollingHills(int x, int y, int z) {
	return PerlinBillow(x, y, z, 0.009, 3, 1.6f, 0.6f);
}

double TerrainGenerator::PerlinRidged(int x, int y, int z, double freq = 0.02, int octaves = 3, float lac = 2.5, float gain = 0.8) {
	double n = 1.0f - abs(perlin(x*freq, y*freq, z*freq));
	double temp = n * 1.5;
	if (temp >= CHUNK_SIZE_Z - 4)
		temp = CHUNK_SIZE_Z - 4;
	if (temp <= 1)
		temp = 1;
	return temp;
}

// Deprecated
double TerrainGenerator::octPerlin(float x, float y, float z, int octaves = 3, float lacun = 1.0) {
	double total = 0;
	double frequency = 1;
	double amplitude = 1;
	double maxValue = 0;  // Used for normalizing result to 0.0 - 1.0
	for (int i = 0; i<octaves; i++) {
		total += perlin(x * frequency, y * frequency, z * frequency) * amplitude;

		maxValue += amplitude;

		amplitude *= lacun;
		frequency *= 2;
	}
	return total / maxValue;
}

// SIMPLEX NOISE FUNCTIONS FOLLOW
// SOURCE FROM http://staffwww.itn.liu.se/~stegu/aqsis/aqsis-newnoise/



double TerrainGenerator::SimplexFBM(int x, int y, double freq, int octaves, float lac, float gain) {
	double sum = 0;
	float amplitude = 1.0;
	glm::dvec2 f; f.x = x * freq;
	f.y = y * freq;
	for (int i = 0; i < octaves; ++i) {
		double n = simplex(f.x, f.y, nullptr, nullptr);
		sum += n*amplitude;
		freq *= lac;
		amplitude *= gain;
	}
	double temp = (sum / amplitude) + 2;
	//std::cerr << temp << std::endl;
	temp = 32 * temp;
	if (temp >= CHUNK_SIZE_Z - 4)
		temp = CHUNK_SIZE_Z - 4;
	if (temp <= 1)
		temp = 1;
	return temp;
}

double TerrainGenerator::SimplexBillow(int x, int y, double freq, int octaves, float lac, float gain) {
	double sum = 0;
	float amplitude = 1.0;
	glm::dvec2 f;
	f.x = x * freq; f.y = y * freq;
	for (int i = 0; i < octaves; ++i) {
		double n = abs(simplex(f.x, f.y, nullptr, nullptr));
		sum += n*amplitude;
		freq *= lac;
		amplitude *= gain;
	}
	double temp = (sum / amplitude) + 1;
	//std::cerr << temp << std::endl;
	temp = 32 * temp;
	if (temp >= CHUNK_SIZE_Z - 4)
		temp = CHUNK_SIZE_Z - 4;
	if (temp <= 1)
		temp = 1;
	return temp;
}

double TerrainGenerator::SimplexRidged(int x, int y, double freq, int octaves,
	float lac, float gain) {
	double sum = 0;
	float amplitude = 1.0f;
	glm::dvec2 f;
	f.x = x * freq;
	f.y = y * freq;
	for (int i = 0; i < octaves; ++i) {
		double n = 1.0f - abs(simplex(x*freq, y*freq, nullptr, nullptr));
		sum += n*amplitude;
		freq *= lac;
		amplitude *= gain;
	}
	double temp = (sum / amplitude);
	temp = 64 * temp;
	if (temp >= CHUNK_SIZE_Z - 4)
		temp = CHUNK_SIZE_Z - 4;
	if (temp <= 1)
		temp = 1;
	return temp;
}

inline double TerrainGenerator::SimplexCaves(int x, int y) {
	return SimplexRidged(x, y, 0.0001, 5, 2.5, 1);
}


double TerrainGenerator::SimplexSwiss(int x, int y, double freq, int octaves,
	float lac, float gain) {
	glm::dvec2 f; f.x = x * freq; f.y = y * freq;
	auto sum = 0.0; auto amplitude = 1.0;
	glm::dvec2 derivSum(0.0, 0.0);
	auto warp = 0.15;
	for (int i = 0; i < octaves; ++i) {
		double *dx = new double;
		double *dy = new double;
		auto n = 1.0 - abs(simplex(f.x + warp*derivSum.x, f.y + warp*derivSum.y, dx, dy));
		sum += amplitude * n;
		derivSum += glm::dvec2(*dx*amplitude*(-n), *dy*amplitude*(-n));
		freq *= lac;
		amplitude *= (gain * glm::clamp(sum, 0.0, 1.0));
		delete dx, dy;
	}
	auto temp = sum / amplitude;
	temp = temp * 16;
	if (temp >= CHUNK_SIZE_Z - 4) {
		temp = CHUNK_SIZE_Z - 4;
	}
	if (temp <= 1) {
		temp = 1;
	}
	return temp;
}


// Jordan-style terrain
static auto JORDAN_FREQ = 0.008; static auto JORDAN_OCTAVES = 7; // This is a slow terrain function because of high octaves
static auto JORDAN_LACUNARITY = 3.0; static auto JORDAN_GAIN = 1.5;
static auto warp0 = 0.4, warp1 = 0.35;
static auto damp0 = 1.0, damp1 = 0.8;

auto TerrainGenerator::SimplexJordan(int x, int y, double freq, int octaves, float lac, float gain) {
	double* dx1 = new double; double* dy1 = new double;
	glm::dvec2 f; f.x = x * freq; f.y = y * freq;
	auto n = simplex(f.x, f.y, dx1, dy1);
	glm::dvec3 nSq = glm::dvec3(n*n, *dx1*n, *dy1*n);
	glm::dvec2 dWarp = glm::dvec2(*dx1*warp0, *dy1*warp0);
	glm::dvec2 dDamp = glm::dvec2(*dx1*damp0, *dy1*warp1);
}