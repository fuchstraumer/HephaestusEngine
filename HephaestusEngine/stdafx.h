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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma warning(disable:4244)
static const float BLOCK_RENDER_SIZE = 1.0f;
static const int CHUNK_SIZE = 32;
static const int CHUNK_SIZE_Z = 64;

typedef unsigned int uint;

// TODO: reference additional headers your program requires here
