#include "MortonChunk.h"
#include "./SIMD/SIMD_VecSet.h"
#include <ctime>`
// Face normals
static const std::vector<glm::vec3> normals = {
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
	{ NULL,NULL,NULL,NULL,NULL,NULL }, // Air block
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


inline void MortonChunk::createCube(int x, int y, int z, bool frontFace, bool rightFace, bool topFace, bool leftFace, bool bottomFace, bool backFace, int uv_type) {
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
	auto buildface = [this, uv_type](glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int norm, int face) {
		// We'll need four indices and four vertices for the two tris defining a face.
		index_t i0, i1, i2, i3;
		vertType v0, v1, v2, v3;
		// Assign each vertex it's appropriate UV coords based on the blocks type
		glm::vec3 uv0 = glm::vec3(0.0, 0.0, blocks[uv_type][face]);
		glm::vec3 uv1 = glm::vec3(1.0, 0.0, blocks[uv_type][face]);
		glm::vec3 uv3 = glm::vec3(0.0, 1.0, blocks[uv_type][face]);
		glm::vec3 uv2 = glm::vec3(1.0, 1.0, blocks[uv_type][face]);
		v0.uv = uv0; v1.uv = uv1; v2.uv = uv2; v3.uv = uv3;
		// Set the vertex positions.
		v0.position.xyz = p0;
		v1.position.xyz = p1;
		v2.position.xyz = p2;
		v3.position.xyz = p3;
		// Set vertex normals.
		v0.normal = normals[norm];
		v1.normal = normals[norm];
		v2.normal = normals[norm];
		v3.normal = normals[norm];
		// Add the verts to the Mesh's vertex container. Returns index to added vert.
		i0 = this->chunkMesh.addVert(v0);
		i1 = this->chunkMesh.addVert(v1);
		i2 = this->chunkMesh.addVert(v2);
		i3 = this->chunkMesh.addVert(v3);
		// Add the triangles to the mesh, via indices
		this->chunkMesh.addTriangle(i0, i1, i2); // Needs UVs {0,0}{1,0}{0,1}
		this->chunkMesh.addTriangle(i0, i2, i3); // Needs UVs {1,0}{0,1}{1,1}
	};
	// If the frontface of this cube will be visible, build the tris needed for that face
	if (frontFace == false) {
		buildface(vertices[0], vertices[1], vertices[2], vertices[3], 0, 0); // Using Points 0, 1, 2, 3 and Normal 0
	}
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

void MortonChunk::BuildTerrain(TerrainGenerator & gen, int terraintype){
	for (int x = 0; x < CHUNK_SIZE; ++x) {
		for (int z = 0; z < CHUNK_SIZE; ++z) {
			this->Blocks[GetBlockIndex(x, 0, z)] = blockTypes::BEDROCK;
				if (terraintype == 0) {
					for (int y = 1; y < gen.SimplexFBM(this->Position.x + x, this->Position.z + z); ++y) {
						// Since we start at y = 1 and iterate up the column progressively
						// set the majority of blocks to be stone, blocks 3 above stone to be grass,
						// blocks 2 above stone to be dirt, blocks 1 above to be dirt to form some basic terrain
						uint32_t currentIndex = GetBlockIndex(x, y, z);
						this->Blocks[currentIndex + negYDelta(y)] = blockTypes::STONE;
						this->Blocks[currentIndex] = blockTypes::DIRT;
						this->Blocks[currentIndex + posYDelta(y)] = blockTypes::GRASS;
					}
				}
				if (terraintype == 1) {
					for (int y = 1; y < gen.SimplexBillow(this->Position.x + x, this->Position.z + z); ++y) {
						// Since we start at y = 1 and iterate up the column progressively
						// set the majority of blocks to be stone, blocks 3 above stone to be grass,
						// blocks 2 above stone to be dirt, blocks 1 above to be dirt to form some basic terrain
						uint32_t currentIndex = GetBlockIndex(x, y, z);
						this->Blocks[currentIndex + negYDelta(y)] = blockTypes::STONE;
						this->Blocks[currentIndex] = blockTypes::DIRT;
						this->Blocks[currentIndex + posYDelta(y)] = blockTypes::GRASS;
					}
				}
				if (terraintype == 2) {
					for (int y = 1; y < gen.SimplexRidged(this->Position.x + x, this->Position.z + z); ++y) {
						// Since we start at y = 1 and iterate up the column progressively
						// set the majority of blocks to be stone, blocks 3 above stone to be grass,
						// blocks 2 above stone to be dirt, blocks 1 above to be dirt to form some basic terrain
						uint32_t currentIndex = GetBlockIndex(x, y, z);
						this->Blocks[currentIndex + negYDelta(y)] = blockTypes::STONE;
						this->Blocks[currentIndex] = blockTypes::DIRT;
						this->Blocks[currentIndex + posYDelta(y)] = blockTypes::GRASS;
					}
				}
		}
	}
}




void MortonChunk::BuildTerrain_SIMD(simd::ivec4 seed) {
	// Set of coords for a chunk in 2D noise will be:
	// x: 0, 1, 2, 3 (one 4-long run) + 4 (per i) until x[3] = 64
	// For each set of these X's, get values of Z across range above
	// So get FBM at (0,0,1) (0,0,1)
	// using x = 0 1 2 3
	// z = 0 - 64
	std::time_t start; double duration;
	start = std::clock();
	// Two sets of coords, one for the noise function and
	std::vector<simd::vec4> xcoords; xcoords.reserve(256);
	std::vector<simd::vec4> zcoords; zcoords.reserve(256);
	// one for the actual mesh generation and indexing into the morton array
	std::vector<glm::vec2> positions; positions.reserve(CHUNK_SIZE * CHUNK_SIZE);
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int k = 0; k < CHUNK_SIZE; ++k) {
			// x, z
			positions.push_back(glm::vec2(i, k));

		}
	}
	std::vector<glm::vec3> data; data.reserve(positions.size());
	for (int i = 0; i < positions.size(); i+=4) {
		simd::vec4 xset(positions[i].x, positions[i + 1].x, positions[i + 2].x, positions[i + 3].x);
		simd::vec4 zset(positions[i].y, positions[i + 1].y, positions[i + 2].y, positions[i + 3].y);
		simd::vec4 results = simd::FBM(seed, xset, zset, 0.1f, 5, 1.5f, 1.0f);
		glm::vec3 res1, res2, res3, res4;
		res1 = glm::vec3(positions[i].x, positions[i].y, results.Data.m128_f32[0]);
		data.push_back(res1);
		res2 = glm::vec3(positions[i + 1].x, positions[i + 1].y, results.Data.m128_f32[1]);
		data.push_back(res2);
		res3 = glm::vec3(positions[i + 2].x, positions[i + 2].y, results.Data.m128_f32[2]);
		data.push_back(res3);
		res4 = glm::vec3(positions[i + 3].x, positions[i + 3].y, results.Data.m128_f32[3]);
		data.push_back(res4);
	}
	for (int i = 0; i < data.size(); ++i) {
		int maxheight = floorf(data[i].z);
		if (maxheight > CHUNK_SIZE - 1) {
			maxheight = CHUNK_SIZE - 1;
		}
		if (maxheight < 1) {
			maxheight = 1;
		}
		for (int y = 1; y < maxheight; ++y) {
			uint_fast32_t currentIndex = GetBlockIndex(data[i].x, y, data[i].y);
			this->Blocks[currentIndex + negYDelta(y)] = blockTypes::STONE;
			this->Blocks[currentIndex] = blockTypes::DIRT;
			this->Blocks[currentIndex + posYDelta(y)] = blockTypes::GRASS;
		}
	}
	duration = (std::clock() - start); 
	std::cerr << "Generating terrain for this chunk took " << duration << " clock cycles, or " << (duration / CLOCKS_PER_SEC) << " seconds." << std::endl;
}


void MortonChunk::BuildChunkMesh() {
	// Default block adjacency value assumes true
	bool def = true;
	for (int x = 0; x < CHUNK_SIZE; ++x) {
		for (int z = 0; z < CHUNK_SIZE; ++z) {
			this->Blocks[GetBlockIndex(x, 0, z)] = blockTypes::BEDROCK;
		}
	}
	// Iterate over each block in the volume via intervals
	// If an interval has a value
	this->chunkMesh.meshIndices.reserve(600000); this->chunkMesh.meshVerts.reserve(400000);
	for (int j = 0; j < CHUNK_SIZE - 1; j++) {
		for (int i = 0; i < CHUNK_SIZE - 1; i++) {
			for (int k = 0; k < CHUNK_SIZE - 1; k++) {
				uint32_t currBlock = MortonEncodeLUT<uint32_t,uint32_t>(i,j,k);
				// If the block at i,j,k is air, we won't build a mesh for it
				if (this->Blocks[currBlock] == blockTypes::AIR) {
					continue;
				}
				else {
					// The uv_type is simpyly the value of the block at the given point
					// This is used to index into the texture array, so each block gets the right textures and UVs
					int uv_type;
					uv_type = Blocks[currBlock];
					// If we are primitively culling invisible faces, run this system
					
					if (SIMPLE_CULLING_GLOBAL == true) {
						// If a face is visible, set that face's value to be false
						bool xNeg = def; // left
						if (i > 0) {
							if (this->Blocks[currBlock + negXDelta(i)] == blockTypes::AIR) {
								xNeg = false;
							}
						}
						bool xPos = def; // right
						if (i < CHUNK_SIZE - 1) {
							if (this->Blocks[currBlock + posXDelta(i)] == blockTypes::AIR) {
								xPos = false;
							}
						}
						bool yPos = def; // bottom
						if (j > 0) {
							if (this->Blocks[currBlock + negYDelta(j)] == blockTypes::AIR) {
								yPos = false;
							}
						}
						bool yNeg = def; // top
						if (j < CHUNK_SIZE_Z - 1) {
							//std::cerr << GetBlockIndex(i, j - 1, k);
							if (this->Blocks[currBlock + posYDelta(j)] == blockTypes::AIR) {
								yNeg = false;
							}
						}
						bool zNeg = def; // back
						if (k < CHUNK_SIZE - 1) {
							if (this->Blocks[currBlock + posZDelta(k)] == blockTypes::AIR) {
								zNeg = false;
							}
						}
						bool zPos = def; // front
						if (k > 0) {
							if (this->Blocks[currBlock + negZDelta(k)] == blockTypes::AIR) {
								zPos = false;
							}
						}
						// Create a cube at i,j,k with certain faces rendered
						// Each false value specifies a face that should be visible, and thus should be rendered
						// At the end, we include the type of block and thus what texture it needs
						this->createCube(i, j, k, zNeg, xPos, yNeg, xNeg, yPos, zPos, uv_type);
					}
					// If we're not doing primitive culling, render every non-air block in the volume regardless of visibility
					else if (SIMPLE_CULLING_GLOBAL == false) {
						this->createCube(i, j, k, false, false, false, false, false, false, uv_type);
					}
				}
			}
		}
	}
	this->chunkMesh.meshIndices.shrink_to_fit(); this->chunkMesh.meshVerts.shrink_to_fit();
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO); glGenBuffers(1, &this->EBO);
	glBindVertexArray(this->VAO);
	// Bind the vertex buffer and then specify what data it will be loaded with
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->chunkMesh.meshVerts.size() * sizeof(vertType), &(this->chunkMesh.meshVerts[0]), GL_STATIC_DRAW);
	// Bind the element array (indice) buffer and fill it with data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->chunkMesh.meshIndices.size() * sizeof(index_t), &(this->chunkMesh.meshIndices[0]), GL_STATIC_DRAW);
	// Pointer to the position attribute of a vertex
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertType), (GLvoid*)0);
	// Pointer to the normal attribute of a vertex
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertType), (GLvoid*)offsetof(vertType, normal));
	// Pointer to the UV attribute of a vertex
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertType), (GLvoid*)offsetof(vertType, uv));
	glBindVertexArray(0);
}


void MortonChunk::RenderChunk(Shader shader) {
	// Bind this chunk's VAO as OpenGL's currently active VAO
	glBindVertexArray(this->VAO);
	// glDrawElements is for use with indexed vertices. It reads from the element array buffer, 
	// going as far as we specify with the second parameter
	glDrawElements(GL_TRIANGLES, (GLsizei)this->chunkMesh.getNumIndices(), GL_UNSIGNED_INT, 0);
	// Done rendering this chunk, unbind it's VAO
	glBindVertexArray(0);
}
