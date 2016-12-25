// stdafx.h 
#pragma once
#include <vector>
#include <iostream>
#include <cmath>
#include <string>
#include <array>
#include <stdint.h>
#include <iterator>

// Import glew for OpenGl pointers and handlers
#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_DLL
#include <GLFW/glfw3.h>
// GLM
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

// Chunk vars
using blockType = uint8_t;
using mortonBlockType = uint32_t;
static const float BLOCK_RENDER_SIZE = 0.5f;
static const int CHUNK_SIZE = 64;
static const int CHUNK_SIZE_Z = 64;
static const bool SIMPLE_CULLING_GLOBAL = true;

// Client settings
static const int MULTISAMPLE_AMOUNT = 2;

// Constant function to convert from spatial coordinates to interval tree vals
#define treeXYZ(x,y,z) ((y) * CHUNK_SIZE * CHUNK_SIZE + (x) * CHUNK_SIZE + (z))
#define treeXZ(x,z) ((x * CHUNK_SIZE) + z)

// Terrain gen
#include "util/TerrainGen.h"

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

