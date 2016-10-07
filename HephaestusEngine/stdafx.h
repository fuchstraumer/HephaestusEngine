// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#pragma warning (disable:4244)
#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <string>
#define GLFW_DLL

// Import glew for OpenGl pointers and handlers
#define GLEW_STATIC
#include <GL/glew.h>

// GLM
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma warning(disable:4244)

// Chunk vars
static const float BLOCK_RENDER_SIZE = 1.0f;
static const int CHUNK_SIZE = 32;
static const int CHUNK_SIZE_Z = 64;
static const bool SIMPLE_CULLING_GLOBAL = true;
static const bool WIREFRAME_MODE = false;
typedef unsigned int uint;

// Texture const
static const float TEXTURE_TILE_SIZE = 16.0f;
static const float TEXTURE_TILE_COUNT = 16.0f;
// TODO: reference additional headers your program requires here
