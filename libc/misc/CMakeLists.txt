
add_library(misc STATIC
	dirname.c
	basename.c)

target_include_directories(misc PUBLIC
	${PROJECT_SOURCE_DIR}/libc/include
	# TODO:
	${PROJECT_SOURCE_DIR}/include)
