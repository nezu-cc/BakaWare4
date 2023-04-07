#include "sdl.h"
#include "../../memory/dll.h"

void sdl::set_relative_mouse_mode(bool mode) {
	using function_t = int32_t(__stdcall*)(bool);
	static function_t fn = dlls::sdl2.get_export("SDL_SetRelativeMouseMode"_hash).cast<function_t>();
    ASSERT(fn);
	fn(mode);
}

void sdl::set_window_polling_mode(void* window, bool mode) {
	using function_t = void(__stdcall*)(void*, bool);
    static function_t fn = dlls::sdl2.get_export("SDL_SetWindowGrab"_hash).cast<function_t>();
    ASSERT(fn);
    fn(window, mode);
}

void sdl::set_mouse_warp_position(void* window, int32_t x, int32_t y) {
	using function_t = void(__stdcall*)(void*, int32_t, int32_t);
    static function_t fn = dlls::sdl2.get_export("SDL_WarpMouseInWindow"_hash).cast<function_t>();
    ASSERT(fn);
    fn(window, x, y);
}