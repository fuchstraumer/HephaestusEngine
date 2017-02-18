#include "stdafx.h"
#include "ChunkManager.h"

namespace objects {

	// Take the player position as a floating point vector and get the nearest chunk center coordinate.
	// Used to set the center around which we iterate 
	inline glm::vec2 playerToGrid(const glm::vec3& player_position) {

	}

	inline auto createEntry(size_t i, size_t j) {
		return std::make_pair<glm::ivec2, Chunk>(glm::ivec2(i, j), Chunk(glm::ivec2(i, j)));
	}

	std::shared_ptr<Chunk> ChunkManager::GetChunk(const glm::ivec2 & pos) {
		return std::shared_ptr<Chunk>(&chunkData.at(pos));
	}

	void ChunkManager::Init(const glm::vec3& initial_position, const unsigned int& view_distance) {
		// Gauss circle problem: how many integer lattice points in a circle of radius R?
		// Since our view distance is in integer units of chunks, we can assume we're working with unit squares.
		chunkData.reserve(view_distance * view_distance);
		for (size_t i = 0; i < view_distance; ++i) {
			for (size_t j = 0; j < view_distance; ++j) {
				if ((i*i) + (j*j) <= (view_distance*view_distance)) {
					Chunk NewChunk(glm::vec2(i, j));
					// Normally we bounce loading/generation off into a thread, but for initialization lets build the whole range
					NewChunk.BuildTerrain(terrainGen, terrainType);
					NewChunk.BuildMesh();
					chunkData.insert(std::pair<glm::ivec2, Chunk>(glm::ivec2(i, j), std::move(NewChunk)));
				}
			}
		}
	}

	void ChunkManager::Update(const glm::vec3& update_position) {
		// For each chunk in the list of active chunks, check them for any blocks that have changed.

		// Check to see if our position has changed enough that we should be checking for chunks to load/unload

	}

}