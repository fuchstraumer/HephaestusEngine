#ifndef CHUNK_H

#define CHUNK_H

#include "stdafx.h"
#include "block.h"
#include "mesh.h"
#include "Terrain_Gen.h"
#include "shader.h"
/*
	Type: Class Object: Chunk -> holds subclass Block
	Data: Blocks[x][y][z] (pointer array), bool Active
	Methods: void setupBlocks(), void buildRender(), void chunkUpdate(float dt), void chunkRender()

	setupBlocks() calls the terrain generator with a list of important parameters (mainly global pos), and then sets
	the appropriate blocks to "true" for blockActive to effectively build the terrain. buildRender() builds the buffers 
	and the VAO for the object, and performs some rudimentary checks to cull faces. chunkUpdate() is used to call for a 
	chunk update and requires the dt from the last update. chunkRender() is a function called by the region class, which
	renders the chunks it needs to from its list of chunks based on the player position and the like.

	The main purpose of this object is to serve as a way to hold all the instances of our block class. The position
	of a block will be inferred from its position in the blocks array of this class. This class will call the blocks
	setupMesh() function as needed based on active blocks in its volume

	The choice to tie the vertex arrays and buffers to this object is to ensure constant render time. This way, refreshing
	on block changes takes the same amount of time whether we destroyed one block or destroyed several. Eventually, this 
	class will also have something resembling an octree structure for managing physics interactions.

	TODO: Add implementation of mesh builder to setupBlocks(), currently only builds instances
*/

class Chunk {
public:
	Chunk(glm::vec2 chunkPos);
	~Chunk();
	glm::vec2 chunkPos = glm::vec2(0,0);
	void createCube(int x, int y, int z,bool leftFace = true, bool rightFace = true, bool frontFace = true,
		bool backFace = true, bool bottomFace = true, bool topFace = true);
	void buildRender();
	//void chunkUpdate(float dt);
	void chunkRender(Shader shader);
	void exportMesh();
	Mesh mesh;
	triLerpCube terrain_cube;
	int activecount;
	int inactivecount;
	bool chunkBuilt;
	bool chunkRendered = false;
	
private:
	Block*** chunkBlocks;
	GLuint VAO, VBO, EBO;
};

#endif // !CHUNK_H

