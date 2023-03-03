add_subdirectory(string)
add_subdirectory(sys)
add_subdirectory(misc)

add_library(libc INTERFACE)
target_link_libraries(libc INTERFACE string sys misc)
