add_subdirectory(string)
add_subdirectory(sys)

add_library(libc INTERFACE)
target_link_libraries(libc INTERFACE string sys)
