#pragma once
#ifndef HEPHAESTUS_ENGINE_CHUNK_MANAGER_HPP
#define HEPHAESTUS_ENGINE_CHUNK_MANAGER_HPP
#include "Chunk.hpp"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include <unordered_map>
#include <memory>

class ChunkManager {
public:

	ChunkManager(const size_t& init_view_radius);
	~ChunkManager();

	ecs::entity_t CreateChunk(const glm::ivec2& grid_position);

	// Initialize the chunk manager by starting at an initial position and using the input
	// view distance (given in terms of a radius of chunks to render)
	void Init(const glm::vec3 & initial_position, const int& view_distance);

	void SetRenderDistance(const size_t & render_distance);
	size_t GetRenderDistance() const noexcept;

	void Update(const glm::vec3& update_position);
	// Cleans up inactive chunks in "pruneChunks" by compressing and then saving their data.
	void Prune();

private:

	// Radius, in chunks, to render
	size_t renderRadius;
	// Main container of chunk data, map allows for searching based on the chunks position.
	std::unordered_map<glm::ivec2, ecs::entity_t> chunkMap;

};

#endif // !CHUNK_MANAGER_H
