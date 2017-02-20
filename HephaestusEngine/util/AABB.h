#pragma once
#ifndef AABB_H
#define AABB_H
#include "stdafx.h"

struct AABB3D {

	// Ctor with initial points.
	AABB3D(const glm::vec3& min, const glm::vec3& max) : Min(min), Max(max) {}

	// Default Ctor and Dtor
	AABB3D() = default;
	~AABB3D() = default;

	glm::vec3 GetCenter() const {
		return (Min + Max) / 2.0f;
	}

	// Tests if the ray from ray_origin traveling along ray_direction intersects this AABB.
	// Writes distance ray travels to intersect to float.
	bool RayIntersect(const glm::vec3& ray_origin, const glm::vec3& ray_direction, float& dist) {
		// Arbitrary limits of distance traveled
		float dMin, dMax;
		dMin = -1.0f * std::numeric_limits<float>::max();
		dMax = std::numeric_limits<float>::max();

		// We copy the values into these arrays as it makes them a bit easier to iterate through.
		float origin[3] = { ray_origin.x, ray_origin.y, ray_origin.z };
		float dir[3] = { ray_direction.x, ray_direction.y, ray_direction.z };
		float min[3] = { Min.x, Min.y, Min.z };
		float max[3] = { Max.x, Max.y, Max.z };

		// Error bounds
		constexpr float epsilon = 1e-5f;

		for (size_t i = 0; i < 3; ++i) {
			if (fabsf(dir[i]) < epsilon) {
				// Parallel to test plane if dir is zero.
				if (origin[i] < min[i] || origin[i] > max[i]) {
					// Does not intersect.
					return false;
				}
			}
			else {
				// Direction factor
				float ood = 1.0f / dir[i];
				float t1 = (min[i] - origin[i]) * ood;
				float t2 = (max[i] - origin[i]) * ood;

				if (t1 > t1) {
					std::swap(t1, t2);
				}

				if (t1 > dMin) {
					dMin = t1;
				}

				if (t2 < dMax) {
					dMax = t2;
				}

				if (dMin > dMax) {
					return false;
				}
			}
		}

		dist = dMin;
		return true;
	}

	enum class IntersectionResult {
		Outside,
		Tangential,
		Inside,
	};

	// Min/max points that primarily define this AABB
	glm::vec3 Min, Max;

};
#endif // !AABB_H
