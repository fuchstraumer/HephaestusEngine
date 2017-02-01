#pragma once
#ifndef LINEAR_CHUNK_H
#define LINEAR_CHUNK_H
#include "../stdafx.h"
#include "../mesh/mesh.h"
// Converts 3D coordinates into 1D space used for storage in the vector
inline int GetBlockIndex(const glm::vec3& pos) {
	return static_cast<int>(((pos.y) * CHUNK_SIZE * CHUNK_SIZE + (pos.x) * CHUNK_SIZE + (pos.z)));
}
// Same as above, with individual positions
inline int GetBlockIndex(const int& x, const int& y, const int& z) {
	return static_cast<int>((y)* CHUNK_SIZE * CHUNK_SIZE + (x)* CHUNK_SIZE + (z));
}

class LinearChunk{
public:
	std::vector<blockType> Blocks;
	glm::ivec3 GridPosition;
	glm::vec3 Position;
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
		mesh.Position = glm::vec3(x_pos, y_pos, z_pos);
		this->Position = glm::vec3(x_pos, y_pos, z_pos);
		mesh.Model = glm::translate(glm::mat4(1.0f), this->Position);
	}

	glm::vec3 GetPosFromGrid(glm::ivec3 gridpos) {
		glm::vec3 res;
		res.x = this->GridPosition.x * ((CHUNK_SIZE / 2.0f));
		res.y = this->GridPosition.y * ((CHUNK_SIZE_Z / 2.0f));
		res.z = this->GridPosition.z * ((CHUNK_SIZE / 2.0f));
		return res;
	}
	~LinearChunk() = default;
	// Calls noise function to build terrain
	void BuildTerrain(TerrainGenerator& gen, int terraintype);
	// Builds the mesh and populates the buffers
	void BuildMesh();
	// Deletes data for blocks that aren't currently set to anything.
	void CleanChunkBlocks();
	// Encodes blocks in this chunk using RLE compression
	void EncodeBlocks();
	// Container for compressed blocks: remove later, after integrating reading/writing
	// to compressed blocks
	std::vector<blockType> encodedBlocks;
	// Keeps list of neighbors of this chunk, ordered as such:
	/*
	0: Top Left
	1: Top Right
	2: Bottom Left
	3: Bottom Right
	*/
	bool neighbors[4];

	Mesh mesh;
private:


	void createCube(int x, int y, int z, bool frontFace, bool rightFace, bool topFace, bool leftFace, bool bottomFace, bool backFace, int uv_type);

};

#endif // !LINEAR_CHUNK_H
