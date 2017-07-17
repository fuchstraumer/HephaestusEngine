#include "stdafx.h"
#include "Chunk.h"

namespace objects {

	// Face normals. Don't change and can be reused. Yay for cubes!
	static const std::array<glm::ivec3, 6> normals = {
		glm::ivec3(0, 0, 1),   // (front)
		glm::ivec3(1, 0, 0),   // (right)
		glm::ivec3(0, 1, 0),   // (top)
		glm::ivec3(-1, 0, 0),   // (left)
		glm::ivec3(0,-1, 0),   // (bottom)
		glm::ivec3(0, 0,-1),   // (back)
	};

	static const std::array<glm::vec3, 8> vertices{
		glm::vec3(x - 0.50f,y - 0.50f,z + 0.50f), // Point 0, left lower front UV{0,0}
		glm::vec3(x + 0.50f,y - 0.50f,z + 0.50f), // Point 1, right lower front UV{1,0}
		glm::vec3(x + 0.50f,y + 0.50f,z + 0.50f), // Point 2, right upper front UV{1,1}
		glm::vec3(x - 0.50f,y + 0.50f,z + 0.50f), // Point 3, left upper front UV{0,1}
		glm::vec3(x + 0.50f,y - 0.50f,z - 0.50f), // Point 4, right lower rear
		glm::vec3(x - 0.50f,y - 0.50f,z - 0.50f), // Point 5, left lower rear
		glm::vec3(x - 0.50f,y + 0.50f,z - 0.50f), // Point 6, left upper rear
		glm::vec3(x + 0.50f,y + 0.50f,z - 0.50f), // Point 7, right upper rear
	};

	static constexpr size_t textures[24][6] = {
		// Each number corresponds to certain face, and thus certain index into texture array. Given as {front, right, top, left, bottom, back}
		// Loading order:: Grass_Top, Grass_Sides, dirt, sand, stone, bedrock
		{ 0,0,0,0,0,0 }, // Air block
		{ 1,1,0,1,2,1 }, // Grass block
		{ 3,3,3,3,3,3 }, // Sand block
		{ 2,2,2,2,2,2 }, // Dirt block
		{ 4,4,4,4,4,4 }, // Stone block
		{ 5,5,5,5,5,5 }, // Bedrock block
		{ 6,6,6,6,6,6 }, // tall grass
		{ 7,7,7,7,7,7 }, // Coal ore
		{ 8,8,8,8,8,8 }, // Iron ore
		{ 9,9,9,9,9,9 }, // Diamond Ore
	};

	Chunk::Chunk(glm::ivec2 gridpos) {
		// Set grid position.
		GridPosition = gridpos;

		// Reserve space in our textures container.
		std::size_t totalBlocks = CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE_Y;
		terrainBlocks.assign(totalBlocks, BlockTypes::AIR);

		// Assign initial zeroed values in our lighting container.
		lightMap.assign(totalBlocks, 0);

		// The gridpos is simply "normalized" world coords to be integral values.
		// The actual position in the world is calculated now - we use this later to offset the chunk using a model matrix
		glm::vec3 float_position;
		float_position.x = this->GridPosition.x * (static_cast<float>(CHUNK_SIZE) / 2.0f);
		float_position.y = 0.0f;
		float_position.z = this->GridPosition.y * (static_cast<float>(CHUNK_SIZE) / 2.0f);

		// Set the updated positions in the mesh (most important), and this chunk (good for reference)
		mesh->position = float_position;
		Position = float_position;

	}

	Chunk::Chunk(Chunk&& other) noexcept : mesh(std::move(other.mesh)), terrainBlocks(std::move(other.terrainBlocks)), blocks(std::move(other.blocks)) {}

	Chunk& Chunk::operator=(Chunk && other) noexcept {
		this->mesh = std::move(other.mesh);
		this->terrainBlocks = std::move(other.terrainBlocks);
		this->blocks = std::move(other.blocks);
		other.clear();
		return *this;
	}

	glm::vec3 Chunk::GetPosFromGrid(glm::ivec2 gridpos) {
		glm::vec3 res;
		res.x = this->GridPosition.x * ((CHUNK_SIZE / 2.0f));
		res.y = static_cast<float>(CHUNK_SIZE_Y) / 2.0f;
		res.z = this->GridPosition.y * (CHUNK_SIZE / 2.0f);
		return res;
	}

	void Chunk::BuildTerrain(const size_t& terrain_type) {
		for (int x = 0; x < CHUNK_SIZE; ++x) {
			for (int z = 0; z < CHUNK_SIZE; ++z) {

			}
		}
	}

	void Chunk::BuildMesh() {
		// Default block adjacency value assumes true
		bool def = true;

		// Iterate through every block in this chunk one-by-one to decide how/if to render it.
		for (int j = 0; j < CHUNK_SIZE_Y - 1; j++) {
			for (int i = 0; i < CHUNK_SIZE - 1; i++) {
				for (int k = 0; k < CHUNK_SIZE - 1; k++) {
					// Get index of the current block we're at.
					int currBlock = GetBlockIndex(i, j, k);

					// If the current block is an air block, we don't need to worry about meshing+rendering it.
					if (this->terrainBlocks[currBlock].GetType() == BlockTypes::AIR) {
						continue;
					}
					// Current block isn't air, lets look at all its adjacent blocks to find out what we need to do next.
					else {
						// The uv_type is simpyly the value of the block at the given point (grabbed from the enum)
						// This is used to index into the texture array, so each block gets the right textures and UVs
						int uv_type;
						uv_type = terrainBlocks[currBlock].GetType();

						// If we are primitively culling invisible faces, run this system
						// Primitive culling merely means that we don't render faces we can't see: this 
						// cuts rendered elements by easily 85-95% compared to no culling at all. 
						if (SIMPLE_CULLING_GLOBAL == true) {

							//// If a face is visible, set that face's value to be false
							//bool xNeg = def; // left
							//if (i > 0) {
							//	if (!this->terrainBlocks[GetBlockIndex(i - 1, j, k)].Active()) {
							//		xNeg = false;
							//	}
							//}

							//bool xPos = def; // right
							//if (i < CHUNK_SIZE - 1) {
							//	if (!this->terrainBlocks[GetBlockIndex(i + 1, j, k)].Active()) {
							//		xPos = false;
							//	}
							//}

							//bool yPos = def; // bottom
							//if (j > 0) {
							//	if (!this->terrainBlocks[GetBlockIndex(i, j - 1, k)].Active()) {
							//		yPos = false;
							//	}
							//}

							//bool yNeg = def; // top
							//if (j < CHUNK_SIZE_Y - 1) {
							//	//std::cerr << GetBlockIndex(i, j - 1, k);
							//	if (!this->terrainBlocks[GetBlockIndex(i, j + 1, k)].Active()) {
							//		yNeg = false;
							//	}
							//}

							//bool zNeg = def; // back
							//if (k < CHUNK_SIZE - 1) {
							//	if (!this->terrainBlocks[GetBlockIndex(i, j, k + 1)].Active()) {
							//		zNeg = false;
							//	}
							//}

							//bool zPos = def; // front
							//if (k > 0) {
							//	if (!this->terrainBlocks[GetBlockIndex(i, j, k - 1)].Active()) {
							//		zPos = false;
							//	}
							//}

							// Create a cube at i,j,k with certain faces rendered
							// Each false value specifies a face that should be visible, and thus should be rendered
							// At the end, we include the type of block and thus what texture it needs
							// this->createCube(i, j, k, zNeg, xPos, yNeg, xNeg, yPos, zPos, uv_type);
						}
						// If we're not doing primitive culling, render every non-air block in the volume regardless of visibility
						else if (SIMPLE_CULLING_GLOBAL == false) {
							this->createCube(i, j, k, false, false, false, false, false, false, uv_type);
						}
					}
				}
			}
		}
	}

	void Chunk::EncodeBlocks() {
		
	}

	void Chunk::clear() {
		// Mesh clear method clears data and calls shrink_to_fit()
		mesh->cleanup();
		// First call clear to empty containers
		terrainBlocks.clear();
		lightMap.clear();
		// Then call shrink to fit to actually free up memory.
		terrainBlocks.shrink_to_fit();
		lightMap.shrink_to_fit();
	}

	int Chunk::GetSunlightLevel(const glm::ivec3 & p) const{
		size_t idx = GetBlockIndex(p);
		return GetFront4(lightMap[idx]);
	}

	int Chunk::GetTorchlightLevel(const glm::ivec3 & p) const{
		size_t idx = GetBlockIndex(p);
		return GetBack4(lightMap[idx]);
	}

	void Chunk::SetSunlightLevel(const glm::ivec3 & p, uint8_t level){
		size_t idx = GetBlockIndex(p);
		SetFront4(lightMap[idx], level);
	}

	void Chunk::SetTorchlightLevel(const glm::ivec3 & p, uint8_t level){
		size_t idx = GetBlockIndex(p);
		SetBack4(lightMap[idx], level);
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
						if (terrainBlocks[GetBlockIndex(x + dx, y + dy + 1, z + dz)].GetType() == BlockTypes::AIR) {
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
		v2.UV = glm::ivec3(0, 1, textures[texture_idx][static_cast<size_t>(face)]);
		v3.UV = glm::ivec3(1, 1, textures[texture_idx][static_cast<size_t>(face)]);
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
		std::array<BlockType, 27> neighbors;
		std::array<float, 27> shades;
		setBlockLightingData(x, y, z, neighbors, shades);

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