#pragma once
#ifndef OCTREE_H
#define OCTREE_H
#include "stdafx.h"
#include <memory>

inline std::uint64_t splitBy3(unsigned int a) {
	std::uint64_t x = a & 0x1fffff; // Only need first 21 bits
	// Shift left 32 bits, OR with self + other number needed to split.
	x = (x | x << 32) & 0x1f0000000ffff; 
	x = (x | x << 16) & 0x1f0000ff0000ff; 
	x = (x | x <<  8) & 0x100f00f00f00f00f; 
	x = (x | x <<  4) & 0x10c30c30c30c30c3; 
	x = (x | x <<  2) & 0x1249249249249249;
	return x;
}

inline std::uint64_t mortonEncode(uint32_t x, uint32_t y, uint32_t z) {
	uint64_t morton;
	morton |= splitBy3(x) | splitBy3(y) << 1 | splitBy3(z) << 2;
	return morton;
}
using Block = struct _block {
	std::uint8_t Texture;
	// If block data is:
	// 0000000000000000
	// |------| = blocktype
	std::uint64_t MortonCode;
	// Last 3 bits of morton code can give active status
	// 000 - inactive 010 - active 100 - active, physics
	_block(std::uint32_t x, std::uint32_t y, std::uint32_t z) {
		this->MortonCode = mortonEncode(x, y, z);
	}
};


using AABB = struct aabb {
	glm::vec3 Position;
	float Radius;
	aabb() { }
	aabb(glm::vec3 pos, float radius) {
		this->Position = pos;
		this->Radius = radius;
	}
};

const glm::vec3 offsets[8] =
{
	{ -0.5f, -0.5f, -0.5f },
	{ +0.5f, -0.5f, -0.5f },
	{ -0.5f, -0.5f, +0.5f },
	{ +0.5f, -0.5f, +0.5f },
	{ -0.5f, +0.5f, -0.5f },
	{ +0.5f, +0.5f, -0.5f },
	{ -0.5f, +0.5f, +0.5f },
	{ +0.5f, +0.5f, +0.5f }
};


class Node {
public:
	using BlockList = std::vector<std::shared_ptr<Block>>;
private:

};

#endif // !OCTREE_H
