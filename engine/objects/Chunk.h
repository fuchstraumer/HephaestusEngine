#pragma once
#ifndef CHUNK_H
#define CHUNK_H
#include "stdafx.h"

#include "common\Constants.h"
#include "common\CommonUtil.h"
#include "Block.h"
#include "..\mesh\mesh.h"

namespace objects {

	/*
	
		Class - Chunk

		This class holds collections of blocks, and is the main way the game logic
		and environment will interface with blocks. Chunks have a set dimensionality, 
		set by the CHUNK_SIZE and CHUNK_SIZE_Y macros in the common header file (stdafx.h)

		Chunks hold two positions. One of these positions is an integer position that is
		used by the ChunkManager for storing individual chunks - this uses a hash map,
		and hashing this integer position is fairly easy. Additionally, it makes rendering
		a circular region around someone/something easier, since we can just "snap-to-grid", 
		so to speak, when choosing where to put our chunks.

		Moving +1 / -1 in this grid moves in steps of CHUNK_SIZE (otherwise chunks would
		overlap when rendered). The other Position (just called Position) is floating-point
		and is used for rendering the chunks.

		Chunks don't store the blocks generated by the terrain generation algorithms when 
		they are unloaded. Instead, they store a hash map of player-placed or "unique" blocks,
		and this is what is saved to a file and/or loaded from a file. 
	
	*/

	class Chunk {
		friend class ChunkManager;
	public:

		// Position in homogenous integer grid defining chunk layout (used for logic, mostly)
		glm::ivec2 GridPosition;
		// Floating-point position used for rendering 
		glm::vec3 Position;

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
		0: Top Left
		1: Top Right
		2: Bottom Left
		3: Bottom Right
		*/
		std::array<std::shared_ptr<Chunk>, 4> Neighbors;

		static constexpr size_t CHUNK_SIZE = 32;
		static constexpr size_t CHUNK_SIZE_Y = 128;

	private:

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

		static constexpr size_t Z_BLOCK_STRIDE = CHUNK_SIZE * CHUNK_SIZE;
		static constexpr size_t X_BLOCK_STRIDE = CHUNK_SIZE;
		static constexpr size_t BLOCKS_PER_CHUNK = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE_Y;

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
