// HephaestusEngine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <time.h>
#include <math.h>
#define LODEPNG_COMPILE_CPP
#include "util/lodepng.h"
#include "util/shader.h"
#include "util/camera.h"
#include "chunk_manager.h"
using namespace lodepng;
static GLuint WIDTH = 1440, HEIGHT = 720;

void load_png_texture(const char *file_name) {
	unsigned int error;
	unsigned char *data;
	unsigned int width, height;
	error = lodepng_decode32_file(&data, &width, &height, file_name);
	if (error) {
		fprintf(stderr, "error %u: %s\n", error, lodepng_error_text(error));
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, data);
	free(data);
}


// Function declarations
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

int main(){
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	
	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glewExperimental = GL_TRUE; glewInit();
	Shader ourShader("./compressed_vertex.glsl", "./compressed_fragment.glsl");

	glViewport(0, 0, WIDTH, HEIGHT);
	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);




	// Load block textures
	unsigned char *grass_top, *grass_side, *dirt, *sand, *stone;
	unsigned int width, height;
	lodepng_decode32_file(&grass_top,&width,&height,"textures/blocks/grass_top.png");
	lodepng_decode32_file(&grass_side, &width, &height, "textures/blocks/grass_side.png");
	lodepng_decode32_file(&dirt, &width, &height, "textures/blocks/dirt.png");
	lodepng_decode32_file(&sand, &width, &height, "textures/blocks/sand.png");
	lodepng_decode32_file(&stone, &width, &height, "textures/blocks/stone.png");

	// Create texture array
	GLuint texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, width, height, 5, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, grass_top);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 1, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, grass_side);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 2, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, dirt);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 3, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, sand);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 4, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, stone);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	GLenum error = glGetError();
	free(grass_top); free(grass_side); free(dirt); free(sand); free(stone);
	// Create chunk manager
	std::vector<Chunk> chunkList;
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			glm::ivec3 grid = glm::ivec3(i, 0, j);
			Chunk* newChunk = new Chunk(grid);
			newChunk->buildTerrain();
			newChunk->buildRender();
			chunkList.push_back(*newChunk);

		}
	}

	glm::vec3 lightPos(160.0f, 100.0f, 160.0f);

	// GLFW main loop
	while (!glfwWindowShouldClose(window)) {

		ourShader.Use();
		// Set frame time

		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Event handling
		glfwPollEvents();
		Do_Movement();

		// Clear the buffers
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Textures 

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
		glUniform1i(glGetUniformLocation(ourShader.Program, "texSampler"), 0);

		// Do lighting stuff
		GLint lightColorLoc = glGetUniformLocation(ourShader.Program, "lightColor");
		GLint lightPosLoc = glGetUniformLocation(ourShader.Program, "lightPos");
		GLint viewPosLoc = glGetUniformLocation(ourShader.Program, "viewPos");
		GLint textureLoc = glGetUniformLocation(ourShader.Program, "texSampler");
		GLint tileLoc = glGetUniformLocation(ourShader.Program, "tileCount");
		GLint sizeLoc = glGetUniformLocation(ourShader.Program, "tileSize");
		glUniform1f(sizeLoc, TEXTURE_TILE_SIZE);
		glUniform1f(tileLoc, TEXTURE_TILE_COUNT);
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

		// Prepare to draw objects
		glm::mat4 view;
		glm::mat4 projection;
		view = camera.GetViewMatrix();
		projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.5f, 200.0f);
		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
		GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		// Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		for (unsigned int i = 0; i < chunkList.size(); ++i) {
			glm::mat4 model; model = glm::translate(model, chunkList[i].chunkPos);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			chunkList[i].chunkRender(ourShader);
		}
		glfwSwapBuffers(window);
	} 
	//chunk0.~Chunk();
	glfwTerminate();
    return 0;
}


// Moves/alters the camera positions based on user input
void Do_Movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//cout << key << endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
		camera.MovementSpeed += 10;
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
		camera.MovementSpeed -= 10;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}