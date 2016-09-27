#include "stdafx.h"
#include "block.h"

/*
Bottom-Left-Back(0): {0,0,0} Bottom-Right-Back(1): {1,0,0} Top-Left-Back(2): {0,1,0} Top-Right-Back(3): {1,1,0}
Bottom-Left-Front(4): {0,0,1} Bottom-Right-Front(5): {1,0,1} Top-Left-Front(6): {0,1,1} Top-Right-Front(7): {1,1,1}
Back face: Tri0{0,1,2} Tri1{1,2,3} Bottom face: Tri0{0,1,4} Tri1{1,4,5} Left face: Tri0{0,6,4} Tri1{0,6,2}
Front face: Tri0{4,5,6} Tri1{5,6,7} Top face: Tri0{2,3,6} Tri1{3,7,6} Right face: Tri0{1,5,3} Tri1{5,7,3}
*/
static const float positions[1][8][3] = {
	{ { 0,0,0 },{ 1,0,0 },{ 0,1,0 },{ 1,1,0 },{ 0,0,1 },{ 1,0,1 },{ 0,1,1 },{ 1,1,1 } }
};

static const float normals[6][3] = {
	{0,0,1},
	{0,-1,0},
	{-1,0,0},
	{0,0,-1},
	{0,1,0},
	{1,0,0},
};

static const uint32_t indices[6][2][3] = {
	{ { 0,1,2 },{ 1,2,3 } }, // Back face
	{ { 0,1,4 },{ 1,4,5 } }, // Bottom face
	{ { 0,6,4 },{ 0,6,2 } }, // Left Face
	{ { 4,5,6 },{ 5,6,7 } }, // Front face
	{ { 2,3,5 },{ 3,7,6 } }, // Top face
	{ { 1,5,3 },{ 5,7,3 } }, // Right face
};


// Block constructor function
// Delete block data
Block::~Block() {
}
// return block's active status
bool Block::isActive(){
	return this->Active;
}
// return block's blockType
blockType Block::getType(){
	return this->Type;
}
// set block's blockType
void Block::setType(blockType type){
	this->Type = type;
}
/*
Mesh Block::setupMesh(){
	vertType vert; Mesh mesh;
	for (unsigned int i = 0; i < 8; i++) {
		glm::vec3 vec;
		vec.x = positions[0][i][0];
		vec.y = positions[0][i][1];
		vec.z = positions[0][i][2];
		vert.position = blockMesh.encodePosition(vec);
		mesh.addVert(vert);
	}
	for (unsigned int i = 0; i < 6; i++) {
			mesh.addTriangle(indices[i][0][0],indices[i][0][1],indices[i][0][2]);
			mesh.addTriangle(indices[i][1][0], indices[i][1][1], indices[i][1][2]);
	}
}
*/

// set block's active status
void Block::setActive(bool act) {
	this->Active = act;
}

void Block::setPos(glm::vec3 pos) {
	this->blockPos = pos;
}

void Block::setPos(float x, float y, float z) {
	this->blockPos.x = x;
	this->blockPos.y = y;
	this->blockPos.z = z;
}

glm::vec3 Block::getPos() {
	return this->blockPos;
}