#include "../stdafx.h"
#include "Viewport.h"

// Tracking of key presses for movement and simultaneous actions
static bool keys[1024];
// Previous mouse position
static GLfloat lastX, lastY;
// Previous mouse zoom
static GLfloat lastZoom;
// Main camera instance
static Camera Cam;

Viewport::Viewport(GLfloat width, GLfloat height){
	Width = width;
	Height = height;

	lastX = Width / 2.0f;
	lastY = Height / 2.0f;

	// Init GLFW
	glfwInit();

	// Base options

	// Set OpenGL version and profile: 3.3 Compatability
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Don't allow the window to be resize (embedded in UI)
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	// Enable 2x anti-aliasing to soften edges just slightly
	glfwWindowHint(GLFW_SAMPLES, 2);

	// Create the actual window instance
	Window = glfwCreateWindow(Width, Height, "HephaestusEngine", nullptr, nullptr);

	// Set the input mode - make the cursor visible and allow it to enter/leave freely
	glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set callback functions
	glfwSetCursorPosCallback(Window, MousePosCallback);
	glfwSetMouseButtonCallback(Window, MouseButtonCallback);
	glfwSetScrollCallback(Window, ScrollCallback);
	glfwSetKeyCallback(Window, KeyCallback);

	// Make context current so we can continue setting up
	glfwMakeContextCurrent(Window);

	// Init GLEW - requires version from above, profile from above, and hints from above
	GLuint init = glewInit();
	if (init != GLEW_OK) {
		throw("GLEW has not initialized properly");
	}
	glewExperimental = GL_TRUE;
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	// Build shaders
	Shader CoreVertex("./shaders/vertex.glsl", VERTEX_SHADER);
	Shader CoreFragment("./shaders/fragment.glsl", FRAGMENT_SHADER);
	CoreProgram.Init();
	CoreProgram.AttachShader(CoreVertex);
	CoreProgram.AttachShader(CoreFragment);
	CoreProgram.CompleteProgram();

	// Build and compile shaders
	std::vector<std::string> Uniforms{
		"normTransform",
		"model",
		"view",
		"projection",
	};
	CoreProgram.BuildUniformMap(Uniforms);
	//WireframeProgram.BuildUniformMap(Uniforms);

	// Set viewport
	glViewport(0, 0, Width, Height);

	// Set projection matrix. This shouldn't really change during runtime.
	Projection = glm::perspectiveFov(glm::radians(75.0f), Width, Height, 0.1f, 600.0f);

	// Perform initial setup of view matrix. This is updated often, but this will work for
	// initilization
	View = glm::lookAt(glm::vec3(0.0f, 0.0f, -40.0f), glm::vec3(0.0f), UP);
}

void Viewport::Use() {
	while (!glfwWindowShouldClose(Window)) {
		CoreProgram.Use();
		
		// Update frame time values
		GLfloat CurrentFrame = (GLfloat)glfwGetTime();
		DeltaTime = CurrentFrame - LastFrame;
		LastFrame = CurrentFrame;

		// Poll events, passing events to callback funcs
		glfwPollEvents();

		// Set the clear color - sets default background color
		glClearColor(160.0f / 255.0f, 239.0f / 255.0f, 1.0f, 1.0f);
		// Clear the depth and color buffers
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		// How to pass in render objects? Standardize a renderable type/class?
		// -- probably, have standard mesh format and vertices already combined with
		// mesh methods to do so. As long as all drawable objects inherit from this, 
		// things should work.

		// Store drawable objects as map, where key is the name of the object and the value is a reference to the object
		// and a reference to the relevant shader program.
		this->Render();

		// Before starting loop again, swap buffers (double-buffered rendering)
		glfwSwapBuffers(Window);
	}
	glfwTerminate();
}



void Viewport::MouseButtonCallback(GLFWwindow * window, int button, int action, int mods){
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		// Mouse down, begin to track movement for dragging
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		// Mouse up, finish drag motion
	}
}

void Viewport::MousePosCallback(GLFWwindow * window, double mouse_x, double mouse_y){
	// Keep track of mouse position
	// Object picking?
	GLfloat xoffset = (GLfloat)mouse_x - lastX;
	GLfloat yoffset = (GLfloat)mouse_y - lastY;

	lastX = static_cast<GLfloat>(mouse_x);
	lastY = static_cast<GLfloat>(mouse_y);

	Cam.ProcessMouseMovement(xoffset, yoffset);
}

void Viewport::ScrollCallback(GLFWwindow * window, double x_offset, double y_offset){
	// scroll_amount is movement along the y-axis
	double scrollAmount = y_offset;
	// change the camera's zoom based on scrollAmount
}

void Viewport::KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	if (key == GLFW_KEY_LEFT_ALT && action == GLFW_RELEASE) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	// Grab all keys pressed at a given instance and set the appropriate value to true 
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			keys[key] = false;
		}
	}
}

auto Viewport::GetRenderObjects(const std::string & shader_name) {
	return RenderObjects.equal_range(shader_name);
}

void Viewport::Render(){
	for (auto str : shaderNames) {
		auto& search = GetRenderObjects(str);
		for (auto iter = search.first; iter != search.second; ++iter) {
			auto& shader = (*iter).second.second;
			auto& obj = (*iter).second.first;
			obj.get().Render(shader);
		}
	}
}

void Viewport::AddRenderObject(const RenderObject & obj, std::string shader_name) {
	RenderMapEntry entry(shader_name, obj);
	RenderObjects.insert(entry);
	shaderNames.push_back(shader_name);
}

void Viewport::UpdateMovement(){
	if (keys[GLFW_KEY_W]) {
		Cam.Translate(MovementDir::FORWARD, DeltaTime);
	}
	if (keys[GLFW_KEY_S]) {
		Cam.Translate(MovementDir::BACKWARD, DeltaTime);
	}
	if (keys[GLFW_KEY_A]) {
		Cam.Translate(MovementDir::LEFT, DeltaTime);
	}
	if (keys[GLFW_KEY_D]) {
		Cam.Translate(MovementDir::RIGHT, DeltaTime);
	}
}
