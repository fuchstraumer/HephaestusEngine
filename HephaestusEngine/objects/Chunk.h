#pragma once
#ifndef CHUNK_H
#define CHUNK_H
#include "stdafx.h"
#include "common\Constants.h"
#include "common\CommonUtil.h"
#include "Block.h"
#include "util\terrain\GeneratorBase.h"
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
		// Chunk manager is allowed to access internal details.
		friend class ChunkManager;
	public:

		// Position in homogenous integer grid defining chunk layout (used for logic, mostly)
		glm::ivec2 GridPosition;

		// Floating-point position used for rendering 
		glm::vec3 Position;

		// Default Ctor
		Chunk(glm::ivec2 grid_position);

		// Delete copy operator: We don't want to copy whole chunks!
		Chunk(const Chunk& other) = delete;

		// Delete copy assignment as well: No copying!
		Chunk& operator=(const Chunk& other) = delete;

		// Move operators okay: just make sure to explicitly define them.

		// Move constructor.
		Chunk(Chunk&& other);

		// Move assignment.
		Chunk& operator=(Chunk&& other);

		// Get position of this chunk in the overall grid.
		glm::vec3 GetPosFromGrid(glm::ivec2 gridpos);

		~Chunk() = default;

		// Calls noise function to build terrain
		void BuildTerrain(terrain::GeneratorBase& gen, int terraintype);

		// Builds the mesh and populates the buffers
		void BuildMesh();

		// Encodes blocks in this chunk using RLE compression
		void EncodeBlocks();

		// Clears chunk of data and frees up memory.
		void clear();

		// Gets block at position xyz
		inline BlockType GetBlock(glm::vec3 pos) const;
		inline BlockType GetBlock(float x, float y, float z) const;

		// Sets block at position to be of provided type
		inline void SetBlock(glm::vec3 pos, BlockType type);
		inline void SetBlock(float x, float y, float z, BlockType type);

		// Attempts to find "ground" level at point in XZ plane
		size_t GetGroundLevel(const glm::vec2& point) const;

		// Get sunlight level of block at point p
		int GetSunlightLevel(const glm::ivec3& p) const;

		// Get torchlight (artifical light) value at point p
		int GetTorchlightLevel(const glm::ivec3& p) const;

		// Set sunlight level of block at point p to be level
		void SetSunlightLevel(const glm::ivec3& p, uint8_t level);

		// Set torchlight level at point p to level
		void SetTorchlightLevel(const glm::ivec3& p, uint8_t level);


		// Keeps list of neighbors of this chunk, ordered as such:
		/*
		0: Top Left
		1: Top Right
		2: Bottom Left
		3: Bottom Right
		*/
		bool Neighbors[4];

		// Mesh object for this chunk.
		Mesh<vertex_t> mesh;

	private:

		// Used to get references to internal nodes
		inline BlockType& GetNodeRef(const glm::vec3& pos);
		inline BlockType& GetNodeRef(const float& x, const float& y, const float& z);

		// Stride of blocks along Y axis
		static constexpr size_t Y_BLOCK_STRIDE = CHUNK_SIZE * CHUNK_SIZE;
		static constexpr size_t X_BLOCK_STRIDE = CHUNK_SIZE;

		// Whether or not this object has had it's terrain generated.
		bool generated;

		// Lightmap for a chunk: stores light values from 0-15 for each block in a chunk.
		std::vector<uint8_t> lightMap;

		// Container for uncompressed block data
		std::vector<Block> terrainBlocks;

		// Container for compressed blocks: remove later, after integrating reading/writing
		// to compressed blocks
		std::vector<Block> encodedBlocks;

		// Container for modified blocks.
		std::unordered_map<glm::ivec3, Block, ivec3Hash> blocks;

		// Creates the mesh data for a cube at xyz using the given opacity values at each face and the specified texture coord.
		void createCube(int x, int y, int z, bool frontFace, bool rightFace, bool topFace, bool leftFace, bool bottomFace, bool backFace, int uv_type);

	};

}
#endif // !LINEAR_CHUNK_H
