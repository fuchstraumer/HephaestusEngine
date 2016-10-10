#include "stdafx.h"
#include "chunk.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <random>
static Terrain_Generator gen(TERRAIN_SEED);

// Face normals
static const vector<glm::vec3> normals = { 
  glm::ivec3( 0, 0, 1),   // (front)
  glm::ivec3( 1, 0, 0),   // (right)
  glm::ivec3( 0, 1, 0),   // (top)
  glm::ivec3(-1, 0, 0),   // (left)
  glm::ivec3( 0,-1, 0),   // (bottom)
  glm::ivec3( 0, 0,-1),   // (back)
};

// Texture indices
static const float blocks[256][6] = {
	// Each number corresponds to certain face, and thus certain index into texture array. Given as {front, right, top, left, bottom, back}
	// Loading order:: Grass_Top, Grass_Sides, dirt, sand, stone, bedrock
	{1,1,0,1,2,1}, // Grass block
	{3,3,3,3,3,3}, // Sand block
	{2,2,2,2,2,2}, // Dirt block
	{4,4,4,4,4,4}, // Stone block
	{5,5,5,5,5,5}, // Bedrock block
	{6,6,6,6,6,6}, // tall grass
	{7,7,7,7,7,7}, // Coal ore
	{8,8,8,8,8,8}, // Iron ore
	{9,9,9,9,9,9}, // Diamond Ore
};

// RLE func
typedef unsigned char UCHAR;
const static int MAX_CNT = 255;
string encode(vector<string>& strs) {
	string ret;
	for (auto &s : strs)
	{
		int i = 0, len = s.length();

		while (i < len)
		{
			UCHAR c = s[i];
			UCHAR cnt = 1;
			while (i < len - 1 && s[i + 1] == c && cnt < (MAX_CNT - 1))
			{
				i++; cnt++;
			}
			ret += UCHAR(cnt);
			ret += UCHAR(c);

			i++;
		}
		ret += UCHAR(MAX_CNT); // 0xFF: end
	}
	return ret;
}
// Returns N points that form an even distribution around a sphere
inline std::vector<glm::vec3> pointsOnSphere(int N) {
	float y, r, phi;
	std::vector<glm::vec3> pts; float pi = glm::pi<float>();
	float inc = pi * (3 - std::sqrt(5));
	float off = 2 / N;
	for (int i = 0; i < N; i++) {
		y = i * off - 1 + (off / 2);
		r = std::sqrt(1 - (y*y));
		phi = i * inc;
		pts.push_back(glm::vec3(std::cos(phi)*r, y, std::sin(phi)*r));
	}
	return pts;
}

// Create block instances
Chunk::Chunk(glm::ivec3 gridpos){

	this->gridPos = gridpos;
	float x_pos, y_pos, z_pos;
	x_pos = this->gridPos.x * ((CHUNK_SIZE / 2.0f) * BLOCK_RENDER_SIZE * 2.0f);
	y_pos = this->gridPos.y * ((CHUNK_SIZE_Z / 2.0f) * BLOCK_RENDER_SIZE * 2.0f);
	z_pos = this->gridPos.z * ((CHUNK_SIZE / 2.0f) * BLOCK_RENDER_SIZE * 2.0f);
	this->chunkPos = glm::vec3(x_pos, y_pos, z_pos);
	this->chunkBlocks = new Block**[CHUNK_SIZE];
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		this->chunkBlocks[i] = new Block*[CHUNK_SIZE_Z];
		for (int j = 0; j < CHUNK_SIZE_Z; ++j) {
			this->chunkBlocks[i][j] = new Block[CHUNK_SIZE];
		}
	}
}
	
// Delete blocks
Chunk::~Chunk(){

}

void Chunk::buildTerrain() {
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {
			this->chunkBlocks[x][0][z].Type = blockType::BEDROCK;
			for (int y = 1; y < 32; y++) {
				this->chunkBlocks[x][y][z].Active = true;
				this->chunkBlocks[x][y][z].Type = blockType::STONE;
			}
			for (int y = 32; y < gen.genTerrain(chunkPos.x + x, chunkPos.z + z); ++y) {
				this->chunkBlocks[x][y][z].Active = true;
				this->chunkBlocks[x][y][z].Type = blockType::STONE;
				this->chunkBlocks[x][y + 3][z].Active = true; this->chunkBlocks[x][y + 3][z].Type = blockType::GRASS;
				this->chunkBlocks[x][y + 2][z].Active = true; this->chunkBlocks[x][y + 1][z].Active = true;
				this->chunkBlocks[x][y + 2][z].Type = blockType::DIRT; this->chunkBlocks[x][y + 1][z].Type = blockType::DIRT;
			}
			/*for (int j = 32; j < CHUNK_SIZE_Z - 1; ++j) {
				if (j < CHUNK_SIZE_Z - 1 && this->chunkBlocks[x][j + 1][z].Type == AIR && j > 36) {
					this->chunkBlocks[x][j][z].Type = blockType::GRASS;
				}
				else if (this->chunkBlocks[x][j + 1][z].Type == AIR || this->chunkBlocks[x][j+2][z].Type == AIR && j > 36) {
					this->chunkBlocks[x][j][z].Type = blockType::DIRT;
				}
			}*/
		}
	}
	this->buildCaves();
}

void Chunk::buildCaves() {
	int cavecount = 0, coal_count = 0, iron_count = 0, diamond_count = 0;
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {
			for (int y = 1; y < CHUNK_SIZE_Z - 92; y++) {
				if (this->chunkBlocks[x][y][z].Active == true) {
					float density = gen.genCave(chunkPos.x + x,chunkPos.y + y,chunkPos.z + z);
					if (density <= 1.7f) {
						this->chunkBlocks[x][y][z].Active = false;
						cavecount++;						
					}				
				}
			}
		}
	}
	std::cerr << "Blocks removed to make caves: " << cavecount << std::endl;
	std::cerr << "Ore distribution: " << coal_count << " coal blocks, " << iron_count << " iron blocks, " << diamond_count << " diamond blocks. " << std::endl;
}



void Chunk::createCube(int x, int y, int z, bool frontFace, bool rightFace, bool topFace, bool leftFace, bool bottomFace, bool backFace, blockType uv_type) {
	static vector<glm::vec3> vertices; vertices.reserve(8);
	vertices ={
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
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1);
		i2 = this->mesh.addVert(vert2); i3 = this->mesh.addVert(vert3);

		this->mesh.addTriangle(i0, i1, i2); // Needs UVs {0,0}{1,0}{0,1}
		this->mesh.addTriangle(i0, i2, i3); // Needs UVs {1,0}{0,1}{1,1}
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
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1);
		i2 = this->mesh.addVert(vert2); i3 = this->mesh.addVert(vert3);

		this->mesh.addTriangle(i0, i1, i2); this->mesh.addTriangle(i0, i2, i3);
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
		
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1);
		i2 = this->mesh.addVert(vert2); i3 = this->mesh.addVert(vert3);

		this->mesh.addTriangle(i0, i1, i2); this->mesh.addTriangle(i0, i2, i3);
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
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1);
		i2 = this->mesh.addVert(vert2); i3 = this->mesh.addVert(vert3);

		this->mesh.addTriangle(i0, i1, i2); this->mesh.addTriangle(i0, i2, i3);
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
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1);
		i2 = this->mesh.addVert(vert2); i3 = this->mesh.addVert(vert3);

		this->mesh.addTriangle(i0, i1, i2); this->mesh.addTriangle(i0, i2, i3);
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
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1);
		i2 = this->mesh.addVert(vert2); i3 = this->mesh.addVert(vert3);

		this->mesh.addTriangle(i0, i1, i2); this->mesh.addTriangle(i0, i2, i3);
	}

}

void Chunk::buildData() {
	bool def = true; 
		for (int i = 0; i < CHUNK_SIZE; i++) {
			for (int j = CHUNK_SIZE_Z - 1; j > 0; j--) {
				for (int k = 0; k < CHUNK_SIZE; k++) {
					if (this->chunkBlocks[i][j][k].isActive() == false) {
						continue;
					}
					else {

						blockType uv_type = this->chunkBlocks[i][j][k].Type;

						if (SIMPLE_CULLING_GLOBAL == true) {
							bool xNeg = def; // left
							if (i > 0)
								xNeg = (this->chunkBlocks[i - 1][j][k].Active);

							bool xPos = def; // right
							if (i < CHUNK_SIZE - 1)
								xPos = (this->chunkBlocks[i + 1][j][k].Active);

							bool yPos = def; // bottom
							if (j > 0) {
								yPos = this->chunkBlocks[i][j - 1][k].Active;
							}
							bool yNeg = def; // top
							if (j < CHUNK_SIZE_Z - 1) {
								yNeg = this->chunkBlocks[i][j + 1][k].Active;
							}
							bool zNeg = def; // back
							if (k < CHUNK_SIZE - 1)
								zNeg = (this->chunkBlocks[i][j][k + 1].Active);

							bool zPos = def; // front
							if (k > 0)
								zPos = (this->chunkBlocks[i][j][k - 1].Active);
							if (this->chunkBlocks[i][j][k].Type == COAL_ORE || this->chunkBlocks[i][j][k].Type == IRON_ORE || this->chunkBlocks[i][j][k].Type == DIAMOND_ORE)
								this->createCube(i, j, k, false, false, false, false, false, false, uv_type);
							else
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

void Chunk::buildRender() {
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO); glGenBuffers(1, &this->EBO);
	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->mesh.meshVerts.size() * sizeof(vertType), &(this->mesh.meshVerts[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->mesh.meshIndices.size() * sizeof(index_t), &(this->mesh.meshIndices[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertType), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertType), (GLvoid*)offsetof(vertType, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertType), (GLvoid*)offsetof(vertType, uv));
	glBindVertexArray(0);
}


void Chunk::chunkRender(Shader shader) {
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES,this->mesh.getNumIndices(),GL_UNSIGNED_INT,0);
	glBindVertexArray(0);
}

void Chunk::compressChunk() {
	this->encodedBlocks.reserve(CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE_Z);
	
	for (int y = 0; y < CHUNK_SIZE_Z; ++y) {
		for (int x = 0; x < CHUNK_SIZE; ++x) {
			std::string buffer; buffer.reserve(CHUNK_SIZE);
			for (int z = 0; z < CHUNK_SIZE; ++z) {
				// encode runs from Y = 0 to Y = CHUNK_SIZE_Z
				buffer.push_back(this->chunkBlocks[x][y][z].Type);
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
			this->encodedBlocks.append(oss.str());
			buffer.clear(); oss.clear(); end.clear();
		}
	}
	this->encodedBlocks.shrink_to_fit();
}