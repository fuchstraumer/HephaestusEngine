#include "stdafx.h"
#include "block.h"
// Block constructor function
Block::Block(bool active, blockType type){
	this->Active = active;
	this->Type = type;
}
// Delete block data
Block::~Block() {
	for (unsigned int i = 0; i < this->blockVerts.size(); ++i) {
		delete[] &blockVerts;
	}
	delete[] &blockFaces;
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
// build the data needed to construct a mesh for this block
void setupMesh() {
	// Question- how to build mesh, and then build vertices too?
}