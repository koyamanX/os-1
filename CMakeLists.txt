cmake_minimum_required(VERSION 3.22)

project(OS-1 C ASM)

if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "")
	message(FATAL_ERROR "CMAKE_BUILD_TYPE is not supported")
endif()

set(TOOLCHAIN_PREFIX "riscv64-linux-gnu-")
set(TOOLCHAIN_SUFFIX "-12")
set(CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}gcc${TOOLCHAIN_SUFFIX}")
set(CMAKE_ASM_COMPILER "${TOOLCHAIN_PREFIX}gcc${TOOLCHAIN_SUFFIX}")
set(CMAKE_LINKER "${TOOLCHAIN_PREFIX}gcc${TOOLCHAIN_SUFFIX}")
set(CMAKE_C_FLAGS   "-Wall -Werror -O0 -ggdb -g3 -gdwarf-2 -march=rv64g -ffreestanding -fno-common -nostdlib")
set(CMAKE_ASM_FLAGS "-Wall -Werror -O0 -ggdb -g3 -gdwarf-2 -march=rv64g -ffreestanding -fno-common -nostdlib")

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/mnt")

add_subdirectory(kernel)
add_subdirectory(userland)
