#ifndef OCTREECHUNK_H

#define OCTREECHUNK_H

#include "stdafx.h"
#include "util/shader.h"
#include "util/mesh.h"
#include <stdint.h>
#include <array>
#include <iterator>




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
/*
	Type: Class Object: Chunk -> holds subclass Block
	Data: Blocks, meshData, position values
	Methods: void buildTerrain, void buildCaves, void compressChunk
	void buildData, void buildRender(), void chunkUpdate(float dt), void chunkRender()

*/

class OctreeChunk {
public:
	// This initializer takes in the normalized integer chunk-space world coords and
	// calculates the actual world coords, then builds the initial list of blocks
	OctreeChunk(glm::ivec3 gridpos);
	~OctreeChunk();
	// Takes pointer to global Terrain_generator, and builds the terrain
	void BuildTerrain(TerrainGenerator& gen, int terrainType);
	// Chunkpos is the worldspace coords, gridPos is the chunk-space coords
	glm::vec3 ChunkPos;
	glm::ivec3 GridPos;
	// Builds the actual render objects, like the VAO+VBO+EBO
	void BuildRender();
	// Builds the mesh
	void BuildData();
	void ChunkRender(Shader shader);
	// Compresses the chunks block data using RLE compression
	void EncodeChunk();
	// Builds an interval tree for the chunk's blockdata
	void BuildTree();
	// Really don't touch this.
	std::vector<blockType> ChunkBlocks;
	bool ChunkBuilt = false;
	bool ChunkRendered = false;
	bool ChunkCompressed = false;
private:
	GLuint VAO, VBO, EBO;
	Mesh chunkMesh;
	// Called by buildData. Used to only make faces that need to be made.
	void createCube(int x, int y, int z, bool leftFace, bool rightFace, bool frontFace,
		bool backFace, bool bottomFace, bool topFace, int type);
	// Offsets for octree subdivision
	const glm::vec3 offsets[8] = 
	{
		{ -0.5f, -0.5f, -0.5f },
		{ +0.5f, -0.5f, -0.5f },
		{ -0.5f, -0.5f, +0.5f },
		{ +0.5f, -0.5f, +0.5f },
		{ -0.5f, +0.5f, -0.5f },
		{ +0.5f, +0.5f, -0.5f },
		{ -0.5f, +0.5f, +0.5f },
		{ +0.5f, +0.5f, +0.5f }
	};

};

#endif // !TREECHUNK_H

