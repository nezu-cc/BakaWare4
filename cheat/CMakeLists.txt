add_definitions(-DBAKAWARE_VERSION="${PROJECT_VERSION}")
add_definitions(-DIMGUI_USER_CONFIG=<imconfig_bw.h>)

add_subdirectory("third_party/json")

file(GLOB_RECURSE SOURCES "src/*.cpp" )
file(GLOB IMGUI "third_party/imgui/*.cpp" "third_party/imgui/backends/imgui_impl_dx11.cpp" "third_party/imgui/backends/imgui_impl_win32.cpp")

add_library(BakaWare4 SHARED ${SOURCES} ${IMGUI})

set_property(TARGET BakaWare4 PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_include_directories(BakaWare4 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui" "${CMAKE_CURRENT_SOURCE_DIR}/third_party/conf")
target_link_libraries(BakaWare4 PRIVATE nlohmann_json::nlohmann_json)

target_compile_features(BakaWare4 PUBLIC c_std_17 cxx_std_20)
target_compile_options(BakaWare4 PUBLIC /Zc:threadSafeInit- /sdl- /GS- /guard:cf-)