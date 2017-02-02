#pragma once
#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H
#include "stdafx.h"
#include "LinearChunk.h"
#include <unordered_map>
#include <memory>


// Simple alias for the type that will be inserted into the unordered map container
using mapEntry = std::pair<glm::ivec2,LinearChunk>;

// Callable hashing object for an ivec3
struct ivecHash {
	
	size_t operator()(const glm::ivec2& vec) const {
		// Starting size/seed of the input vector, 3 in this case
		size_t seed = 3;
		// throw vector members into an initializer list so we can
		// iterate really easily
		auto nums = { vec.x, vec.y };
		// Build the hash
		for (auto i : nums) {
			seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}

};

// The main container of chunks in this object, used for all chunks regardless of status
using chunkMap = std::unordered_map<glm::ivec2, LinearChunk, ivecHash>;

// Just a vector containing pointers to the underlying chunks: used for updating, pruning
using chunkContainer = std::vector<std::shared_ptr<LinearChunk>>;

class ChunkManager {
public:

	ChunkManager() = default;
	~ChunkManager() = default;

	void AddChunk(LinearChunk& chk);

	std::shared_ptr<LinearChunk> GetChunk(const glm::ivec2& pos);

	// Initialize the chunk manager by starting at an initial position and using the input
	// view distance (given in terms of a radius of chunks to render)
	void Init(const glm::vec3 & initial_position, const unsigned int& view_distance);

	void Update(const glm::vec3& update_position);

	// Renders chunks in the 
	void Render();

	// Cleans up inactive chunks in "pruneChunks" by compressing and then saving their data.
	void Prune();
private:

	// Tracks player position, for keeping chunks updated around player
	glm::vec3 playerPosition;

	// Keeps track of grid center.
	glm::vec2 gridCenter;

	// Radius, in chunks, to render
	int renderRadius;

	// Container of chunks that need to have their contents updated, outside of
	// the standard activation/deactivation of rendering.
	chunkContainer updateChunks;

	// Container of chunks to be pruned/cleared in this mesh, meaning compressed + written to a file
	// then deallocated.
	chunkContainer pruneChunks;

	/*
	
		How to implement std::async for generating terrain and mesh data for chunks?
		Mesh data relies on terrain data. How to use futures with this system?
	
	*/

	// Main container of chunk data, map allows for searching based on the chunks position.
	chunkMap chunkData;

};
#endif // !CHUNK_MANAGER_H
