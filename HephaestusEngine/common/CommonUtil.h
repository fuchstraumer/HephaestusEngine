#pragma once
#ifndef COMMON_UTIL_H
#define COMMON_UTIL_H
#include "stdafx.h"
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
#endif // !COMMON_UTIL_H
