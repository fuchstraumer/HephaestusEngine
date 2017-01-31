#include "stdafx.h"
#include "LinearChunk.h"

// Face normals
static const std::vector<glm::ivec3> normals = {
	glm::ivec3(0, 0, 1),   // (front)
	glm::ivec3(1, 0, 0),   // (right)
	glm::ivec3(0, 1, 0),   // (top)
	glm::ivec3(-1, 0, 0),   // (left)
	glm::ivec3(0,-1, 0),   // (bottom)
	glm::ivec3(0, 0,-1),   // (back)
};

// Texture indices
static const float blocks[256][6] = {
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


inline void LinearChunk::createCube(int x, int y, int z, bool frontFace, bool rightFace, bool topFace, bool leftFace, bool bottomFace, bool backFace, int uv_type) {
	// Use a std::array since the data isn't modified, rather it's used like a template to build the individual points from
	// This setup means that the xyz of a given block is actually the center of the block's mesh
	std::array<glm::vec3, 8> vertices{
	  { glm::vec3(x - BLOCK_RENDER_SIZE,y - BLOCK_RENDER_SIZE,z + BLOCK_RENDER_SIZE), // Point 0, left lower front UV{0,0}
		glm::vec3(x + BLOCK_RENDER_SIZE,y - BLOCK_RENDER_SIZE,z + BLOCK_RENDER_SIZE), // Point 1, right lower front UV{1,0}
		glm::vec3(x + BLOCK_RENDER_SIZE,y + BLOCK_RENDER_SIZE,z + BLOCK_RENDER_SIZE), // Point 2, right upper front UV{1,1}
		glm::vec3(x - BLOCK_RENDER_SIZE,y + BLOCK_RENDER_SIZE,z + BLOCK_RENDER_SIZE), // Point 3, left upper front UV{0,1}
		glm::vec3(x + BLOCK_RENDER_SIZE,y - BLOCK_RENDER_SIZE,z - BLOCK_RENDER_SIZE), // Point 4, right lower rear
		glm::vec3(x - BLOCK_RENDER_SIZE,y - BLOCK_RENDER_SIZE,z - BLOCK_RENDER_SIZE), // Point 5, left lower rear
		glm::vec3(x - BLOCK_RENDER_SIZE,y + BLOCK_RENDER_SIZE,z - BLOCK_RENDER_SIZE), // Point 6, left upper rear
		glm::vec3(x + BLOCK_RENDER_SIZE,y + BLOCK_RENDER_SIZE,z - BLOCK_RENDER_SIZE) } // Point 7, right upper rear
	};

	// Given the four points defining a cube's face, and the index into the normal + vertices array, build the mesh data
	// and add the appropriate elements to the mesh.
	auto buildface = [this, uv_type](glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int norm, int face) {
		// We'll need four indices and four vertices for the two tris defining a face.
		index_t i0, i1, i2, i3;
		vertex_t v0, v1, v2, v3;
	
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

		// Assign each vertex it's appropriate UV coords based on the blocks type
		v0.UV = glm::ivec3(0.0, 0.0, blocks[uv_type][face]);
		v1.UV = glm::ivec3(1.0, 0.0, blocks[uv_type][face]);
		v2.UV = glm::ivec3(0.0, 1.0, blocks[uv_type][face]);
		v3.UV = glm::ivec3(1.0, 1.0, blocks[uv_type][face]);

		// Add the verts to the Mesh's vertex container. Returns index to added vert.
		i0 = this->mesh.AddVert(v0);
		i1 = this->mesh.AddVert(v1);
		i2 = this->mesh.AddVert(v2);
		i3 = this->mesh.AddVert(v3);

		// Add the triangles to the mesh, via indices (specifies which vertices to use when rendering)
		this->mesh.AddTriangle(i0, i1, i2); // Needs UVs {0,0}{1,0}{0,1}
		this->mesh.AddTriangle(i0, i2, i3); // Needs UVs {1,0}{0,1}{1,1}
	};

	// This set of if statements checks if we need to actually build a given face before doing so.

	// If the front face of this cube will be visible, build the tris needed for that face
	if (frontFace == false) {
		buildface(vertices[0], vertices[1], vertices[2], vertices[3], 0, 0); // Using Points 0, 1, 2, 3 and Normal 0
	}

	// If right face of this cube will be visible, build elements needed for this face.
	if (rightFace == false) {
		buildface(vertices[1], vertices[4], vertices[7], vertices[2], 1, 1); // Using Points 1, 4, 7, 2 and Normal 1
	}

	if (topFace == false) {
		buildface(vertices[3], vertices[2], vertices[7], vertices[6], 2, 2); // Using Points 3, 2, 7, 6 and Normal 2
	}

	if (leftFace == false) {
		buildface(vertices[5], vertices[0], vertices[3], vertices[6], 3, 3); // Using Points 5, 0, 3, 6 and Normal 3
	}

	if (bottomFace == false) {
		buildface(vertices[5], vertices[4], vertices[1], vertices[0], 4, 4); // Using Points 5, 4, 1, 0 and Normal 4
	}

	if (backFace == false) {
		buildface(vertices[4], vertices[5], vertices[6], vertices[7], 5, 5); // Using Points 4, 5, 6, 7 and Normal 5
	}

}

void LinearChunk::BuildTerrain(TerrainGenerator & gen, int terraintype){
	for (int x = 0; x < CHUNK_SIZE; ++x) {
		for (int z = 0; z < CHUNK_SIZE; ++z) {

			// Recall that XZ is our left/right forward/back, so set every single block at Y = 0 and 
			// with any XZ value to be bedrock, defining the base layer of our world. 
			this->Blocks[GetBlockIndex(x, 0, z)] = blockTypes::BEDROCK;

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
					this->Blocks[GetBlockIndex(x, y - 1, z)] = blockTypes::STONE;
					this->Blocks[currentIndex] = blockTypes::DIRT;
					this->Blocks[GetBlockIndex(x, y + 1, z)] = blockTypes::DIRT;
					this->Blocks[GetBlockIndex(x, y + 2, z)] = blockTypes::GRASS;
				}
			}

			// Billow generator.
			if (terraintype == 1) {
				for (int y = 1; y < static_cast<int>(gen.SimplexBillow(static_cast<int>(Position.x) + x, static_cast<int>(Position.z) + z)); ++y) {
					// Since we start at y = 1 and iterate up the column progressively
					// set the majority of blocks to be stone, blocks 3 above stone to be grass,
					// blocks 2 above stone to be dirt, blocks 1 above to be dirt to form some basic terrain
					int currentIndex = GetBlockIndex(x, y, z);
					this->Blocks[GetBlockIndex(x, y - 1, z)] = blockTypes::STONE;
					this->Blocks[currentIndex] = blockTypes::DIRT;
					this->Blocks[GetBlockIndex(x, y + 1, z)] = blockTypes::DIRT;
					this->Blocks[GetBlockIndex(x, y + 2, z)] = blockTypes::GRASS;
				}
			}

			// Ridged generator.
			if (terraintype == 2) {
				for (int y = 1; y < static_cast<int>(gen.SimplexRidged(static_cast<int>(Position.x) + x, static_cast<int>(Position.z) + z)); ++y) {
					// Since we start at y = 1 and iterate up the column progressively
					// set the majority of blocks to be stone, blocks 3 above stone to be grass,
					// blocks 2 above stone to be dirt, blocks 1 above to be dirt to form some basic terrain
					int currentIndex = GetBlockIndex(x, y, z);
					this->Blocks[GetBlockIndex(x, y - 1, z)] = blockTypes::STONE;
					this->Blocks[currentIndex] = blockTypes::DIRT;
					this->Blocks[GetBlockIndex(x, y + 1, z)] = blockTypes::DIRT;
					this->Blocks[GetBlockIndex(x, y + 2, z)] = blockTypes::GRASS;
				}
			}
		}
	}
}

void LinearChunk::BuildMesh(){

	// Default block adjacency value assumes true
	bool def = true;

	// Reserve some space in our containers, hopefully cutting down on costly re-allocations.
	mesh.Indices.reserve(60000); 
	mesh.Vertices.reserve(80000);

	// Iterate through every block in this chunk one-by-one to decide how/if to render it.
	for (int j = 0; j < CHUNK_SIZE_Z - 1; j++) {
		for (int i = 0; i < CHUNK_SIZE - 1; i++) {
			for (int k = 0; k < CHUNK_SIZE - 1; k++) {
				// Get index of the current block we're at.
				int currBlock = GetBlockIndex(i, j, k);

				// If the current block is an air block, we don't need to worry about meshing+rendering it.
				if (this->Blocks[currBlock] == blockTypes::AIR) {
					continue;
				}
				// Current block isn't air, lets look at all its adjacent blocks to find out what we need to do next.
				else {
					// The uv_type is simpyly the value of the block at the given point (grabbed from the enum)
					// This is used to index into the texture array, so each block gets the right textures and UVs
					int uv_type;
					uv_type = Blocks[currBlock];

					// If we are primitively culling invisible faces, run this system
					// Primitive culling merely means that we don't render faces we can't see: this 
					// cuts rendered elements by easily 85-95% compared to no culling at all. 
					if (SIMPLE_CULLING_GLOBAL == true) {

						// If a face is visible, set that face's value to be false
						bool xNeg = def; // left
						if (i > 0) {
							if (this->Blocks[GetBlockIndex(i - 1, j, k)] == blockTypes::AIR) {
								xNeg = false;
							}
						}

						bool xPos = def; // right
						if (i < CHUNK_SIZE - 1) {
							if (this->Blocks[GetBlockIndex(i + 1, j, k)] == blockTypes::AIR) {
								xPos = false;
							}
						}

						bool yPos = def; // bottom
						if (j > 0) {
							if (this->Blocks[GetBlockIndex(i, j - 1, k)] == blockTypes::AIR) {
								yPos = false;
							}
						}

						bool yNeg = def; // top
						if (j < CHUNK_SIZE_Z - 1) {
							//std::cerr << GetBlockIndex(i, j - 1, k);
							if (this->Blocks[GetBlockIndex(i, j + 1, k)] == blockTypes::AIR) {
								yNeg = false;
							}
						}

						bool zNeg = def; // back
						if (k < CHUNK_SIZE - 1) {
							if (this->Blocks[GetBlockIndex(i, j, k + 1)] == blockTypes::AIR) {
								zNeg = false;
							}
						}

						bool zPos = def; // front
						if (k > 0) {
							if (this->Blocks[GetBlockIndex(i, j, k - 1)] == blockTypes::AIR) {
								zPos = false;
							}
						}

						// Create a cube at i,j,k with certain faces rendered
						// Each false value specifies a face that should be visible, and thus should be rendered
						// At the end, we include the type of block and thus what texture it needs
						this->createCube(i, j, k, zNeg, xPos, yNeg, xNeg, yPos, zPos, uv_type);
					}

					// If we're not doing primitive culling, render every non-air block in the volume regardless of visibility
					// WARNING: This is really slow, don't use it with more than one chunk unless you want to see your RAM (and VRAM!) disappear!
					else if (SIMPLE_CULLING_GLOBAL == false) {
						this->createCube(i, j, k, false, false, false, false, false, false, uv_type);
					}
				}
			}
		}
	}

	// If we over-allocated room in our containers, time to release that memory so we can some RAM.
	mesh.Indices.shrink_to_fit(); 
	mesh.Vertices.shrink_to_fit();
}

