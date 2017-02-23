#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H
#include "stdafx.h"

// Chunk size constants. 32x32 in XZ, and 128 in Y.
static constexpr int CHUNK_SIZE = 32;
static constexpr int CHUNK_SIZE_Y = 128;

// Just pi lol
static constexpr float pi = 3.14159265359f;

// Light has a range from 0-15. 15 is absolute highest, equivalent to sunlight.
// 14 is highest we allow for all non-sunlight sources.
static constexpr int SUNLIGHT_LEVEL = 15;
static constexpr int MAX_LIGHT_INTENSITY = 14;

// Used to set width/height of screen rendered. Should eventually be a config file value.
static constexpr GLuint WIDTH = 1440, HEIGHT = 900;

// Alias declarations for various constants.

// Base block data type - 
using BlockType = uint8_t;

// Block types. Set here so that it can be globally accessed: this is probably unwise, and should be changed 
// to a better solution when one is found.
// Note that it automatically inherits a type from blockType. Usually a uint8, so maximum variety is 255 blocks
// (or 127 blocks if we use RLE, but that's another problem)
enum BlockTypes : BlockType {
	EMPTY = -1,
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

// Texture files to import
// TODO: That ^

#endif // !CONSTANTS_H
