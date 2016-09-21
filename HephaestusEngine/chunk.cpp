#include "stdafx.h"
#include "chunk.h"


// Create block instances
Chunk::Chunk(){
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
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			delete[] chunkBlocks[i][j];
		}
		delete[] chunkBlocks[i];
	}
	delete[] chunkBlocks;
}
