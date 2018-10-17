// stdafx.h - precompiled header to increase compiliation speed.
// Especially helpful with highly templated libraries, and libraries used nearly everywhere.
#pragma once

#pragma warning(push, 0)

#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <array>
#include <memory>
#include <iostream>

#include <future>
#include <random>


// Number of odd/broken defines in this, as it includes windows.h 
#define NOMINMAX
#include "util/easylogging++.h"
#undef NOMINMAX

#include "common\Constants.hpp"
#include "common\CommonUtil.hpp"

#pragma warning(pop)


