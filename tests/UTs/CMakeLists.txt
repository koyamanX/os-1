set(CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}gcc${TOOLCHAIN_SUFFIX}")
set(CMAKE_ASM_COMPILER "${TOOLCHAIN_PREFIX}gcc${TOOLCHAIN_SUFFIX}")
set(CMAKE_LINKER "${TOOLCHAIN_PREFIX}gcc${TOOLCHAIN_SUFFIX}")
set(CMAKE_C_FLAGS   "-Wall -fprofile-arcs -ftest-coverage  -g")
set(CMAKE_ASM_FLAGS "-Wall")

add_subdirectory(test_framework)
add_subdirectory(list)
