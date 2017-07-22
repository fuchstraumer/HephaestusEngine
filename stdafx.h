// stdafx.h - precompiled header to increase compiliation speed.
// Especially helpful with highly templated libraries, and libraries used nearly everywhere.
#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <array>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <stdlib.h>  
#include <iostream>
#include <chrono>
#include <forward_list>
#include <future>

#define GLFW_DLL
#define GLFW_INCLUDE_VULKAN
#include "ext/include/glfw/glfw3.h"

#ifdef _WIN32
// Allows for easier hooking of window/context with ImGui.
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include "ext/include/GLFW/glfw3native.h"
#endif

// hash used to allow inserting glm vec's into unordered containers.
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_SWIZZLE
#include "ext/include/glm/glm.hpp"
#include "ext/include/glm/gtc/matrix_transform.hpp"
#include "ext/include/glm/gtc/type_ptr.hpp"
#include "ext/include/glm/gtx/hash.hpp"

// Both of these have silly errors we can ignore.

#include "ext/include/gli/gli.hpp"
#include "vulkan/vulkan.h"


// Number of odd/broken defines in this, as it includes windows.h 
#define NOMINMAX
#include "util/easylogging++.h"
#undef NOMINMAX

#pragma warning(pop)


