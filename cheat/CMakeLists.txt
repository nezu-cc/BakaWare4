add_definitions(-DBAKAWARE_VERSION="${PROJECT_VERSION}")
add_definitions(-DIMGUI_USER_CONFIG=<imconfig_bw.h>)

find_package(imgui CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp" )
add_library(BakaWare4 SHARED ${SOURCES})

target_include_directories(BakaWare4 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(BakaWare4 PRIVATE nlohmann_json::nlohmann_json imgui::imgui)

target_compile_features(BakaWare4 PUBLIC c_std_17 cxx_std_20)
target_compile_options(BakaWare4 PUBLIC /Zc:threadSafeInit- /sdl- /GS- /guard:cf-)