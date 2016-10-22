#include "stdafx.h"
#include "chunk_manager.h"
#include "util/shader.h"

chunkManager::chunkManager(bool wireframeRender, bool primitiveCulling){
	this->wireframeRender = wireframeRender;
	this->primitiveCulling = primitiveCulling;
	this->numChunksLoaded = 0; this->numChunksRendered = 0;

}

void chunkManager::UpdateAsyncChunkManager(){
}

void chunkManager::UpdateLoadList() {

}

void chunkManager::UpdateRebuildList() {

}

void chunkManager::UpdateSetupList(){
}

void chunkManager::UpdateVisibilityList(glm::vec3 camerapos){
	glm::vec3 renderDist; 
	renderDist.x = abs(camerapos.x) + VIEW_DISTANCE;
	renderDist.y = abs(camerapos.y) + VIEW_DISTANCE; 
	renderDist.z = abs(camerapos.z) + VIEW_DISTANCE;
	for (auto chunk : this->chunkList) {
		glm::vec3 chunkDist;
		chunkDist.x = abs(camerapos.x - chunk.ChunkPos.x);
		chunkDist.y = abs(camerapos.y - chunk.ChunkPos.y);
		chunkDist.z = abs(camerapos.z - chunk.ChunkPos.z);
		if (chunkDist.x > renderDist.x
			|| chunkDist.y > renderDist.y
			|| chunkDist.z > renderDist.z) {
			chunk.ChunkRendered = false;
		}
	}
}


void chunkManager::UpdateRenderList(){
}

void chunkManager::update(float dt, glm::vec3 camerapos, glm::mat4 cameraview) {
	UpdateAsyncChunkManager();
	UpdateLoadList();
	UpdateRebuildList();
	UpdateSetupList();
	UpdateVisibilityList(cameraPos);
	

	if (this->cameraPos != camerapos) {
		UpdateRenderList();
	}
	this->cameraPos = camerapos;
}

void chunkManager::InitChunkBuild(Camera camera) {
	float x = camera.Position.x; float y = camera.Position.y; float z = camera.Position.z;
	int gridX = (int)((x + BLOCK_RENDER_SIZE) / (CHUNK_SIZE));
	int gridY = (int)((y + BLOCK_RENDER_SIZE) / (CHUNK_SIZE_Z));
	int gridZ = (int)((z + BLOCK_RENDER_SIZE) / (CHUNK_SIZE));
	createChunk(gridX, gridY, gridZ);
}

void chunkManager::createChunk(int grid_x, int grid_y, int grid_z) {
	normChunkPos normPos; normPos.x = grid_x; normPos.y = grid_y; normPos.z = grid_z;
	TreeChunk *newChunk = new TreeChunk(glm::ivec3(grid_x, grid_y, grid_z));
	//newChunk->buildTerrain(int seed);
	newChunk->BuildRender();
	newChunk->ChunkBuilt = true;
	this->chunkList.push_back(*newChunk);
}

void chunkManager::renderChunks(Shader shader) {
	for (unsigned int i = 0; i < this->chunkList.size(); ++i) {
		if (this->chunkList[i].ChunkRendered == true)
			this->chunkList[i].ChunkRender(shader);	
	}
}

