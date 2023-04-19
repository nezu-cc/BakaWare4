add_executable(PerfectInjector "src/PerfectInjector.cpp")

target_compile_features(PerfectInjector PUBLIC c_std_17 cxx_std_20)
set_target_properties(PerfectInjector PROPERTIES LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\"")

if (VCPKG_TARGET_TRIPLET MATCHES "x64-windows-static")
    set_property(TARGET PerfectInjector PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

add_custom_command(
    TARGET PerfectInjector POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "@cd /D \"%~dp0\"" > run.bat
    COMMAND ${CMAKE_COMMAND} -E echo "@start $<TARGET_FILE_NAME:PerfectInjector> cs2.exe $<TARGET_FILE_NAME:BakaWare4> waitkey" >> run.bat
    WORKING_DIRECTORY $<TARGET_FILE_DIR:PerfectInjector>
    VERBATIM
)