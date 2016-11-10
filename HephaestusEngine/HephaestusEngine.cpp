// HephaestusEngine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "util\lodeTexture.h"
#include "util/shader.h"
#include "util/camera.h"
#include "treeChunk.h"
#include "util\MortonChunk.h"
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

int main(){
	std::string stringSeed;
	std::cout << "Enter a string of characters to use as the terrain gen seed value: " << std::endl;
	std::cin >> stringSeed;
	int chunks;
	std::cout << "Enter an integer number from 2 through 48 to specify the amount of chunks to generate: " << std::endl;
	std::cout << "Warning: Using values greater than 32 may cause memory allocation crashes. " << std::endl;
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
	if (chunks > 48) {
		chunks = 48;
	}

	MortonChunk test1(glm::ivec3(0,0,0));
	test1.Blocks[6]
	// Init GLFW to get OpenGL functions and pointers
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 2);
	
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
	glewExperimental = GL_TRUE; glewInit();
	Shader ourShader("compressed_vertex.glsl", "compressed_fragment.glsl");
	glViewport(0, 0, WIDTH, HEIGHT);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	// Are we in wireframe mode?
	if (WIREFRAME_MODE == true) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

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
	textures.BuildTextureArray(GL_TEXTURE0);
	
	std::vector<std::shared_ptr<TreeChunk>> chunkList; chunkList.reserve(chunks*chunks);
	for (int i = 0; i < chunks; ++i) {
		for (int j = 0; j < chunks; ++j) {
			glm::ivec3 grid = glm::ivec3(i, 0, j);
			std::shared_ptr<TreeChunk> NewChunk(new TreeChunk(grid));
			NewChunk->BuildTerrain(gen, terrainType);
			NewChunk->BuildData();
			NewChunk->BuildRender();
			NewChunk->EncodeChunk();
			chunkList.push_back(NewChunk);
			if(chunkList.size() % 10 == 0)
				std::cerr << "Chunk number " << chunkList.size() << " built. " << std::endl;
		}
	}
	chunkList.shrink_to_fit();


	// Set the global light position
	glm::vec3 lightPos(320.0f, 300.0f, 320.0f);

	// GLFW main loop
	while (!glfwWindowShouldClose(window)) {

		ourShader.Use();
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

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textures.GL_Handle);
		glUniform1i(glGetUniformLocation(ourShader.Program, "texSampler"), 0);

		// Do lighting stuff
		GLint lightColorLoc = glGetUniformLocation(ourShader.Program, "lightColor");
		GLint lightPosLoc = glGetUniformLocation(ourShader.Program, "lightPos");
		GLint viewPosLoc = glGetUniformLocation(ourShader.Program, "viewPos");
		GLint textureLoc = glGetUniformLocation(ourShader.Program, "texSampler");
		glUniform3f(lightColorLoc, 219.0f / 255.0f, 244.0f / 255.0f, 255.0f / 255.0f);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

		// Prepare to draw objects
		glm::mat4 view;
		glm::mat4 projection;
		view = camera.GetViewMatrix();
		projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 1.0f, 1000.0f);
		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
		GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		// Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		// Each mesh needs to be transformed to be appear in the right spot.
		// glm::translate builds the appropriate transformation matrix based on the chunk's position
		// Once transformed, we can then render that chunk
		for (unsigned int i = 0; i < chunkList.size(); ++i) {
			glm::mat4 model; 
			model = glm::translate(model, chunkList[i]->ChunkPos);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			chunkList[i]->ChunkRender(ourShader);
		}
		// Swap the buffers to avoid visible refresh
		glfwSwapBuffers(window);

	} 
	glfwTerminate();
    return 0;
}


// Moves/alters the camera positions based on user input
void Do_Movement(){
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
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
	//cout << key << endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key >= 0 && key < 1024){
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
		camera.MovementSpeed += 20;
	}
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
		camera.MovementSpeed -= 20;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
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


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
	camera.ProcessMouseScroll((GLfloat)yoffset);
}