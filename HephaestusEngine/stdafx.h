// stdafx.h - precompiled header to increase compiliation speed.
// Especially helpful with highly templated libraries, and libraries used nearly everywhere.
#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <array>
#include <future>
#include <algorithm>
#include <ctime>
#include <bitset>
#include <unordered_map>

// Import glew for OpenGl pointers and handlers
// GLFW uses this, so include this before GLFW
// Define GLEW_STATIC because we're using the static library.
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW, used for Windowing and context handling.
// TODO: Move to GLFW static, including the DLL kinda sucks
#define GLFW_DLL
#include <GLFW/glfw3.h>

// GLM
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Chunk vars

// Type of blocks - can be safely changed, but shouldn't be. Expands size of data everywhere and doesn't offer any advantages currently.
using blockType = uint8_t;

// Type of blocks for morton encoding. Need at least 32-bit unsigned ints.
using mortonBlockType = uint32_t;

// Sets XZ dimensions of chunk sizes
static constexpr int CHUNK_SIZE = 32;

// Sets Y dimensions - or height - of chunk sizes.
static constexpr int CHUNK_SIZE_Y = 128;

// Whether or not ot use the cheap/simple occlusion-based culling method when building the meshes.
static constexpr bool SIMPLE_CULLING_GLOBAL = true;

// Block types. Set here so that it can be globally accessed: this is probably unwise, and should be changed 
// to a better solution when one is found.
// Note that it automatically inherits a type from blockType. Usually a uint8, so maximum variety is 255 blocks
// (or 127 blocks if we use RLE, but that's another problem)
enum blockTypes : blockType {
	AIR = 0,
	GRASS,
	SAND,
	DIRT,
	STONE,
	BEDROCK,
	TALL_GRASS,
	COAL_ORE,
	IRON_ORE,
	DIAMOND_ORE,
	BRICK,
	WOOD,
	CEMENT,
	PLANK,
	SNOW,
	GLASS,
	COBBLE,
	LIGHT_STONE,
	DARK_STONE,
	CHEST,
	LEAVES,
	YELLOW_FLOWER,
	RED_FLOWER,
	PURPLE_FLOWER,
	SUN_FLOWER,
	WHITE_FLOWER,
	BLUE_FLOWER,
	COLOR_00,
	COLOR_01,
	COLOR_02,
	COLOR_03,
	COLOR_04,
	COLOR_05,
	COLOR_06,
	COLOR_07,
	COLOR_08,
	COLOR_09,
	COLOR_10,
	COLOR_11,
	COLOR_12,
	COLOR_13,
	COLOR_14,
	COLOR_15,
	COLOR_16,
	COLOR_17,
	COLOR_18,
	COLOR_19,
	COLOR_20,
	COLOR_21,
	COLOR_22,
	COLOR_23,
	COLOR_24,
	COLOR_25,
	COLOR_26,
	COLOR_27,
	COLOR_28,
	COLOR_29,
	COLOR_30,
	COLOR_31,
};



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

