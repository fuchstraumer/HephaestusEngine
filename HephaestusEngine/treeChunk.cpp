#include "treeChunk.h"
#include <array>
#include <iterator>
// Enum specifying the sort of blocks we can have in our world
enum blockType : std::uint8_t{
	AIR = 0,
	GRASS,
	SAND,
	DIRT,
	STONE,
	BEDROCK,
	TALL_GRASS,
	COAL_ORE,
	IRON_ORE,
	DIAMOND_ORE,
	BRICK,
	WOOD,
	CEMENT,
	PLANK,
	SNOW,
	GLASS,
	COBBLE,
	LIGHT_STONE,
	DARK_STONE,
	CHEST,
	LEAVES,
	YELLOW_FLOWER,
	RED_FLOWER,
	PURPLE_FLOWER,
	SUN_FLOWER,
	WHITE_FLOWER,
	BLUE_FLOWER,
	COLOR_00,
	COLOR_01,
	COLOR_02,
	COLOR_03,
	COLOR_04,
	COLOR_05,
	COLOR_06,
	COLOR_07,
	COLOR_08,
	COLOR_09,
	COLOR_10,
	COLOR_11,
	COLOR_12,
	COLOR_13,
	COLOR_14,
	COLOR_15,
	COLOR_16,
	COLOR_17,
	COLOR_18,
	COLOR_19,
	COLOR_20,
	COLOR_21,
	COLOR_22,
	COLOR_23,
	COLOR_24,
	COLOR_25,
	COLOR_26,
	COLOR_27,
	COLOR_28,
	COLOR_29,
	COLOR_30,
	COLOR_31,

};
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
	{ NULL,NULL,NULL,NULL,NULL,NULL}, // Air block
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

TreeChunk::TreeChunk(glm::ivec3 gridpos){
	this->GridPos = gridpos;
	float x_pos, y_pos, z_pos;
	// The gridpos is simply "normalized" world coords to be integral values.
	// The actual position in the world is calculated now - we use this later to offset the chunk using a model matrix
	x_pos = this->GridPos.x * ((CHUNK_SIZE / 2.0f) * BLOCK_RENDER_SIZE * 2.0f);
	y_pos = this->GridPos.y * ((CHUNK_SIZE_Z / 2.0f) * BLOCK_RENDER_SIZE * 2.0f);
	z_pos = this->GridPos.z * ((CHUNK_SIZE / 2.0f) * BLOCK_RENDER_SIZE * 2.0f);
	// Set the position, and reserve enough space in our chunkBlocks vector to hold all of our values
	// Pre-allocating, especially at this size, is more performance-friendly than dynamically expanding the vector
	this->ChunkPos = glm::vec3(x_pos, y_pos, z_pos); this->ChunkBlocks.reserve(CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE_Z);
	// Fill our chunkBlocks vector with air blocks entirely
	this->ChunkBlocks.assign(this->ChunkBlocks.capacity(),blockType::AIR);
}

TreeChunk::~TreeChunk() {
	// While the compiler / program will clean up most things, we must explicitly delete the OpenGL objects
	glDeleteBuffers(1, &this->VBO); glDeleteBuffers(1, &this->EBO);
	glDeleteVertexArrays(1, &this->VAO);
}
// Build this chunk's terrain. The reference to the terrain generator insures we only have one global instance 
// of this class. The function used to randomize it's seed value is a CPU killer, so let's try to only do that once
// We also get to access this TerrainGenerator's RNG, instead of having to build and seed a new one. yay!
void TreeChunk::BuildTerrain(TerrainGenerator& gen) {
	for (int x = 0; x < CHUNK_SIZE; ++x) {
		for (int z = 0; z < CHUNK_SIZE; ++z) {
			// Set all blocks at y = 0 to be bedrock and form the base layer of the world
			this->ChunkBlocks[treeXYZ(x, 0, z)] = blockType::BEDROCK;
			// The SimplexFBM function here is used like a heightmap. The value returned ranges within the bounds of the Y size
			for (int y = 1; y < gen.SimplexFBM(this->ChunkPos.x + x,this->ChunkPos.z + z); ++y) {
				// Since we start at y = 1 and iterate up the column progressively
				// set the majority of blocks to be stone, blocks 3 above stone to be grass,
				// blocks 2 above stone to be dirt, blocks 1 above to be dirt to form some basic terrain
				this->ChunkBlocks[treeXYZ(x,y,z)] = blockType::STONE;
				this->ChunkBlocks[treeXYZ(x,y+3,z)] = blockType::GRASS;
				this->ChunkBlocks[treeXYZ(x,y+2,z)] = blockType::DIRT; this->ChunkBlocks[treeXYZ(x,y+1,z)] = blockType::DIRT;
			}
		}
	}
	//this->BuildCaves(gen);
}

inline void TreeChunk::createCube(int x, int y, int z, bool frontFace, bool rightFace, bool topFace, bool leftFace, bool bottomFace, bool backFace, int uv_type) {
	// Use a std::array since the data isn't modified, rather it's used like a template to build the individual points from
	// This setup means that the xyz of a given block is actually the center of the block's mesh
	std::array<glm::vec3, 8> vertices{
		{glm::vec3(x - BLOCK_RENDER_SIZE,y - BLOCK_RENDER_SIZE,z + BLOCK_RENDER_SIZE), // Point 0, left lower front UV{0,0}
		glm::vec3(x + BLOCK_RENDER_SIZE,y - BLOCK_RENDER_SIZE,z + BLOCK_RENDER_SIZE), // Point 1, right lower front UV{1,0}
		glm::vec3(x + BLOCK_RENDER_SIZE,y + BLOCK_RENDER_SIZE,z + BLOCK_RENDER_SIZE), // Point 2, right upper front UV{1,1}
		glm::vec3(x - BLOCK_RENDER_SIZE,y + BLOCK_RENDER_SIZE,z + BLOCK_RENDER_SIZE), // Point 3, left upper front UV{0,1}
		glm::vec3(x + BLOCK_RENDER_SIZE,y - BLOCK_RENDER_SIZE,z - BLOCK_RENDER_SIZE), // Point 4, right lower rear
		glm::vec3(x - BLOCK_RENDER_SIZE,y - BLOCK_RENDER_SIZE,z - BLOCK_RENDER_SIZE), // Point 5, left lower rear
		glm::vec3(x - BLOCK_RENDER_SIZE,y + BLOCK_RENDER_SIZE,z - BLOCK_RENDER_SIZE), // Point 6, left upper rear
		glm::vec3(x + BLOCK_RENDER_SIZE,y + BLOCK_RENDER_SIZE,z - BLOCK_RENDER_SIZE)} // Point 7, right upper rear
	};
	// If the frontface of this cube will be visible, build the tris needed for that face
	if (frontFace == false) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert2, vert3; // Using Points 0, 1, 2, 3 and Normal 0
		// Set the relevant UV's. The front face uses the first column of the blocks array
		glm::vec3 uv0 = glm::vec3(0.0, 0.0, blocks[uv_type][0]);
		glm::vec3 uv1 = glm::vec3(1.0, 0.0, blocks[uv_type][0]);
		glm::vec3 uv3 = glm::vec3(0.0, 1.0, blocks[uv_type][0]);
		glm::vec3 uv2 = glm::vec3(1.0, 1.0, blocks[uv_type][0]);
		// Assign each vertice it's appropriate UV
		vert0.uv = uv0; vert1.uv = uv1; vert2.uv = uv2; vert3.uv = uv3;
		// Assign each vertice a position. the W component of each vert is currently empty
		// It will likely be used for calculating/holding AO data
		vert0.position.xyz = vertices[0]; vert1.position.xyz = vertices[1];
		vert2.position.xyz = vertices[2]; vert3.position.xyz = vertices[3];
		// Assign each vert a normal - the normals are all the same per face,
		// but each vert needs it's own copy
		vert0.normal = normals[0];  vert1.normal = normals[0];
		vert2.normal = normals[0];  vert3.normal = normals[0];
		// Add the verts to the chunkMesh's vert list
		i0 = this->chunkMesh.addVert(vert0); i1 = this->chunkMesh.addVert(vert1);
		i2 = this->chunkMesh.addVert(vert2); i3 = this->chunkMesh.addVert(vert3);
		// Add the triangles to the mesh, via indices. Note shared verts.
		this->chunkMesh.addTriangle(i0, i1, i2); // Needs UVs {0,0}{1,0}{0,1}
		this->chunkMesh.addTriangle(i0, i2, i3); // Needs UVs {1,0}{0,1}{1,1}
	}
	// The rest of the faces follow the same format as the front face.
	if (rightFace == false) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert2, vert3; // Using Points 1, 4, 7, 2 and Normal 1
		glm::vec3 uv0 = glm::vec3(0.0, 0.0, blocks[uv_type][1]);
		glm::vec3 uv1 = glm::vec3(1.0, 0.0, blocks[uv_type][1]);
		glm::vec3 uv3 = glm::vec3(0.0, 1.0, blocks[uv_type][1]);
		glm::vec3 uv2 = glm::vec3(1.0, 1.0, blocks[uv_type][1]);
		vert0.uv = uv0; vert1.uv = uv1; vert2.uv = uv2; vert3.uv = uv3;
		vert0.position.xyz = vertices[1]; vert1.position.xyz = vertices[4];
		vert2.position.xyz = vertices[7]; vert3.position.xyz = vertices[2];
		vert0.normal = normals[1];  vert1.normal = normals[1];
		vert2.normal = normals[1];  vert3.normal = normals[1];
		i0 = this->chunkMesh.addVert(vert0); i1 = this->chunkMesh.addVert(vert1);
		i2 = this->chunkMesh.addVert(vert2); i3 = this->chunkMesh.addVert(vert3);

		this->chunkMesh.addTriangle(i0, i1, i2); this->chunkMesh.addTriangle(i0, i2, i3);
	}

	if (topFace == false) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert2, vert3; // Using Points 3, 2, 7, 6 and Normal 2
		glm::vec3 uv0 = glm::vec3(0.0, 0.0, blocks[uv_type][2]);
		glm::vec3 uv1 = glm::vec3(1.0, 0.0, blocks[uv_type][2]);
		glm::vec3 uv3 = glm::vec3(0.0, 1.0, blocks[uv_type][2]);
		glm::vec3 uv2 = glm::vec3(1.0, 1.0, blocks[uv_type][2]);
		vert0.uv = uv0; vert1.uv = uv1; vert2.uv = uv2; vert3.uv = uv3;
		vert0.position.xyz = vertices[3]; vert1.position.xyz = vertices[2];
		vert2.position.xyz = vertices[7]; vert3.position.xyz = vertices[6];
		vert0.normal = normals[2];  vert1.normal = normals[2];
		vert2.normal = normals[2];  vert3.normal = normals[2];

		i0 = this->chunkMesh.addVert(vert0); i1 = this->chunkMesh.addVert(vert1);
		i2 = this->chunkMesh.addVert(vert2); i3 = this->chunkMesh.addVert(vert3);

		this->chunkMesh.addTriangle(i0, i1, i2); this->chunkMesh.addTriangle(i0, i2, i3);
	}

	if (leftFace == false) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert2, vert3; // Using Points 5, 0, 3, 6 and Normal 3
		glm::vec3 uv0 = glm::vec3(0.0, 0.0, blocks[uv_type][3]);
		glm::vec3 uv1 = glm::vec3(1.0, 0.0, blocks[uv_type][3]);
		glm::vec3 uv3 = glm::vec3(0.0, 1.0, blocks[uv_type][3]);
		glm::vec3 uv2 = glm::vec3(1.0, 1.0, blocks[uv_type][3]);
		vert0.uv = uv0; vert1.uv = uv1; vert2.uv = uv2; vert3.uv = uv3;
		vert0.position.xyz = vertices[5]; vert1.position.xyz = vertices[0];
		vert2.position.xyz = vertices[3]; vert3.position.xyz = vertices[6];
		vert0.normal = normals[3];  vert1.normal = normals[3];
		vert2.normal = normals[3];  vert3.normal = normals[3];
		i0 = this->chunkMesh.addVert(vert0); i1 = this->chunkMesh.addVert(vert1);
		i2 = this->chunkMesh.addVert(vert2); i3 = this->chunkMesh.addVert(vert3);

		this->chunkMesh.addTriangle(i0, i1, i2); this->chunkMesh.addTriangle(i0, i2, i3);
	}

	if (bottomFace == false) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert2, vert3; // Using Points 5, 4, 1, 0 and Normal 4
		glm::vec3 uv0 = glm::vec3(0.0, 0.0, blocks[uv_type][4]);
		glm::vec3 uv1 = glm::vec3(1.0, 0.0, blocks[uv_type][4]);
		glm::vec3 uv3 = glm::vec3(0.0, 1.0, blocks[uv_type][4]);
		glm::vec3 uv2 = glm::vec3(1.0, 1.0, blocks[uv_type][4]);
		vert0.uv = uv0; vert1.uv = uv1; vert2.uv = uv2; vert3.uv = uv3;
		vert0.position.xyz = vertices[5]; vert1.position.xyz = vertices[4];
		vert2.position.xyz = vertices[1]; vert3.position.xyz = vertices[0];
		vert0.normal = normals[4];  vert1.normal = normals[4];
		vert2.normal = normals[4];  vert3.normal = normals[4];
		i0 = this->chunkMesh.addVert(vert0); i1 = this->chunkMesh.addVert(vert1);
		i2 = this->chunkMesh.addVert(vert2); i3 = this->chunkMesh.addVert(vert3);

		this->chunkMesh.addTriangle(i0, i1, i2); this->chunkMesh.addTriangle(i0, i2, i3);
	}

	if (backFace == false) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert2, vert3; // Using Points 4, 5, 6, 7 and Normal 5
		glm::vec3 uv0 = glm::vec3(0.0, 0.0, blocks[uv_type][5]);
		glm::vec3 uv1 = glm::vec3(1.0, 0.0, blocks[uv_type][5]);
		glm::vec3 uv3 = glm::vec3(0.0, 1.0, blocks[uv_type][5]);
		glm::vec3 uv2 = glm::vec3(1.0, 1.0, blocks[uv_type][5]);
		vert0.uv = uv0; vert1.uv = uv1; vert2.uv = uv2; vert3.uv = uv3;
		vert0.position.xyz = vertices[4]; vert1.position.xyz = vertices[5];
		vert2.position.xyz = vertices[6]; vert3.position.xyz = vertices[7];
		vert0.normal = normals[5];  vert1.normal = normals[5];
		vert2.normal = normals[5];  vert3.normal = normals[5];
		i0 = this->chunkMesh.addVert(vert0); i1 = this->chunkMesh.addVert(vert1);
		i2 = this->chunkMesh.addVert(vert2); i3 = this->chunkMesh.addVert(vert3);

		this->chunkMesh.addTriangle(i0, i1, i2); this->chunkMesh.addTriangle(i0, i2, i3);
	}

}

void TreeChunk::BuildData() {
	// Default block adjacency value assumes true
	bool def = true;
	// Iterate over each block in the volume
	for (int i = 0; i < CHUNK_SIZE; i++) {
		for (int j = 0; j < CHUNK_SIZE_Z - 1; j++) {
			for (int k = 0; k < CHUNK_SIZE; k++) {
				// If the block at i,j,k is air, we won't build a mesh for it
				if (this->ChunkBlocks[treeXYZ(i,j,k)] == blockType::AIR) {
					continue;
				}
				else {
					// The uv_type is simpyly the value of the block at the given point
					// This is used to index into the texture array, so each block gets the right textures and UVs
					int uv_type;
					uv_type = ChunkBlocks[treeXYZ(i, j, k)];
					// If we are primitively culling invisible faces, run this system
					if (SIMPLE_CULLING_GLOBAL == true) {
						// If a face is visible, set that face's value to be false
						bool xNeg = def; // left
						if (i > 0) {
							if (this->ChunkBlocks[treeXYZ(i - 1, j, k)] == blockType::AIR) {
								xNeg = false;
							}
						}
						bool xPos = def; // right
						if (i < CHUNK_SIZE - 1) {
							if (this->ChunkBlocks[treeXYZ(i + 1, j, k)] == blockType::AIR) {
								xPos = false;
							}
						}
						bool yPos = def; // bottom
						if (j > 0) {
							if (this->ChunkBlocks[treeXYZ(i, j - 1, k)] == blockType::AIR) {
								yPos = false;
							}
						}
						bool yNeg = def; // top
						if (j < CHUNK_SIZE_Z - 1) {
							//std::cerr << treeXYZ(i, j - 1, k);
							if (this->ChunkBlocks[treeXYZ(i, j + 1, k)] == blockType::AIR) {
								yNeg = false;
							}
						}
						bool zNeg = def; // back
						if (k < CHUNK_SIZE - 1) {
							if (this->ChunkBlocks[treeXYZ(i, j, k + 1)] == blockType::AIR) {
								zNeg = false;
							}
						}
						bool zPos = def; // front
						if (k > 0) {
							if (this->ChunkBlocks[treeXYZ(i, j, k - 1)] == blockType::AIR) {
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
}

void TreeChunk::BuildRender() {
	// Generate our buffers and vertex arrays, and bind our VAO
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

void TreeChunk::ChunkRender(Shader shader) {
	// Bind this chunk's VAO as OpenGL's currently active VAO
	glBindVertexArray(this->VAO);
	// glDrawElements is for use with indexed vertices. It reads from the element array buffer, 
	// going as far as we specify with the second parameter
	glDrawElements(GL_TRIANGLES, this->chunkMesh.getNumIndices(), GL_UNSIGNED_INT, 0);
	// Done rendering this chunk, unbind it's VAO
	glBindVertexArray(0);
}

void TreeChunk::encodeChunk(){
	// Set c = -1 so that we won't ever have a block be equal to c (initially)
	uint8_t c = -1; uint8_t num = 0;
	std::vector<uint8_t> encodedBlocks;
	for (std::vector<uint8_t>::iterator blocks = this->ChunkBlocks.begin(); 
		blocks != this->ChunkBlocks.end(); ++blocks) {
		// If the current block is different than the last, we've reached the end of a run
		if (*blocks != c) {
			// No need to add the num if we only had one of the last type
			if (num > 0) {
				encodedBlocks.push_back(num);
			}
			// Reset c, push back the new blocktype
			c = *blocks;
			encodedBlocks.push_back(*blocks);
		}
		// If the current block is the same as the last, we have a run
		else {
			// So long as num < 255, increment num
			if (num != UINT8_MAX) {
				num++;
			}
			// if num = 255, we effectively have reset like we found a new blocktype
			else {
				encodedBlocks.push_back(num);
				encodedBlocks.push_back(*blocks);
				c = *blocks;
				num = 0;
			}
		}
	}
	// Don't forget to push back our final count, and shrink the vector to the size of the data
	encodedBlocks.push_back(num); encodedBlocks.shrink_to_fit();
	// Clear out the raw block data to make way for compressed data
	this->ChunkBlocks.clear();
	// Make this chunk's blocks the compressed version, and shrink the vector to fit,
	// especially because this one was the full size required to fit every block in the volume
	this->ChunkBlocks = encodedBlocks; this->ChunkBlocks.shrink_to_fit();
}

