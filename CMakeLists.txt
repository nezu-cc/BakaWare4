cmake_minimum_required(VERSION 3.0.0)
project(BakaWare4 VERSION 4.0.0)

file(GLOB_RECURSE SOURCES "src/*.cpp" )
file(GLOB IMGUI "third_party/imgui/*.cpp")
add_library(BakaWare4 SHARED ${SOURCES} ${IMGUI})

target_include_directories(BakaWare4 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui")

target_compile_features(BakaWare4 PUBLIC c_std_17 cxx_std_20)

