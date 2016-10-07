#include "stdafx.h"
#include "chunk.h"
#include "Terrain_Gen.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLfloat vertices[] = {
	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5,
	0.5f,  0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f,  0.5f,
	0.5f, -0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,

	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,

	0.5f,  0.5f,  0.5f,
	0.5f,  0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,

	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f,  0.5f,
	0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f,  0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
	0.5f,  0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
};

static GLfloat positions[] = {
     0,0,0 , // 0
	 1,0,0 , // 1
	 0,1,0 , // 2
	 1,1,0 , // 3
	 0,0,1 , // 4
	 1,0,1 , // 5
	 0,1,1 , // 6
	 1,1,1 , // 7
};

static const float normals[6][3] = {
	{ 0,0,1 },
	{ 0,-1,0 },
	{ -1,0,0 },
	{ 0,0,-1 },
	{ 0,1,0 },
	{ 1,0,0 },
};

static const GLuint indices[] = {
	0,1,2, 1,2,3, // Back face
	0,1,4, 1,4,5, // Bottom face
	0,6,4, 0,6,2, // Left Face
	4,5,6, 5,6,7, // Front face
	2,3,5, 3,7,6, // Top face
	1,5,3, 5,7,3, // Right face
};


// Create block instances
Chunk::Chunk(){
	Terrain_Generator gen;
	this->terrain = CArray3Dd(CHUNK_SIZE, CHUNK_SIZE_Z, CHUNK_SIZE);
	this->terrain = gen.generator(CHUNK_SIZE, CHUNK_SIZE_Z, CHUNK_SIZE);
	chunkBlocks = new Block**[CHUNK_SIZE];
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		chunkBlocks[i] = new Block*[CHUNK_SIZE];
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			chunkBlocks[i][j] = new Block[CHUNK_SIZE_Z];
		}
	}
	
}
// Delete blocks
Chunk::~Chunk(){
	glDeleteBuffers(1, &this->VBO); //glDeleteBuffers(1, &this->EBO);
	glDeleteVertexArrays(1, &this->VAO);
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			delete[] chunkBlocks[i][j];
		}
		delete[] chunkBlocks[i];
	}
	delete[] chunkBlocks;
}

void Chunk::buildRender(Shader ourShader) {
	/*vertType vert; std::vector<glm::vec3> test_verts;
	for (unsigned int t = 0; t < 8; t++) {
		glm::vec3 vec;
		vec.x = positions[t+2];
		vec.y = positions[t+1];
		vec.z = positions[t];
		vert.position = mesh.encodePosition(vec);
		test_verts.push_back(vec);
		mesh.addVert(vert);
	}
	for (unsigned int t = 0; t < 6; t++) {
		mesh.addTriangle(indices[t][0][0], indices[t][0][1], indices[t][0][2]);
		mesh.addTriangle(indices[t][1][0], indices[t][1][1], indices[t][1][2]);
	}*/
	for (int i = 0; i < 1296; i++) {
		vertices[i] = (GLshort)vertices[i]*256.0f;
	}
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO); //glGenBuffers(1, &this->EBO);
	glBindVertexArray(this->VAO);
	glBindBuffer(this->VBO, GL_ARRAY_BUFFER);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glBindBuffer(this->EBO, GL_ELEMENT_ARRAY_BUFFER);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_SHORT, GL_FALSE, 3*sizeof(GLshort), (GLvoid*)0);

	glBindVertexArray(0);
}


void Chunk::chunkRender(Shader ourShader) {

	glBindVertexArray(this->VAO);
	GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
	for (unsigned int i = 0; i < CHUNK_SIZE;i++) {
		for (unsigned int j = 0; j < CHUNK_SIZE_Z; j++) {
			for (unsigned int k = 0; k < CHUNK_SIZE; k++) {
				if (this->terrain.get(i, j, k) != 0) {
					glm::vec3 translate; 
					translate.x = i; translate.y = j; translate.z = k;
					glm::mat4 model;
					model = glm::translate(model, translate);
					glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
					glDrawArrays(GL_TRIANGLES, 0, 36);
					
				}
			}
		}
	}
	
}