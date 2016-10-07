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
}

void chunkManager::UpdateFrustrumList(glm::mat4 cameraview){
}

void chunkManager::UpdateRenderList(){
}

void chunkManager::update(float dt, glm::vec3 camerapos, glm::mat4 cameraview) {
	UpdateAsyncChunkManager();
	UpdateLoadList();
	UpdateRebuildList();
	UpdateSetupList();
	UpdateVisibilityList(cameraPos);
	UpdateFrustrumList(cameraView);

	if (this->cameraPos != camerapos || this->cameraView != cameraview) {
		UpdateRenderList();
	}
	this->cameraPos = camerapos;
	this->cameraView = cameraview;
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
	Chunk *newChunk = new Chunk(glm::ivec3(grid_x, grid_y, grid_z));
	newChunk->buildTerrain();
	newChunk->buildRender();
	newChunk->chunkBuilt = true;
	this->chunkList.push_back(*newChunk);
}

void chunkManager::renderChunks(Shader shader) {
	for (unsigned int i = 0; i < this->chunkList.size(); ++i) {
		this->chunkList[i].chunkRender(shader);
		this->chunkList[i].chunkRendered = true;
	}
}

