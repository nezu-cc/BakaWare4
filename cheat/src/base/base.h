#pragma once

#define NOMINMAX

#include <chrono>
using namespace std::chrono_literals;
#include <cstddef>
#include <cstdint>
#include <thread>

#include "logger.h"
#include "types/angle.h"
#include "types/quaternion.h"
#include "types/color.h"
#include "types/dimension.h"
#include "types/matrix.h"
#include "types/pattern.h"
#include "types/vector.h"
#include "types/bounding_box.h"
#include "../crypt/xorstr.h"