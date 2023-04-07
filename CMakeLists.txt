cmake_minimum_required(VERSION 3.1)
project(BakaWare4 VERSION 4.0.0)

if (MSVC)
    # multi core parallel building
    include(ProcessorCount)
    ProcessorCount(PROCESSOR_COUNT)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP${PROCESSOR_COUNT}")
    set(CMAKE_VS_MSBUILD_COMMAND "${CMAKE_VS_MSBUILD_COMMAND} /p:CL_MPCount=${PROCESSOR_COUNT} /m")
endif ()

add_subdirectory(cheat)
add_subdirectory(injector)