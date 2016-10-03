#ifndef BLOCK_H

#define BLOCK_H

/*	
	Type: Class. Object: Block. 
	Data: enum blockType, vector blockVerts, vector blockFaces, bool blockActive
	Methods: bool isActive, void setActive,blockType getType,void setType,void setupMesh, vector<blockVerts,blockFaces> getMesh

	This object represents the base block in the voxel engine. Its main attributes are "blockActive" and "blockType". blockActive states
	whether or not a block should exist in the overall world. blockType specifies the block's type and is used to fetch/set its rendering data.

	blockVerts and blockFaces are related to rendering. blockVerts is a vector containing sub-vectors (glm::vec3) for the position coordinates and
	the normal vectors. blockFaces is a vector of glm::vec3's acting as indices into the blockVerts vector. Each glm::vec3 contains the 3 vertices
	needed to draw a triangle, and each row contains two vec3s specifying one whole face. blockFaces is also ordered by face as such:
	face = vector<glm::vec3> blockFaces = vector<face>
	blockFaces = ( leftFace, rightFace, topFace, bottomFace, frontFace, backFace )

	Keeping track of these indices and what face is where is important for later functions used to optimize rendering, primarily by not rendering
	obscured triangles.
*/

#include "stdafx.h"

// Enum specifying the sort of blocks we can have in our world
enum blockType {
	blockType_Default,
	GRASS,
	SAND,
	STONE,
	BRICK,
	WOOD,
	CEMENT,
	DIRT,
	PLANK,
	SNOW,
	GLASS,
	COBBLE,
	LIGHT_STONE,
	DARK_STONE,
	CHEST,
	LEAVES,
	TALL_GRASS,
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
	COLOR_31
};
// Array holding indices into the texture atlas of texture.png
const int blocks[256][6] = {
	// w => (left, right, top, bottom, front, back) tiles
	{ 0, 0, 0, 0, 0, 0 }, // 0 - empty
	{ 16, 16, 32, 0, 16, 16 }, // 1 - grass
	{ 1, 1, 1, 1, 1, 1 }, // 2 - sand
	{ 2, 2, 2, 2, 2, 2 }, // 3 - stone
	{ 3, 3, 3, 3, 3, 3 }, // 4 - brick
	{ 20, 20, 36, 4, 20, 20 }, // 5 - wood
	{ 5, 5, 5, 5, 5, 5 }, // 6 - cement
	{ 6, 6, 6, 6, 6, 6 }, // 7 - dirt
	{ 7, 7, 7, 7, 7, 7 }, // 8 - plank
	{ 24, 24, 40, 8, 24, 24 }, // 9 - snow
	{ 9, 9, 9, 9, 9, 9 }, // 10 - glass
	{ 10, 10, 10, 10, 10, 10 }, // 11 - cobble
	{ 11, 11, 11, 11, 11, 11 }, // 12 - light stone
	{ 12, 12, 12, 12, 12, 12 }, // 13 - dark stone
	{ 13, 13, 13, 13, 13, 13 }, // 14 - chest
	{ 14, 14, 14, 14, 14, 14 }, // 15 - leaves
	{ 15, 15, 15, 15, 15, 15 }, // 16 - cloud
	{ 0, 0, 0, 0, 0, 0 }, // 17
	{ 0, 0, 0, 0, 0, 0 }, // 18
	{ 0, 0, 0, 0, 0, 0 }, // 19
	{ 0, 0, 0, 0, 0, 0 }, // 20
	{ 0, 0, 0, 0, 0, 0 }, // 21
	{ 0, 0, 0, 0, 0, 0 }, // 22
	{ 0, 0, 0, 0, 0, 0 }, // 23
	{ 0, 0, 0, 0, 0, 0 }, // 24
	{ 0, 0, 0, 0, 0, 0 }, // 25
	{ 0, 0, 0, 0, 0, 0 }, // 26
	{ 0, 0, 0, 0, 0, 0 }, // 27
	{ 0, 0, 0, 0, 0, 0 }, // 28
	{ 0, 0, 0, 0, 0, 0 }, // 29
	{ 0, 0, 0, 0, 0, 0 }, // 30
	{ 0, 0, 0, 0, 0, 0 }, // 31
	{ 176, 176, 176, 176, 176, 176 }, // 32
	{ 177, 177, 177, 177, 177, 177 }, // 33
	{ 178, 178, 178, 178, 178, 178 }, // 34
	{ 179, 179, 179, 179, 179, 179 }, // 35
	{ 180, 180, 180, 180, 180, 180 }, // 36
	{ 181, 181, 181, 181, 181, 181 }, // 37
	{ 182, 182, 182, 182, 182, 182 }, // 38
	{ 183, 183, 183, 183, 183, 183 }, // 39
	{ 184, 184, 184, 184, 184, 184 }, // 40
	{ 185, 185, 185, 185, 185, 185 }, // 41
	{ 186, 186, 186, 186, 186, 186 }, // 42
	{ 187, 187, 187, 187, 187, 187 }, // 43
	{ 188, 188, 188, 188, 188, 188 }, // 44
	{ 189, 189, 189, 189, 189, 189 }, // 45
	{ 190, 190, 190, 190, 190, 190 }, // 46
	{ 191, 191, 191, 191, 191, 191 }, // 47
	{ 192, 192, 192, 192, 192, 192 }, // 48
	{ 193, 193, 193, 193, 193, 193 }, // 49
	{ 194, 194, 194, 194, 194, 194 }, // 50
	{ 195, 195, 195, 195, 195, 195 }, // 51
	{ 196, 196, 196, 196, 196, 196 }, // 52
	{ 197, 197, 197, 197, 197, 197 }, // 53
	{ 198, 198, 198, 198, 198, 198 }, // 54
	{ 199, 199, 199, 199, 199, 199 }, // 55
	{ 200, 200, 200, 200, 200, 200 }, // 56
	{ 201, 201, 201, 201, 201, 201 }, // 57
	{ 202, 202, 202, 202, 202, 202 }, // 58
	{ 203, 203, 203, 203, 203, 203 }, // 59
	{ 204, 204, 204, 204, 204, 204 }, // 60
	{ 205, 205, 205, 205, 205, 205 }, // 61
	{ 206, 206, 206, 206, 206, 206 }, // 62
	{ 207, 207, 207, 207, 207, 207 }, // 63
};
// The block class itself. Note that the default constructor assumes the block is NOT active and has generic textureless blockType blockType_Default. 
// This is generally wise due to how chunks are populated with noise functions. Easier to only set a few blocks active instead of go through and set 
// most inactive.

class Block {
public:
	
	// Constructor
	Block(bool active = false,blockType type = blockType_Default) : Type(type), Active(active) { }
	~Block();
	// Methods
	// Returns bool this->Active
	bool isActive();
	// Sets bool this->Active
	void setActive(bool act);
	// Sets position. Deprecated.
	void setPos(glm::vec3 pos);
	void setPos(float x, float y, float z);
	// Gets position. Deprecated.
	glm::vec3 getPos();
	// returns enum blockType this->Type
	blockType getType();
	// sets this->Type as blockType type
	void setType(blockType type);

	// Attributes
	blockType Type;
	bool Active;
	glm::vec3 blockPos;
};

#endif // !BLOCK_H

