// stdafx.h - precompiled header to increase compiliation speed.
// Especially helpful with highly templated libraries, and libraries used nearly everywhere.
#pragma once

#pragma warning(push, 0)

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
#include <random>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/hash.hpp"

#include "gli/gli.hpp"

#include "vulkan/vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include "glfw/glfw3.h"

// Number of odd/broken defines in this, as it includes windows.h 
#define NOMINMAX
#include "util/easylogging++.h"
#undef NOMINMAX

#include "common\Constants.hpp"
#include "common\CommonUtil.hpp"

#pragma warning(pop)


