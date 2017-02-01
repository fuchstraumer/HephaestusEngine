#include "stdafx.h"
#include "ChunkManager.h"

// Take the player position as a floating point vector and get the nearest chunk center coordinate.
// Used to set the center around which we iterate 
inline glm::vec2 playerToGrid(const glm::vec3& player_position) {

}

inline auto createEntry(size_t i, size_t j) {
	return std::make_pair<glm::ivec2, LinearChunk>(glm::ivec2(i, j), LinearChunk(glm::ivec2(i, j)));
}

std::shared_ptr<LinearChunk> ChunkManager::GetChunk(const glm::ivec2 & pos) const{
	return std::shared_ptr<LinearChunk>(&chunkData.at(pos));
}

void ChunkManager::Init(const glm::vec3& initial_position, const unsigned int& view_distance) {
	// Gauss circle problem: how many integer lattice points in a circle of radius R?
	// Since our view distance is in integer units of chunks, we can assume we're working with unit squares.
	chunkData.reserve(view_distance * view_distance);
	// Generate chunks at set of points. Add to load list since we're not ready for them yet.
	for (size_t i = 0; i < view_distance; ++i) {
		for (size_t j = 0; j < view_distance; ++j) {
			if (i*i + j*j <= (view_distance * view_distance)) {
				chunkData.insert(createEntry(i,j));
			}
			else {
				continue;
			}
		}
	}

	// Generate data for each entry in map.
	for (auto iter = chunkData.begin(); iter != chunkData.end(); ++iter) {

	}
}

void ChunkManager::Update(const glm::vec3& update_position){
	// For each chunk in the list of active chunks, check them for any blocks that have changed.

	// Check to see if our position has changed enough that we should be checking for chunks to load/unload

}
