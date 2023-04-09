#pragma once

#ifdef __clang__
#pragma clang diagnostic ignored "-Wunused-value"
#pragma clang diagnostic ignored "-Wpragma-once-outside-header"
#pragma clang diagnostic ignored "-Wreorder-ctor"
#endif

#define NOMINMAX

#include <chrono>
using namespace std::chrono_literals;
#include <cstddef>
#include <cstdint>
#include <thread>

#include "logger.h"
#include "types/angle.h"
// #include "types/bitfield.h"
#include "types/color.h"
#include "types/dimension.h"
#include "types/matrix.h"
#include "types/pattern.h"
#include "types/vector.h"
#include "types/bounding_box.h"
#include "../crypt/xorstr.h"