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
	blockType_Default = 0,
	GRASS,
	SAND,
	DIRT,
	STONE,
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
	COLOR_31,
	AIR,
	BEDROCK,
};
// Array holding indices into the texture atlas of texture.png

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
	float density;
};

#endif // !BLOCK_H

