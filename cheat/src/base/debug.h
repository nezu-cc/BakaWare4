#pragma once

#include <cassert>
#include "logger.h"

#ifdef NDEBUG
[[noreturn]]
#endif
static void dbg_fail(std::string_view fn, std::string_view msg = "") noexcept
{
#ifdef _DEBUG
    if (msg.empty())
        LOG_ERROR(XOR("An error occurred in {}!"), fn);
    else
        LOG_ERROR(XOR("{} ({})"), msg, fn);
    assert(false);
#else
    std::abort();
#endif
}

#ifdef __clang__
#define FUNCTION_NAME __PRETTY_FUNCTION__
#else
#define FUNCTION_NAME __FUNCTION__
#endif
#define ASSERT(expr) do { if (!(expr)) dbg_fail(XOR(FUNCTION_NAME)); } while (false)
#define ASSERT_MSG(expr, msg) do { if (!(expr)) dbg_fail(XOR(FUNCTION_NAME), msg); } while (false)
