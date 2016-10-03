#include "stdafx.h"
#include "block.h"

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