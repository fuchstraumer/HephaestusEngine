#include "stdafx.h"
#include "chunk.h"
#include "Terrain_Gen.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma warning(disable:4244)
static const float BLOCK_RENDER_SIZE = 1.0001f;
static const bool simple_culling = true;
/*vector<glm::vec3> verts2 ={ 
glm::vec3(0.5 + x,0.5 + y,0.5 + z),glm::vec3(-0.5 + x,0.5 + y,0.5 + z),glm::vec3(-0.5 + x,-0.5 + y,0.5 + z),glm::vec3(0.5 + x,-0.5 + y,0.5 + z),   // v0,v0.5,v2,v3 (front)
glm::vec3(0.5 + x,0.5 + y,0.5 + z),glm::vec3(0.5 + x,-0.5 + y, 0.5 + z),glm::vec3(0.5 + x,-0.5 + y,-0.5 + z),glm::vec3(0.5 + x,0.5 + y,-0.5 + z),   // v0,v3,v4,v5 (right)
glm::vec3(0.5 + x,0.5 + y,0.5 + z),glm::vec3(0.5 + x,0.5 + y,-0.5 + z),glm::vec3(-0.5 + x,0.5 + y,-0.5 + z),glm::vec3(-0.5 + x,0.5 + y,0.5 + z),   // v0,v5,v6,v0.5 (top)
glm::vec3(-0.5 + x,0.5 + y,0.5 + z),glm::vec3(-0.5 + x,0.5 + y,-0.5 + z),glm::vec3(-0.5 + x,-0.5 + y,-0.5 + z),glm::vec3(-0.5 + x,-0.5 + y,0.5 + z),   // v0.5,v6,v7,v2 (left)
glm::vec3(-0.5 + x,-0.5 + y,-0.5 + z),glm::vec3(0.5 + x,-0.5 + y,-0.5 + z),glm::vec3(0.5 + x,-0.5 + y,0.5 + z),glm::vec3(-0.5 + x,-0.5 + y,0.5 + z),   // v7,v4,v3,v2 (bottom)
glm::vec3(0.5 + x,-0.5 + y,-0.5 + z),glm::vec3(-0.5 + x,-0.5 + y,-0.5 + z),glm::vec3(-0.5 + x,0.5 + y,-0.5 + z),glm::vec3(0.5 + x,0.5 + y,-0.5 + z) }; // v4,v7,v6,v5 (back)*/
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

static float lerp(float x, float x1, float x2, float q00, float q01) {
	return ((x2 - x) / (x2 - x1)) * q00 + ((x - x1) / (x2 - x1)) * q01;
}

static float biLerp(float x, float y, float q11, float q12, float q21, float q22, float x1, float x2, float y1, float y2) {
	float r1 = lerp(x, x1, x2, q11, q21);
	float r2 = lerp(x, x1, x2, q12, q22);

	return lerp(y, y1, y2, r1, r2);
}

static float triLerp(float x, float y, float z, float q000, float q001, float q010, float q011, float q100, float q101, float q110, float q111, float x1, float x2, float y1, float y2, float z1, float z2) {
	float x00 = lerp(x, x1, x2, q000, q100);
	float x10 = lerp(x, x1, x2, q010, q110);
	float x01 = lerp(x, x1, x2, q001, q101);
	float x11 = lerp(x, x1, x2, q011, q111);
	float r0 = lerp(y, y1, y2, x00, x01);
	float r1 = lerp(y, y1, y2, x10, x11);

	return lerp(z, z1, z2, r0, r1);
}

// Create block instances
Chunk::Chunk(){
	Terrain_Generator gen; 
	this->terrain = CArray3Dd(CHUNK_SIZE, CHUNK_SIZE_Y, CHUNK_SIZE);
	this->terrain = gen.generator(CHUNK_SIZE, CHUNK_SIZE_Y, CHUNK_SIZE);
	this->chunkBlocks = new Block**[CHUNK_SIZE];
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		this->chunkBlocks[i] = new Block*[CHUNK_SIZE_Y];
		for (int j = 0; j < CHUNK_SIZE_Y; ++j) {
	
			this->chunkBlocks[i][j] = new Block[CHUNK_SIZE];
		}
	}
	this->activecount = 0;
	for (int i = 0; i < CHUNK_SIZE; i++) {
		for (int j = 0; j < CHUNK_SIZE_Y; j++) {
			for (int k = 0; k < CHUNK_SIZE; k++) {
				if (this->terrain.get(i, j, k) == 1) {
					this->activecount++;
					this->chunkBlocks[i][j][k].setActive(true);
					//this->chunkBlocks[i][j][k].setPos(i, j, k);
					//test_positions.push_back(this->chunkBlocks[i][j][k].blockPos);
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

void Chunk::createCube(GLfloat x, GLfloat y, GLfloat z, bool frontFace, bool rightFace, bool topFace, bool leftFace, bool bottomFace, bool backFace) {
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
	if (frontFace == true) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert2, vert3; // Using Points 0, 1, 2, 3 and Normal 0
		vert0.position = vertices[0]; vert1.position = vertices[1];
		vert2.position = vertices[2]; vert3.position = vertices[3];
		vert0.normal = normals[0];  vert1.normal = normals[0];
		vert2.normal = normals[0];  vert3.normal = normals[0];
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1);
		i2 = this->mesh.addVert(vert2); i3 = this->mesh.addVert(vert3);
		this->mesh.addTriangle(i0, i1, i2); this->mesh.addTriangle(i0, i2, i3);
	}
	if (rightFace == true) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert2, vert3; // Using Points 1, 4, 7, 2 and Normal 1
		vert0.position = vertices[1]; vert1.position = vertices[4];
		vert2.position = vertices[7]; vert3.position = vertices[2];
		vert0.normal = normals[1];  vert1.normal = normals[1];
		vert2.normal = normals[1];  vert3.normal = normals[1];
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1);
		i2 = this->mesh.addVert(vert2); i3 = this->mesh.addVert(vert3);
		this->mesh.addTriangle(i0, i1, i2); this->mesh.addTriangle(i0, i2, i3);
	}
	if (topFace == true) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert2, vert3; // Using Points 3, 2, 7, 6 and Normal 2
		vert0.position = vertices[3]; vert1.position = vertices[2];
		vert2.position = vertices[7]; vert3.position = vertices[6];
		vert0.normal = normals[2];  vert1.normal = normals[2];
		vert2.normal = normals[2];  vert3.normal = normals[2];
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1);
		i2 = this->mesh.addVert(vert2); i3 = this->mesh.addVert(vert3);
		this->mesh.addTriangle(i0, i1, i2); this->mesh.addTriangle(i0, i2, i3);
	}
	if (leftFace == true) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert2, vert3; // Using Points 5, 0, 3, 6 and Normal 3
		vert0.position = vertices[5]; vert1.position = vertices[0];
		vert2.position = vertices[3]; vert3.position = vertices[6];
		vert0.normal = normals[3];  vert1.normal = normals[3];
		vert2.normal = normals[3];  vert3.normal = normals[3];
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1);
		i2 = this->mesh.addVert(vert2); i3 = this->mesh.addVert(vert3);
		this->mesh.addTriangle(i0, i1, i2); this->mesh.addTriangle(i0, i2, i3);
	}
	if (bottomFace == true) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert2, vert3; // Using Points 5, 4, 1, 0 and Normal 4
		vert0.position = vertices[5]; vert1.position = vertices[4];
		vert2.position = vertices[1]; vert3.position = vertices[0];
		vert0.normal = normals[4];  vert1.normal = normals[4];
		vert2.normal = normals[4];  vert3.normal = normals[4];
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1);
		i2 = this->mesh.addVert(vert2); i3 = this->mesh.addVert(vert3);
		this->mesh.addTriangle(i0, i1, i2); this->mesh.addTriangle(i0, i2, i3);
	}
	if (backFace == true) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert2, vert3; // Using Points 4, 5, 6, 7 and Normal 5
		vert0.position = vertices[4]; vert1.position = vertices[5];
		vert2.position = vertices[6]; vert3.position = vertices[7];
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
			for (int j = 0; j < CHUNK_SIZE_Y; j++) {
				for (int k = 0; k < CHUNK_SIZE; k++) {
					if (this->chunkBlocks[i][j][k].isActive() == false) {
						continue;
					}
					else{
						bool xNeg = def; // left
						if (i > 0)
							xNeg = (this->chunkBlocks[i - 1][j][k].isActive());
						bool xPos = def; // right
						if (i < CHUNK_SIZE - 1)
							xPos = (this->chunkBlocks[i + 1][j][k].isActive());
						bool yPos = def; // bottom
						if (j > 0)
							yPos = (this->chunkBlocks[i][j - 1][k].isActive());
						bool yNeg = def; // top
						if (j < CHUNK_SIZE_Y - 1)
							yNeg = (this->chunkBlocks[i][j + 1][k].isActive());
						bool zNeg = def; // back
						if (k > 0)
							zNeg = (this->chunkBlocks[i][j][k - 1].isActive());
						bool zPos = def; // front
						if (k < CHUNK_SIZE - 1)
							zPos = (this->chunkBlocks[i][j][k + 1].isActive());
						this->createCube((GLfloat)i,(GLfloat)j,(GLfloat)k, true, true, true, true, true, true);
						
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