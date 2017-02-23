// stdafx.h - precompiled header to increase compiliation speed.
// Especially helpful with highly templated libraries, and libraries used nearly everywhere.
#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <array>
#include <future>
#include <algorithm>
#include <ctime>
#include <bitset>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
// Import glew for OpenGl pointers and handlers
// GLFW uses this, so include this before GLFW
// Define GLEW_STATIC because we're using the static library.
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW, used for Windowing and context handling.
// TODO: Move to GLFW static, including the DLL kinda sucks
#define GLFW_DLL
#include <GLFW/glfw3.h>

// GLM
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

