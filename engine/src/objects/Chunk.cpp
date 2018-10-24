#include "objects/Chunk.hpp"

Chunk::Chunk(glm::ivec2 gridpos) : GridPosition(gridpos) {

	terrainBlocks.fill(Block(BlockTypes::AIR));

	Position = glm::vec3{ 
        GridPosition.x * (static_cast<float>(CHUNK_SIZE)),
        0.0f,
        GridPosition.y * (static_cast<float>(CHUNK_SIZE))
    };

}

void Chunk::BuildTerrain(const size_t& terrain_type) {
	for (int x = 0; x < CHUNK_SIZE; ++x) {
		for (int z = 0; z < CHUNK_SIZE; ++z) {
			terrainBlocks[GetBlockIndex(x, 0, z)].SetType(static_cast<uint8_t>(BlockTypes::BEDROCK));
			double height = noiseGen.Sample(static_cast<double>(x) + Position.x, static_cast<double>(z) + Position.z) * 10.0f;
			height += 50.0f;
			height = floor(height);
			for (int y = 1; y < static_cast<int>(height); ++y) {
				terrainBlocks[GetBlockIndex(x, y, z)].SetType(static_cast<uint8_t>(BlockTypes::STONE));
			}
		}
	}
}

void Chunk::CreateMeshBuffers(const vulpes::Device * _device) {
	mesh->create_buffers(_device);
}

void Chunk::LoadComplete() {
	if (mesh) {
		mesh->free_cpu_data();
	}
}
