#pragma once
#ifndef HEPHAESTUS_ENGINE_CHUNK_HPP
#define HEPHAESTUS_ENGINE_CHUNK_HPP
#include "common/Constants.hpp"
#include "ecs/entity.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include <array>

struct ChunkComponent {
    std::array<ecs::entity_t, BLOCKS_PER_CHUNK> Blocks;
    glm::vec3 WorldPosition;
    glm::ivec2 GridPosition;
};

class Chunk {
	friend class ChunkManager;
public:

	Chunk(glm::ivec2 grid_position);
	void BuildTerrain(const size_t& terrain_type);
	void BuildMesh();


	void LoadComplete(); // frees cpu data.

};


#endif // !HEPHAESTUS_ENGINE_CHUNK_HPP
