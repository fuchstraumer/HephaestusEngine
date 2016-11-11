#ifndef TREECHUNK_H

#define TREECHUNK_H

#include "stdafx.h"
#include "util/shader.h"
#include "util/mesh.h"
#include "util\intervaltree.h"

/*
	Type: Class Object: Chunk -> holds subclass Block
	Data: Blocks, meshData, position values
	Methods: void buildTerrain, void buildCaves, void compressChunk
	void buildData, void buildRender(), void chunkUpdate(float dt), void chunkRender()

*/

class TreeChunk {
public:
	// This initializer takes in the normalized integer chunk-space world coords and
	// calculates the actual world coords, then builds the initial list of blocks
	TreeChunk(glm::ivec3 gridpos);
	~TreeChunk();
	// Takes pointer to global Terrain_generator, and builds the terrain
	void BuildTerrain(TerrainGenerator& gen, int terrainType);
	// Chunkpos is the worldspace coords, gridPos is the chunk-space coords
	glm::vec3 Position;
	glm::ivec3 GridPos;
	// Builds the actual render objects, like the VAO+VBO+EBO
	// Builds the mesh
	void BuildChunkMesh();
	void RenderChunk(Shader shader);
	// Compresses the chunks block data using RLE compression
	void EncodeChunk();
	// Builds an interval tree for the chunk's blockdata
	void BuildTree();
	// Really don't touch this.
	std::vector<blockType> ChunkBlocks;
	IntervalTree<blockType> ChunkTree;
	bool ChunkBuilt = false;
	bool ChunkRendered = false;
	bool ChunkCompressed = false;
private:
	GLuint VAO, VBO, EBO;
	Mesh chunkMesh;
	// Called by buildData. Used to only make faces that need to be made.
	void createCube(int x, int y, int z, bool leftFace, bool rightFace, bool frontFace,
		bool backFace, bool bottomFace, bool topFace, int type);
};

#endif // !TREECHUNK_H

