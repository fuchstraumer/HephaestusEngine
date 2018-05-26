#include "stdafx.h"
#include "engine/objects/Chunk.hpp"

namespace objects {


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

	void Chunk::BuildMesh() {

		// Iterate through every block in this chunk one-by-one to decide how/if to render it.
		for (size_t j = 0; j < CHUNK_SIZE_Y; j++) {
			for (size_t i = 0; i < CHUNK_SIZE; i++) {
				for (size_t k = 0; k < CHUNK_SIZE; k++) {
					// Get index of the current block we're at.
					size_t currBlock = GetBlockIndex(i, j, k);

					if (!terrainBlocks[currBlock].Active()) {
						continue;
					}
					else {
						// The uv_type is simpyly the value of the block at the given point (grabbed from the enum)
						// This is used to index into the texture array, so each block gets the right textures and UVs
						int uv_type;
						uv_type = terrainBlocks[currBlock].GetType();

						// If we are primitively culling invisible faces, run this system
						// Primitive culling merely means that we don't render faces we can't see: this 
						// cuts rendered elements by easily 85-95% compared to no culling at all. 
						if (SIMPLE_CULLING_GLOBAL) {

							bool xNeg = true; // left
							if (i > 0) {
								if (!this->terrainBlocks[GetBlockIndex(i - 1, j, k)].Active()) {
									xNeg = false;
								}
							}

							bool xPos = true; // right
							if (i < CHUNK_SIZE - 1) {
								if (!this->terrainBlocks[GetBlockIndex(i + 1, j, k)].Active()) {
									xPos = false;
								}
							}

							bool yPos = true; // bottom
							if (j > 0) {
								if (!this->terrainBlocks[GetBlockIndex(i, j - 1, k)].Active()) {
									yPos = false;
								}
							}

							bool yNeg = true; // top
							if (j < CHUNK_SIZE_Y - 1) {
								//std::cerr << GetBlockIndex(i, j - 1, k);
								if (!this->terrainBlocks[GetBlockIndex(i, j + 1, k)].Active()) {
									yNeg = false;
								}
							}

							bool zNeg = true; // back
							if (k < CHUNK_SIZE - 1) {
								if (!this->terrainBlocks[GetBlockIndex(i, j, k + 1)].Active()) {
									zNeg = false;
								}
							}

							bool zPos = true; // front
							if (k > 0) {
								if (!this->terrainBlocks[GetBlockIndex(i, j, k - 1)].Active()) {
									zPos = false;
								}
							}

							 this->createCube(i, j, k, zNeg, xPos, yNeg, xNeg, yPos, zPos, uv_type);
						}
						else {
							this->createCube(i, j, k, false, false, false, false, false, false, uv_type);
						}
					}
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

}