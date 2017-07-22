// stdafx.h - precompiled header to increase compiliation speed.
// Especially helpful with highly templated libraries, and libraries used nearly everywhere.
#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <array>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>  
#include <chrono>
#include <forward_list>
#include <future>

#define GLFW_DLL
#define GLFW_INCLUDE_VULKAN
#include "glfw\glfw3.h"
#ifdef _WIN32
// Allows for easier hooking of window/context with ImGui.
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include "GLFW\glfw3native.h"
#endif

// hash used to allow inserting glm vec's into unordered containers.
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_SWIZZLE
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\quaternion.hpp"
#include "glm\gtx\hash.hpp"

// Both of these have silly errors we can ignore.
#pragma warning(push, 0)
#include "gli\gli.hpp"
#include "util\lodepng.h"
#pragma warning(pop)

// Forces assert even in release mode: convienient for catching
// errors that occur due to release optimizations.
#define  VK_FORCE_ASSERT
#include "vulkan/vulkan.h"
#include "common/CreateInfoBase.h"
#include "common/vkAssert.h"
#include "common/vk_constants.h"
#include "common/Constants.h"

#define NOMINMAX
#include "util\easylogging++.h"
#undef NOMINMAX

