cmake_minimum_required(VERSION 3.22)

project(OS-1 C ASM)

option(LOG_LEVEL "LOG_LEVEL" 0)

add_definitions(-DLOG_LEVEL=${LOG_LEVEL})

if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "")
	message(FATAL_ERROR "CMAKE_BUILD_TYPE is not supported")
endif()

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/root")

add_subdirectory(kernel)
add_subdirectory(libgloss)
add_subdirectory(userland)
