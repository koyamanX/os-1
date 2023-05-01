add_subdirectory(string)
add_subdirectory(sys2)
add_subdirectory(stdlib)
add_subdirectory(stdio)
add_subdirectory(misc)

add_library(libc INTERFACE)
target_link_libraries(libc INTERFACE string sys2 stdlib misc stdio)
