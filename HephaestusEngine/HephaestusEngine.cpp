#include "stdafx.h"
#include "util\lodeTexture.h"
#include "util\MortonChunk.h"
#include "util\Viewport.h"
#include <ctime>

static GLuint WIDTH = 1440, HEIGHT = 900;

int main() {

	std::string stringSeed;
	std::cout << "Enter a string of characters to use as the terrain gen seed value: " << std::endl;
	std::cin >> stringSeed;
	int chunks;
	std::cout << "Enter an integer number from 2 through 48 to specify the amount of chunks to generate: " << std::endl;
	std::cout << "Warning: Using values greater than 16 may cause memory allocation crashes. " << std::endl;
	std::cin >> chunks;
	std::cout << "What terrain generator would you like to use?" << std::endl;
	std::cout << "Options: 0 = FBM, 1 = Billow, 2 = RidgedMulti, 3 = SwissNoise" << std::endl;
	int terrainType; std::cin >> terrainType;
	if (terrainType < 0 || terrainType > 3) {
		std::cout << "Erronenous terrain generator value chosen. Defaulting to FBM..." << std::endl;
		terrainType = 0;
	}
	int intSeed;
	std::stringstream(stringSeed) >> intSeed;
	// Build and seed our terrain generator
	TerrainGenerator gen(intSeed);
	if (chunks > 16) {
		chunks = 16;
	}

	// Create main window
	Viewport MainWindow(WIDTH, HEIGHT);

	auto& currProgram = MainWindow.CoreProgram;

	// List of uniforms we expect to access
	std::vector<std::string> uniforms = {
		{"model"},
		{"view"},
		{"projection"},
		{"lightPos"},
		{"viewPos"},
		{"lightColor"},
		{"texSampler"},
		{"normTransform"},
	};

	// Build the hash map for the uniforms we need
	currProgram.BuildUniformMap(uniforms);

	// List of textures to use in the texture array
	std::vector<std::string> filelist = {
		{ "./textures/blocks/grass_top.png" },
		{ "./textures/blocks/grass_side.png" },
		{ "./textures/blocks/dirt.png" },
		{ "./textures/blocks/sand.png" },
		{ "./textures/blocks/stone.png" },
		{ "./textures/blocks/bedrock.png" },
		{ "./textures/blocks/tallgrass.png" },
		{ "./textures/blocks/coal_ore.png" },
		{ "./textures/blocks/iron_ore.png" },
		{ "./textures/blocks/diamond_ore.png" },
	};
	// Create texture array
	TextureArray textures(filelist, 16);
	// Build the texture array, readying it for use/activation
	textures.BuildTexture();

	std::clock_t time; 
	std::vector<MortonChunk*> chunkList; chunkList.reserve(chunks*chunks);
	time = std::clock();
	for (int i = 0; i < chunks; ++i) {
		for (int j = 0; j < chunks; ++j) {
			glm::ivec3 grid = glm::ivec3(i, 0, j);
			MortonChunk* NewChunk = new MortonChunk(grid);
			NewChunk->BuildTerrain(gen, terrainType);
			//NewChunk->BuildTerrain(gen, terrainType);
			NewChunk->BuildMesh();
			NewChunk->mesh.Position = NewChunk->Position;
			NewChunk->mesh.BuildRenderData(currProgram);
			//NewChunk->CleanChunkBlocks();
			chunkList.push_back(std::move(NewChunk));
			if (chunkList.size() % 10 == 0)
				std::cerr << "Chunk number " << chunkList.size() << " built. " << std::endl;
		}
	}
	auto duration = (std::clock() - time) / CLOCKS_PER_SEC;
	std::cerr << "Total time to generate all chunks was: " << duration << " seconds." << std::endl;
	chunkList.shrink_to_fit();
	
	for (auto chk : chunkList) {
		auto& obj = MainWindow.CreateRenderObject(chk->mesh, currProgram);
		MainWindow.AddRenderObject(obj, "core");
	}


	// Set the global light position
	glm::vec3 lightPos(320.0f, 300.0f, 320.0f);
	GLuint lightPosLoc = currProgram.GetUniformLocation("lightPos");
	textures.BindTexture();
	MainWindow.Use();
	
    return 0;
}