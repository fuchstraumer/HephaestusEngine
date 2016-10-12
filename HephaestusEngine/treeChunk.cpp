#include "treeChunk.h"

// Enum specifying the sort of blocks we can have in our world
static enum blockType : std::uint8_t{
	AIR = -1,
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

treeChunk::treeChunk(glm::ivec3 gridpos){
	this->gridPos = gridpos;
	float x_pos, y_pos, z_pos;
	x_pos = this->gridPos.x * ((CHUNK_SIZE / 2.0f) * BLOCK_RENDER_SIZE * 2.0f);
	y_pos = this->gridPos.y * ((CHUNK_SIZE_Z / 2.0f) * BLOCK_RENDER_SIZE * 2.0f);
	z_pos = this->gridPos.z * ((CHUNK_SIZE / 2.0f) * BLOCK_RENDER_SIZE * 2.0f);
	this->chunkPos = glm::vec3(x_pos, y_pos, z_pos); this->chunkBlocks.reserve(CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE_Z);
	this->chunkBlocks.assign(this->chunkBlocks.capacity(),blockType::AIR);
}


void treeChunk::buildTerrain(Terrain_Generator& gen) {
	for (int x = 0; x < CHUNK_SIZE; ++x) {
		for (int z = 0; z < CHUNK_SIZE; ++z) {
			this->chunkBlocks[treeXYZ(x, 0, z)] = blockType::BEDROCK;
			for (int y = 1; y < gen.FBM(chunkPos.x + x,chunkPos.y,chunkPos.z + z); ++y) {
				this->chunkBlocks[treeXYZ(x,y,z)] = blockType::STONE;
				this->chunkBlocks[treeXYZ(x,y+3,z)] = blockType::GRASS;
				this->chunkBlocks[treeXYZ(x,y+2,z)] = blockType::DIRT; this->chunkBlocks[treeXYZ(x,y+1,z)] = blockType::DIRT;
			}
		}
	}
}

void treeChunk::buildCaves() {

}

void treeChunk::createCube(int x, int y, int z, bool frontFace, bool rightFace, bool topFace, bool leftFace, bool bottomFace, bool backFace, int uv_type) {
	std::array<glm::vec3, 8> vertices{
		glm::vec3(x - BLOCK_RENDER_SIZE,y - BLOCK_RENDER_SIZE,z + BLOCK_RENDER_SIZE), // Point 0, left lower front UV{0,0}
		glm::vec3(x + BLOCK_RENDER_SIZE,y - BLOCK_RENDER_SIZE,z + BLOCK_RENDER_SIZE), // Point 1, right lower front UV{1,0}
		glm::vec3(x + BLOCK_RENDER_SIZE,y + BLOCK_RENDER_SIZE,z + BLOCK_RENDER_SIZE), // Point 2, right upper front UV{1,1}
		glm::vec3(x - BLOCK_RENDER_SIZE,y + BLOCK_RENDER_SIZE,z + BLOCK_RENDER_SIZE), // Point 3, left upper front UV{0,1}
		glm::vec3(x + BLOCK_RENDER_SIZE,y - BLOCK_RENDER_SIZE,z - BLOCK_RENDER_SIZE), // Point 4, right lower rear
		glm::vec3(x - BLOCK_RENDER_SIZE,y - BLOCK_RENDER_SIZE,z - BLOCK_RENDER_SIZE), // Point 5, left lower rear
		glm::vec3(x - BLOCK_RENDER_SIZE,y + BLOCK_RENDER_SIZE,z - BLOCK_RENDER_SIZE), // Point 6, left upper rear
		glm::vec3(x + BLOCK_RENDER_SIZE,y + BLOCK_RENDER_SIZE,z - BLOCK_RENDER_SIZE), // Point 7, right upper rear
	};
	if (frontFace == false) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert2, vert3; // Using Points 0, 1, 2, 3 and Normal 0
		glm::vec3 uv0 = glm::vec3(0.0, 0.0, blocks[uv_type][0]);
		glm::vec3 uv1 = glm::vec3(1.0, 0.0, blocks[uv_type][0]);
		glm::vec3 uv3 = glm::vec3(0.0, 1.0, blocks[uv_type][0]);
		glm::vec3 uv2 = glm::vec3(1.0, 1.0, blocks[uv_type][0]);
		vert0.uv = uv0; vert1.uv = uv1; vert2.uv = uv2; vert3.uv = uv3;
		vert0.position.xyz = vertices[0]; vert1.position.xyz = vertices[1];
		vert2.position.xyz = vertices[2]; vert3.position.xyz = vertices[3];
		vert0.normal = normals[0];  vert1.normal = normals[0];
		vert2.normal = normals[0];  vert3.normal = normals[0];
		i0 = this->chunkMesh.addVert(vert0); i1 = this->chunkMesh.addVert(vert1);
		i2 = this->chunkMesh.addVert(vert2); i3 = this->chunkMesh.addVert(vert3);

		this->chunkMesh.addTriangle(i0, i1, i2); // Needs UVs {0,0}{1,0}{0,1}
		this->chunkMesh.addTriangle(i0, i2, i3); // Needs UVs {1,0}{0,1}{1,1}
	}

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

void treeChunk::buildData() {
	bool def = true;
	for (int i = 0; i < CHUNK_SIZE; i++) {
		for (int j = 0; j < CHUNK_SIZE_Z - 1; j++) {
			for (int k = 0; k < CHUNK_SIZE; k++) {
				if (this->chunkBlocks[treeXYZ(i,j,k)] == blockType::AIR) {
					continue;
				}
				else {

					int uv_type;
					uv_type = chunkBlocks[treeXYZ(i, j, k)];

					if (SIMPLE_CULLING_GLOBAL == true) {
						bool xNeg = def; // left
						if (i > 0) {
							if (this->chunkBlocks[treeXYZ(i - 1, j, k)] == blockType::AIR) {
								xNeg = false;
							}
						}
						bool xPos = def; // right
						if (i < CHUNK_SIZE - 1) {
							if (this->chunkBlocks[treeXYZ(i + 1, j, k)] == blockType::AIR) {
								xPos = false;
							}
						}
						bool yPos = def; // bottom
						if (j > 0) {
							if (this->chunkBlocks[treeXYZ(i, j - 1, k)] == blockType::AIR) {
								yPos = false;
							}
						}
						bool yNeg = def; // top
						if (j < CHUNK_SIZE_Z - 1) {
							//std::cerr << treeXYZ(i, j - 1, k);
							if (this->chunkBlocks[treeXYZ(i, j + 1, k)] == blockType::AIR) {
								yNeg = false;
							}
						}
						bool zNeg = def; // back
						if (k < CHUNK_SIZE - 1) {
							if (this->chunkBlocks[treeXYZ(i, j, k + 1)] == blockType::AIR) {
								zNeg = false;
							}
						}
						bool zPos = def; // front
						if (k > 0) {
							if (this->chunkBlocks[treeXYZ(i, j, k - 1)] == blockType::AIR) {
								zPos = false;
							}
						}
						this->createCube(i, j, k, zNeg, xPos, yNeg, xNeg, yPos, zPos, uv_type);
					}
					else if (SIMPLE_CULLING_GLOBAL == false) {
						this->createCube(i, j, k, false, false, false, false, false, false, uv_type);
					}
				}
			}
		}
	}
}

void treeChunk::buildRender() {
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO); glGenBuffers(1, &this->EBO);
	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->chunkMesh.meshVerts.size() * sizeof(vertType), &(this->chunkMesh.meshVerts[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->chunkMesh.meshIndices.size() * sizeof(index_t), &(this->chunkMesh.meshIndices[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertType), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertType), (GLvoid*)offsetof(vertType, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertType), (GLvoid*)offsetof(vertType, uv));
	glBindVertexArray(0);
}

void treeChunk::chunkRender(Shader shader) {
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->chunkMesh.getNumIndices(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void treeChunk::compressChunk(){
	std::vector<char> encodedBlocks;
	for (int y = 0; y < CHUNK_SIZE_Z; ++y) {
		for (int x = 0; x < CHUNK_SIZE; ++x) {
			std::string buffer; buffer.reserve(CHUNK_SIZE);
			for (int z = 0; z < CHUNK_SIZE; ++z) {
				// encode runs from Y = 0 to Y = CHUNK_SIZE_Z
				buffer.push_back(this->chunkBlocks[treeXYZ(x,y,z)]);
			}
			std::string::size_type found = 0, nextFound = 0;
			std::ostringstream oss;
			nextFound = buffer.find_first_not_of(buffer[found], found);
			while (nextFound != std::string::npos) {
				oss << nextFound - found;
				oss << buffer[found];
				found = nextFound;
				nextFound = buffer.find_first_not_of(buffer[found], found);
			}
			std::string end(buffer.substr(found));
			oss << end.length() << buffer[found];
			for (auto chr:oss.str()) {
				encodedBlocks.push_back(chr);
			}
			encodedBlocks.shrink_to_fit();
		}
	}
	this->chunkBlocks.clear();
	this->chunkBlocks.resize(encodedBlocks.size());
	this->chunkBlocks.push_back(*encodedBlocks.data());
}

