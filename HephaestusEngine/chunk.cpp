#include "stdafx.h"
#include "chunk.h"
#include "Terrain_Gen.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma warning(disable:4244)

static const bool simple_culling = true;

static vector<glm::vec3> normals =
{ glm::vec3(0, 0, 1),glm::vec3(0, 0, 1),glm::vec3(0, 0, 1),glm::vec3(0, 0, 1),   // v0,v1,v2,v3 (front)
  glm::vec3(1, 0, 0),glm::vec3(1, 0, 0),glm::vec3(1, 0, 0),glm::vec3(1, 0, 0),   // v0,v3,v4,v5 (right)
  glm::vec3(0, 1, 0),glm::vec3(0, 1, 0),glm::vec3(0, 1, 0),glm::vec3(0, 1, 0),   // v0,v5,v6,v1 (top)
  glm::vec3(-1, 0, 0),glm::vec3(-1, 0, 0),glm::vec3(-1, 0, 0),glm::vec3(-1, 0, 0),   // v1,v6,v7,v2 (left)
  glm::vec3(0,-1, 0),glm::vec3(0,-1, 0),glm::vec3(0,-1, 0),glm::vec3(0,-1, 0),   // v7,v4,v3,v2 (bottom)
  glm::vec3(0, 0,-1),glm::vec3(0, 0,-1),glm::vec3(0, 0,-1),glm::vec3(0, 0,-1) };
static vector<index_t> indices = 
{ 0, 1, 2,   2, 3, 0,      // front
  4, 5, 6,   6, 7, 4,      // right
  8, 9,10,  10,11, 8,      // top
 12,13,14,  14,15,12,      // left
 16,17,18,  18,19,16,      // bottom
 20,21,22,  22,23,20 };    // back

// Create block instances
Chunk::Chunk(){
	Terrain_Generator gen;
	this->terrain = CArray3Dd(CHUNK_SIZE, CHUNK_SIZE_Z, CHUNK_SIZE);
	this->terrain = gen.generator(CHUNK_SIZE, CHUNK_SIZE_Z, CHUNK_SIZE);
	this->chunkBlocks = new Block**[CHUNK_SIZE];
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		this->chunkBlocks[i] = new Block*[CHUNK_SIZE_Z];
		for (int j = 0; j < CHUNK_SIZE_Z; ++j) {
			this->chunkBlocks[i][j] = new Block[CHUNK_SIZE];
		}
	}
	this->activecount = 0;
	for (int i = 0; i < CHUNK_SIZE; i++) {
		for (int j = 0; j < CHUNK_SIZE_Z; j++) {
			for (int k = 0; k < CHUNK_SIZE; k++) {
				if (this->terrain.get(i, j, k) == 1) {
					this->activecount++;
					this->chunkBlocks[i][j][k].setActive(true);				
				}
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

void Chunk::createCube(float x, float y, float z, bool frontFace, bool rightFace, bool topFace, bool leftFace, bool bottomFace, bool backFace) {
	vector<glm::vec3> vertices =
	{   glm::vec3(x+1,y+1,1+z),glm::vec3(-1+x,1+y,1+z),glm::vec3(-1+x,-1+y,1+z),glm::vec3(1+x,-1+y,1+z),   // v0,v1,v2,v3 (front)
		glm::vec3(1+x,1+y,1+z),glm::vec3(1+x,-1+y, 1+z),glm::vec3(1+x,-1+y,-1+z),glm::vec3(1+x,1+y,-1+z),   // v0,v3,v4,v5 (right)
		glm::vec3(1+x,1+y,1+z),glm::vec3(1+x,1+y,-1+z),glm::vec3(-1+x,1+y,-1+z),glm::vec3(-1+x,1+y,1+z),   // v0,v5,v6,v1 (top)
		glm::vec3(-1+x,1+y,1+z),glm::vec3(-1+x,1+y,-1+z),glm::vec3(-1+x,-1+y,-1+z),glm::vec3(-1+x,-1+y,1+z),   // v1,v6,v7,v2 (left)
		glm::vec3(-1+x,-1+y,-1+z),glm::vec3(1+x,-1+y,-1+z),glm::vec3(1+x,-1+y,1+z),glm::vec3(-1+x,-1+y,1+z),   // v7,v4,v3,v2 (bottom)
		glm::vec3(1+x,-1+y,-1+z),glm::vec3(-1+x,-1+y,-1+z),glm::vec3(-1+x,1+y,-1+z),glm::vec3(1+x,1+y,-1+z) }; // v4,v7,v6,v5 (back)
	if (frontFace == true) {
		index_t i0, i1, i2; vertType vert0, vert1, vert2;
		vert0.position = mesh.encodePosition(vertices[0]); vert1.position = mesh.encodePosition(vertices[1]); vert2.position = mesh.encodePosition(vertices[2]);
		vert0.normal = mesh.encodeNormal(normals[0]);  vert1.normal = mesh.encodeNormal(normals[1]); vert2.normal = mesh.encodeNormal(normals[2]);
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1); i2 = this->mesh.addVert(vert2);
		this->mesh.addTriangle(i0, i1, i2);
		vert0.position = mesh.encodePosition(vertices[2]); vert1.position = mesh.encodePosition(vertices[3]); vert2.position = mesh.encodePosition(vertices[0]);
		vert0.normal = mesh.encodeNormal(normals[2]); vert1.normal = mesh.encodeNormal(normals[3]); vert2.normal = mesh.encodeNormal(normals[0]);
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1); i2 = this->mesh.addVert(vert2);
		this->mesh.addTriangle(i0, i1, i2);
	}
	if (rightFace == true) {
		index_t i0, i1, i2; vertType vert0, vert1, vert2;
		vert0.position = mesh.encodePosition(vertices[4]); vert1.position = mesh.encodePosition(vertices[5]); vert2.position = mesh.encodePosition(vertices[6]);
		vert0.normal = mesh.encodeNormal(normals[4]); vert1.normal = mesh.encodeNormal(normals[5]); vert2.normal = mesh.encodeNormal(normals[6]);
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1); i2 = this->mesh.addVert(vert2);
		this->mesh.addTriangle(i0, i1, i2);
		vert0.position = mesh.encodePosition(vertices[6]); vert1.position = mesh.encodePosition(vertices[7]); vert2.position = mesh.encodePosition(vertices[4]);
		vert0.normal = mesh.encodeNormal(normals[6]); vert1.normal = mesh.encodeNormal(normals[7]); vert2.normal = mesh.encodeNormal(vertices[4]);
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1); i2 = this->mesh.addVert(vert2);
		this->mesh.addTriangle(i0, i1, i2);
	}
	if (topFace == true) {
		index_t i0, i1, i2; vertType vert0, vert1, vert2;
		vert0.position = mesh.encodePosition(vertices[8]); vert1.position = mesh.encodePosition(vertices[9]); vert2.position = mesh.encodePosition(vertices[10]);
		vert0.normal = mesh.encodeNormal(normals[8]); vert1.normal = mesh.encodeNormal(normals[9]); vert2.normal = mesh.encodeNormal(normals[10]);
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1); i2 = this->mesh.addVert(vert2);
		this->mesh.addTriangle(i0, i1, i2);
		vert0.position = mesh.encodePosition(vertices[10]); vert1.position = mesh.encodePosition(vertices[11]); vert2.position = mesh.encodePosition(vertices[8]);
		vert0.normal = mesh.encodeNormal(normals[10]); vert1.normal = mesh.encodeNormal(normals[11]); vert2.normal = mesh.encodeNormal(vertices[8]);
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1); i2 = this->mesh.addVert(vert2);
		this->mesh.addTriangle(i0, i1, i2);
	}
	if (leftFace == true) {
		index_t i0, i1, i2; vertType vert0, vert1, vert2;
		vert0.position = mesh.encodePosition(vertices[12]); vert1.position = mesh.encodePosition(vertices[13]); vert2.position = mesh.encodePosition(vertices[14]);
		vert0.normal = mesh.encodeNormal(normals[12]); vert1.normal = mesh.encodeNormal(normals[13]); vert2.normal = mesh.encodeNormal(normals[14]);
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1); i2 = this->mesh.addVert(vert2);
		this->mesh.addTriangle(i0, i1, i2);
		vert0.position = mesh.encodePosition(vertices[14]); vert1.position = mesh.encodePosition(vertices[15]); vert2.position = mesh.encodePosition(vertices[12]);
		vert0.normal = mesh.encodeNormal(normals[14]); vert1.normal = mesh.encodeNormal(normals[15]); vert2.normal = mesh.encodeNormal(vertices[12]);
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1); i2 = this->mesh.addVert(vert2);
		this->mesh.addTriangle(i0, i1, i2);
	}
	if (bottomFace == true) {
		index_t i0, i1, i2; vertType vert0, vert1, vert2;
		vert0.position = mesh.encodePosition(vertices[16]); vert1.position = mesh.encodePosition(vertices[17]); vert2.position = mesh.encodePosition(vertices[18]);
		vert0.normal = mesh.encodeNormal(normals[17]); vert1.normal = mesh.encodeNormal(normals[17]); vert2.normal = mesh.encodeNormal(normals[18]);
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1); i2 = this->mesh.addVert(vert2);
		this->mesh.addTriangle(i0, i1, i2);
		vert0.position = mesh.encodePosition(vertices[18]); vert1.position = mesh.encodePosition(vertices[19]); vert2.position = mesh.encodePosition(vertices[16]);
		vert0.normal = mesh.encodeNormal(normals[18]); vert1.normal = mesh.encodeNormal(normals[19]); vert2.normal = mesh.encodeNormal(vertices[16]);
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1); i2 = this->mesh.addVert(vert2);
		this->mesh.addTriangle(i0, i1, i2);
	}
	if (backFace == true) {
		index_t i0, i1, i2; vertType vert0, vert1, vert2;
		vert0.position = mesh.encodePosition(vertices[20]); vert1.position = mesh.encodePosition(vertices[21]); vert2.position = mesh.encodePosition(vertices[22]);
		vert0.normal = mesh.encodeNormal(normals[20]); vert1.normal = mesh.encodeNormal(normals[21]); vert2.normal = mesh.encodeNormal(normals[22]);
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1); i2 = this->mesh.addVert(vert2);
		this->mesh.addTriangle(i0, i1, i2);
		vert0.position = mesh.encodePosition(vertices[22]); vert1.position = mesh.encodePosition(vertices[23]); vert2.position = mesh.encodePosition(vertices[20]);
		vert0.normal = mesh.encodeNormal(normals[22]); vert1.normal = mesh.encodeNormal(normals[23]); vert2.normal = mesh.encodeNormal(vertices[20]);
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1); i2 = this->mesh.addVert(vert2);
		this->mesh.addTriangle(i0, i1, i2);
	}
}

void Chunk::buildRender() {
	bool def = true;
	for (int i = 0; i < CHUNK_SIZE; i++) {
		for (int j = 0; j < CHUNK_SIZE_Z; j++) {
			for (int k = 0; k < CHUNK_SIZE; k++) {
					bool xNeg = def; // left
					if (i > 0)
						xNeg = (this->chunkBlocks[i - 1][j][k].isActive());
					bool xPos = def; // right
					if (i < CHUNK_SIZE - 1)
						xPos = (this->chunkBlocks[i + 1][j][k].isActive());
					bool yNeg = def; // bottom
					if (j > 0)
						yNeg = (this->chunkBlocks[i][j - 1][k].isActive());
					bool yPos = def; // top
					if (j < CHUNK_SIZE_Z - 1)
						yPos = (this->chunkBlocks[i][j + 1][k].isActive());
					bool zNeg = def; // back
					if (k > 0)
						zNeg = (this->chunkBlocks[i][j][k - 1].isActive());
					bool zPos = def; // front
					if (k < CHUNK_SIZE - 1)
						zPos = (this->chunkBlocks[i][j][k + 1].isActive());
					this->createCube(i, j, k, zPos, xPos, yPos, xNeg, yNeg, zNeg);		
			}
		}
	}
	glGenVertexArrays(1, &this->VAO); 
	glGenBuffers(1, &this->VBO); glGenBuffers(1, &this->EBO);
	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->mesh.meshVerts.size() * sizeof(vertType), &this->mesh.meshVerts[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->mesh.meshIndices.size() * sizeof(index_t), &this->mesh.meshIndices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_SHORT, GL_FALSE, sizeof(compressed_vertex), (GLvoid*)(offsetof(compressed_vertex, position)));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(compressed_vertex), (GLvoid*)(offsetof(compressed_vertex, normal)));
	std::cerr << "Size of indices: " << (this->mesh.meshIndices.size() * sizeof(index_t)) << std::endl;
	std::cerr << " Size of vertices: " << (this->mesh.meshVerts.size() * sizeof(compressed_vertex)) << std::endl;
	glBindVertexArray(0);
}


void Chunk::chunkRender() {
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES,this->mesh.meshIndices.size(),GL_UNSIGNED_INT,0);
}