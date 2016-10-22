// stdafx.h 
#pragma once
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
#include <glm/gtc/constants.hpp>

// Chunk vars
static const float BLOCK_RENDER_SIZE = 0.5f;
static const int CHUNK_SIZE = 32;
static const int CHUNK_SIZE_Z = 128;
static const bool SIMPLE_CULLING_GLOBAL = true;
static const bool WIREFRAME_MODE = false;
typedef unsigned int uint;

// Client settings
static const int MULTISAMPLE_AMOUNT = 8;
static const int VIEW_DISTANCE = 24;
static const bool DAY_NIGHT_CYCLE = true;

// Constant function to convert from spatial coordinates to interval tree vals
#define treeXYZ(x,y,z) ((y) * CHUNK_SIZE * CHUNK_SIZE + (x) * CHUNK_SIZE + (z))
#define treeXZ(x,z) ((x * CHUNK_SIZE) + z)

// Terrain gen
#include "util/TerrainGen.h"

// TODO: reference additional headers your program requires here
