#pragma once
#ifndef LINEAR_CHUNK_H
#define LINEAR_CHUNK_H
#include "stdafx.h"
#include "..\util\TerrainGen.h"
#include "..\mesh\mesh.h"

class LinearChunk{
	// Chunk manager is allowed to access internal details.
	friend class ChunkManager;
public:

	// Position in homogenous integer grid defining chunk layout (used for logic, mostly)
	glm::ivec2 GridPosition;

	// Floating-point position used for rendering 
	glm::vec3 Position;

	// Default Ctor
	LinearChunk(glm::ivec2 grid_position);

	// Delete copy operator: We don't want to copy whole chunks!
	LinearChunk(const LinearChunk& other) = delete;

	// Delete copy assignment as well: No copying!
	LinearChunk& operator=(const LinearChunk& other) = delete;

	// Move operators okay: just make sure to explicitly define them.

	// Move constructor.
	LinearChunk(LinearChunk&& other) : mesh(other.mesh), Blocks(other.Blocks), encodedBlocks(other.encodedBlocks) {
		other.clear();
	}

	// Move assignment.
	LinearChunk& operator=(LinearChunk&& other) {
		if (this != &other) {
			this->mesh = other.mesh;
			this->Blocks = other.Blocks;
			this->encodedBlocks = other.encodedBlocks;
			other.clear();
		}
		return *this;
	}

	// Get position of this chunk in the overall grid.
	glm::vec3 GetPosFromGrid(glm::ivec2 gridpos);

	~LinearChunk() = default;

	// Calls noise function to build terrain
	void BuildTerrain(TerrainGenerator& gen, int terraintype);

	// Builds the mesh and populates the buffers
	void BuildMesh();

	// Encodes blocks in this chunk using RLE compression
	void EncodeBlocks();
	
	// Clears chunk of data and frees up memory.
	void clear();

	// Keeps list of neighbors of this chunk, ordered as such:
	/*
	0: Top Left
	1: Top Right
	2: Bottom Left
	3: Bottom Right
	*/
	bool Neighbors[4];

	Mesh mesh;
private:

	// Container for uncompressed block data
	std::vector<blockType> Blocks;

	// Container for compressed blocks: remove later, after integrating reading/writing
	// to compressed blocks
	std::vector<blockType> encodedBlocks;

	// Creates the mesh data for a cube at xyz using the given opacity values at each face and the specified texture coord.
	void createCube(int x, int y, int z, bool frontFace, bool rightFace, bool topFace, bool leftFace, bool bottomFace, bool backFace, int uv_type);

};

#endif // !LINEAR_CHUNK_H
