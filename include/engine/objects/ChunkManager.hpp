#pragma once
#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H
#include "stdafx.h"

namespace objects {

	class ChunkManager {
	public:

		ChunkManager(const size_t& init_view_radius);
		~ChunkManager();

		void CreateChunk(const glm::ivec2& grid_position);

		// Initialize the chunk manager by starting at an initial position and using the input
		// view distance (given in terms of a radius of chunks to render)
		void Init(const glm::vec3 & initial_position, const int& view_distance);

		void SetRenderDistance(const size_t & render_distance);
		size_t GetRenderDistance() const noexcept;

		void Update(const glm::vec3& update_position);
		// Cleans up inactive chunks in "pruneChunks" by compressing and then saving their data.
		void Prune();

		void SetLightPos(const glm::vec3& light_pos);
		void SetLightColor(const glm::vec3& light_color);
		glm::vec3 GetLightPos() const noexcept;
		glm::vec3 GetLightColor() const noexcept;

	private:

		// Radius, in chunks, to render
		size_t renderRadius;

		// Main container of chunk data, map allows for searching based on the chunks position.
		std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>> chunkMap;
        
		std::forward_list<const Chunk*> transferChunks;
		std::set<const Chunk*> renderChunks;

	};
}
#endif // !CHUNK_MANAGER_H
