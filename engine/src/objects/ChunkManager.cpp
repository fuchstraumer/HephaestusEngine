#include "ecs/registry.hpp"
#include "objects/ChunkManager.hpp"

struct area_t {
	glm::ivec2 min, max;
};

ChunkManager::ChunkManager( const size_t & init_view_radius) : renderRadius(init_view_radius) {}

ecs::entity_t ChunkManager::CreateChunk(const glm::ivec2& grid_position) {
		
	//auto new_chunk = std::make_shared<Chunk>(grid_position);
	//new_chunk->BuildTerrain(0);

}

void ChunkManager::Init(const glm::vec3 & initial_position, const int & view_distance) {

	area_t area;
	
	for (int i = 0; i < view_distance; ++i) {
		area.min = glm::ivec2(static_cast<int>(initial_position.x) - i, static_cast<int>(initial_position.y) - i);
		area.max = glm::ivec2(static_cast<int>(initial_position.x) + i, static_cast<int>(initial_position.y) + i);

		for (int x = area.min.x; x < area.max.x; ++x) {
			for (int y = area.min.y; y < area.max.y; ++y) {
				CreateChunk(glm::ivec2(x, y));
			}
		}
	}

}

void ChunkManager::SetRenderDistance(const size_t& render_distance) {
	renderRadius = render_distance;
}

size_t ChunkManager::GetRenderDistance() const noexcept {
	return renderRadius;
}

void ChunkManager::Update(const glm::vec3 & update_position) {

	glm::ivec2 camera_chunk_pos = glm::ivec2(static_cast<int>(update_position.x) / CHUNK_SIZE, static_cast<int>(update_position.z) / CHUNK_SIZE);

	{
			
		area_t area;
		for (size_t i = 0; i < renderRadius; ++i) {
			area.min = camera_chunk_pos - static_cast<int>(i);
			area.max = camera_chunk_pos + static_cast<int>(i);

			for (int x = area.min.x; x < area.max.x; ++x) {
				for (int y = area.min.y; y < area.max.y; ++y) {
                    glm::ivec2 chunk_pos{ x, y };
					if (chunkMap.count(chunk_pos) == 0) {
						chunkMap.emplace(chunk_pos, CreateChunk(chunk_pos));
					}
				}
			}
		}

	}

	{

		area_t area;
		area.min = camera_chunk_pos - static_cast<int>(renderRadius);
		area.max = camera_chunk_pos + static_cast<int>(renderRadius);

		auto iter = chunkMap.begin();
		while(iter != chunkMap.end()) {
			const auto& pos = iter->second->GridPosition;
			if (pos.x <= area.min.x || pos.y <= area.min.y || pos.x >= area.max.x || pos.y >= area.max.y) {
				renderChunks.erase((iter++)->second);
				chunkMap.erase(pos);
			}
			else {
				++iter;
			}
		}
	}
}
