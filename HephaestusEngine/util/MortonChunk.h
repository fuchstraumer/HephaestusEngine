#pragma once
#ifndef MORTON_CHUNK_H
#define MORTON_CHUNK_H
#include "../stdafx.h"
#include <array>
#include "mesh.h"
#include "MortonChunkBlockLUT.h"
#include "Morton.h"
#include <bitset>
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

// First 32 bits of a block give its morton code and pos
const unsigned int codeBits(0xFFFF0000);
// Bits 33-48 give the blocks type
const unsigned int typeBits(0x0000FF00);
// Bits 49-64 are for currently unused attributes
const unsigned int attrBits(0x000000FF);

using uchar = unsigned char;

class Block{
public:
	blockType GetType() {

	}
	glm::vec3 GetPosition() {
		uint32_t code;
		code = (this->Data & codeBits);
		glm::uvec3 pos = MortonDecodeLUT<uint32_t,uint32_t>(code);
		return pos;
	}
	void SetType(uint16_t type) {
		
		
	}
	void SetPosition() {

	}
	std::uint64_t Data;
};

static const uint32_t codeBits = std::numeric_limits<uint32_t>::max();

class MortonChunk {
public:
	std::array<Block, 131072> Blocks;
	glm::ivec3 GridPosition;
	glm::vec3 Position;
	MortonChunk(glm::ivec3 gridpos) {
		this->GridPosition = gridpos;
		float x_pos, y_pos, z_pos;
		// The gridpos is simply "normalized" world coords to be integral values.
		// The actual position in the world is calculated now - we use this later to offset the chunk using a model matrix
		x_pos = this->GridPosition.x * ((CHUNK_SIZE / 2.0f) * BLOCK_RENDER_SIZE * 2.0f);
		y_pos = this->GridPosition.y * ((CHUNK_SIZE_Z / 2.0f) * BLOCK_RENDER_SIZE * 2.0f);
		z_pos = this->GridPosition.z * ((CHUNK_SIZE / 2.0f) * BLOCK_RENDER_SIZE * 2.0f);
		this->Position = glm::vec3(x_pos, y_pos, z_pos);
		// Set our initial block array values using the prebuilt LUT
		for (unsigned int i = 0; i < LUTBlocks.size(); ++i) {
			this->Blocks[i].Data = LUTBlocks[i];
		}
	}
	~MortonChunk() {
		glDeleteBuffers(1, &this->VBO);
		glDeleteBuffers(1, &this->EBO);
		glDeleteVertexArrays(1, &this->VAO);
	}
	// Calls noise function to build terrain
	void BuildTerrain(TerrainGenerator& gen);
	// Builds the mesh and populates the buffers
	void BuildChunkMesh();
	// Renders this chunk
	void RenderChunk();
private:
	GLuint VAO, VBO, EBO;
	Mesh chunkMesh;
};
#endif // !MORTON_CHUNK_H
