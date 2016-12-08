#pragma once
#ifndef MORTON_CHUNK_H
#define MORTON_CHUNK_H
#include "../stdafx.h"
#include "mesh.h"
#include "Morton.h"
#include <bitset>
#include "shader.h"


// First 32 bits of a block give its morton code and pos
const uint32_t codeBits(0x0000FFFF);
// Bits 33-48 give the blocks type
const uint32_t typeBits(0x00FF0000);
// Bits 49-64 are for currently unused attributes
const uint32_t attrBits(0xFF000000);
using uint = unsigned int;
using uchar = unsigned char;

class Block{
public:
	Block() { 
		this->Data = new uint32_t(~0xFFFFFFFF);
		SetType(blockTypes::AIR);
	}
	~Block() {
		if (Data != nullptr) {
			delete Data;
		}
	}
	blockType GetType() {
		return static_cast<uint8_t>((*this->Data & typeBits) >> 16);
	}
	glm::vec3 GetPosition() {
		uint16_t code;
		code = (*this->Data & codeBits);
		glm::uvec3 pos = MortonDecodeLUT<uint32_t,uint16_t>(code);
		return pos;
	}
	void SetType(uint16_t type) {
		*this->Data |= (static_cast<uint32_t>(type) << 16);
	}
	std::uint32_t* Data;
};

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
		this->Position = glm::vec3(x_pos, y_pos, z_pos);
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

	Mesh mesh;
	void createCube(int x, int y, int z, bool frontFace, bool rightFace, bool topFace, bool leftFace, bool bottomFace, bool backFace, int uv_type);

};
#endif // !MORTON_CHUNK_H
