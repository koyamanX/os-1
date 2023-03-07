
add_library(stdlib STATIC
	exit.c)

target_include_directories(stdlib PUBLIC
	${PROJECT_SOURCE_DIR}/libc/include
	# TODO:
	${PROJECT_SOURCE_DIR}/include)
