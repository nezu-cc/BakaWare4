#include "bakaware.h"
#include "../memory/memory.h"

void bakaware::Setup() {
    logger::initialize(L"csgo", L"log.txt");
    dlls::initialize();

    LOG_SUCCESS("game_overlay_renderer64: {}", (void*)dlls::game_overlay_renderer.base);
}