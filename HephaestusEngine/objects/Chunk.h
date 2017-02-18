#pragma once
#ifndef CHUNK_H
#define CHUNK_H
#include "stdafx.h"
#include "Block.h"
#include "..\util\TerrainGen.h"
#include "..\mesh\mesh.h"

namespace objects {

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
		Chunk(Chunk&& other) : mesh(other.mesh), terrainBlocks(other.terrainBlocks), encodedBlocks(other.encodedBlocks), Neighbors[other.Neighbors] {
			other.clear();
		}

		// Move assignment.
		Chunk& operator=(Chunk&& other) {
			if (this != &other) {
				this->mesh = std::move(other.mesh);
				this->terrainBlocks = std::move(other.terrainBlocks);
				this->encodedBlocks = std::move(other.encodedBlocks);
				other.clear();
			}
			return *this;
		}

		// Get position of this chunk in the overall grid.
		glm::vec3 GetPosFromGrid(glm::ivec2 gridpos);

		~Chunk() = default;

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

		// Mesh object for this chunk.
		Mesh mesh;

	private:

		// Container for uncompressed block data
		std::vector<blockType> terrainBlocks;

		// Container for compressed blocks: remove later, after integrating reading/writing
		// to compressed blocks
		std::vector<blockType> encodedBlocks;

		// Container for modified blocks.
		std::unordered_map<glm::ivec3, Block, ivec3Hash> blocks;

		// Creates the mesh data for a cube at xyz using the given opacity values at each face and the specified texture coord.
		void createCube(int x, int y, int z, bool frontFace, bool rightFace, bool topFace, bool leftFace, bool bottomFace, bool backFace, int uv_type);

	};

}
#endif // !LINEAR_CHUNK_H
