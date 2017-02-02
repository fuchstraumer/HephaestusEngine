// HephaestusEngine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "util\lodeTexture.h"
#include "util\shader.h"
#include "util\camera.h"
#include "objects\LinearChunk.h"



// Used to set width/height of screen rendered.
static constexpr GLuint WIDTH = 1440, HEIGHT = 900;

// Method declarations
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int main() {
	// Get the seed to use for the terrain generator. Use a stringstream to convert it to an int value.
	std::string stringSeed;
	std::cout << "Enter a string of characters to use as the terrain gen seed value: " << std::endl;
	std::cin >> stringSeed;
	int intSeed;
	std::stringstream(stringSeed) >> intSeed;

	// Get number of chunks to render/build in this run
	int chunks;
	std::cout << "The world is rendered as a square of chunks. Enter the side length of this square (number of chunks rendered = side_length * sidelength): " << std::endl;
	std::cout << "Warning: Using values greater than 16 is unsupported, as it causes excessive memory use (256 chunks are a bit much!)" << std::endl;
	std::cin >> chunks;
	if (chunks > 16) {
		chunks = 16;
	}

	// Allow selection of which terrain generator function to use.
	std::cout << "What terrain generator would you like to use?" << std::endl;
	std::cout << "Options: 0 = FBM, 1 = Billow, 2 = RidgedMulti, 3 = SwissNoise" << std::endl;
	int terrainType; std::cin >> terrainType;
	// Make sure valid value was chosen.
	if (terrainType < 0 || terrainType > 3) {
		std::cout << "Erronenous terrain generator value chosen. Defaulting to FBM..." << std::endl;
		terrainType = 0;
	}
	
	// Build and seed our terrain generator
	TerrainGenerator gen(intSeed);
	
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

	// Set up the list of files to read from when building our texture array.
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

	// Instantiate and build our texture array.
	TextureArray textures(filelist, 16);
	textures.BuildTexture();

	// Start a timer as we prepare to build the chunks.
	std::clock_t time;
	time = std::clock();

	// Reserve space in our chunk container
	std::vector<LinearChunk> chunkList; 
	chunkList.reserve(chunks*chunks);

	for (int i = 0; i < chunks; ++i) {
		for (int j = 0; j < chunks; ++j) {
			glm::ivec2 grid = glm::ivec2(i, j);
			LinearChunk NewChunk(grid);
			NewChunk.BuildTerrain(gen, terrainType);
			NewChunk.BuildMesh();
			NewChunk.mesh.BuildRenderData(mainProgram);
			// Use std::move to avoid a costly (and pointless) copy.
			chunkList.push_back(std::move(NewChunk));
			if (chunkList.size() % 5 == 0) {
				std::cerr << "Chunk number " << chunkList.size() << " built. " << std::endl;
			}
		}
	}

	// I think duration defaults to a clock_t?
	auto duration = (std::clock() - time) / CLOCKS_PER_SEC;
	std::cerr << "Total time to generate all chunks was: " << duration << " seconds." << std::endl;

	// Bind our texture array and set its location.
	textures.BindTexture();
	GLint textureLoc = mainProgram.GetUniformLocation("textureAtlas");
	glUniform1i(textureLoc, 0);


	// Set the global light position in the shader.
	glm::vec3 lightPos(320.0f, 300.0f, 320.0f);
	GLint lightPosLoc = mainProgram.GetUniformLocation("lightPos");
	glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);

	// Set the global light color in the shader.
	GLint lightColorLoc = mainProgram.GetUniformLocation("lightColor");
	glUniform3f(lightColorLoc, 219.0f / 255.0f, 255.0f / 255.0f, 240.0f / 255.0f);
	GLint viewPosLoc = mainProgram.GetUniformLocation("cameraPos");

	// Setup the projection matrix and set its value in the shader.
	glm::mat4 projection;
	projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 1.0f, 10000.0f);
	GLint projLoc = mainProgram.GetUniformLocation("projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// Get the location of the view matrix.
	GLint viewLoc = mainProgram.GetUniformLocation("view");

	// Get the location of the model matrix.
	GLuint modelLoc = mainProgram.GetUniformLocation("model");

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
		
		// Pass camera position so we can properly calculate diffuse and specular components of the lighting.
		glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

		// Get the view matrix from the camera.
		glm::mat4 view;
		view = camera.GetViewMatrix();
		
		// Pass view matrix to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		
		// Render all chunks
		for (unsigned int i = 0; i < chunkList.size(); ++i) {
			// Set model matrix. Will be different for each chunk, so we need to re-set this value in each iteration.
			// Note: I set this in the chunk ctor. Why re-set it here?
			glm::mat4 model(1.0f);
			model = glm::translate(model, chunkList[i].Position);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			
			// Call the rendering method of the actual mesh now, passing a ref to the shader in as well. 
			chunkList[i].mesh.Render(mainProgram);
		}

		// Swap the buffers to avoid visible refresh
		glfwSwapBuffers(window);

	}
	glfwTerminate();
	return 0;
}


// Moves/alters the camera positions based on user input
// This implementation avoids lockup or odd behavior when more than one key is pressed.
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

	// Set key flag in "keys", which is used to update movement.
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			keys[key] = false;
		}
	}

	// TODO: Migrate these keys to do_movement()
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
		camera.MovementSpeed += 60;
	}
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
		camera.MovementSpeed -= 60;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

// Updates mouse position in camera.
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

// Updates zoom amount in camera.
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll((GLfloat)yoffset);
}