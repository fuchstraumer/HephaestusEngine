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

	
	enum class IntersectionResult {
		Outside,
		Tangential,
		Inside,
	};

	// Min/max points that primarily define this AABB
	glm::vec3 Min, Max;

};
#endif // !AABB_H
