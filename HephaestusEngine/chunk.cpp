#include "stdafx.h"
#include "chunk.h"
#include "util/Terrain_Gen.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// Face normals
static vector<glm::vec3> normals = { 
  glm::vec3( 0, 0, 1),   // (front)
  glm::vec3( 1, 0, 0),   // (right)
  glm::vec3( 0, 1, 0),   // (top)
  glm::vec3(-1, 0, 0),   // (left)
  glm::vec3( 0,-1, 0),   // (bottom)
  glm::vec3( 0, 0,-1),   // (back)
};
const float atlas_size = 256.0f;
const float blocks[256][6] = {
	// Order is (pictured as texture coords) - {0,0}, {1,0}, {0,1}, {1,1} Tri1 needs {0,0}{1,0}{0,1} Tri2 needs {1,0}{0,1}{1,1}
	// Each number corresponds to certain face, and thus certain index into texture array. Given as {front, right, top, left, bottom, back}
	{1,1,0,1,2,1}, // Grass block
};

inline int vertAO(bool side1,bool side2,bool corner) {
	if (!side1 && !side2) {
		return 0;
	}
	return 3 - (side1 + side2 + corner);
};

// Create block instances
Chunk::Chunk(glm::ivec3 gridpos){

	this->gridPos = gridpos;
	float x_pos, y_pos, z_pos;
	x_pos = this->gridPos.x * (CHUNK_SIZE * BLOCK_RENDER_SIZE * 1.0f);
	y_pos = this->gridPos.y * (CHUNK_SIZE * BLOCK_RENDER_SIZE * 1.0f);
	z_pos = this->gridPos.z * (CHUNK_SIZE * BLOCK_RENDER_SIZE * 1.0f);
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
	Terrain_Generator gen;
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {
			for (int y = 0; y < gen.genTerrain(chunkPos.x + x, chunkPos.z + z); ++y) {
				this->chunkBlocks[x][y][z].Active = true;
				this->chunkBlocks[x][y][z].Type = blockType::STONE;
			}
		}
	}
}

void Chunk::BuildTerrainCubeVer() {
	Terrain_Generator cubeGen; triLerpCube terCube;
	terCube = cubeGen.getNoiseCube(this->gridPos.x, this->gridPos.y, this->gridPos.z);
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int y = 0; y < CHUNK_SIZE_Z; y++) {
			for (int z = 0; z < CHUNK_SIZE; z++) {
				this->chunkBlocks[x][y][z].density = triLerp(x, y, z, terCube[0].w, terCube[1].w, terCube[2].w, terCube[3].w, terCube[4].w, terCube[5].w, terCube[6].w, terCube[7].w);
				std::cerr << chunkBlocks[x][y][z].density << " ";
			}
			std::cerr << std::endl;
		}
		std::cerr << std::endl;
	}
}

void Chunk::createCube(int x, int y, int z, bool frontFace, bool rightFace, bool topFace, bool leftFace, bool bottomFace, bool backFace, blockType uv_type) {
	vector<glm::highp_vec3> vertices = {
		glm::highp_vec3(x - BLOCK_RENDER_SIZE,y - BLOCK_RENDER_SIZE,z + BLOCK_RENDER_SIZE), // Point 0, left lower front UV{0,0}
		glm::highp_vec3(x + BLOCK_RENDER_SIZE,y - BLOCK_RENDER_SIZE,z + BLOCK_RENDER_SIZE), // Point 1, right lower front UV{1,0}
		glm::highp_vec3(x + BLOCK_RENDER_SIZE,y + BLOCK_RENDER_SIZE,z + BLOCK_RENDER_SIZE), // Point 2, right upper front UV{1,1}
		glm::highp_vec3(x - BLOCK_RENDER_SIZE,y + BLOCK_RENDER_SIZE,z + BLOCK_RENDER_SIZE), // Point 3, left upper front UV{0,1}
		glm::highp_vec3(x + BLOCK_RENDER_SIZE,y - BLOCK_RENDER_SIZE,z - BLOCK_RENDER_SIZE), // Point 4, right lower rear
		glm::highp_vec3(x - BLOCK_RENDER_SIZE,y - BLOCK_RENDER_SIZE,z - BLOCK_RENDER_SIZE), // Point 5, left lower rear
		glm::highp_vec3(x - BLOCK_RENDER_SIZE,y + BLOCK_RENDER_SIZE,z - BLOCK_RENDER_SIZE), // Point 6, left upper rear
		glm::highp_vec3(x + BLOCK_RENDER_SIZE,y + BLOCK_RENDER_SIZE,z - BLOCK_RENDER_SIZE), // Point 7, right upper rear
	};

	if (frontFace == false) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert2, vert3; // Using Points 0, 1, 2, 3 and Normal 0
		glm::vec3 uv0 = glm::vec3(0, 0, blocks[0][0]);
		glm::vec3 uv1 = glm::vec3(1, 0, blocks[0][0]);
		glm::vec3 uv3 = glm::vec3(0, 1, blocks[0][0]);
		glm::vec3 uv2 = glm::vec3(1, 1, blocks[0][0]);
		vert0.uv = uv0; vert1.uv = uv1; vert2.uv = uv2; vert3.uv = uv3;
		vert0.position.xyz = vertices[0]; vert1.position.xyz = vertices[1];
		vert2.position.xyz = vertices[2]; vert3.position.xyz = vertices[3];
		vert0.normal = normals[0];  vert1.normal = normals[0];
		vert2.normal = normals[0];  vert3.normal = normals[0];
		float a00, a01, a10, a11; 
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1);
		i2 = this->mesh.addVert(vert2); i3 = this->mesh.addVert(vert3);
		this->mesh.addTriangle(i0, i1, i2); // Needs UVs {0,0}{1,0}{0,1}
		this->mesh.addTriangle(i0, i2, i3); // Needs UVs {1,0}{0,1}{1,1}
	}

	if (rightFace == false) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert2, vert3; // Using Points 1, 4, 7, 2 and Normal 1
		glm::vec3 uv0 = glm::vec3(0, 0, blocks[0][1]);
		glm::vec3 uv1 = glm::vec3(1, 0, blocks[0][1]);
		glm::vec3 uv3 = glm::vec3(0, 1, blocks[0][1]);
		glm::vec3 uv2 = glm::vec3(1, 1, blocks[0][1]);
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
		glm::vec3 uv0 = glm::vec3(0, 0, blocks[0][2]);
		glm::vec3 uv1 = glm::vec3(1, 0, blocks[0][2]);
		glm::vec3 uv3 = glm::vec3(0, 1, blocks[0][2]);
		glm::vec3 uv2 = glm::vec3(1, 1, blocks[0][2]);
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
		glm::vec3 uv0 = glm::vec3(0, 0, blocks[0][3]);
		glm::vec3 uv1 = glm::vec3(1, 0, blocks[0][3]);
		glm::vec3 uv3 = glm::vec3(0, 1, blocks[0][3]);
		glm::vec3 uv2 = glm::vec3(1, 1, blocks[0][3]);
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
		glm::vec3 uv0 = glm::vec3(0, 0, blocks[0][4]);
		glm::vec3 uv1 = glm::vec3(1, 0, blocks[0][4]);
		glm::vec3 uv3 = glm::vec3(0, 1, blocks[0][4]);
		glm::vec3 uv2 = glm::vec3(1, 1, blocks[0][4]);
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
		glm::vec3 uv0 = glm::vec3(0, 0, blocks[0][5]);
		glm::vec3 uv1 = glm::vec3(1, 0, blocks[0][5]);
		glm::vec3 uv3 = glm::vec3(0, 1, blocks[0][5]);
		glm::vec3 uv2 = glm::vec3(1, 1, blocks[0][5]);
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

void Chunk::buildRender() {

	bool def = true; 

		for (int i = 0; i < CHUNK_SIZE; i++) {
			for (int j = 0; j < CHUNK_SIZE_Z; j++) {
				for (int k = 0; k < CHUNK_SIZE; k++) {
					if (SIMPLE_CULLING_GLOBAL == true) {
						if (this->chunkBlocks[i][j][k].isActive() == false) {
							chunkBlocks[i][j][k].Type = blockType::AIR;
							continue;
						}

						else {
							bool xNeg = def; // left
							if (i > 0)
								xNeg = (this->chunkBlocks[i - 1][j][k].Active);

							bool xPos = def; // right
							if (i < CHUNK_SIZE - 1)
								xPos = (this->chunkBlocks[i + 1][j][k].Active);

							bool yPos = def; // bottom
							if (j > 0)
								yPos = this->chunkBlocks[i][j - 1][k].Active;
							bool yNeg = def; // top
							if (j < CHUNK_SIZE_Z - 1)
								yNeg = this->chunkBlocks[i][j + 1][k].Active;

							bool zNeg = def; // back
							if (k < CHUNK_SIZE - 1)
								zNeg = (this->chunkBlocks[i][j][k - 1].Active);

							bool zPos = def; // front
							if (k > 0)
								zPos = (this->chunkBlocks[i][j][k + 1].Active);

							if (j == 0) {
								this->chunkBlocks[i][j][k].Type = blockType::BEDROCK;
							}
							blockType uv_type = this->chunkBlocks[i][j][k].Type;
							this->createCube(i, j, k, zPos, xPos, yNeg, xNeg, yPos, zNeg, uv_type);
						}
					}
					else if (SIMPLE_CULLING_GLOBAL == false) {
						this->createCube(i, j, k, false, false, false, false, false, false);
					}
				}
			}
		}
		
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
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertType), (GLvoid*)offsetof(vertType,normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertType), (GLvoid*)offsetof(vertType, uv));

	glBindVertexArray(0);

}


void Chunk::chunkRender(Shader shader) {
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES,this->mesh.getNumIndices(),GL_UNSIGNED_INT,0);
	glBindVertexArray(0);
}