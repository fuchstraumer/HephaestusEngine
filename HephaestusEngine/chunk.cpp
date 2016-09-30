#include "stdafx.h"
#include "chunk.h"
#include "Terrain_Gen.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


static const bool simple_culling = true;

static vector<glm::highp_vec3> normals = { 
  glm::highp_vec3( 0, 0, 1),   // (front)
  glm::highp_vec3( 1, 0, 0),   // (right)
  glm::highp_vec3( 0, 1, 0),   // (top)
  glm::highp_vec3(-1, 0, 0),   // (left)
  glm::highp_vec3( 0,-1, 0),   // (bottom)
  glm::highp_vec3( 0, 0,-1),   // (back)
};
static vector<index_t> indices = 
{ 0, 1, 2,   2, 3, 0,      // front
  4, 5, 6,   6, 7, 4,      // right
  8, 9,10,  10,11, 8,      // top
 12,13,14,  14,15,12,      // left
 16,17,18,  18,19,16,      // bottom
 20,21,22,  22,23,20 };    // back

float triLerp_UseCube(float x, float y, float z, triLerpCube cube) {
	float s1 = (cube.cube_verts[0].w) * (1 - x) * (1 - y) * (1 - z);
	float s2 = (cube.cube_verts[1].w) * x * (1 - y) * (1 - z);
	float s3 = (cube.cube_verts[2].w) * (1 - x) * y * (1 - z);
	float s4 = (cube.cube_verts[3].w) * (1 - x) * (1 - y) * z;
	float s5 = (cube.cube_verts[4].w) * x * (1 - y) * z;
	float s6 = (cube.cube_verts[5].w) * (1 - x) * y * z;
	float s7 = (cube.cube_verts[6].w) * x * y * (1 - z);
	float s8 = (cube.cube_verts[7].w) * x * y * z;
	return (s1 + s2 + s3 + s4 + s5 + s6 + s7 + s8);
}

// Create block instances
Chunk::Chunk(glm::vec2 chunkPos){
	double*** terrain;
	this->chunkPos = chunkPos;
	Terrain_Generator gen; 
	terrain = gen.Generate();
	
	this->chunkBlocks = new Block**[CHUNK_SIZE];
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		this->chunkBlocks[i] = new Block*[CHUNK_SIZE_Y];
		for (int j = 0; j < CHUNK_SIZE_Y; ++j) {
	
			this->chunkBlocks[i][j] = new Block[CHUNK_SIZE];
		}
	}
	this->activecount = 0;
	this->inactivecount = 0;
	for (int i = 0; i < CHUNK_SIZE; i++) {
		for (int j = 0; j < CHUNK_SIZE_Y; j++) {
			for (int k = 0; k < CHUNK_SIZE; k++) {
				if (terrain[i,j,k] > 0.0) {
					this->activecount++;
					this->chunkBlocks[i][j][k].setActive(true);
					//this->chunkBlocks[i][j][k].setPos(i, j, k);
					//test_positions.push_back(this->chunkBlocks[i][j][k].blockPos);
				}
				//else if (terrain.get(i, j, k) <= 0.1) {
					//this->chunkBlocks[i][j][k].setActive(false);
					//this->inactivecount++;
				//}
			}
		}
	}
	
}
// Delete blocks
Chunk::~Chunk(){
	glDeleteBuffers(1, &this->VBO); glDeleteBuffers(1, &this->EBO);
	glDeleteVertexArrays(1, &this->VAO);
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			delete[] this->chunkBlocks[i][j];
		}
		delete[] this->chunkBlocks[i];
	}
	delete[] this->chunkBlocks;
}

void Chunk::createCube(int x, int y, int z, bool frontFace, bool rightFace, bool topFace, bool leftFace, bool bottomFace, bool backFace) {
	vector<glm::highp_vec3> vertices = {
		glm::highp_vec3(x - BLOCK_RENDER_SIZE,y - BLOCK_RENDER_SIZE,z + BLOCK_RENDER_SIZE), // Point 0
		glm::highp_vec3(x + BLOCK_RENDER_SIZE,y - BLOCK_RENDER_SIZE,z + BLOCK_RENDER_SIZE), // Point 1
		glm::highp_vec3(x + BLOCK_RENDER_SIZE,y + BLOCK_RENDER_SIZE,z + BLOCK_RENDER_SIZE), // Point 2
		glm::highp_vec3(x - BLOCK_RENDER_SIZE,y + BLOCK_RENDER_SIZE,z + BLOCK_RENDER_SIZE), // Point 3
		glm::highp_vec3(x + BLOCK_RENDER_SIZE,y - BLOCK_RENDER_SIZE,z - BLOCK_RENDER_SIZE), // Point 4
		glm::highp_vec3(x - BLOCK_RENDER_SIZE,y - BLOCK_RENDER_SIZE,z - BLOCK_RENDER_SIZE), // Point 5
		glm::highp_vec3(x - BLOCK_RENDER_SIZE,y + BLOCK_RENDER_SIZE,z - BLOCK_RENDER_SIZE), // Point 6
		glm::highp_vec3(x + BLOCK_RENDER_SIZE,y + BLOCK_RENDER_SIZE,z - BLOCK_RENDER_SIZE), // Point 7
	};
	if (backFace == true) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert4, vert3; // Using Points 0, 1, 2, 3 and Normal 0
		vert0.position = vertices[0]; vert1.position = vertices[1];
		vert4.position = vertices[2]; vert3.position = vertices[3];
		vert0.normal = normals[0];  vert1.normal = normals[0];
		vert4.normal = normals[0];  vert3.normal = normals[0];
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1);
		i2 = this->mesh.addVert(vert4); i3 = this->mesh.addVert(vert3);
		this->mesh.addTriangle(i0, i1, i2); this->mesh.addTriangle(i0, i2, i3);
	}
	if (leftFace == true) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert4, vert3; // Using Points 1, 4, 7, 2 and Normal 1
		vert0.position = vertices[1]; vert1.position = vertices[4];
		vert4.position = vertices[7]; vert3.position = vertices[2];
		vert0.normal = normals[1];  vert1.normal = normals[1];
		vert4.normal = normals[1];  vert3.normal = normals[1];
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1);
		i2 = this->mesh.addVert(vert4); i3 = this->mesh.addVert(vert3);
		this->mesh.addTriangle(i0, i1, i2); this->mesh.addTriangle(i0, i2, i3);
	}
	if (topFace == true) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert4, vert3; // Using Points 3, 2, 7, 6 and Normal 2
		vert0.position = vertices[3]; vert1.position = vertices[2];
		vert4.position = vertices[7]; vert3.position = vertices[6];
		vert0.normal = normals[2];  vert1.normal = normals[2];
		vert4.normal = normals[2];  vert3.normal = normals[2];
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1);
		i2 = this->mesh.addVert(vert4); i3 = this->mesh.addVert(vert3);
		this->mesh.addTriangle(i0, i1, i2); this->mesh.addTriangle(i0, i2, i3);
	}
	if (rightFace == true) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert4, vert3; // Using Points 5, 0, 3, 6 and Normal 3
		vert0.position = vertices[5]; vert1.position = vertices[0];
		vert4.position = vertices[3]; vert3.position = vertices[6];
		vert0.normal = normals[3];  vert1.normal = normals[3];
		vert4.normal = normals[3];  vert3.normal = normals[3];
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1);
		i2 = this->mesh.addVert(vert4); i3 = this->mesh.addVert(vert3);
		this->mesh.addTriangle(i0, i1, i2); this->mesh.addTriangle(i0, i2, i3);
	}
	if (bottomFace == true) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert4, vert3; // Using Points 5, 4, 1, 0 and Normal 4
		vert0.position = vertices[5]; vert1.position = vertices[4];
		vert4.position = vertices[1]; vert3.position = vertices[0];
		vert0.normal = normals[4];  vert1.normal = normals[4];
		vert4.normal = normals[4];  vert3.normal = normals[4];
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1);
		i2 = this->mesh.addVert(vert4); i3 = this->mesh.addVert(vert3);
		this->mesh.addTriangle(i0, i1, i2); this->mesh.addTriangle(i0, i2, i3);
	}
	if (frontFace == true) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert4, vert3; // Using Points 4, 5, 6, 7 and Normal 5
		vert0.position = vertices[4]; vert1.position = vertices[5];
		vert4.position = vertices[6]; vert3.position = vertices[7];
		vert0.normal = normals[5];  vert1.normal = normals[5];
		vert4.normal = normals[5];  vert3.normal = normals[5];
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1);
		i2 = this->mesh.addVert(vert4); i3 = this->mesh.addVert(vert3);
		this->mesh.addTriangle(i0, i1, i2); this->mesh.addTriangle(i0, i2, i3);
	}
}

void Chunk::buildRender() {
	bool def = true; 
		for (int i = 0; i < CHUNK_SIZE; i++) {
			for (int j = 0; j < CHUNK_SIZE_Y; j++) {
				for (int k = 0; k < CHUNK_SIZE; k++) {
					if (this->chunkBlocks[i][j][k].isActive() == false) {
						continue;
					}
					else{
						bool xNeg = def; // left
						if (i > 0)
							xNeg = (this->chunkBlocks[i - 1][j][k].Active);
						bool xPos = def; // right
						if (i < CHUNK_SIZE - 1)
							xPos = (this->chunkBlocks[i + 1][j][k].Active);
						bool yPos = def; // bottom
						if (j > 0)
							yPos = (this->chunkBlocks[i][j - 1][k].Active);
						bool yNeg = def; // top
						if (j < CHUNK_SIZE_Y - 1)
							yNeg = (this->chunkBlocks[i][j + 1][k].Active);
						bool zNeg = def; // back
						if (k < CHUNK_SIZE - 1)
							zNeg = (this->chunkBlocks[i][j][k - 1].Active);
						bool zPos = def; // front
						if (k > 0)
							zPos = (this->chunkBlocks[i][j][k + 1].Active);
						this->createCube(i,j,k, zPos, xPos, yPos, xNeg, yNeg, zNeg);
						
					}
				}
			}
		}
		
	glGenVertexArrays(1, &this->VAO); 
	glGenBuffers(1, &this->VBO); glGenBuffers(1, &this->EBO);
	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->mesh.meshVerts.size() * sizeof(vertType), &(this->mesh.meshVerts[0]), GL_STATIC_DRAW);
	std::cerr << "buffer size is " << this->mesh.meshVerts.data() << std::endl;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->mesh.meshIndices.size() * sizeof(index_t), &(this->mesh.meshIndices[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertType), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertType), (GLvoid*)offsetof(vertType,normal));
	std::cerr << "Size of indices: " << (this->mesh.meshIndices.size() * sizeof(index_t)) << std::endl;
	std::cerr << " Size of vertices: " << (this->mesh.meshVerts.size() * sizeof(vertType)) << std::endl;
	glBindVertexArray(0);
}


void Chunk::chunkRender(Shader shader) {
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES,this->mesh.getNumIndices(),GL_UNSIGNED_INT,0);
}