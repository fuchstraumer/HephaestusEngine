// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#pragma warning (disable:4244)
#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <string>
#define GLFW_DLL

// Import glew for OpenGl pointers and handlers
#define GLEW_STATIC
#include <GL/glew.h>

// GLM
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#pragma warning(disable:4244)

// Chunk vars
static const float BLOCK_RENDER_SIZE = 0.5f;
static const int CHUNK_SIZE = 32;
static const int CHUNK_SIZE_Z = 128;
static const bool SIMPLE_CULLING_GLOBAL = true;
static const bool WIREFRAME_MODE = false;
typedef unsigned int uint;

// Constant function to convert from spatial coordinates to interval tree vals
#define treeXYZ(x,y,z) ((y) * CHUNK_SIZE * CHUNK_SIZE + (x) * CHUNK_SIZE + (z))
#define treeXZ(x,z) ((x * CHUNK_SIZE) + z)

// Terrain seed
#include "util/Terrain_Gen.h"


// Platform issues

// Block enums
// Enum specifying the sort of blocks we can have in our world
static enum blockType {
	AIR = -1,
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

// TODO: reference additional headers your program requires here
