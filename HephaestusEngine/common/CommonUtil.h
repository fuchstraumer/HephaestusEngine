#pragma once
#ifndef COMMON_UTIL_H
#define COMMON_UTIL_H
#include "stdafx.h"
#include "Constants.h"
/*

	Various common utility methods and using declarations
	that don't belong with any single file/class/whatever

	Attempting to move these out of the stdafx.h file, 
	so that I don't have so many issues moving to 
	cmakelists.txt down the road.

*/

// Hashing object for ivec3's
// Callable hashing object for an ivec3
struct ivec3Hash {
	size_t operator()(const glm::ivec3& vec) const {
		// Starting size/seed of the input vector, 3 in this case
		size_t seed = 3;
		// throw vector members into an initializer list so we can
		// iterate really easily
		auto nums = { vec.x, vec.y, vec.z };
		// Build the hash
		for (auto i : nums) {
			seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}

};

using BlockUnorderedMap = std::unordered_map<glm::ivec3, uint8_t, ivec3Hash>;
using BlockUnorderedSet = std::unordered_set<glm::ivec3, ivec3Hash>;

/*

These used to be macro's, but macros are bad for a number of reasons. A number of different
methods and classes need this, so I'm leaving them here in stdafx.h for now.

Takes a 3D cartesian coordinate and converts it into the 1D coordinate space of our block
storage containers.

*/

// Converts 3D coordinates into 1D space used for storage in the vector
inline int GetBlockIndex(const glm::vec3 pos) {
	return static_cast<int>(((pos.y) * CHUNK_SIZE * CHUNK_SIZE + (pos.x) * CHUNK_SIZE + (pos.z)));
}

// Same as above, with individual positions
inline int GetBlockIndex(const int x, const int y, const int z) {
	return static_cast<int>((y)* CHUNK_SIZE * CHUNK_SIZE + (x)* CHUNK_SIZE + (z));
}

inline glm::vec3 GetBlockPos(const size_t& idx) {
	return glm::vec3(idx % CHUNK_SIZE, idx / (CHUNK_SIZE * CHUNK_SIZE), (idx % (CHUNK_SIZE * CHUNK_SIZE)) / CHUNK_SIZE);
}

// Used for grabbing front 4 bits of a given uint8_t
inline int GetFront4(const uint8_t val) {
	return (val >> 4) & 0xF;
}

// Used for grabbing back 4 bits of a given uint8_t
inline int GetBack4(const uint8_t val) {
	return (val & 0xF);
}

// Used for setting front 4 bits of given uint8_t to value int (doesn't check to make sure within range)
inline int SetFront4(uint8_t dest, int val) {
	dest = (dest & 0xF) | (val << 4);
}

// Used for setting back 4 bits of given uint8_t
inline int SetBack4(uint8_t dest, int val) {
	dest = (dest & 0xF0) | val;
}

#endif // !COMMON_UTIL_H
