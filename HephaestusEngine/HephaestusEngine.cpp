// HephaestusEngine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "util\lodeTexture.h"
#include "util/shader.h"
#include "util/camera.h"
#include "objects\LinearChunk.h"
#include <ctime>
#include <fstream>
static GLuint WIDTH = 1440, HEIGHT = 900;

// Function declarations
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();

// RenderDoc Manager

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

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



	//glm::vec3 pos = test1.Blocks[6].GetPosition();
	// Init GLFW to get OpenGL functions and pointers
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "HephaestusEngine", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize glew and build our shader program
	glewExperimental = GL_TRUE; 
	glewInit();

	Shader vertShader("./shaders/vertex.glsl", VERTEX_SHADER);
	Shader fragShader("./shaders/fragment.glsl", FRAGMENT_SHADER);
	ShaderProgram mainProgram;
	mainProgram.Init();
	mainProgram.AttachShader(vertShader);
	mainProgram.AttachShader(fragShader);
	mainProgram.CompleteProgram();
	std::vector<std::string> uniforms{
		"model",
		"view",
		"projection",
		"lightPos",
		"lightColor",
		"cameraPos",
		"textureAtlas",
		"normTransform",
	};
	mainProgram.BuildUniformMap(uniforms);
	mainProgram.Use();
	glViewport(0, 0, WIDTH, HEIGHT);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
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

	TextureArray textures(filelist, 16);
	textures.BuildTexture();
	std::clock_t time;
	std::vector<LinearChunk> chunkList; chunkList.reserve(chunks*chunks);
	time = std::clock();
	// Array that will hold the name of each chunk
	char fname[100];
	// File object used to write chunks
	std::fstream chunkWriter;
	for (int i = 0; i < chunks; ++i) {
		for (int j = 0; j < chunks; ++j) {
			glm::ivec3 grid = glm::ivec3(i, 0, j);
			LinearChunk NewChunk(grid);
			NewChunk.BuildTerrain(gen, terrainType);
			NewChunk.BuildMesh();
			NewChunk.mesh.BuildRenderData(mainProgram);
			//NewChunk->CleanChunkBlocks();
			chunkList.push_back(NewChunk);
			sprintf(fname, "./chunks/chunk%zd.txt", chunkList.size());
			NewChunk.EncodeBlocks();
			chunkWriter.open(fname, std::ios::out);
			if (chunkList.size() % 10 == 0) {
				std::cerr << "Chunk number " << chunkList.size() << " built. " << std::endl;
			}
			chunkWriter << NewChunk.encodedBlocks.data();
			chunkWriter.close();
		}
	}
	auto duration = (std::clock() - time) / CLOCKS_PER_SEC;
	std::cerr << "Total time to generate all chunks was: " << duration << " seconds." << std::endl;
	chunkList.shrink_to_fit();

	textures.BindTexture();
	GLint textureLoc = mainProgram.GetUniformLocation("textureAtlas");
	glUniform1i(textureLoc, 0);


	// Set the global light position
	glm::vec3 lightPos(320.0f, 300.0f, 320.0f);
	GLint lightPosLoc = mainProgram.GetUniformLocation("lightPos");
	glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
	GLint lightColorLoc = mainProgram.GetUniformLocation("lightColor");
	glUniform3f(lightColorLoc, 219.0f / 255.0f, 255.0f / 255.0f, 240.0f / 255.0f);
	GLint viewPosLoc = mainProgram.GetUniformLocation("cameraPos");

	glm::mat4 projection;
	projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 1.0f, 10000.0f);
	GLint viewLoc = mainProgram.GetUniformLocation("view");
	GLint projLoc = mainProgram.GetUniformLocation("projection");

	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// GLFW main loop
	while (!glfwWindowShouldClose(window)) {

		
		// Set frame time to avoid slowdowns and speedups etc
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Event handling
		glfwPollEvents();
		Do_Movement();

		// Clear the buffers, set clear color
		glClearColor(160.0f / 255.0f, 239.0f / 255.0f, 255.0f / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Textures 
		
		// Do lighting stuff
		glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

		// Prepare to draw objects
		glm::mat4 view;
		view = camera.GetViewMatrix();
		
		// Pass view matrix to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		
		// Render all chunks
		for (unsigned int i = 0; i < chunkList.size(); ++i) {
			GLuint modelLoc = mainProgram.GetUniformLocation("model");
			glm::mat4 model(1.0f);
			model = glm::translate(model, chunkList[i].Position);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			chunkList[i].mesh.Render(mainProgram);
		}

		// Swap the buffers to avoid visible refresh
		glfwSwapBuffers(window);

	}
	glfwTerminate();
	return 0;
}


// Moves/alters the camera positions based on user input
void Do_Movement() {
	// Camera controls
	if (keys[GLFW_KEY_W]) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (keys[GLFW_KEY_S]) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (keys[GLFW_KEY_A]) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (keys[GLFW_KEY_D]) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	//cout << key << endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
		camera.MovementSpeed += 60;
	}
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
		camera.MovementSpeed -= 60;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = (GLfloat)xpos;
		lastY = (GLfloat)ypos;
		firstMouse = false;
	}

	GLfloat xoffset = (GLfloat)xpos - lastX;
	GLfloat yoffset = lastY - (GLfloat)ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = (GLfloat)xpos;
	lastY = (GLfloat)ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll((GLfloat)yoffset);
}