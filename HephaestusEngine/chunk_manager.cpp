#include "stdafx.h"
#include "chunk_manager.h"


void chunkManager::UpdateLoadList() {

}

void chunkManager::UpdateRebuildList() {

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

void chunkManager::createChunk(int x, int y, int z) {
	normChunkPos normPos; normPos.x = x; normPos.y = y; normPos.z = z;
	Chunk* newChunk = new Chunk(glm::vec2(x, y));
	newChunk->buildRender();
	newChunk->chunkBuilt = true;

}

