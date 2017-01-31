#pragma once
#ifndef LINEAR_CHUNK_H
#define LINEAR_CHUNK_H
#include "../stdafx.h"
#include "../mesh/mesh.h"

class LinearChunk{
public:
	
	// Default constructor, takes position of chunk.
	LinearChunk(glm::ivec3 gridpos) {
		this->GridPosition = gridpos;
		float x_pos, y_pos, z_pos;
		std::size_t totalBlocks = CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE_Z;
		this->Blocks.resize(totalBlocks); this->Blocks.assign(totalBlocks, blockTypes::AIR);
		// The gridpos is simply "normalized" world coords to be integral values.
		// The actual position in the world is calculated now - we use this later to offset the chunk using a model matrix
		x_pos = this->GridPosition.x * ((CHUNK_SIZE / 2.0f));
		y_pos = this->GridPosition.y * ((CHUNK_SIZE_Z / 2.0f));
		z_pos = this->GridPosition.z * ((CHUNK_SIZE / 2.0f));
		mesh.Position = GetPosFromGrid(GridPosition);
	}

	// Converts integer position to floating-point position.
	glm::vec3 GetPosFromGrid(glm::ivec3 gridpos) {
		glm::vec3 res;
		res.x = this->GridPosition.x * ((CHUNK_SIZE / 2.0f));
		res.y = this->GridPosition.y * ((CHUNK_SIZE_Z / 2.0f));
		res.z = this->GridPosition.z * ((CHUNK_SIZE / 2.0f));
		return res;
	}

	// Defaulted destructor.
	~LinearChunk() = default;

	// Calls noise function to build terrain
	void BuildTerrain(TerrainGenerator& gen, int terraintype);

	// Builds the mesh and populates the buffers
	void BuildMesh();

	// Converts 3D coordinates into 1D space used for storage in the vector
	int GetBlockIndex(const glm::vec3& pos) const {
		return static_cast<int>(((pos.y) * CHUNK_SIZE * CHUNK_SIZE + (pos.x) * CHUNK_SIZE + (pos.z))); 
	}
	// Same as above, with individual positions
	int GetBlockIndex(const int& x, const int& y, const int& z) const {
		return static_cast<int>((y) * CHUNK_SIZE * CHUNK_SIZE + (x) * CHUNK_SIZE + (z));
	}


	// Keeps list of neighbors of this chunk, ordered as such:
	/*
	0: Top Left
	1: Top Right
	2: Bottom Left
	3: Bottom Right
	*/
	// Currently unimplemented.
	bool neighbors[4];

	// Mesh for this chunk.
	Mesh mesh;
	
	// Stores block data for this chunk.
	std::vector<blockType> Blocks;

	// Gives position of chunk in int grid
	glm::ivec3 GridPosition;

	// Gives floating-point position of this chunk.
	glm::vec3 Position;

private:
	
	// Private function used to create the mesh vertices and indices for a cube at position x,y,z. Boolean values specify whether or not
	// a block exists at the given face. If a value is false, that means there isn't a block at that face and as such we NEED to render that face.
	// (otherwise there'd be nothing there, and a hole in our chunk)
	void createCube(int x, int y, int z, bool frontFace, bool rightFace, bool topFace, bool leftFace, bool bottomFace, bool backFace, int uv_type);

};

#endif // !LINEAR_CHUNK_H
