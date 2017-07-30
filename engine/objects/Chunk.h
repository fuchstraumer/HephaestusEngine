#pragma once
#ifndef CHUNK_H
#define CHUNK_H
#include "stdafx.h"
#include "Block.h"
#include "..\mesh\mesh.h"
#include "../generation/NoiseSampler.h"
namespace objects {

	class Chunk {
		friend class ChunkManager;
	public:

		Chunk(glm::ivec2 grid_position);

		Chunk(const Chunk& other) = delete;
		Chunk& operator=(const Chunk& other) = delete;

		Chunk(Chunk&& other) noexcept;
		Chunk& operator=(Chunk&& other) noexcept;

		~Chunk() = default;

		glm::vec3 GetPosFromGrid(glm::ivec2 gridpos);

		void BuildTerrain(const size_t& terrain_type);
		void BuildMesh(const vulpes::Device * _device);
		void EncodeBlocks();
		void clear();

		const Block& GetBlock(const uint32_t& x, const uint32_t& y, const uint32_t& z) const;
		void SetBlock(const uint32_t& x, const uint32_t& y, const uint32_t& z, Block _new);

		// Keeps list of neighbors of this chunk, ordered as such:
		/*
		0: Top Left	   1: Top Right
		2: Bottom Left 3: Bottom Right
		*/
		std::array<std::shared_ptr<Chunk>, 4> Neighbors;

		// Position in homogenous integer grid defining chunk layout (used for logic, mostly)
		glm::ivec2 GridPosition;
		// Floating-point position used for rendering 
		glm::vec3 Position;

	private:

		noise::NoiseSampler sampler;

		enum class blockFace : size_t {
			FRONT,
			RIGHT,
			TOP,
			LEFT,
			BOTTOM,
			BACK,
		};

		void setBlockLightingData(const uint32_t&, const uint32_t& y, const uint32_t& z, std::array<BlockType, 27>& neighbor_blocks, std::array<float, 27>& neighbor_shades) const;
		void getFaceVertices(const blockFace& face, block_vertex_t& v0, block_vertex_t& v1, block_vertex_t& v2, block_vertex_t& v3, const size_t& texture_idx);
		void createBlockFace(const blockFace& face, const size_t& uv_idx, const glm::vec3& pos);

		void createCube(const size_t& x, const size_t& y, const size_t& z, const bool& front_face, const bool& right_face,
			const bool& top_face, const bool& leftFace, const bool& bottom_face, const bool& backFace, const size_t& uv_idx);

		Block& getBlock(const uint32_t& x, const uint32_t& y, const uint32_t& z);

		bool generated = false;

		// Lightmap for a chunk: stores light values from 0-15 for each block in a chunk.
		std::array<uint8_t, BLOCKS_PER_CHUNK> lightMap;
		std::array<Block, BLOCKS_PER_CHUNK> terrainBlocks;

		// Mesh object for this chunk.
		std::unique_ptr<mesh::Mesh<mesh::BlockVertices, block_vertex_t>> mesh;
		// Container for modified blocks.
		std::unordered_map<glm::ivec3, std::reference_wrapper<Block>> uniqueBlocks;
		const vulpes::Device* device;
		ChunkStatus status;
	};

}
#endif // !LINEAR_CHUNK_H
