
add_library(string STATIC string.c)
target_include_directories(string PUBLIC
	${PROJECT_SOURCE_DIR}/lib/libc/include
	# TODO:
	${PROJECT_SOURCE_DIR}/include)
