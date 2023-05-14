set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE static)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_BUILD_TYPE release)

set(VCPKG_CXX_FLAGS_RELEASE "/Zc:threadSafeInit- /sdl- /GS- /guard:cf-")
set(VCPKG_C_FLAGS_RELEASE "/Zc:threadSafeInit- /sdl- /GS- /guard:cf-")