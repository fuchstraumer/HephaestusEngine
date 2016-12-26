#pragma once
#ifndef MORTON_CHUNK_H
#define MORTON_CHUNK_H
#include "../stdafx.h"
#include "../mesh/mesh.h"
#include "../util/Morton.h"
#include <bitset>
#include "../util/Shader.h"


// First 32 bits of a block give its morton code and pos
const uint32_t codeBits(0x0000FFFF);
// Bits 33-48 give the blocks type
const uint32_t typeBits(0x00FF0000);
// Bits 49-64 are for currently unused attributes
const uint32_t attrBits(0xFF000000);
using uint = unsigned int;
using uchar = unsigned char;

class BlockLite {
public:
	BlockLite() {
		SetType(blockTypes::AIR);
	}
	blockType GetType() {
		return this->Type;
	}
	void SetPosition(uint x, uint y, uint z) {
		this->Data = MortonEncodeLUT<uint32_t, uint32_t>(x, y, z);
	}
	glm::vec3 GetPosition() {
		glm::uvec3 pos = MortonDecodeLUT<uint32_t, uint32_t>(Data);
		return pos;
	}
	void SetType(blockType type) {
		this->Type = type;
	}
	std::uint32_t Data;
	blockType Type;
};


class MortonChunk {
public:
	std::vector<blockType> Blocks;
	glm::ivec3 GridPosition;
	glm::vec3 Position;
	MortonChunk(glm::ivec3 gridpos) {
		this->GridPosition = gridpos;
		float x_pos, y_pos, z_pos;
		std::size_t totalBlocks = CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE_Z;
		this->Blocks.resize(totalBlocks); this->Blocks.assign(totalBlocks,blockTypes::AIR);
		// The gridpos is simply "normalized" world coords to be integral values.
		// The actual position in the world is calculated now - we use this later to offset the chunk using a model matrix
		x_pos = this->GridPosition.x * ((CHUNK_SIZE / 2.0f) * BLOCK_RENDER_SIZE * 2.0f);
		y_pos = this->GridPosition.y * ((CHUNK_SIZE_Z / 2.0f) * BLOCK_RENDER_SIZE * 2.0f);
		z_pos = this->GridPosition.z * ((CHUNK_SIZE / 2.0f) * BLOCK_RENDER_SIZE * 2.0f);
		mesh.Position = GetPosFromGrid(GridPosition);
	}

	glm::vec3 GetPosFromGrid(glm::ivec3 gridpos) {
		glm::vec3 res;
		res.x = this->GridPosition.x * ((CHUNK_SIZE / 2.0f) * BLOCK_RENDER_SIZE * 2.0f);
		res.y = this->GridPosition.y * ((CHUNK_SIZE_Z / 2.0f) * BLOCK_RENDER_SIZE * 2.0f);
		res.z = this->GridPosition.z * ((CHUNK_SIZE / 2.0f) * BLOCK_RENDER_SIZE * 2.0f);
		return res;
	}
	~MortonChunk() = default;
	// Calls noise function to build terrain
	void BuildTerrain(TerrainGenerator& gen, int terraintype);
	// Builds the mesh and populates the buffers
	void BuildMesh();
	// Deletes data for blocks that aren't currently set to anything.
	void CleanChunkBlocks();
	// Returns index to block in vector
	inline uint32_t GetBlockIndex(uint32_t x, uint32_t y, uint32_t z){
		return (MortonEncodeLUT<uint32_t,uint32_t>(x, y, z));
	}
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
#endif // !MORTON_CHUNK_H
