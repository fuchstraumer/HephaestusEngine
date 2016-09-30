#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H
#include "stdafx.h"
#include "chunk.h"
#include <glm/glm.hpp>


// This keeps the coordinates of a chunk as whole numbers. The actual world-space position
// of a chunk is calculated when generated, but keeping this position as whole numbers makes
// for easy indexing during update operations!
typedef glm::vec3 normChunkPos;

class chunkManager {
public:
	void update(float dt, glm::vec3 camerapos, glm::mat4 cameraview);

	void createChunk(int x, int y, int z);


	glm::vec3 cameraPos;
	glm::mat4 cameraView;
	int numChunksLoaded;
	std::vector<Chunk> chunkList;
	std::vector<Chunk> chunkBuildList;
	std::vector<Chunk> chunkLoadList;
	std::vector<Chunk> chunkUpdateList;
	std::vector<Chunk> chunkRenderList;
	std::vector<Chunk> chunkVisList;

	void UpdateAsyncChunkManager();
	void UpdateLoadList();
	void UpdateRebuildList();
	void UpdateSetupList();
	void UpdateVisibilityList(glm::vec3 camerapos);
	void UpdateFrustrumList(glm::mat4 cameraview);
	void UpdateRenderList();


};
#endif // !CHUNK_MANAGER_H
