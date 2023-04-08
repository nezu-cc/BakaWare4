#include "logger.h"
#include "../memory/dll.h"

void logger::valve_logger::initialize(const char* channel_name, logging_severity severity, clr4 color) noexcept {
    using function_t = logging_channel_id(__stdcall*)(const char*, void*, int, logging_severity, clr4);
    static function_t fn = dlls::tier0.get_export("LoggingSystem_RegisterLoggingChannel"_hash).cast<function_t>();
    ASSERT(fn);
    channel_id = fn(channel_name, nullptr, 0, severity, color);
}

bool logger::valve_logger::is_channel_enabled(logging_severity severity) noexcept {
    using function_t = bool(__stdcall*)(logging_channel_id, logging_severity);
    static function_t fn = dlls::tier0.get_export("LoggingSystem_IsChannelEnabled"_hash).cast<function_t>();
    ASSERT(fn);
    return fn(channel_id, severity);
}

void logger::valve_logger::log(logging_severity severity, const char *message) noexcept {
    using function_t = void(__stdcall*)(logging_channel_id, logging_severity, const char*, ...);
    static function_t fn = dlls::tier0.get_export("LoggingSystem_LogDirect"_hash).cast<function_t>();
    ASSERT(fn);
    fn(channel_id, severity, message);
}