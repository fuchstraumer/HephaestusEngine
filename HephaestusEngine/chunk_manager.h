#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H
#include "stdafx.h"
#include "chunk.h"
#include <glm/glm.hpp>
#include "camera.h"


// This keeps the coordinates of a chunk as whole numbers. The actual world-space position
// of a chunk is calculated when generated, but keeping this position as whole numbers makes
// for easy indexing during update operations!
typedef glm::ivec3 normChunkPos;

class chunkManager {
public:
	chunkManager(bool wireframeRender = false, bool primitiveCulling = true);
	void update(float dt, glm::vec3 camerapos, glm::mat4 cameraview);

	void createChunk(int x, int y, int z );


	glm::vec3 cameraPos;
	glm::mat4 cameraView;
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
	void InitChunkBuild(Camera camera);
	void renderChunks(Shader shader);
private:
	bool wireframeRender = false;
	bool primitiveCulling = true;
	uint numChunksLoaded = 0;
	uint numChunksRendered = 0;

};
#endif // !CHUNK_MANAGER_H
