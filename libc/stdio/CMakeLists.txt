
add_library(stdio STATIC
	fprintf.c
	stdio.c)

target_include_directories(stdio PUBLIC
	${PROJECT_SOURCE_DIR}/libc/include
	# TODO:
	${PROJECT_SOURCE_DIR}/include)
