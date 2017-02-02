// stdafx.h 
#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <array>
#include <future>
#include <algorithm>

// Import glew for OpenGl pointers and handlers
// GLFW uses this, so include this before GLFW
#define GLEW_STATIC
#include <GL/glew.h>


// GLFW, used for Windowing and context handling.
#define GLFW_DLL
#include <GLFW/glfw3.h>

// GLM
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Chunk vars
// Type of blocks - can be safely changed, but shouldn't be.
using blockType = uint8_t;
// Type of blocks for morton encoding. Need at least 32-bit unsigned 
// integers
using mortonBlockType = uint32_t;
static const float BLOCK_RENDER_SIZE = 0.5f;
static const int CHUNK_SIZE = 32;
static const int CHUNK_SIZE_Z = 128;
static const bool SIMPLE_CULLING_GLOBAL = true;

// Block types. Set here so that it can be globally accessed: this is probably unwise, and should be changed 
// to a better solution when one is found.
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

// Converts 3D coordinates into 1D space used for storage in the vector
inline int GetBlockIndex(const glm::vec3& pos) {
	return static_cast<int>(((pos.y) * CHUNK_SIZE * CHUNK_SIZE + (pos.x) * CHUNK_SIZE + (pos.z)));
}
// Same as above, with individual positions
inline int GetBlockIndex(const int& x, const int& y, const int& z) {
	return static_cast<int>((y)* CHUNK_SIZE * CHUNK_SIZE + (x)* CHUNK_SIZE + (z));
}

