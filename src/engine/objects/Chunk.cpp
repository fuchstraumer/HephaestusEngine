#include "stdafx.h"
#include "engine/objects/Chunk.hpp"

namespace objects {

	// Face normals. Don't change and can be reused. Yay for cubes!
	static const std::array<glm::ivec3, 6> normals = {
		glm::ivec3( 0, 0, 1),   // (front)
		glm::ivec3( 1, 0, 0),   // (right)
		glm::ivec3( 0, 1, 0),   // (top)
		glm::ivec3(-1, 0, 0),   // (left)
		glm::ivec3( 0,-1, 0),   // (bottom)
		glm::ivec3( 0, 0,-1),   // (back)
	};

	static const std::array<glm::vec3, 8> vertices{
		glm::vec3(-0.50f,-0.50f, 0.50f), // Point 0, left lower front
		glm::vec3( 0.50f,-0.50f, 0.50f), // Point 1, right lower front
		glm::vec3( 0.50f, 0.50f, 0.50f), // Point 2, right upper front 
		glm::vec3(-0.50f, 0.50f, 0.50f), // Point 3, left upper front
		glm::vec3( 0.50f,-0.50f,-0.50f), // Point 4, right lower rear
		glm::vec3(-0.50f,-0.50f,-0.50f), // Point 5, left lower rear
		glm::vec3(-0.50f, 0.50f,-0.50f), // Point 6, left upper rear
		glm::vec3( 0.50f, 0.50f,-0.50f), // Point 7, right upper rear
	};

	
	static const std::array<std::initializer_list<uint32_t>, 6> face_indices{
		std::initializer_list<uint32_t>{ 5, 4, 1, 0 }, // Bottom
		std::initializer_list<uint32_t>{ 3, 2, 7, 6 }, // Top
		std::initializer_list<uint32_t>{ 4, 5, 6, 7 }, // Back
		std::initializer_list<uint32_t>{ 0, 1, 2, 3 }, // Front 
		std::initializer_list<uint32_t>{ 5, 0, 3, 6 }, // Left
		std::initializer_list<uint32_t>{ 1, 4, 7, 2 } // Right
	};

	static constexpr size_t textures[24][6] = {
		// Each number corresponds to certain face, and thus certain index into texture array. Given as {front, right, top, left, bottom, back}
		// Current array order: Bedrock, Grass Top, Grass Sides, Dirt, Stone, Gravel, Sand, Cobble, Coal, Iron, Gold, Diamond, Emerald, Log, Log Top,
		// Leaves, Planks, Glass, Stonebricks, Bricks, Tall grass, Fern, Flower, Grass Lower, Grass Upper,
		{  0, 0, 0, 0, 0, 0 }, // Bedrock block
		{  1, 1, 1, 1, 1, 1 }, // Stone
		{  2, 2, 1, 2, 3, 2 }, // Grass block
		{  5, 5, 5, 5, 5, 5 }, // Sand block
		{  3, 3, 3, 3, 3, 3 }, // Dirt block
		{  5, 5, 5, 5, 5, 5 }, // Gravel
		{  6, 6, 6, 6, 6, 6 }, // Sand
		{  7, 7, 7, 7, 7, 7 }, // Cobblestone
		{  8, 8, 8, 8, 8, 8 }, // Coal
		{  9, 9, 9, 9, 9, 9 }, // Iron
		{ 10,10,10,10,10,10 }, // Gold
		{ 11,11,11,11,11,11 }, // Diamond
		{ 12,12,12,12,12,12 }, // Emerald
		{ 20,20,20,20,20,20 }, // tall grass
	};


	Chunk::Chunk(glm::ivec2 gridpos) : mesh(std::make_unique<mesh::Mesh<mesh::BlockVertices, block_vertex_t>>()) {
		// Set grid position.
		GridPosition = gridpos;

		// Reserve space in our textures container.
		std::size_t totalBlocks = CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE_Y;
		terrainBlocks.fill(Block(BlockTypes::AIR));

		// The gridpos is simply "normalized" world coords to be integral values.
		// The actual position in the world is calculated now - we use this later to offset the chunk using a model matrix
		glm::vec3 float_position;
		float_position.x = GridPosition.x * (static_cast<float>(CHUNK_SIZE));
		float_position.y = 0.0f;
		float_position.z = GridPosition.y * (static_cast<float>(CHUNK_SIZE));

		// Set the updated positions in the mesh (most important), and this chunk (good for reference)
		mesh->position = float_position;
		Position = float_position;
		mesh->update_model_matrix();

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
		// Default block adjacency value assumes true
		bool def = true;

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

							bool xNeg = def; // left
							if (i > 0) {
								if (!this->terrainBlocks[GetBlockIndex(i - 1, j, k)].Active()) {
									xNeg = false;
								}
							}

							bool xPos = def; // right
							if (i < CHUNK_SIZE - 1) {
								if (!this->terrainBlocks[GetBlockIndex(i + 1, j, k)].Active()) {
									xPos = false;
								}
							}

							bool yPos = def; // bottom
							if (j > 0) {
								if (!this->terrainBlocks[GetBlockIndex(i, j - 1, k)].Active()) {
									yPos = false;
								}
							}

							bool yNeg = def; // top
							if (j < CHUNK_SIZE_Y - 1) {
								//std::cerr << GetBlockIndex(i, j - 1, k);
								if (!this->terrainBlocks[GetBlockIndex(i, j + 1, k)].Active()) {
									yNeg = false;
								}
							}

							bool zNeg = def; // back
							if (k < CHUNK_SIZE - 1) {
								if (!this->terrainBlocks[GetBlockIndex(i, j, k + 1)].Active()) {
									zNeg = false;
								}
							}

							bool zPos = def; // front
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

	void Chunk::setBlockLightingData(const uint32_t& x, const uint32_t& y, const uint32_t& z, std::array<BlockType, 27>& neighbor_blocks, std::array<float, 27>& neighbor_shades) const {
		size_t idx = 0;
		if (x > 0 && y > 0 && z > 0 && x < CHUNK_SIZE && y < CHUNK_SIZE_Y && z < CHUNK_SIZE) {
			for (int dx = -1; dx <= 1; ++dx) {
				for (int dy = -1; dy <= 1; ++dy) {
					for (int dz = -1; dz <= 1; ++dz) {
						neighbor_blocks[idx] = terrainBlocks[GetBlockIndex(x + dx, y + dy, z + dz)].GetType();
						neighbor_shades[idx] = 0.0f;
						// If current block is the topmost in this column, do the following.
						if (terrainBlocks[GetBlockIndex(x + dx, y + dy + 1, z + dz)].Active()) {
							for (int offset_y = 0; offset_y < 8; ++offset_y) {
								if (terrainBlocks[GetBlockIndex(x + dx, y + dy + offset_y, z + dz)].Opaque()) {
									neighbor_shades[idx] = 1.0f - (static_cast<float>(offset_y) * 0.125f);
									break;
								}
							}
						}
					}
				}
				idx++;
			}
		}
	}


	void Chunk::getFaceVertices(const blockFace & face, block_vertex_t & v0, block_vertex_t & v1, block_vertex_t & v2, block_vertex_t & v3, const size_t& texture_idx) {
		switch (face) {
		case blockFace::BOTTOM:
			v0.Position = vertices[5];
			v1.Position = vertices[4];
			v2.Position = vertices[1];
			v3.Position = vertices[0];
			break;
		case blockFace::TOP:
			v0.Position = vertices[3];
			v1.Position = vertices[2];
			v2.Position = vertices[7];
			v3.Position = vertices[6];
			break;
		case blockFace::BACK:
			v0.Position = vertices[4];
			v1.Position = vertices[5];
			v2.Position = vertices[6];
			v3.Position = vertices[7];
			break;
		case blockFace::FRONT:
			v0.Position = vertices[0];
			v1.Position = vertices[1];
			v2.Position = vertices[2];
			v3.Position = vertices[3];
			break;
		case blockFace::LEFT:
			v0.Position = vertices[5];
			v1.Position = vertices[0];
			v2.Position = vertices[3];
			v3.Position = vertices[6];
			break;
		case blockFace::RIGHT:
			v0.Position = vertices[1];
			v1.Position = vertices[4];
			v2.Position = vertices[7];
			v3.Position = vertices[2];
			break;
		default:
			throw std::runtime_error("Tried to create face with invalid face type enum value");
		}

		v0.Normal = v1.Normal = v2.Normal = v3.Normal = normals[static_cast<size_t>(face)];
		v0.UV = glm::ivec3(0, 0, textures[texture_idx][static_cast<size_t>(face)]);
		v1.UV = glm::ivec3(1, 0, textures[texture_idx][static_cast<size_t>(face)]);
		v2.UV = glm::ivec3(1, 1, textures[texture_idx][static_cast<size_t>(face)]);
		v3.UV = glm::ivec3(0, 1, textures[texture_idx][static_cast<size_t>(face)]);
	}

	void Chunk::createBlockFace(const blockFace& face, const size_t& uv_idx, const glm::vec3 & pos) {
		block_vertex_t v0, v1, v2, v3;

		getFaceVertices(face, v0, v1, v2, v3, uv_idx);

		v0.Position += pos;
		v1.Position += pos;
		v2.Position += pos;
		v3.Position += pos;

		index_t i0, i1, i2, i3;
		i0 = mesh->add_vertex(std::move(v0));
		i1 = mesh->add_vertex(std::move(v1));
		i2 = mesh->add_vertex(std::move(v2));
		i3 = mesh->add_vertex(std::move(v3));

		mesh->add_triangle(i0, i1, i2);
		mesh->add_triangle(i0, i2, i3);

	}

	void Chunk::createCube(const size_t & x, const size_t & y, const size_t & z, const bool & front_face, const bool & right_face, const bool & top_face, const bool & left_face, 
		const bool & bottom_face, const bool & back_face, const size_t & uv_idx) {

		// Following method for generating lighting data from:
		// https://github.com/fogleman/Craft/blob/master/src/main.c#L1077
		/*std::array<BlockType, 27> neighbors;
		std::array<float, 27> shades;
		setBlockLightingData(x, y, z, neighbors, shades);*/

		// Builds a side of a cube
		glm::vec3 block_pos = glm::vec3(x, y, z);

		if (front_face == false) {
			createBlockFace(blockFace::FRONT, uv_idx, block_pos);
		}

		if (right_face == false) {
			createBlockFace(blockFace::RIGHT, uv_idx, block_pos);
		}

		if (top_face == false) {
			createBlockFace(blockFace::TOP, uv_idx, block_pos);
		}

		if (left_face == false) {
			createBlockFace(blockFace::LEFT, uv_idx, block_pos);
		}

		if (bottom_face == false) {
			createBlockFace(blockFace::BOTTOM, uv_idx, block_pos);
		}

		if (back_face == false) {
			createBlockFace(blockFace::BACK, uv_idx, block_pos);
		}

	}

}