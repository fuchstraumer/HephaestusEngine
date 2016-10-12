#ifndef TREECHUNK_H

#define TREECHUNK_H

#include "stdafx.h"
#include "util/shader.h"
#include "util/mesh.h"
#include <stdint.h>
/*
	Type: Class Object: Chunk -> holds subclass Block
	Data: Blocks, meshData, position values
	Methods: void buildTerrain, void buildCaves, void compressChunk
	void buildData, void buildRender(), void chunkUpdate(float dt), void chunkRender()

*/

class treeChunk {
public:
	// This initializer takes in the normalized integer chunk-space world coords and
	// calculates the actual world coords, then builds the initial list of blocks
	treeChunk(glm::ivec3 gridpos);
	// Takes pointer to global Terrain_generator, and builds the terrain
	void buildTerrain(Terrain_Generator& gen);
	// Chunkpos is the worldspace coords, gridPos is the chunk-space coords
	glm::vec3 chunkPos;
	glm::ivec3 gridPos;
	// Builds the actual render objects, like the VAO+VBO+EBO
	void buildRender();
	// Builds the mesh
	void buildData();
	void chunkRender(Shader shader);
	// Don't touch this.
	void compressChunk();
	// Really don't touch this.
	std::vector<std::uint8_t> chunkBlocks;
private:
	// Called by buildData. Used to only make faces that need to be made.
	void createCube(int x, int y, int z, bool leftFace, bool rightFace, bool frontFace,
		bool backFace, bool bottomFace, bool topFace, int type);
	// Called by terrain gen.
	void buildCaves();
	// Prototype of new block type.
	GLuint VAO, VBO, EBO;
	Mesh chunkMesh;
};

#endif // !TREECHUNK_H

