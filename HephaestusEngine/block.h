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
#include <glm/glm.hpp>

// Type and structure definitions

//  Structure to hold all of a blocks face indices. While verbose, being able to explicitly access individual faces is very useful
enum blockType {
	blockType_Default,
	Grass,
	Stone,
	Dirt,
	Cobblestone,
	Air,
	Water,
	Leaves,
	Log,
	Fire,
	Lava,
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
	// returns enum blockType this->Type
	blockType getType();
	// sets this->Type as blockType type
	void setType(blockType type);
	// builds the blockVerts and blockFaces structure. textures not accounted for. 
	// lighting stuff is TBD
	// Writes data to the pointers given to this function
	// In order to insure that the core attributes aren't modified 
	// out-of-scope, blockType and Active are left private.
	// The vertices and faces are commonly accessed and copied from
	// so lets leave those public
	blockType Type;
	bool Active;
};

#endif // !BLOCK_H

