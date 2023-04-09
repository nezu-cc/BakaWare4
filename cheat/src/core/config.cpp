#include "config.h"
#include "../base/base.h"

using json = nlohmann::json;

void config::load(std::wstring_view file) noexcept {}

void config::save(std::wstring_view file) noexcept {
    json j = cfg;
    LOG_INFO(XOR("config: {}"), j.dump(2));
}
