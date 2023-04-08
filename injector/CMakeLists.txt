
add_executable(PerfectInjector "src/PerfectInjector.cpp")

target_compile_features(PerfectInjector PUBLIC c_std_17 cxx_std_20)
set_target_properties(PerfectInjector PROPERTIES LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\"")