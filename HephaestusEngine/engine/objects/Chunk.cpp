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

	/*

		Texture indices array:

		Each block will always use the same UV coordinates: recall that texture coordinates
		are usually 2D coordinates, (u, v). The only component that will change is the third component:
		our UV coords are unconventially 3D because our 3rd component is how we select different textures.
		Instead of binding a different texture for each block, we use a texture array. We index through
		this array using this 3rd component, like it was the index into a regular ol array or vector.

		Note that even among blocks there is variation in these indices. This is because some blocks have
		different textures on each face: grass blocks, for example, use the pure grass texture only on
		the upper face, and actually re-use the dirt texture on their bottom face.

	*/

	static constexpr unsigned int textures[256][6] = {
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

	// Build the mesh data for a cube at position XYZ, building the faces specified by each boolean if that boolean is false (false = there isn't another block in this location)
	void Chunk::createCube(int x, int y, int z, bool frontFace, bool rightFace, bool topFace, bool leftFace, bool bottomFace, bool backFace, int uv_type) {
		// Use a std::array since the data isn't modified, rather it's used like a template to build the individual points from
		// This setup means that the xyz of a given block is actually the center of the block's mesh
		std::array<glm::vec3, 8> vertices{
		    glm::vec3(x - 0.50f,y - 0.50f,z + 0.50f), // Point 0, left lower front UV{0,0}
			glm::vec3(x + 0.50f,y - 0.50f,z + 0.50f), // Point 1, right lower front UV{1,0}
			glm::vec3(x + 0.50f,y + 0.50f,z + 0.50f), // Point 2, right upper front UV{1,1}
			glm::vec3(x - 0.50f,y + 0.50f,z + 0.50f), // Point 3, left upper front UV{0,1}
			glm::vec3(x + 0.50f,y - 0.50f,z - 0.50f), // Point 4, right lower rear
			glm::vec3(x - 0.50f,y - 0.50f,z - 0.50f), // Point 5, left lower rear
			glm::vec3(x - 0.50f,y + 0.50f,z - 0.50f), // Point 6, left upper rear
			glm::vec3(x + 0.50f,y + 0.50f,z - 0.50f), // Point 7, right upper rear
		};

		// Following method for generating lighting data from:
		// https://github.com/fogleman/Craft/blob/master/src/main.c#L1077
		std::array<BlockType, 27> neighbors;
		std::array<float, 27> shades;
		size_t idx = 0;
		if (x > 0 && y > 0 && z > 0 && x < CHUNK_SIZE && y < CHUNK_SIZE_Y && z < CHUNK_SIZE) {
			for (int dx = -1; dx <= 1; ++dx) {
				for (int dy = -1; dy <= 1; ++dy) {
					for (int dz = -1; dz <= 1; ++dz) {
						neighbors[idx] = terrainBlocks[GetBlockIndex(x + dx, y + dy, z + dz)].GetType();
						shades[idx] = 0.0f;
						// If current block is the topmost in this column, do the following.
						if (terrainBlocks[GetBlockIndex(x + dx, y + dy + 1, z + dz)].GetType() == BlockTypes::AIR) {
							for (int offset_y = 0; offset_y < 8; ++offset_y) {
								if (terrainBlocks[GetBlockIndex(x + dx, y + dy + offset_y, z + dz)].Opaque()) {
									shades[idx] = 1.0f - (static_cast<float>(offset_y) * 0.125f);
									break;
								}
							}
						}
					}
				}
				idx++;
			}
		}

		// Builds a side of a cube
		glm::vec3 blockPosition = glm::vec3(x, y, z);
		auto buildface = [this, uv_type, blockPosition, vertices](index_t i00, index_t i01, index_t i02, index_t i03, int norm, int face) {
			// Get points from input indices into pre-built vertex array
			glm::vec3 p0, p1, p2, p3;
			p0 = vertices[i00];
			p1 = vertices[i01];
			p2 = vertices[i02];
			p3 = vertices[i03];

			// We'll need four indices and four vertices for the two tris defining a face.
			index_t i0, i1, i2, i3;
			block_vertex_t v0, v1, v2, v3;

			// Assign each vertex it's appropriate UV coords based on the blocks type
			glm::vec3 uv0 = glm::ivec3(0.0, 0.0, textures[uv_type][face]);
			glm::vec3 uv1 = glm::ivec3(1.0, 0.0, textures[uv_type][face]);
			glm::vec3 uv3 = glm::ivec3(0.0, 1.0, textures[uv_type][face]);
			glm::vec3 uv2 = glm::ivec3(1.0, 1.0, textures[uv_type][face]);
			v0.UV = uv0;
			v1.UV = uv1;
			v2.UV = uv2;
			v3.UV = uv3;

			// Set the vertex positions.
			v0.Position.xyz = p0;
			v1.Position.xyz = p1;
			v2.Position.xyz = p2;
			v3.Position.xyz = p3;

			// Set vertex normals.
			v0.Normal = normals[norm];
			v1.Normal = normals[norm];
			v2.Normal = normals[norm];
			v3.Normal = normals[norm];

		};

		// The following statements build a face based on the boolean value given to this method,
		// using the lambda (inline) function above.
		if (frontFace == false) {
			buildface(0, 1, 2, 3, 0, 0); // Using Points 0, 1, 2, 3 and Normal 0
		}
		if (rightFace == false) {
			buildface(1, 4, 7, 2, 1, 1); // Using Points 1, 4, 7, 2 and Normal 1
		}
		if (topFace == false) {
			buildface(3, 2, 7, 6, 2, 2); // Using Points 3, 2, 7, 6 and Normal 2
		}

		if (leftFace == false) {
			buildface(5, 0, 3, 6, 3, 3); // Using Points 5, 0, 3, 6 and Normal 3
		}

		if (bottomFace == false) {
			buildface(5, 4, 1, 0, 4, 4); // Using Points 5, 4, 1, 0 and Normal 4
		}

		if (backFace == false) {
			buildface(4, 5, 6, 7, 5, 5); // Using Points 4, 5, 6, 7 and Normal 5
		}

	}

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
		if (this != &other) {
			this->mesh = std::move(other.mesh);
			this->terrainBlocks = std::move(other.terrainBlocks);
			this->blocks = std::move(other.blocks);
			other.clear();
		}
		return *this;
	}

	glm::vec3 Chunk::GetPosFromGrid(glm::ivec2 gridpos) {
		glm::vec3 res;
		res.x = this->GridPosition.x * ((CHUNK_SIZE / 2.0f));
		res.y = static_cast<float>(CHUNK_SIZE_Y) / 2.0f;
		res.z = this->GridPosition.y * (CHUNK_SIZE / 2.0f);
		return res;
	}

	void Chunk::BuildTerrain(terrain::GeneratorBase& gen, int terraintype) {
		for (int x = 0; x < CHUNK_SIZE; ++x) {
			for (int z = 0; z < CHUNK_SIZE; ++z) {

				// Recall that XZ is our left/right forward/back, so set every single block at Y = 0 and 
				// with any XZ value to be bedrock, defining the base layer of our world. 
				this->terrainBlocks[GetBlockIndex(x, 0, z)] = BlockTypes::BEDROCK;

				/*

				Following terrain generation method is fast but primitive. We query the noise generator for a value,
				and use that value as a height value. We iterate from above the bedrock layer (Y=1) to whatever this
				height value is.

				*/

				// If we're using the FBM generator, use this branch.
				if (terraintype == 0) {
					for (int y = 1; y < static_cast<int>(gen.SimplexFBM(static_cast<int>(Position.x) + x, static_cast<int>(Position.z) + z)); ++y) {
						// Since we start at y = 1 and iterate up the column progressively
						// set the majority of blocks to be stone, blocks 3 above stone to be grass,
						// blocks 2 above stone to be dirt, blocks 1 above to be dirt to form some basic terrain
						int currentIndex = GetBlockIndex(x, y, z);
						this->terrainBlocks[GetBlockIndex(x, y - 1, z)] = Block(BlockTypes::STONE);
						this->terrainBlocks[currentIndex] = Block(BlockTypes::DIRT);
						this->terrainBlocks[GetBlockIndex(x, y + 1, z)] = Block(BlockTypes::DIRT);
						this->terrainBlocks[GetBlockIndex(x, y + 2, z)] = Block(BlockTypes::GRASS);
					}
				}

				// Billow generator.
				if (terraintype == 1) {
					for (int y = 1; y < static_cast<int>(gen.SimplexBillow(static_cast<int>(Position.x) + x, static_cast<int>(Position.z) + z)); ++y) {
						// Since we start at y = 1 and iterate up the column progressively
						// set the majority of blocks to be stone, blocks 3 above stone to be grass,
						// blocks 2 above stone to be dirt, blocks 1 above to be dirt to form some basic terrain
						int currentIndex = GetBlockIndex(x, y, z);
						this->terrainBlocks[GetBlockIndex(x, y - 1, z)] = Block(BlockTypes::STONE);
						this->terrainBlocks[currentIndex] = Block(BlockTypes::DIRT);
						this->terrainBlocks[GetBlockIndex(x, y + 1, z)] = Block(BlockTypes::DIRT);
						this->terrainBlocks[GetBlockIndex(x, y + 2, z)] = Block(BlockTypes::GRASS);
					}
				}

				// Ridged generator.
				if (terraintype == 2) {
					for (int y = 1; y < static_cast<int>(gen.SimplexRidged(static_cast<int>(Position.x) + x, static_cast<int>(Position.z) + z)); ++y) {
						// Since we start at y = 1 and iterate up the column progressively
						// set the majority of blocks to be stone, blocks 3 above stone to be grass,
						// blocks 2 above stone to be dirt, blocks 1 above to be dirt to form some basic terrain
						int currentIndex = GetBlockIndex(x, y, z);
						this->terrainBlocks[GetBlockIndex(x, y - 1, z)] = Block(BlockTypes::STONE);
						this->terrainBlocks[currentIndex] = Block(BlockTypes::DIRT);
						this->terrainBlocks[GetBlockIndex(x, y + 1, z)] = Block(BlockTypes::DIRT);
						this->terrainBlocks[GetBlockIndex(x, y + 2, z)] = Block(BlockTypes::GRASS);
					}
				}
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

}