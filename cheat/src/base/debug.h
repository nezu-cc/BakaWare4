#pragma once

#include <cassert>
#include <format>
#include <string_view>
#include "../crypt/xorstr.h"
#include "../base/crash_handler.h"

#ifdef NDEBUG
[[noreturn]]
#endif
static void dbg_fail(std::string_view fn, std::string_view msg = "") noexcept {
    std::string message = msg.empty() ? 
        std::vformat(XOR("An error occurred in {}!"), std::make_format_args(fn)) :
        std::vformat(XOR("{} ({})"), std::make_format_args(msg, fn));
    
    crash_handler::handle_crash(message);
}

#ifdef _DEBUG
#define ASSERT(expr) assert(expr);
#define ASSERT_MSG(expr, msg) assert(((void)msg, expr));
#else
#define ASSERT(expr) do { if (!(expr)) dbg_fail(XOR(__FUNCTION__)); } while (false)
#define ASSERT_MSG(expr, msg) do { if (!(expr)) dbg_fail(XOR(__FUNCTION__), msg); } while (false)
#endif
