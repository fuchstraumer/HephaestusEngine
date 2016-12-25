#pragma once
#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H
#include "../stdafx.h"
#include "MortonChunk.h"
#include <unordered_map>
#include <memory>

// Simple alias for the type that will be inserted into the unordered map container
using mapEntry = std::pair<glm::ivec3,std::shared_ptr<MortonChunk>>;
// Callable hashing object for an ivec3
struct ivecHash {
	
	size_t operator()(const glm::ivec3& vec) {
		// Starting size/seed of the input vector, 3 in this case
		size_t seed = 3;
		// throw vector members into an initializer list so we can
		// iterate really easily
		auto nums = { vec.x, vec.y, vec.z };
		// Build the hash
		for (auto i : nums) {
			seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}
};
// The main container of chunks in this object, used for all chunks regardless of status
using chunkMap = std::unordered_map<glm::ivec3, std::shared_ptr<MortonChunk>, ivecHash>;
// Just a vector containing pointers to the underlying chunks: used for updating, pruning
using chunkContainer = std::vector<std::shared_ptr<MortonChunk>>;
class ChunkManager {
public:
	ChunkManager() = default;
	~ChunkManager() = default;

	void AddChunk(MortonChunk& chk);

	std::shared_ptr<MortonChunk> GetChunk(const glm::ivec3& pos) const;

	void Update();

	void Render();

	void Prune();
private:
	// Tracks player position, for keeping chunks updated around player
	glm::vec3 playerPosition;
	// Radius, in chunks, to render
	int renderRadius;
	// Container of chunks that need to have their contents updated, outside of
	// the standard activation/deactivation of rendering.
	chunkContainer updateChunks;
	// Container of chunks to be pruned/cleared in this mesh.
	chunkContainer pruneChunks;
};
#endif // !CHUNK_MANAGER_H
