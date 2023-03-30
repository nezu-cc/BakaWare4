cmake_minimum_required(VERSION 3.0.0)
project(BakaWare4 VERSION 4.0.0)

file(GLOB_RECURSE SOURCES "src/*.cpp" )
file(GLOB IMGUI "third_party/imgui/*.cpp" "third_party/imgui/backends/imgui_impl_dx11.cpp" "third_party/imgui/backends/imgui_impl_win32.cpp")
add_library(BakaWare4 SHARED ${SOURCES} ${IMGUI})

target_include_directories(BakaWare4 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui")

target_compile_features(BakaWare4 PUBLIC c_std_17 cxx_std_20)

if (MSVC)
    # multi core parallel building
    include(ProcessorCount)
    ProcessorCount(PROCESSOR_COUNT)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP${PROCESSOR_COUNT}")
    set(CMAKE_VS_MSBUILD_COMMAND "${CMAKE_VS_MSBUILD_COMMAND} /p:CL_MPCount=${PROCESSOR_COUNT} /m")
endif ()