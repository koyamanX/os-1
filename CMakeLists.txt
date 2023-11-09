cmake_minimum_required(VERSION 3.22)
project(OS-1 C ASM)
enable_testing()

if(NOT CMAKE_C_COMPILER_ID MATCHES GNU)
	message(FATAL_ERROR "Unsupported compiler: ${CMAKE_C_COMPILER_ID}")
endif()

if(CMAKE_C_COMPILER_VERSION VERSION_LESS 10)
	message(FATAL_ERROR "Unsupported gcc version: ${CMAKE_C_COMPILER_VERSION}")
endif()

if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "")
	message(FATAL_ERROR "CMAKE_BUILD_TYPE is not supported")
endif()

option(LOG_LEVEL "LOG_LEVEL" 0)
add_definitions(-DLOG_LEVEL=${LOG_LEVEL})
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/root")

add_subdirectory(kernel)
add_subdirectory(libgloss)
add_subdirectory(userland)
add_subdirectory(tests)