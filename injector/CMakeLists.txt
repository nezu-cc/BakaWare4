
add_executable(PerfectInjector "src/PerfectInjector.cpp")

target_compile_features(PerfectInjector PUBLIC c_std_17 cxx_std_20)
set_target_properties(PerfectInjector PROPERTIES LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\"")

if (VCPKG_TARGET_TRIPLET MATCHES "x64-windows-static")
    set_property(TARGET PerfectInjector PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()